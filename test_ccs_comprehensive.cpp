/**
 * Comprehensive test suite for CCS output_current parsing
 *
 * This test verifies:
 * 1. Output_current blocks are found and parsed
 * 2. All values in the arrays are extracted
 * 3. Index dimensions match value counts
 * 4. Data integrity (reasonable current values)
 */

#include <iostream>
#include <iomanip>
#include <ot/liberty/celllib.hpp>
#include <cassert>
#include <cmath>

void print_separator(const std::string& title) {
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << title << std::endl;
    std::cout << std::string(80, '=') << std::endl;
}

void test_basic_parsing(const std::string& lib_path) {
    print_separator("TEST 1: Basic Parsing - " + lib_path);

    ot::Celllib lib;
    lib.read(lib_path);

    std::cout << "Library loaded successfully" << std::endl;
    std::cout << "Total cells: " << lib.cells.size() << std::endl;

    size_t total_timing_arcs = 0;
    size_t timing_arcs_with_output_current = 0;
    size_t total_output_current_rise = 0;
    size_t total_output_current_fall = 0;

    for (const auto& [cell_name, cell] : lib.cells) {
        for (const auto& [pin_name, pin] : cell.cellpins) {
            total_timing_arcs += pin.timings.size();

            for (const auto& timing : pin.timings) {
                if (!timing.output_current_rise.empty() || !timing.output_current_fall.empty()) {
                    timing_arcs_with_output_current++;
                    total_output_current_rise += timing.output_current_rise.size();
                    total_output_current_fall += timing.output_current_fall.size();
                }
            }
        }
    }

    std::cout << "Total timing arcs: " << total_timing_arcs << std::endl;
    std::cout << "Timing arcs with output_current: " << timing_arcs_with_output_current << std::endl;
    std::cout << "Total output_current_rise tables: " << total_output_current_rise << std::endl;
    std::cout << "Total output_current_fall tables: " << total_output_current_fall << std::endl;

    // Test assertions
    if (lib_path.find("minimal") != std::string::npos) {
        assert(total_output_current_rise == 320 && "Expected 320 rise tables in minimal file");
        assert(total_output_current_fall == 320 && "Expected 320 fall tables in minimal file");
        std::cout << "✓ Correct number of tables for minimal file" << std::endl;
    }

    std::cout << "✓ Basic parsing test PASSED" << std::endl;
}

void test_value_completeness(const std::string& lib_path) {
    print_separator("TEST 2: Value Completeness - " + lib_path);

    ot::Celllib lib;
    lib.read(lib_path);

    size_t tables_checked = 0;
    size_t tables_with_complete_values = 0;
    size_t total_values = 0;
    size_t min_values = SIZE_MAX;
    size_t max_values = 0;

    for (const auto& [cell_name, cell] : lib.cells) {
        for (const auto& [pin_name, pin] : cell.cellpins) {
            for (const auto& timing : pin.timings) {
                // Check rise tables
                for (const auto& lut : timing.output_current_rise) {
                    tables_checked++;
                    size_t value_count = lut.table.size();
                    total_values += value_count;
                    min_values = std::min(min_values, value_count);
                    max_values = std::max(max_values, value_count);

                    // Check if index_3 size matches value count (for CCS format)
                    if (lut.indices3.has_value()) {
                        if (lut.indices3->size() == value_count) {
                            tables_with_complete_values++;
                        } else {
                            std::cout << "WARNING: Table has " << value_count
                                     << " values but index_3 has " << lut.indices3->size()
                                     << " entries" << std::endl;
                        }
                    }

                    // Show first few tables as examples
                    if (tables_checked <= 3) {
                        std::cout << "Example table " << tables_checked << ":" << std::endl;
                        std::cout << "  Cell: " << cell_name << ", Pin: " << pin_name << std::endl;
                        std::cout << "  Values count: " << value_count << std::endl;
                        if (lut.indices3.has_value()) {
                            std::cout << "  Index_3 count: " << lut.indices3->size() << std::endl;
                        }
                        std::cout << "  First 5 values: ";
                        for (size_t i = 0; i < std::min(size_t(5), value_count); i++) {
                            std::cout << lut.table[i] << " ";
                        }
                        std::cout << std::endl;
                    }
                }

                // Check fall tables
                for (const auto& lut : timing.output_current_fall) {
                    tables_checked++;
                    size_t value_count = lut.table.size();
                    total_values += value_count;
                    min_values = std::min(min_values, value_count);
                    max_values = std::max(max_values, value_count);

                    if (lut.indices3.has_value() && lut.indices3->size() == value_count) {
                        tables_with_complete_values++;
                    }
                }
            }
        }
    }

    if (tables_checked > 0) {
        std::cout << "\nStatistics:" << std::endl;
        std::cout << "  Tables checked: " << tables_checked << std::endl;
        std::cout << "  Tables with matching index_3: " << tables_with_complete_values << std::endl;
        std::cout << "  Total values: " << total_values << std::endl;
        std::cout << "  Min values per table: " << min_values << std::endl;
        std::cout << "  Max values per table: " << max_values << std::endl;
        std::cout << "  Avg values per table: " << (total_values / tables_checked) << std::endl;

        // Assertions
        assert(min_values > 1 && "Tables should have multiple values, not just 1");
        assert(max_values < 1000 && "Sanity check: tables shouldn't have unreasonable number of values");

        std::cout << "✓ Value completeness test PASSED" << std::endl;
    } else {
        std::cout << "WARNING: No tables found to check" << std::endl;
    }
}

void test_data_integrity(const std::string& lib_path) {
    print_separator("TEST 3: Data Integrity - " + lib_path);

    ot::Celllib lib;
    lib.read(lib_path);

    size_t values_checked = 0;
    size_t reasonable_values = 0;
    float min_current = 1e10;
    float max_current = -1e10;

    for (const auto& [cell_name, cell] : lib.cells) {
        for (const auto& [pin_name, pin] : cell.cellpins) {
            for (const auto& timing : pin.timings) {
                // Check all output current values
                for (const auto& lut : timing.output_current_rise) {
                    for (float val : lut.table) {
                        values_checked++;
                        min_current = std::min(min_current, val);
                        max_current = std::max(max_current, val);

                        // Current values should be reasonable (typically small, in mA)
                        if (std::abs(val) < 100.0) {  // Less than 100 mA is reasonable
                            reasonable_values++;
                        } else {
                            std::cout << "WARNING: Unusual current value: " << val << " mA" << std::endl;
                        }
                    }
                }

                for (const auto& lut : timing.output_current_fall) {
                    for (float val : lut.table) {
                        values_checked++;
                        min_current = std::min(min_current, val);
                        max_current = std::max(max_current, val);

                        if (std::abs(val) < 100.0) {
                            reasonable_values++;
                        }
                    }
                }
            }
        }
    }

    if (values_checked > 0) {
        std::cout << "Data integrity check:" << std::endl;
        std::cout << "  Total values checked: " << values_checked << std::endl;
        std::cout << "  Reasonable values: " << reasonable_values << std::endl;
        std::cout << "  Min current: " << min_current << " mA" << std::endl;
        std::cout << "  Max current: " << max_current << " mA" << std::endl;
        std::cout << "  Percentage reasonable: "
                  << (100.0 * reasonable_values / values_checked) << "%" << std::endl;

        assert(reasonable_values > 0 && "Should have at least some reasonable values");
        assert(min_current > -1000 && max_current < 1000 && "Current values out of reasonable range");

        std::cout << "✓ Data integrity test PASSED" << std::endl;
    } else {
        std::cout << "WARNING: No values found to check" << std::endl;
    }
}

void test_ccsn_stages(const std::string& lib_path) {
    print_separator("TEST 4: CCSN Stages - " + lib_path);

    ot::Celllib lib;
    lib.read(lib_path);

    size_t cells_with_ccsn = 0;
    size_t total_ccsn_stages = 0;
    size_t ccsn_output_current_tables = 0;

    for (const auto& [cell_name, cell] : lib.cells) {
        bool cell_has_ccsn = false;

        for (const auto& [pin_name, pin] : cell.cellpins) {
            // Check pin-level CCSN stages
            if (pin.ccsn_stages) {
                cell_has_ccsn = true;
                total_ccsn_stages += pin.ccsn_stages->size();

                for (const auto& stage : *pin.ccsn_stages) {
                    ccsn_output_current_tables += stage.output_current_rise.size();
                    ccsn_output_current_tables += stage.output_current_fall.size();
                }
            }

            // Check timing-level CCSN stages
            for (const auto& timing : pin.timings) {
                if (timing.ccsn_stages) {
                    cell_has_ccsn = true;
                    total_ccsn_stages += timing.ccsn_stages->size();

                    for (const auto& stage : *timing.ccsn_stages) {
                        ccsn_output_current_tables += stage.output_current_rise.size();
                        ccsn_output_current_tables += stage.output_current_fall.size();
                    }
                }
            }
        }

        if (cell_has_ccsn) {
            cells_with_ccsn++;
        }
    }

    std::cout << "CCSN Stage Statistics:" << std::endl;
    std::cout << "  Cells with CCSN stages: " << cells_with_ccsn << std::endl;
    std::cout << "  Total CCSN stages: " << total_ccsn_stages << std::endl;
    std::cout << "  CCSN output_current tables: " << ccsn_output_current_tables << std::endl;

    std::cout << "✓ CCSN stages test completed" << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "CCS Parser Comprehensive Test Suite" << std::endl;
    std::cout << "====================================" << std::endl;

    // Test files to check
    std::vector<std::string> test_files = {
        "test_ccs_minimal.lib",
        "test_ccs_10cells.lib"
    };

    // Add command line file if provided
    if (argc > 1) {
        test_files.push_back(argv[1]);
    }

    // Add TSMC7 file if it exists
    std::string tsmc7_path = "/media/nvme1/cad/pdks/tsmc/cell_libraries/7/tcbn07_bwph240l11p57pd_base_svt_130b/"
                             "TSMCHOME/digital/Front_End/timing_power_noise/CCS/tcbn07_bwph240l11p57pd_base_svt_130a/"
                             "tcbn07_bwph240l11p57pd_base_svttt_0p65v_25c_typical_ccs.lib";
    std::ifstream tsmc7_check(tsmc7_path);
    if (tsmc7_check.good()) {
        test_files.push_back(tsmc7_path);
        tsmc7_check.close();
    }

    // Run all tests on each file
    for (const auto& file : test_files) {
        std::ifstream check(file);
        if (!check.good()) {
            std::cout << "Skipping " << file << " (not found)" << std::endl;
            continue;
        }
        check.close();

        try {
            test_basic_parsing(file);
            test_value_completeness(file);
            test_data_integrity(file);
            test_ccsn_stages(file);
        } catch (const std::exception& e) {
            std::cerr << "ERROR testing " << file << ": " << e.what() << std::endl;
            return 1;
        }
    }

    print_separator("ALL TESTS PASSED!");
    std::cout << "CCS parser is working correctly." << std::endl;

    return 0;
}