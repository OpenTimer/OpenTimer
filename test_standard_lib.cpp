#include <iostream>
#include <ot/liberty/celllib.hpp>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <lib_file>" << std::endl;
        return 1;
    }

    try {
        ot::Celllib lib;
        lib.read(argv[1]);
        
        std::cout << "SUCCESS: Library parsed without errors" << std::endl;
        std::cout << "Cells: " << lib.cells.size() << std::endl;
        
        // Count timing arcs
        size_t total_timings = 0;
        for (const auto& [cell_name, cell] : lib.cells) {
            for (const auto& [pin_name, pin] : cell.cellpins) {
                total_timings += pin.timings.size();
            }
        }
        std::cout << "Total timing arcs: " << total_timings << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
