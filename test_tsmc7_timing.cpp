#include <iostream>
#include <chrono>
#include <ot/liberty/celllib.hpp>

int main() {
    const std::string tsmc7_path = "/media/nvme1/cad/pdks/tsmc/cell_libraries/7/tcbn07_bwph240l11p57pd_base_svt_130b/TSMCHOME/digital/Front_End/timing_power_noise/CCS/tcbn07_bwph240l11p57pd_base_svt_130a/tcbn07_bwph240l11p57pd_base_svttt_0p65v_25c_typical_ccs.lib";

    std::cout << "Timing TSMC7 CCS library parsing..." << std::endl;
    std::cout << "File: " << tsmc7_path << std::endl;

    try {
        auto start_time = std::chrono::high_resolution_clock::now();

        ot::Celllib lib;
        lib.read(tsmc7_path);

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

        std::cout << "\n=== TIMING RESULTS ===" << std::endl;
        std::cout << "Parse time: " << duration.count() << " ms (" << duration.count() / 1000.0 << " seconds)" << std::endl;

        std::cout << "\n=== LIBRARY STATS ===" << std::endl;
        std::cout << "Number of cells: " << lib.cells.size() << std::endl;
        std::cout << "Number of normalized driver waveforms: " << lib.normalized_driver_waveforms.size() << std::endl;

        // Count total pins and timing arcs
        size_t total_pins = 0;
        size_t total_timing_arcs = 0;
        size_t total_ccsn_stages = 0;

        for (const auto& [cell_name, cell] : lib.cells) {
            total_pins += cell.cellpins.size();
            for (const auto& [pin_name, pin] : cell.cellpins) {
                total_timing_arcs += pin.timings.size();
                if (pin.ccsn_stages) {
                    total_ccsn_stages += pin.ccsn_stages->size();
                }
                for (const auto& timing : pin.timings) {
                    if (timing.ccsn_stages) {
                        total_ccsn_stages += timing.ccsn_stages->size();
                    }
                }
            }
        }

        std::cout << "Total pins: " << total_pins << std::endl;
        std::cout << "Total timing arcs: " << total_timing_arcs << std::endl;
        std::cout << "Total CCSN stages: " << total_ccsn_stages << std::endl;

        // Performance metrics
        double parse_rate_mb_per_sec = (1200.0 / (duration.count() / 1000.0)); // Assuming 1.2GB file
        std::cout << "Parse rate: " << parse_rate_mb_per_sec << " MB/s" << std::endl;

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}