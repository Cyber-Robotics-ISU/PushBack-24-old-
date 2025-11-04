#include "pid.hpp"

void PID::variables_reset() {
  output = 0;
  target = 0;
  error = 0;
  prev_error = 0;
  integral = 0;
  cur = 0;
  prev_current = 0;
  i = j = k = l = m = 0;
  is_mA = false;
}

PID::PID() { variables_reset(); }

PID::PID(double p, double i, double d, double start_i, const std::string &name)
    : name(name) {
  variables_reset();
  constants_set(p, i, d, start_i);
  name_active = !name.empty();
}

void PID::constants_set(double p, double i, double d, double start_i) {
  constants.kp = p;
  constants.ki = i;
  constants.kd = d;
  constants.start_i = start_i;
}

bool PID::constants_set_check() const {
  return !(constants.kp == 0 && constants.ki == 0 && constants.kd == 0 && constants.start_i == 0);
}

void PID::exit_condition_set(int small_exit_time, double small_error, int big_exit_time,
                             double big_error, int velocity_exit_time, int mA_timeout) {
  exit.small_exit_time = small_exit_time;
  exit.small_error = small_error;
  exit.big_exit_time = big_exit_time;
  exit.big_error = big_error;
  exit.velocity_exit_time = velocity_exit_time;
  exit.mA_timeout = mA_timeout;
}

void PID::target_set(double input) { target = input; }
double PID::target_get() const { return target; }

void PID::i_reset_toggle(bool toggle) { reset_i_sgn = toggle; }
bool PID::i_reset_get() const { return reset_i_sgn; }

double PID::compute(double current) { return compute_error(target - current, current); }

double PID::compute_error(double err, double current) {
  error = err;
  cur = current;
  return raw_compute();
}

double PID::raw_compute() {
  derivative = cur - prev_current;

  if (constants.ki != 0) {
    if (fabs(error) < constants.start_i)
      integral += error;

    if (sgn(error) != sgn(prev_error) && reset_i_sgn)
      integral = 0;
  }

  output = (error * constants.kp) + (integral * constants.ki) - (derivative * constants.kd);
  prev_current = cur;
  prev_error = error;

  return output;
}

void PID::timers_reset() { i = j = k = l = m = 0; is_mA = false; }

void PID::name_set(const std::string &p_name) {
  name = p_name;
  name_active = !name.empty();
}

void PID::exit_condition_print(ExitOutput type) {
  std::string msg = exit_to_string(type);
  if (name_active)
    std::cout << name << " PID " << msg << " Exit.\n";
  else
    std::cout << msg << " Exit.\n";
}

std::string PID::exit_to_string(ExitOutput type) const {
  switch (type) {
  case SMALL_EXIT: return "Small";
  case BIG_EXIT: return "Big";
  case VELOCITY_EXIT: return "Velocity";
  case mA_EXIT: return "mA";
  case ERROR_NO_CONSTANTS: return "Error: No constants";
  default: return "Running";
  }
}

void PID::velocity_sensor_secondary_toggle_set(bool toggle) { use_second_sensor = toggle; }
bool PID::velocity_sensor_secondary_toggle_get() const { return use_second_sensor; }

void PID::velocity_sensor_secondary_set(double sensor) { second_sensor = sensor; }
double PID::velocity_sensor_secondary_get() const { return second_sensor; }

void PID::velocity_sensor_main_exit_set(double zero) { velocity_zero_main = zero; }
double PID::velocity_sensor_main_exit_get() const { return velocity_zero_main; }

void PID::velocity_sensor_secondary_exit_set(double zero) { velocity_zero_secondary = zero; }
double PID::velocity_sensor_secondary_exit_get() const { return velocity_zero_secondary; }

PID::ExitOutput PID::exit_condition(bool print) {
  if (exit.small_error == 0 && exit.small_exit_time == 0 &&
      exit.big_error == 0 && exit.big_exit_time == 0 &&
      exit.velocity_exit_time == 0 && exit.mA_timeout == 0) {
    if (print) exit_condition_print(ERROR_NO_CONSTANTS);
    return ERROR_NO_CONSTANTS;
  }

  if (exit.small_error != 0) {
    if (fabs(error) < exit.small_error) {
      j += DELAY_TIME;
      i = 0;
      if (j > exit.small_exit_time) {
        timers_reset();
        if (print) exit_condition_print(SMALL_EXIT);
        return SMALL_EXIT;
      }
    } else j = 0;
  } else if (exit.big_error != 0 && exit.big_exit_time != 0) {
    if (fabs(error) < exit.big_error) {
      i += DELAY_TIME;
      if (i > exit.big_exit_time) {
        timers_reset();
        if (print) exit_condition_print(BIG_EXIT);
        return BIG_EXIT;
      }
    } else i = 0;
  }

  if (exit.velocity_exit_time != 0) {
    if (fabs(derivative) <= velocity_zero_main) {
      k += DELAY_TIME;
      if (k > exit.velocity_exit_time) {
        timers_reset();
        if (print) exit_condition_print(VELOCITY_EXIT);
        return VELOCITY_EXIT;
      }
    } else k = 0;
  }

  if (use_second_sensor && exit.velocity_exit_time != 0) {
    if (fabs(second_sensor) <= velocity_zero_secondary) {
      m += DELAY_TIME;
      if (m > exit.velocity_exit_time) {
        timers_reset();
        if (print) exit_condition_print(VELOCITY_EXIT);
        return VELOCITY_EXIT;
      }
    } else m = 0;
  }

  return RUNNING;
}

PID::ExitOutput PID::exit_condition(pros::Motor sensor, bool print) {
  if (exit.mA_timeout != 0) {
    if (sensor.is_over_current()) {
      l += DELAY_TIME;
      if (l > exit.mA_timeout) {
        timers_reset();
        if (print) exit_condition_print(mA_EXIT);
        return mA_EXIT;
      }
    } else l = 0;
  }
  return exit_condition(print);
}

PID::ExitOutput PID::exit_condition(std::vector<pros::Motor> sensors, bool print) {
  if (exit.mA_timeout != 0) {
    is_mA = false;
    for (auto &m : sensors)
      if (m.is_over_current()) { is_mA = true; break; }

    if (is_mA) {
      l += DELAY_TIME;
      if (l > exit.mA_timeout) {
        timers_reset();
        if (print) exit_condition_print(mA_EXIT);
        return mA_EXIT;
      }
    } else l = 0;
  }
  return exit_condition(print);
}

PID::ExitOutput PID::exit_condition(pros::MotorGroup sensor, bool print) {
  std::vector<pros::Motor> motors;
  for (int idx = 0; idx < sensor.size(); ++idx)
    motors.push_back(pros::Motor(sensor.get_port(idx)));
  return exit_condition(motors, print);
}
