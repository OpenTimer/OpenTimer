#include <iostream>
#include <ot/liberty/celllib.hpp>

int main() {
    std::cout << "DEBUG: Starting test..." << std::endl;

    try {
        ot::Celllib lib;

        std::cout << "DEBUG: About to read library..." << std::endl;
        lib.read("test_ccs_minimal.lib");
        std::cout << "DEBUG: Library read complete!" << std::endl;

        std::cout << "DEBUG: Number of cells: " << lib.cells.size() << std::endl;

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "ERROR: Unknown exception!" << std::endl;
        return 1;
    }
}