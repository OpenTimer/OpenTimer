#ifndef CCSN_HPP
#define CCSN_HPP

#include <string>
#include <vector>
#include <optional>
#include <ot/liberty/lut.hpp>
#include <ot/liberty/power.hpp>

namespace ot
{
    enum class CCSNStageType
    {
        CCSN_LAST_STAGE,
        CCSN_FIRST_STAGE,
        CCSN_STAGE
    };

    struct CCSNStage
    {
        CCSNStageType ccsn_stage_type;

        std::optional<std::string> pin; // This should be the output pin if it is an output pin. Otherwise it is not assigned. We mark this
        std::optional<std::string> related_pin;

        bool is_inverting;
        bool is_needed;
        double miller_cap_fall;
        double miller_cap_rise;

        std::vector<Lut> dc_current;
        std::vector<Lut> output_voltage_fall;
        std::vector<Lut> output_voltage_rise;
        std::vector<Lut> propagated_noise_high;
        std::vector<Lut> propagated_noise_low;

        std::string stage_type;

        std::optional<std::string> when; // Optional condition for the CCSN stage
    };

}

#endif // CCSN_HPP
