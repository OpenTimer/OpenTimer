#include <iostream>
#include <ot/liberty/celllib.hpp>

int main() {
    std::cout << "Testing simple timing parsing..." << std::endl;

    try {
        ot::Celllib lib;
        lib.read("test_ccs_minimal.lib");

        std::cout << "Library loaded successfully!" << std::endl;
        std::cout << "Number of cells: " << lib.cells.size() << std::endl;

        for (const auto& [cell_name, cell] : lib.cells) {
            std::cout << "Cell: " << cell_name << std::endl;

            for (const auto& [pin_name, pin] : cell.cellpins) {
                std::cout << "  Pin: " << pin_name << " has " << pin.timings.size() << " timing arcs" << std::endl;

                for (size_t i = 0; i < pin.timings.size(); i++) {
                    const auto& timing = pin.timings[i];
                    std::cout << "    Timing " << i << ": " << std::endl;
                    std::cout << "      output_current_fall size: " << timing.output_current_fall.size() << std::endl;
                    std::cout << "      output_current_rise size: " << timing.output_current_rise.size() << std::endl;
                }
            }
            break; // Just test first cell
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}