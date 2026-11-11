// This file is distributed under the MIT License. See LICENSE for details.
#pragma once

#include <algorithm>
#include <cmath>
#include <ot/liberty/cellpin.hpp>

namespace ot {
struct RcResponse {
  float delay;
  float slew;
};

namespace detail {
// A prepared ramp stays local to one wire response or fitting iteration.
// Cache only threshold-independent values; no state is shared between pins.
class RcRamp {
public:
  RcRamp(double time_constant, double ramp_duration)
      : tau(time_constant), duration(ramp_duration) {
    if (tau <= 0 || duration <= 0)
      return;
    r = duration / tau;
    if (r > 1e8)
      return;
    tail = -std::expm1(-r) / r;
    end_voltage = r < 1e-4 ? r * (.5 - r / 6 + r * r / 24) : 1 - tail;
  }

  // Threshold crossing of a unit linear ramp filtered by H(s)=1/(1+s*tau).
  // Time is measured from the ramp's start. After the ramp ends its residual
  // exponential has a closed-form crossing; during the ramp solve the monotone
  // response x + exp(-x) - 1 = v*T/tau with safeguarded Newton iteration.
  double crossing(double voltage) {
    if (tau <= 0)
      return duration * voltage;
    if (duration <= 0)
      return -tau * std::log1p(-voltage);
    if (r > 1e8)
      return duration * voltage + tau;
    if (voltage >= end_voltage) {
      return duration + tau * (tail_log() - std::log1p(-voltage));
    }
    const double target = voltage * r;
    // Invert x + exp(-x) - 1 = target near zero. Starting at target+1
    // spends several Newton steps contracting toward sqrt(2*target) during
    // each effective-capacitance fit. This series supplies only the initial
    // guess; the same bracket and residual tolerance still govern the solve.
    double seed = target + 1;
    if (target < 1) {
      const double root = std::sqrt(2 * target);
      seed = root * (1 + root * (1. / 6 + root * (1. / 36 + root / 270)));
    }
    double low = 0, high = r, x = std::min(r, seed);
    for (int step = 0; step < 24; ++step) {
      // expm1 protects cancellation near zero. Away from zero, exp-1 has
      // ample precision for this residual and avoids the costlier kernel.
      const double expm1 = x >= .5 ? std::exp(-x) - 1 : std::expm1(-x);
      const double error = x + expm1 - target;
      if (std::abs(error) <= 1e-10 * std::max(1., target))
        break;
      if (error > 0)
        high = x;
      else
        low = x;
      const double next = x - error / -expm1;
      x = next > low && next < high ? next : (low + high) * .5;
    }
    return x * tau;
  }

private:
  double tail_log() {
    if (!has_log_tail) {
      log_tail = std::log(tail);
      has_log_tail = true;
    }
    return log_tail;
  }

  const double tau, duration;
  double r = 0, tail = 0, end_voltage = 0, log_tail = 0;
  bool has_log_tail = false;
};
} // namespace detail

inline double rc_ramp_crossing(double tau, double duration, double voltage) {
  return detail::RcRamp(tau, duration).crossing(voltage);
}

// Elmore is the pole's time constant, not the delay at a Liberty threshold.
// This remains a one-pole approximation of a general RC tree; it does not
// model driver effective-capacitance shielding. Each pin keeps its own
// library's thresholds, including the falling-edge voltage convention.
inline RcResponse rc_wire_response(double tau, float input_slew,
                                   const SlewThresholds &from,
                                   const SlewThresholds &to, Tran rf) {
  const double duration = std::abs(input_slew) * from.derate / from.span(rf);
  const auto progress = [rf](double v) { return rf == RISE ? v : 1 - v; };
  detail::RcRamp ramp(tau, duration);
  const double arrival = ramp.crossing(progress(to.input[rf]));
  const double launch = duration * progress(from.output[rf]);
  const double a = ramp.crossing(progress(to.lower[rf]));
  const double b = ramp.crossing(progress(to.upper[rf]));
  return {static_cast<float>(arrival - launch),
          static_cast<float>(std::abs(b - a) / to.derate)};
}
} // namespace ot
