#include <iostream>
#include <ot/liberty/celllib.hpp>

void test_output_current_parsing(const std::string& lib_path, const std::string& lib_name) {
    std::cout << "\n=== Testing Output Current Parsing: " << lib_name << " ===" << std::endl;

    try {
        ot::Celllib lib;
        lib.read(lib_path);

        size_t total_output_current_fall = 0;
        size_t total_output_current_rise = 0;
        size_t cells_with_output_current = 0;
        size_t pins_with_output_current = 0;
        size_t timing_arcs_with_output_current = 0;

        for (const auto& [cell_name, cell] : lib.cells) {
            bool cell_has_output_current = false;

            for (const auto& [pin_name, pin] : cell.cellpins) {
                bool pin_has_output_current = false;

                // Check pin-level CCSN stages
                if (pin.ccsn_stages) {
                    for (const auto& stage : *pin.ccsn_stages) {
                        total_output_current_fall += stage.output_current_fall.size();
                        total_output_current_rise += stage.output_current_rise.size();

                        if (!stage.output_current_fall.empty() || !stage.output_current_rise.empty()) {
                            pin_has_output_current = true;
                            cell_has_output_current = true;
                        }
                    }
                }

                // Check timing arc-level output current (not in CCSN stages)
                for (const auto& timing : pin.timings) {
                    total_output_current_fall += timing.output_current_fall.size();
                    total_output_current_rise += timing.output_current_rise.size();

                    if (!timing.output_current_fall.empty() || !timing.output_current_rise.empty()) {
                        timing_arcs_with_output_current++;
                        pin_has_output_current = true;
                        cell_has_output_current = true;
                    }

                    // Also check CCSN stages for completeness
                    if (timing.ccsn_stages) {
                        for (const auto& stage : *timing.ccsn_stages) {
                            total_output_current_fall += stage.output_current_fall.size();
                            total_output_current_rise += stage.output_current_rise.size();

                            if (!stage.output_current_fall.empty() || !stage.output_current_rise.empty()) {
                                timing_arcs_with_output_current++;
                                pin_has_output_current = true;
                                cell_has_output_current = true;
                            }
                        }
                    }
                }

                if (pin_has_output_current) {
                    pins_with_output_current++;
                }
            }

            if (cell_has_output_current) {
                cells_with_output_current++;
            }
        }

        std::cout << "Total cells: " << lib.cells.size() << std::endl;
        std::cout << "Cells with output current: " << cells_with_output_current << std::endl;
        std::cout << "Pins with output current: " << pins_with_output_current << std::endl;
        std::cout << "Timing arcs with output current: " << timing_arcs_with_output_current << std::endl;
        std::cout << "Total output_current_fall tables: " << total_output_current_fall << std::endl;
        std::cout << "Total output_current_rise tables: " << total_output_current_rise << std::endl;

        // Print detailed info for first few cells with output current
        int detailed_count = 0;
        for (const auto& [cell_name, cell] : lib.cells) {
            if (detailed_count >= 3) break;

            bool found_output_current = false;
            for (const auto& [pin_name, pin] : cell.cellpins) {
                // Check timing level output current
                for (size_t t = 0; t < pin.timings.size(); t++) {
                    const auto& timing = pin.timings[t];
                    if (!timing.output_current_fall.empty() || !timing.output_current_rise.empty()) {
                        if (!found_output_current) {
                            std::cout << "\nCell: " << cell_name << std::endl;
                            found_output_current = true;
                            detailed_count++;
                        }
                        std::cout << "  Pin " << pin_name << ", Timing " << t << ":" << std::endl;
                        std::cout << "    output_current_fall tables: " << timing.output_current_fall.size() << std::endl;
                        std::cout << "    output_current_rise tables: " << timing.output_current_rise.size() << std::endl;

                        // Show first table details if available
                        if (!timing.output_current_fall.empty()) {
                            const auto& table = timing.output_current_fall[0];
                            std::cout << "    First fall table - values: " << table.table.size() << std::endl;
                            if (!table.table.empty()) {
                                std::cout << "      Sample values: ";
                                for (size_t j = 0; j < std::min(3ul, table.table.size()); j++) {
                                    std::cout << table.table[j] << " ";
                                }
                                std::cout << std::endl;
                            }
                        }
                    }
                }

                // Also check CCSN stages
                if (pin.ccsn_stages) {
                    for (size_t i = 0; i < pin.ccsn_stages->size(); i++) {
                        const auto& stage = (*pin.ccsn_stages)[i];
                        if (!stage.output_current_fall.empty() || !stage.output_current_rise.empty()) {
                            if (!found_output_current) {
                                std::cout << "\nCell: " << cell_name << std::endl;
                                found_output_current = true;
                                detailed_count++;
                            }
                            std::cout << "  Pin " << pin_name << ", CCSN Stage " << i << ":" << std::endl;
                            std::cout << "    output_current_fall tables: " << stage.output_current_fall.size() << std::endl;
                            std::cout << "    output_current_rise tables: " << stage.output_current_rise.size() << std::endl;
                        }
                    }
                }
            }
        }

        if (total_output_current_fall == 0 && total_output_current_rise == 0) {
            std::cout << "WARNING: No output current data found!" << std::endl;
        } else {
            std::cout << "SUCCESS: Output current data parsed correctly!" << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error testing " << lib_name << ": " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "Testing Output Current Field Parsing" << std::endl;

    // Test with minimal test file first
    test_output_current_parsing("test_ccs_minimal.lib", "Minimal CCS");

    // Test with larger files
    test_output_current_parsing("test_ccs_10cells.lib", "10 Cells CCS");

    // Test with TSMC7 file
    const std::string tsmc7_path = "/media/nvme1/cad/pdks/tsmc/cell_libraries/7/tcbn07_bwph240l11p57pd_base_svt_130b/TSMCHOME/digital/Front_End/timing_power_noise/CCS/tcbn07_bwph240l11p57pd_base_svt_130a/tcbn07_bwph240l11p57pd_base_svttt_0p65v_25c_typical_ccs.lib";
    test_output_current_parsing(tsmc7_path, "TSMC7 CCS");

    return 0;
}