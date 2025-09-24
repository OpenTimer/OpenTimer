/**
 * Quick test for CCS output_current parsing
 *
 * Usage: ./test_ccs_quick [library_file]
 *
 * If no file is provided, tests with test_ccs_minimal.lib
 */

#include <iostream>
#include <ot/liberty/celllib.hpp>

int main(int argc, char* argv[]) {
    std::string lib_path = (argc > 1) ? argv[1] : "test_ccs_minimal.lib";

    std::cout << "Testing CCS parser with: " << lib_path << std::endl;
    std::cout << std::string(60, '-') << std::endl;

    try {
        ot::Celllib lib;
        lib.read(lib_path);

        // Count output_current tables
        size_t total_rise = 0, total_fall = 0;
        size_t total_values = 0;
        size_t timing_arcs_with_oc = 0;

        for (const auto& [cell_name, cell] : lib.cells) {
            for (const auto& [pin_name, pin] : cell.cellpins) {
                for (const auto& timing : pin.timings) {
                    if (!timing.output_current_rise.empty() || !timing.output_current_fall.empty()) {
                        timing_arcs_with_oc++;
                        total_rise += timing.output_current_rise.size();
                        total_fall += timing.output_current_fall.size();

                        // Count values
                        for (const auto& lut : timing.output_current_rise) {
                            total_values += lut.table.size();
                        }
                        for (const auto& lut : timing.output_current_fall) {
                            total_values += lut.table.size();
                        }
                    }
                }
            }
        }

        std::cout << "✓ Library parsed successfully!" << std::endl;
        std::cout << "\nResults:" << std::endl;
        std::cout << "  Cells: " << lib.cells.size() << std::endl;
        std::cout << "  Timing arcs with output_current: " << timing_arcs_with_oc << std::endl;
        std::cout << "  Output_current_rise tables: " << total_rise << std::endl;
        std::cout << "  Output_current_fall tables: " << total_fall << std::endl;
        std::cout << "  Total tables: " << (total_rise + total_fall) << std::endl;
        std::cout << "  Total values parsed: " << total_values << std::endl;

        if (total_rise + total_fall > 0) {
            std::cout << "  Average values per table: " << (total_values / (total_rise + total_fall)) << std::endl;
        }

        // Show a sample table
        bool found_sample = false;
        for (const auto& [cell_name, cell] : lib.cells) {
            if (found_sample) break;
            for (const auto& [pin_name, pin] : cell.cellpins) {
                if (found_sample) break;
                for (const auto& timing : pin.timings) {
                    if (!timing.output_current_rise.empty()) {
                        const auto& lut = timing.output_current_rise[0];
                        std::cout << "\nSample table from " << cell_name << "/" << pin_name << ":" << std::endl;
                        std::cout << "  Values in table: " << lut.table.size() << std::endl;
                        if (lut.indices3.has_value()) {
                            std::cout << "  Index_3 size: " << lut.indices3->size() << std::endl;
                        }
                        std::cout << "  First 5 values: ";
                        for (size_t i = 0; i < std::min(size_t(5), lut.table.size()); i++) {
                            std::cout << lut.table[i] << " ";
                        }
                        std::cout << std::endl;
                        found_sample = true;
                        break;
                    }
                }
            }
        }

        // Success indicator
        if (total_values > total_rise + total_fall) {
            std::cout << "\n✓✓✓ SUCCESS: CCS parser is working correctly! ✓✓✓" << std::endl;
            std::cout << "    (Multiple values per table parsed successfully)" << std::endl;
        } else if (total_rise + total_fall > 0) {
            std::cout << "\n⚠ WARNING: Tables found but may have incomplete values" << std::endl;
        } else {
            std::cout << "\n✗ ERROR: No output_current tables found!" << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "✗ ERROR: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}