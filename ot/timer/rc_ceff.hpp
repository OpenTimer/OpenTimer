// This file is distributed under the MIT License. See LICENSE for details.
#pragma once
#include <array>
#include <limits>
#include <optional>
#include <ot/timer/rc_waveform.hpp>

namespace ot {
// Driving-point admittance matched through its third moment.
struct RcPi {
  double near_cap = 0, far_cap = 0, resistance = 0;
};

inline double rc_ramp_voltage(double t, double tau, double duration) {
  if (t <= 0)
    return 0;
  if (tau <= 0)
    return duration > 0 ? std::min(1., t / duration) : 1;
  if (duration <= 0)
    return -std::expm1(-t / tau);
  if (t < duration) {
    const double x = t / tau;
    const double integral =
        x < 1e-3 ? x * x * (.5 - x / 6 + x * x / 24 - x * x * x / 120)
                 : x + std::expm1(-x);
    return tau * integral / duration;
  }
  return 1 - (-std::expm1(-duration / tau) * tau / duration) *
                 std::exp(-(t - duration) / tau);
}

inline double rc_cascade_voltage(double t, double a, double b,
                                 double duration) {
  if (a <= 0)
    return rc_ramp_voltage(t, b, duration);
  if (b <= 0)
    return rc_ramp_voltage(t, a, duration);
  // Symmetric divided difference also handles coincident poles without
  // subtracting nearly equal exponentials at machine precision.
  if (std::abs(a - b) < 1e-4 * std::max(a, b)) {
    const double mean = (a + b) * .5;
    a = mean * 1.00005;
    b = mean * .99995;
  }
  return (a * rc_ramp_voltage(t, a, duration) -
          b * rc_ramp_voltage(t, b, duration)) /
         (a - b);
}

struct RcDriver {
  double duration = 0, slow = 0, fast = 0, slow_weight = 1;
  double logic_time = 0, slew = 0;
  double voltage(double t, double wire_tau = 0) const {
    if (wire_tau <= 0)
      return slow_weight * rc_ramp_voltage(t, slow, duration) +
             (1 - slow_weight) * rc_ramp_voltage(t, fast, duration);
    return slow_weight * rc_cascade_voltage(t, slow, wire_tau, duration) +
           (1 - slow_weight) * rc_cascade_voltage(t, fast, wire_tau, duration);
  }
  double crossing(double v, double wire_tau = 0) const {
    return crossings<1>({v}, wire_tau)[0];
  }
  // Arrival and slew query one waveform at several thresholds. Prepare its
  // poles and initial bracket sample once; each threshold retains its own
  // safeguarded solve, without ordering assumptions or shared iteration state.
  template <size_t N>
  std::array<double, N> crossings(const std::array<double, N> &voltages,
                                  double wire_tau = 0) const {
    std::array<double, N> result{};
    double initial_hi = duration;
    if (initial_hi <= 0)
      initial_hi = std::max({slow, fast, wire_tau});
    if (initial_hi <= 0)
      return result;
    // The tail coefficient depends only on the pole and ramp duration.
    // Reuse it across the bracket search and Newton/bisection evaluations.
    const auto tail = [this](double tau) {
      return tau > 0 && duration > 0
                 ? -std::expm1(-duration / tau) * tau / duration
                 : 0.;
    };
    const auto prepare = [&](double a, double b) {
      if (a <= 0)
        return std::array<double, 4>{b, 0, tail(b), 0};
      if (b <= 0)
        return std::array<double, 4>{a, 0, tail(a), 0};
      if (std::abs(a - b) < 1e-4 * std::max(a, b)) {
        const double mean = (a + b) * .5;
        a = mean * 1.00005;
        b = mean * .99995;
      }
      return std::array<double, 4>{a, b, tail(a), tail(b)};
    };
    struct Sample {
      double voltage, slope;
    };
    const auto ramp = [this](double t, double tau,
                             double coefficient) -> Sample {
      if (t <= 0)
        return {0, 0};
      if (tau <= 0)
        return duration > 0 && t < duration ? Sample{t / duration, 1 / duration}
                                            : Sample{1, 0};
      if (duration <= 0) {
        const double response = -std::expm1(-t / tau);
        return {response, (1 - response) / tau};
      }
      if (t >= duration) {
        const double tail = coefficient * std::exp(-(t - duration) / tau);
        return {1 - tail, tail / tau};
      }
      const double x = t / tau, response = -std::expm1(-x);
      const double integral =
          x < 1e-3 ? x * x * (.5 - x / 6 + x * x / 24 - x * x * x / 120)
                   : x - response;
      return {tau * integral / duration, response / duration};
    };
    const auto cascade = [](const Sample &a, const Sample &b,
                            const std::array<double, 4> &poles) -> Sample {
      const double difference = poles[0] - poles[1];
      return {(poles[0] * a.voltage - poles[1] * b.voltage) / difference,
              (poles[0] * a.slope - poles[1] * b.slope) / difference};
    };
    const auto response = [&](double t,
                              const std::array<double, 4> &poles) -> Sample {
      const auto a = ramp(t, poles[0], poles[2]);
      return poles[1] <= 0 ? a : cascade(a, ramp(t, poles[1], poles[3]), poles);
    };
    const auto slow_poles = prepare(slow, wire_tau <= 0 ? 0 : wire_tau);
    const auto fast_poles = prepare(fast, wire_tau <= 0 ? 0 : wire_tau);
    // Both cascades usually share the wire pole. Its value and derivative use
    // the same exponential, evaluated once per sample. Coincident poles keep
    // their existing perturbed divided-difference model.
    const bool shared_wire =
        slow_poles[1] > 0 && slow_poles[1] == fast_poles[1];
    const auto sample = [&](double t) -> Sample {
      Sample a, b;
      if (shared_wire) {
        const auto wire = ramp(t, slow_poles[1], slow_poles[3]);
        a = cascade(ramp(t, slow_poles[0], slow_poles[2]), wire, slow_poles);
        b = cascade(ramp(t, fast_poles[0], fast_poles[2]), wire, fast_poles);
      } else {
        a = response(t, slow_poles);
        b = response(t, fast_poles);
      }
      return {slow_weight * a.voltage + (1 - slow_weight) * b.voltage,
              slow_weight * a.slope + (1 - slow_weight) * b.slope};
    };
    const double initial_voltage = sample(initial_hi).voltage;
    const auto solve = [&](double v) {
      if (duration <= 0 && wire_tau <= 0) {
        const double jump =
            (slow <= 0 ? slow_weight : 0) + (fast <= 0 ? 1 - slow_weight : 0);
        if (v <= jump)
          return 0.;
      }
      double lo = 0, hi = initial_hi;
      // The successful bracket sample is also the validity check below. Retain
      // it rather than evaluating both pole responses again at the same time.
      double high_voltage = initial_voltage;
      for (int k = 0; k < 128 && high_voltage < v; ++k) {
        hi *= 2;
        high_voltage = sample(hi).voltage;
      }
      if (!std::isfinite(hi) || high_voltage < v)
        return std::numeric_limits<double>::infinity();
      // Solve the same monotone waveform, retaining the bracket whenever a
      // Newton step is unsafe. Time-relative tolerance is much finer than the
      // float timing values returned by the engine and is invariant to units.
      double x = (lo + hi) * .5;
      for (int k = 0; k < 48; ++k) {
        const auto value = sample(x);
        const double error = value.voltage - v;
        if (error < 0)
          lo = x;
        else
          hi = x;
        const double tolerance = 1e-10 * std::max(x, 1e-300);
        const double correction = value.slope > 0
                                      ? error / value.slope
                                      : std::numeric_limits<double>::infinity();
        if (std::abs(correction) <= tolerance)
          return x;
        if (hi - lo <= tolerance)
          break;
        const double next = x - correction;
        x = next > lo && next < hi ? next : (lo + hi) * .5;
      }
      return (lo + hi) * .5;
    };
    for (size_t i = 0; i < N; ++i)
      result[i] = solve(voltages[i]);
    return result;
  }
};

struct RcGateResult {
  float capacitance = 0, delay = 0, slew = 0;
  RcDriver driver;
};

inline std::optional<double> rc_fit_ramp(double tau, double target, double low,
                                         double logic) {
  if (target <= 0 || logic <= low)
    return {};
  const double minimum = tau * std::log((1 - low) / (1 - logic));
  if (target < minimum * (1 - 1e-8))
    return {};
  if (target <= minimum * (1 + 1e-8))
    return 0;
  double lo = 0, hi = 2 * target / (logic - low) + 2 * tau;
  double t = target / (logic - low);
  // When both thresholds cross during the ramp, eliminate its duration from
  // v*T/tau = x + exp(-x) - 1. With d=target/tau and x the lower crossing,
  // the remaining equation is x + q*exp(-x) = m. Its upper root supplies a
  // seed without repeatedly solving two crossings inside the duration fit.
  // The ordinary residual check below still validates the seed; mixed/tail
  // crossings and ill-conditioned cases keep the original bounded solve.
  // Far into the ramp, exp(-x) is already below the fitting tolerance and
  // the original linear-ramp seed needs no refinement.
  if (tau > 0 && low > 0 && logic < 1 && low * (t / tau) < 20) {
    const double d = target / tau, ratio = low / (logic - low);
    const double q = 1 + ratio * -std::expm1(-d);
    const double m = 1 + ratio * d;
    double x = m;
    for (int k = 0; k < 24; ++k) {
      const double exponential = q * std::exp(-x);
      const double residual = x - m + exponential;
      if (std::abs(residual) <= 1e-12 * std::max(1., m)) {
        const double duration = (x + std::expm1(-x)) * tau / low;
        if (std::isfinite(duration) && duration > lo && duration < hi &&
            (x + d) * tau <= duration)
          t = duration;
        break;
      }
      const double next = x - residual / (1 - exponential);
      if (!(next > 0 && std::isfinite(next)))
        break;
      x = next;
    }
  }
  const auto derivative = [tau](double duration, double v, double crossing) {
    if (tau <= 0)
      return v;
    if (crossing < duration)
      return v / -std::expm1(-crossing / tau);
    const double r = duration / tau;
    if (r < 1e-3)
      return .5 + r / 12 - r * r * r / 720;
    return 1 + (r > 700 ? 0 : 1 / std::expm1(r)) - 1 / r;
  };
  for (int k = 0; k < 24; ++k) {
    detail::RcRamp ramp(tau, t);
    const double a = ramp.crossing(logic), b = ramp.crossing(low);
    const double error = a - b - target;
    if (std::abs(error) < 1e-7 * target)
      return t;
    if (error > 0)
      hi = t;
    else
      lo = t;
    const double slope = derivative(t, logic, a) - derivative(t, low, b);
    const double next = slope > 0 ? t - error / slope : -1;
    t = next > lo && next < hi ? next : (lo + hi) * .5;
  }
  return {};
}

// Model reference: https://doi.org/10.1145/196244.196562
// Charge-equivalent capacitance of a fitted ramp/Thevenin driver on a Pi
// load (Dartu/Menezes/Pileggi model). This implementation reduces the fit to
// bounded scalar solves, rather than a multidimensional Newton Jacobian.
// No fit is allowed outside [near_cap, total_cap]; failure keeps NLDM at the
// full load. Units are the timer's consistent R/C/time units.
template <class Table>
std::optional<RcGateResult> rc_effective_gate(const RcPi &pi,
                                              const SlewThresholds &thresholds,
                                              Tran rf, Table table) {
  const double total = pi.near_cap + pi.far_cap;
  if (!(total > 0 && pi.far_cap > total * 1e-6 && pi.resistance > 0))
    return {};
  const auto base = table(total);
  if (!base || base->second <= 0)
    return {};
  const double delta = total * .01;
  const auto larger = table(total + delta);
  if (!larger)
    return {};
  const auto progress = [rf](double v) { return rf == RISE ? v : 1 - v; };
  const double logic = progress(thresholds.output[rf]);
  const double low =
      rf == RISE ? thresholds.lower[rf] : 1 - thresholds.upper[rf];
  const double high =
      rf == RISE ? thresholds.upper[rf] : 1 - thresholds.lower[rf];
  if (!(0 < low && low < logic && logic < high && high < 1))
    return {};
  const double rd =
      -std::log(logic) * std::abs(larger->first - base->first) / delta;
  if (!(rd > 0) || pi.resistance * pi.far_cap < base->second * 1e-5)
    return {};
  const double b = rd * total + pi.resistance * pi.far_cap;
  const double a = rd * pi.resistance * pi.near_cap * pi.far_cap;
  const double disc = std::sqrt(std::max(0., b * b - 4 * a));
  RcDriver driver;
  driver.slow = (b + disc) * .5;
  driver.fast = driver.slow > 0 ? a / driver.slow : 0;
  if (!(driver.slow > driver.fast))
    return {};
  driver.slow_weight =
      (driver.slow - pi.resistance * pi.far_cap) / (driver.slow - driver.fast);
  const double far_weight = driver.slow / (driver.slow - driver.fast);
  struct Fit {
    double error, cap, delay;
    RcDriver driver;
  };
  const auto fit = [&](double cap) -> std::optional<Fit> {
    const auto values = table(cap);
    if (!values || values->second <= 0)
      return {};
    const double measured = values->second * thresholds.derate;
    const auto duration = rc_fit_ramp(
        rd * cap, measured * (logic - low) / (high - low), low, logic);
    if (!duration || *duration <= 0)
      return {};
    auto d = driver;
    d.duration = *duration;
    // Match charge during the driver's transition. Limiting the integration
    // window prevents its settled tail from erasing resistance shielding.
    const double window = std::min(measured / (high - low), 1.4 * d.duration);
    const double far =
        far_weight * rc_ramp_voltage(window, d.slow, d.duration) +
        (1 - far_weight) * rc_ramp_voltage(window, d.fast, d.duration);
    const double charge = pi.near_cap * d.voltage(window) + pi.far_cap * far;
    const double reference =
        cap * rc_ramp_voltage(window, rd * cap, d.duration);
    return Fit{(charge - reference) / total, cap, values->first, d};
  };
  double lo = std::max(pi.near_cap, total * 1e-6), hi = total;
  constexpr double fit_tolerance = 1e-7;
  std::optional<double> low_error, high_error;
  // Short wires put the solution close to full capacitance, while strongly
  // shielded loads approach the near capacitance. Sample those bounds first
  // instead of spending most trials discovering the opposite residual sign.
  std::optional<Fit> best = fit(hi);
  if (best)
    high_error = best->error;
  if (!best || std::abs(best->error) >= fit_tolerance) {
    const auto low_fit = fit(lo);
    if (low_fit) {
      low_error = low_fit->error;
      if (!best || std::abs(low_fit->error) < std::abs(best->error))
        best = low_fit;
    }
  }
  // Each trial fits another waveform. Interpolate inside a valid bracket;
  // every other step still bisects to bound work in non-smooth LUT regions.
  // The saved trials allow a tighter charge residual than the old 1e-5 fit.
  // 48 steps guarantee 24 bisections, enough for the 1e-7 interval bound.
  for (int k = 0; k < 48 && (!best || std::abs(best->error) >= fit_tolerance);
       ++k) {
    double c = (lo + hi) * .5;
    if (k % 2 == 0 && low_error && high_error) {
      const double interpolated =
          lo + (hi - lo) * *low_error / (*low_error - *high_error);
      if (interpolated > lo && interpolated < hi)
        c = interpolated;
    }
    auto f = fit(c);
    if (!f) {
      hi = c;
      high_error.reset();
      continue;
    }
    best = f;
    if (std::abs(f->error) < fit_tolerance || hi - lo < total * fit_tolerance)
      break;
    if (f->error > 0) {
      lo = c;
      low_error = f->error;
    } else {
      hi = c;
      high_error = f->error;
    }
  }
  if (!best || std::abs(best->error) > 1e-3)
    return {};
  auto d = best->driver;
  const auto crossings = d.crossings(std::array<double, 3>{logic, high, low});
  d.logic_time = crossings[0];
  d.slew = (crossings[1] - crossings[2]) / thresholds.derate;
  if (!(std::isfinite(d.slew) && d.slew > 0))
    return {};
  return RcGateResult{float(best->cap), float(best->delay), float(d.slew), d};
}
} // namespace ot
