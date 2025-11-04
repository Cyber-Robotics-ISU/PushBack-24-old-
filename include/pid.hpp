#pragma once
#ifndef PID_HPP
#define PID_HPP

#include "api.h"
#include "pros/motors.hpp"
#include <cmath>
#include <string>
#include <vector>
#include <iostream>

class PID {
public:
  enum ExitOutput {
    RUNNING,
    SMALL_EXIT,
    BIG_EXIT,
    VELOCITY_EXIT,
    mA_EXIT,
    ERROR_NO_CONSTANTS
  };

  struct Constants {
    double kp{0};
    double ki{0};
    double kd{0};
    double start_i{0};
  };

  struct ExitCondition {
    int small_exit_time{0};
    double small_error{0};
    int big_exit_time{0};
    double big_error{0};
    int velocity_exit_time{0};
    int mA_timeout{0};
  };

  PID();
  PID(double p, double i, double d, double start_i = 0, const std::string &name = "");

  void constants_set(double p, double i, double d, double start_i);
  bool constants_set_check() const;

  void exit_condition_set(int small_exit_time, double small_error, int big_exit_time,
                          double big_error, int velocity_exit_time, int mA_timeout);

  void target_set(double input);
  double target_get() const;

  void i_reset_toggle(bool toggle);
  bool i_reset_get() const;

  double compute(double current);
  double compute_error(double err, double current);
  double raw_compute();

  void timers_reset();
  void name_set(const std::string &p_name);

  void velocity_sensor_secondary_toggle_set(bool toggle);
  bool velocity_sensor_secondary_toggle_get() const;

  void velocity_sensor_secondary_set(double secondary_sensor);
  double velocity_sensor_secondary_get() const;

  void velocity_sensor_main_exit_set(double zero);
  double velocity_sensor_main_exit_get() const;

  void velocity_sensor_secondary_exit_set(double zero);
  double velocity_sensor_secondary_exit_get() const;

  ExitOutput exit_condition(bool print = false);
  ExitOutput exit_condition(pros::Motor sensor, bool print = false);
  ExitOutput exit_condition(std::vector<pros::Motor> sensors, bool print = false);
  ExitOutput exit_condition(pros::MotorGroup sensor, bool print = false);

private:
  // --- PID variables ---
  double output{0};
  double target{0};
  double error{0};
  double prev_error{0};
  double integral{0};
  double derivative{0};
  double cur{0};
  double prev_current{0};

  bool reset_i_sgn{true};
  bool name_active{false};
  bool use_second_sensor{false};
  bool is_mA{false};

  // Timers
  int i{0}, j{0}, k{0}, l{0}, m{0};

  // Exit & constants
  Constants constants;
  ExitCondition exit;

  double second_sensor{0};
  double velocity_zero_main{0.0};
  double velocity_zero_secondary{0.0};

  std::string name;

  // --- Utility ---
  static constexpr int DELAY_TIME = 10;
  static inline int sgn(double val) { return (val > 0) - (val < 0); }

  void variables_reset();
  void exit_condition_print(ExitOutput type);
  std::string exit_to_string(ExitOutput type) const;
};

#endif
