#ifndef MECANUM_DRIVE_HPP
#define MECANUM_DRIVE_HPP

#include "global.hpp"  
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
        double x = 0.0;
        double y = 0.0;
        double theta = 0.0; // radians
    };

    struct PID {
        double kP = 0, kI = 0, kD = 0;
        double error = 0, prevError = 0, integral = 0, output = 0;

        double integralLimit = 5000;
        double iZone = 1e9;
        double dt = 0.01; // 10ms loop

        PID(double p = 0, double i = 0, double d = 0)
            : kP(p), kI(i), kD(d) {}

        double calculate(double target, double current, double maxOutput = 127) {
            error = target - current;

            if (std::fabs(error) < iZone) {
                integral += error * dt;
                integral = std::clamp(integral, -integralLimit, integralLimit);
            } else {
                integral = 0;
            }
            
            double derivative = (error - prevError) / dt;
            output = kP * error + kI * integral + kD * derivative;
            output = std::clamp(output, -maxOutput, maxOutput);

            prevError = error;
            return output;
        }

        void reset() {
            error = prevError = integral = output = 0;
        }
    };

    // ---------------- Constructors ----------------
    // 4-motor drivetrain
    MecanumDrive(pros::Motor &fl, pros::Motor &fr,
                 pros::Motor &bl, pros::Motor &br,
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

    // 8-motor drivetrain
    MecanumDrive(std::vector<pros::Motor*> fl,
                 std::vector<pros::Motor*> fr,
                 std::vector<pros::Motor*> bl,
                 std::vector<pros::Motor*> br,
                 pros::Imu *imu = nullptr,
                 pros::Rotation *vertEnc = nullptr,
                 pros::Rotation *horizEnc = nullptr,
                 double wheelDiameter = 2.75,
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
        drivePID.kP = kP; drivePID.kI = kI; drivePID.kD = kD;
    }

    void setStrafePID(double kP, double kI, double kD) {
        strafePID.kP = kP; strafePID.kI = kI; strafePID.kD = kD;
    }

    void setTurnPID(double kP, double kI, double kD) {
        turnPID.kP = kP; turnPID.kI = kI; turnPID.kD = kD;
    }

    void getDrivePID(double &kP, double &kI, double &kD) const {
        kP = drivePID.kP;
        kI = drivePID.kI;
        kD = drivePID.kD;
    }

    void getStrafePID(double &kP, double &kI, double &kD) const {
        kP = strafePID.kP;
        kI = strafePID.kI;
        kD = strafePID.kD;
    }

    void getTurnPID(double &kP, double &kI, double &kD) const {
        kP = turnPID.kP;
        kI = turnPID.kI;
        kD = turnPID.kD;
    }


    // ---------------- Drive Utilities ----------------
    void setFieldOriented(bool enabled) { fieldOriented = enabled; }

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
        while (imu->is_calibrating()) pros::delay(20);
        std::cout << "IMU calibration complete." << std::endl;
    }

    // ---------------- Odometry ----------------
    void updateOdometry() {
        double vertDeg = vertEnc ? rotationToDegrees(vertEnc->get_position()) : 0.0;
        double horizDeg = horizEnc ? rotationToDegrees(horizEnc->get_position()) : 0.0;

        double vertIn = degreesToInches(vertDeg - prevVertDeg);
        double horizIn = degreesToInches(horizDeg - prevHorizDeg);

        prevVertDeg = vertDeg;
        prevHorizDeg = horizDeg;

        double headingRad = imu ? degToRad(imu->get_heading()) : pose.theta;
        pose.theta = headingRad;

        pose.x += vertIn * cos(headingRad) - horizIn * sin(headingRad);
        pose.y += vertIn * sin(headingRad) + horizIn * cos(headingRad);
    }

    // ---------------- PID Movement ----------------
    void moveStraight(double targetInches, double maxPower = 127) {
        drivePID.reset();
        setPose(0, 0, imu ? imu->get_heading() : 0);
        double startVertDeg = vertEnc ? rotationToDegrees(vertEnc->get_position()) : 0.0;   
        uint32_t startTime = pros::millis();
        uint32_t settledTime = 0;

        while (true) {
            updateOdometry();
            double traveledIn = vertEnc
                ? degreesToInches(rotationToDegrees(vertEnc->get_position()) - startVertDeg)
                : pose.x;
            double error = targetInches - traveledIn;

            if (std::fabs(error) < 0.5) {
                if (settledTime == 0) settledTime = pros::millis();
                if (pros::millis() - settledTime >= 350) break;
            } else {
                settledTime = 0;
            }

            if (pros::millis() - startTime >= 9000) break;

            double power = drivePID.calculate(targetInches, traveledIn, maxPower);
            // Forward/back is vy in this drive() mapping.
            drive(0, power, 0, maxPower);

            pros::delay(10);
        }
        stop();
    }

    void strafePIDMove(double targetInches, double maxPower = 127) {
        strafePID.reset();
        setPose(0, 0, imu ? imu->get_heading() : 0);
        uint32_t startTime = pros::millis();
        uint32_t settledTime = 0;

        while (true) {
            updateOdometry();
            double error = targetInches - pose.y;

            if (std::fabs(error) < 0.5) {
                if (settledTime == 0) settledTime = pros::millis();
                if (pros::millis() - settledTime >= 350) break;
            } else {
                settledTime = 0;
            }

            if (pros::millis() - startTime >= 3000) break;

            double power = strafePID.calculate(targetInches, pose.y, maxPower);
            // Strafe left/right is vx in this drive() mapping.
            drive(power, 0, 0, maxPower);
            pros::delay(10);
        }
        stop();
    }

    void turnPIDAngle(double targetDegrees, double maxPower = 127) {
        if (!imu) return;
        turnPID.reset();

        double target = imu->get_heading() + targetDegrees;
        uint32_t startTime = pros::millis();
        uint32_t settledTime = 0;

        while (true) {
            double current = imu->get_heading();
            double error = std::remainder(target - current, 360.0);

            if (std::fabs(error) < 1.0) {
                if (settledTime == 0) settledTime = pros::millis();
                if (pros::millis() - settledTime >= 350) break;
            } else {
                settledTime = 0;
            }

            if (pros::millis() - startTime >= 9000) break;

            double power = turnPID.calculate(0, -error, maxPower);
            drive(0, 0, power, maxPower);
            pros::delay(10);
        }
        stop();
    }

    // ---------------- Driving ----------------
    void drive(double vx, double vy, double omega, double maxPower = 127) {
        if (fieldOriented && imu) {
            double heading = -degToRad(imu->get_heading());
            double cosA = cos(heading);
            double sinA = sin(heading);

            double x = vx * cosA - vy * sinA;
            double y = vx * sinA + vy * cosA;
            vx = x; vy = y;
        }

        double fl = vy + vx + omega;
        double fr = vy - vx - omega;
        double bl = vy - vx + omega;
        double br = vy + vx - omega;

        double maxMag = std::max({std::fabs(fl), std::fabs(fr),
                                  std::fabs(bl), std::fabs(br)});
        if (maxMag > maxPower) {
            fl = fl / maxMag * maxPower;
            fr = fr / maxMag * maxPower;
            bl = bl / maxMag * maxPower;
            br = br / maxMag * maxPower;
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

    void setPose(double x, double y, double theta_deg = 0) {
        pose = {x, y, degToRad(theta_deg)};
        if (vertEnc) vertEnc->reset_position();
        if (horizEnc) horizEnc->reset_position();
        prevVertDeg = prevHorizDeg = 0;
    }

    const Pose& getPose() const { return pose; }

private:
    std::vector<pros::Motor*> motorsFL, motorsFR, motorsBL, motorsBR;
    pros::Imu *imu = nullptr;
    pros::Rotation *vertEnc = nullptr;
    pros::Rotation *horizEnc = nullptr;
    bool fieldOriented = false;

    Pose pose;
    double prevVertDeg = 0;
    double prevHorizDeg = 0;

    double WHEEL_DIAMETER_IN = 2.75;
    double TRACK_WIDTH_IN;
    double TRACK_BASE_IN;
    double GEAR_RATIO;

    PID drivePID{5.0, 0.0, 2.0};
    PID strafePID{4.0, 0.0, 2.0};
    PID turnPID{3.0, 0.0, 1.5};

    double degToRad(double deg) const { return deg * M_PI / 180.0; }

    // Rotation sensors report centidegrees; convert to degrees.
    double rotationToDegrees(double raw) const { return raw / 100.0; }

    double degreesToInches(double deg) const {
        return (deg / 360.0 / GEAR_RATIO) * (M_PI * WHEEL_DIAMETER_IN);
    }

    void setMotorPower(std::vector<pros::Motor*> &motors, double power) {
        for (auto &m : motors) m->move(power);
    }
};

#endif
