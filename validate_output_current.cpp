#include <iostream>
#include <chrono>
#include <ot/liberty/celllib.hpp>

void validate_output_current_parsing(const std::string& lib_path, const std::string& lib_name) {
    std::cout << "\n=== Validating Output Current Parsing: " << lib_name << " ===" << std::endl;

    try {
        auto start_time = std::chrono::high_resolution_clock::now();

        ot::Celllib lib;
        lib.read(lib_path);

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

        std::cout << "Parse time: " << duration.count() << " ms" << std::endl;
        std::cout << "Total cells: " << lib.cells.size() << std::endl;

        size_t total_output_current_fall = 0;
        size_t total_output_current_rise = 0;
        size_t timing_arcs_with_output_current = 0;
        size_t total_timing_arcs = 0;

        // Detailed validation
        for (const auto& [cell_name, cell] : lib.cells) {
            for (const auto& [pin_name, pin] : cell.cellpins) {
                for (const auto& timing : pin.timings) {
                    total_timing_arcs++;

                    total_output_current_fall += timing.output_current_fall.size();
                    total_output_current_rise += timing.output_current_rise.size();

                    if (!timing.output_current_fall.empty() || !timing.output_current_rise.empty()) {
                        timing_arcs_with_output_current++;
                    }
                }
            }
        }

        std::cout << "Total timing arcs: " << total_timing_arcs << std::endl;
        std::cout << "Timing arcs with output current: " << timing_arcs_with_output_current << std::endl;
        std::cout << "Total output_current_fall tables: " << total_output_current_fall << std::endl;
        std::cout << "Total output_current_rise tables: " << total_output_current_rise << std::endl;

        // Show percentage
        if (total_timing_arcs > 0) {
            double percentage = (double)timing_arcs_with_output_current / total_timing_arcs * 100.0;
            std::cout << "Percentage of timing arcs with output current: " << percentage << "%" << std::endl;
        }

        // Show sample data from first timing arc with output current
        bool found_sample = false;
        for (const auto& [cell_name, cell] : lib.cells) {
            if (found_sample) break;

            for (const auto& [pin_name, pin] : cell.cellpins) {
                if (found_sample) break;

                for (size_t i = 0; i < pin.timings.size(); i++) {
                    const auto& timing = pin.timings[i];

                    if (!timing.output_current_fall.empty() || !timing.output_current_rise.empty()) {
                        std::cout << "\nSample Output Current Data:" << std::endl;
                        std::cout << "Cell: " << cell_name << ", Pin: " << pin_name << ", Timing arc: " << i << std::endl;

                        if (!timing.output_current_fall.empty()) {
                            const auto& table = timing.output_current_fall[0];
                            std::cout << "  output_current_fall[0]:" << std::endl;
                            std::cout << "    Table name: " << table.name << std::endl;
                            std::cout << "    Table values: " << table.table.size() << std::endl;
                            std::cout << "    Indices1 size: " << table.indices1.size() << std::endl;
                            std::cout << "    Indices2 size: " << table.indices2.size() << std::endl;

                            // Show first few values
                            if (!table.table.empty()) {
                                std::cout << "    First few values: ";
                                for (size_t j = 0; j < std::min(5ul, table.table.size()); j++) {
                                    std::cout << table.table[j] << " ";
                                }
                                std::cout << std::endl;
                            }

                            // Show indices
                            if (!table.indices1.empty()) {
                                std::cout << "    Indices1: ";
                                for (size_t j = 0; j < std::min(3ul, table.indices1.size()); j++) {
                                    std::cout << table.indices1[j] << " ";
                                }
                                std::cout << std::endl;
                            }
                        }

                        if (!timing.output_current_rise.empty()) {
                            const auto& table = timing.output_current_rise[0];
                            std::cout << "  output_current_rise[0]:" << std::endl;
                            std::cout << "    Table name: " << table.name << std::endl;
                            std::cout << "    Table values: " << table.table.size() << std::endl;
                            std::cout << "    Indices1 size: " << table.indices1.size() << std::endl;
                            std::cout << "    Indices2 size: " << table.indices2.size() << std::endl;
                        }

                        found_sample = true;
                        break;
                    }
                }
            }
        }

        // Validation results
        if (total_output_current_fall == 0 && total_output_current_rise == 0) {
            std::cout << "\n❌ VALIDATION FAILED: No output current data found!" << std::endl;
            std::cout << "   This suggests the parsing is not working correctly." << std::endl;
        } else {
            std::cout << "\n✅ VALIDATION PASSED: Output current data found and parsed!" << std::endl;
            std::cout << "   Found " << (total_output_current_fall + total_output_current_rise) << " output current tables total." << std::endl;

            // Additional validation checks
            bool has_valid_data = false;
            for (const auto& [cell_name, cell] : lib.cells) {
                for (const auto& [pin_name, pin] : cell.cellpins) {
                    for (const auto& timing : pin.timings) {
                        for (const auto& table : timing.output_current_fall) {
                            if (!table.table.empty() && !table.indices1.empty()) {
                                has_valid_data = true;
                                break;
                            }
                        }
                        for (const auto& table : timing.output_current_rise) {
                            if (!table.table.empty() && !table.indices1.empty()) {
                                has_valid_data = true;
                                break;
                            }
                        }
                        if (has_valid_data) break;
                    }
                    if (has_valid_data) break;
                }
                if (has_valid_data) break;
            }

            if (has_valid_data) {
                std::cout << "   ✅ Tables contain valid data (non-empty values and indices)" << std::endl;
            } else {
                std::cout << "   ⚠️  WARNING: Tables found but appear to be empty" << std::endl;
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "❌ ERROR: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "🧪 Validating Output Current Field Parsing Implementation" << std::endl;
    std::cout << "=========================================================" << std::endl;

    // Test 1: Minimal CCS file
    validate_output_current_parsing("test_ccs_minimal.lib", "Minimal CCS");

    // Test 2: 10 cells file
    validate_output_current_parsing("test_ccs_10cells.lib", "10 Cells CCS");

    // Test 3: TSMC7 file (if accessible)
    const std::string tsmc7_path = "/media/nvme1/cad/pdks/tsmc/cell_libraries/7/tcbn07_bwph240l11p57pd_base_svt_130b/TSMCHOME/digital/Front_End/timing_power_noise/CCS/tcbn07_bwph240l11p57pd_base_svt_130a/tcbn07_bwph240l11p57pd_base_svttt_0p65v_25c_typical_ccs.lib";
    validate_output_current_parsing(tsmc7_path, "TSMC7 CCS (1.2GB)");

    std::cout << "\n🎯 Validation Complete!" << std::endl;
    return 0;
}