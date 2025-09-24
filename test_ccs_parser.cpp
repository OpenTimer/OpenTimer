#include <iostream>
#include <ot/liberty/celllib.hpp>

int main() {
    try {
        ot::Celllib lib;
        lib.read("test_ccs_minimal.lib");

        std::cout << "Library loaded successfully!" << std::endl;
        std::cout << "Number of cells: " << lib.cells.size() << std::endl;
        std::cout << "Number of normalized driver waveforms: " << lib.normalized_driver_waveforms.size() << std::endl;

        // Print details about normalized driver waveforms
        for (size_t i = 0; i < lib.normalized_driver_waveforms.size(); i++) {
            const auto& wf = lib.normalized_driver_waveforms[i];
            std::cout << "  Waveform " << i << ": " << wf.driver_waveform_name
                      << " (template: " << wf.template_name << ")" << std::endl;
            std::cout << "    Index1 size: " << wf.index_1.size()
                      << ", Index2 size: " << wf.index_2.size()
                      << ", Values size: " << wf.values.size() << std::endl;
        }

        // Check if we can access CCSN stages
        for (const auto& [cell_name, cell] : lib.cells) {
            std::cout << "Cell: " << cell_name << std::endl;

            // Check pins for CCSN stages
            for (const auto& [pin_name, pin] : cell.cellpins) {
                if (pin.ccsn_stages) {
                    std::cout << "  Pin " << pin_name << " has " << pin.ccsn_stages->size() << " CCSN stages" << std::endl;

                    for (size_t i = 0; i < pin.ccsn_stages->size(); i++) {
                        const auto& stage = (*pin.ccsn_stages)[i];
                        std::cout << "    Stage " << i << ":" << std::endl;
                        std::cout << "      DC current tables: " << stage.dc_current.size() << std::endl;
                        std::cout << "      Output voltage fall: " << stage.output_voltage_fall.size() << std::endl;
                        std::cout << "      Output voltage rise: " << stage.output_voltage_rise.size() << std::endl;
                        std::cout << "      Propagated noise high: " << stage.propagated_noise_high.size() << std::endl;
                        std::cout << "      Propagated noise low: " << stage.propagated_noise_low.size() << std::endl;
                    }
                }

                // Check timing arcs within pins for CCSN stages
                for (const auto& timing : pin.timings) {
                    if (timing.ccsn_stages) {
                        std::cout << "    Timing arc has " << timing.ccsn_stages->size() << " CCSN stages" << std::endl;
                    }
                }
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}