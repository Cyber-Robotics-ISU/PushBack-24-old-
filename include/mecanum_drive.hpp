#ifndef MECANUM_DRIVE_HPP
#define MECANUM_DRIVE_HPP

#include "pros/motors.hpp"
#include "pros/imu.hpp"
#include "pros/rotation.hpp"
#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>

class MecanumDrive {
public:
    // ---------------- Structs ----------------
    struct Pose {
        double x = 0.0;     // inches
        double y = 0.0;     // inches
        double theta = 0.0; // radians
    };

    struct PID {
        double kP, kI, kD;
        double error = 0, prevError = 0, integral = 0, output = 0;

        PID(double p = 0, double i = 0, double d = 0)
            : kP(p), kI(i), kD(d) {}

        double calculate(double target, double current) {
            error = target - current;
            integral += error;
            double derivative = error - prevError;
            output = kP * error + kI * integral + kD * derivative;
            prevError = error;
            return output;
        }

        void reset() { error = prevError = integral = output = 0; }
    };

    // ---------------- Constructors ----------------
    // 4-motor version
    MecanumDrive(pros::Motor &fl, pros::Motor &fr, pros::Motor &bl, pros::Motor &br,
                 pros::Imu *imu = nullptr,
                 pros::Rotation *vertEnc = nullptr,
                 pros::Rotation *horizEnc = nullptr,
                 double wheelDiameter = 4.0,
                 double trackWidth = 13.5,
                 double trackBase = 13.0,
                 double gearRatio = 1.0)
        : imu(imu), vertEnc(vertEnc), horizEnc(horizEnc),
          WHEEL_DIAMETER_IN(wheelDiameter),
          TRACK_WIDTH_IN(trackWidth),
          TRACK_BASE_IN(trackBase),
          GEAR_RATIO(gearRatio) 
    {
        motorsFL = {&fl};
        motorsFR = {&fr};
        motorsBL = {&bl};
        motorsBR = {&br};
    }

    // 8-motor version
    MecanumDrive(std::vector<pros::Motor *> fl, std::vector<pros::Motor *> fr,
                 std::vector<pros::Motor *> bl, std::vector<pros::Motor *> br,
                 pros::Imu *imu = nullptr,
                 pros::Rotation *vertEnc = nullptr,
                 pros::Rotation *horizEnc = nullptr,
                 double wheelDiameter = 4.0,
                 double trackWidth = 13.5,
                 double trackBase = 13.0,
                 double gearRatio = 1.0)
        : motorsFL(fl), motorsFR(fr), motorsBL(bl), motorsBR(br),
          imu(imu), vertEnc(vertEnc), horizEnc(horizEnc),
          WHEEL_DIAMETER_IN(wheelDiameter),
          TRACK_WIDTH_IN(trackWidth),
          TRACK_BASE_IN(trackBase),
          GEAR_RATIO(gearRatio) {}
    
    // ---------------- PID Configuration ----------------
    void setDrivePID(double kP, double kI, double kD) {
        drivePID.kP = kP;
        drivePID.kI = kI;
        drivePID.kD = kD;
    }

    void setStrafePID(double kP, double kI, double kD) {
        strafePID.kP = kP;
        strafePID.kI = kI;
        strafePID.kD = kD;
    }

    void setTurnPID(double kP, double kI, double kD) {
        turnPID.kP = kP;
        turnPID.kI = kI;
        turnPID.kD = kD;
    }

    void drive_brake_set(pros::motor_brake_mode_e_t mode) {
        for (auto &m : motorsFL) m->set_brake_mode(mode);
        for (auto &m : motorsFR) m->set_brake_mode(mode);
        for (auto &m : motorsBL) m->set_brake_mode(mode);
        for (auto &m : motorsBR) m->set_brake_mode(mode);
    }

    void drive_imu_calibrate() {
        if (!imu) return;
        imu->reset();
        std::cout << "Calibrating IMU..." << std::endl;
        while (imu->is_calibrating()) {
            pros::delay(20);
        }
        std::cout << "IMU calibration complete." << std::endl;
    }

    // ---------------- Odometry ----------------
    void updateOdometry() {
        double vertDeg = vertEnc ? vertEnc->get_position() : 0.0;
        double horizDeg = horizEnc ? horizEnc->get_position() : 0.0;

        double vertIn = degreesToInches(vertDeg - prevVertDeg);
        double horizIn = degreesToInches(horizDeg - prevHorizDeg);

        prevVertDeg = vertDeg;
        prevHorizDeg = horizDeg;

        double headingRad = imu ? degToRad(imu->get_heading()) : pose.theta;
        pose.theta = headingRad;

        // Field transformation for tracking
        pose.x += vertIn * cos(headingRad) - horizIn * sin(headingRad);
        pose.y += vertIn * sin(headingRad) + horizIn * cos(headingRad);
    }

    Pose getPose() const { return pose; }

    void setPose(double x, double y, double theta_deg = 0.0) {
        pose = {x, y, degToRad(theta_deg)};
        if (vertEnc) vertEnc->reset_position();
        if (horizEnc) horizEnc->reset_position();
        prevVertDeg = prevHorizDeg = 0;
    }

    // ---------------- PID Control ----------------
    void moveStraight(double targetInches, double maxPower = 127) {
        drivePID.reset();
        setPose(0, 0, imu ? imu->get_heading() : 0);
        const double tolerance = 0.5;

        while (true) {
            updateOdometry();
            double error = targetInches - pose.x;
            if (std::fabs(error) < tolerance) break;

            double power = drivePID.calculate(targetInches, pose.x);
            drive(power, 0, 0, maxPower);
            pros::delay(10);
        }
        stop();
    }

    void strafePIDMove(double targetInches, double maxPower = 127) {
        strafePID.reset();
        setPose(0, 0, imu ? imu->get_heading() : 0);
        const double tolerance = 0.5;

        while (true) {
            updateOdometry();
            double error = targetInches - pose.y;
            if (std::fabs(error) < tolerance) break;

            double power = strafePID.calculate(targetInches, pose.y);
            drive(0, power, 0, maxPower);
            pros::delay(10);
        }
        stop();
    }

    void turnPIDAngle(double targetDegrees, double maxPower = 127) {
        if (!imu) return;
        turnPID.reset();

        double target = imu->get_heading() + targetDegrees;
        const double tolerance = 1.0;

        while (true) {
            double current = imu->get_heading();
            double error = target - current;
            if (std::fabs(error) < tolerance) break;

            double power = turnPID.calculate(target, current);
            drive(0, 0, power, maxPower);
            pros::delay(10);
        }
        stop();
    }

    // Aliases for cleaner API
    void driveDistance(double inches, double maxPower = 127) { moveStraight(inches, maxPower); }
    void strafeDistance(double inches, double maxPower = 127) { strafePIDMove(inches, maxPower); }
    void turnAngle(double degrees, double maxPower = 127) { turnPIDAngle(degrees, maxPower); }

    // ---------------- Driving ----------------
    void drive(double vx, double vy, double omega, double maxPower = 127.0) {
        if (imu) {
            double heading = degToRad(imu->get_heading());
            double cosA = cos(heading);
            double sinA = sin(heading);

            // Field-centric transform
            double vx_field = vx * cosA - vy * sinA;
            double vy_field = vx * sinA + vy * cosA;
            vx = vx_field;
            vy = vy_field;
        }
        // devank & simon fixed
        double fl = vy - vx + omega;
        double fr = vy + vx - omega;
        double bl = vy + vx + omega;
        double br = vy - vx - omega;


        double maxMagnitude = std::max({std::fabs(fl), std::fabs(fr), std::fabs(bl), std::fabs(br)});
        if (maxMagnitude > maxPower) {
            fl = fl / maxMagnitude * maxPower;
            fr = fr / maxMagnitude * maxPower;
            bl = bl / maxMagnitude * maxPower;
            br = br / maxMagnitude * maxPower;
        }

        setMotorPower(motorsFL, fl);
        setMotorPower(motorsFR, fr);
        setMotorPower(motorsBL, bl);
        setMotorPower(motorsBR, br);
    }

    void stop() {
        setMotorPower(motorsFL, 0);
        setMotorPower(motorsFR, 0);
        setMotorPower(motorsBL, 0);
        setMotorPower(motorsBR, 0);
    }

    // ---------------- Utilities ----------------
    void printPose() {
        std::cout << "X: " << pose.x
                  << " | Y: " << pose.y
                  << " | Theta: " << radToDeg(pose.theta) << "°" << std::endl;
    }

private:
    // Motor groups
    std::vector<pros::Motor*> motorsFL, motorsFR, motorsBL, motorsBR;

    // Sensors
    pros::Imu *imu = nullptr;
    pros::Rotation *vertEnc = nullptr;
    pros::Rotation *horizEnc = nullptr;

    // Odometry
    Pose pose;
    double prevVertDeg = 0.0;
    double prevHorizDeg = 0.0;

    // Physical constants
    double WHEEL_DIAMETER_IN = 4.0;
    double TRACK_WIDTH_IN = 13.5;
    double TRACK_BASE_IN = 13.0;
    double GEAR_RATIO = 1.0;

    // PID controllers
    PID drivePID{5.0, 0.0, 2.0};
    PID strafePID{4.0, 0.0, 2.0};
    PID turnPID{3.0, 0.0, 1.5};

    // ---------------- Helper functions ----------------
    double degToRad(double deg) const { return deg * M_PI / 180.0; }
    double radToDeg(double rad) const { return rad * 180.0 / M_PI; }

    double degreesToInches(double degrees) const {
        double rotations = degrees / 360.0 / GEAR_RATIO;
        return rotations * (M_PI * WHEEL_DIAMETER_IN);
    }

    void setMotorPower(std::vector<pros::Motor*> &motors, double power) {
        for (auto &m : motors) m->move(power);
    }
};

#endif
