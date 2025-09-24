#include <iostream>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <signal.h>
#include <sys/resource.h>
#include <ot/liberty/celllib.hpp>

// Global flag for timeout handling
volatile sig_atomic_t timeout_flag = 0;

void timeout_handler(int signum) {
    timeout_flag = 1;
    std::cout << "\n⏰ TIMEOUT: Test exceeded time limit!" << std::endl;
}

// Memory usage monitoring
size_t get_memory_usage_mb() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss / 1024; // Convert KB to MB on Linux
}

class ProgressMonitor {
private:
    std::chrono::steady_clock::time_point start_time;
    size_t initial_memory;

public:
    ProgressMonitor() : start_time(std::chrono::steady_clock::now()), initial_memory(get_memory_usage_mb()) {}

    void report(const std::string& phase) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count();
        size_t current_memory = get_memory_usage_mb();

        std::cout << "📊 [" << elapsed << "s] " << phase
                  << " | Memory: " << current_memory << " MB (+";
        if (current_memory >= initial_memory) {
            std::cout << (current_memory - initial_memory) << " MB)" << std::endl;
        } else {
            std::cout << "0 MB)" << std::endl;
        }

        if (timeout_flag) {
            throw std::runtime_error("Test timed out");
        }
    }
};

int main(int argc, char* argv[]) {
    // Default to full TSMC file
    std::string filename = "/media/nvme1/cad/pdks/tsmc/cell_libraries/7/tcbn07_bwph240l11p57pd_base_svt_130b/TSMCHOME/digital/Front_End/timing_power_noise/CCS/tcbn07_bwph240l11p57pd_base_svt_130a/tcbn07_bwph240l11p57pd_base_svttt_0p65v_25c_typical_ccs.lib";

    // Allow override via command line
    if (argc > 1) {
        filename = argv[1];
    }

    // Set up timeout (10 minutes)
    signal(SIGALRM, timeout_handler);
    alarm(600);

    ProgressMonitor monitor;

    try {
        std::cout << "🚀 Starting CCS Parser Scale Test" << std::endl;
        std::cout << "File: " << filename << std::endl;

        // Check file exists and get size
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filename);
        }

        size_t file_size_mb = file.tellg() / (1024 * 1024);
        file.close();

        std::cout << "📁 File size: " << file_size_mb << " MB" << std::endl;
        monitor.report("File size check completed");

        // Parse the library
        std::cout << "⚡ Starting library parsing..." << std::endl;
        ot::Celllib lib;

        monitor.report("Parser initialized");

        // This is where the heavy lifting happens
        lib.read(filename);

        monitor.report("Library parsing completed");

        // Collect comprehensive statistics
        std::cout << "\n=== CCS PARSER SCALE TEST RESULTS ===" << std::endl;
        std::cout << "✅ Parsing Status: SUCCESS" << std::endl;
        std::cout << "📊 Library: " << lib.name << std::endl;
        std::cout << "📊 Total cells: " << lib.cells.size() << std::endl;
        std::cout << "📊 LUT templates: " << lib.lut_templates.size() << std::endl;
        std::cout << "📊 Wireloads: " << lib.wireloads.size() << std::endl;
        std::cout << "📊 Normalized driver waveforms: " << lib.normalized_driver_waveforms.size() << std::endl;

        // Count CCS-specific templates
        int ccs_templates = 0;
        for (const auto& [name, tmpl] : lib.lut_templates) {
            if (name.find("ccsn") != std::string::npos || name.find("ccs") != std::string::npos) {
                ccs_templates++;
            }
        }
        std::cout << "📊 CCS-specific templates: " << ccs_templates << std::endl;

        // Comprehensive CCSN analysis
        int total_stages = 0;
        int total_dc_current = 0;
        int total_voltage_fall = 0;
        int total_voltage_rise = 0;
        int total_current_fall = 0;
        int total_current_rise = 0;
        int total_noise_high = 0;
        int total_noise_low = 0;
        int cells_with_ccsn = 0;
        int pins_with_ccsn = 0;
        int timing_arcs_with_ccsn = 0;

        for (const auto& [cell_name, cell] : lib.cells) {
            bool cell_has_ccsn = false;

            for (const auto& [pin_name, pin] : cell.cellpins) {
                bool pin_has_ccsn = false;

                if (pin.ccsn_stages) {
                    pin_has_ccsn = true;
                    cell_has_ccsn = true;
                    total_stages += pin.ccsn_stages->size();

                    for (const auto& stage : *pin.ccsn_stages) {
                        total_dc_current += stage.dc_current.size();
                        total_voltage_fall += stage.output_voltage_fall.size();
                        total_voltage_rise += stage.output_voltage_rise.size();
                        total_current_fall += stage.output_current_fall.size();
                        total_current_rise += stage.output_current_rise.size();
                        total_noise_high += stage.propagated_noise_high.size();
                        total_noise_low += stage.propagated_noise_low.size();
                    }
                }

                // Check timing arcs for CCSN stages
                for (const auto& timing : pin.timings) {
                    if (timing.ccsn_stages) {
                        if (!pin_has_ccsn) {
                            pin_has_ccsn = true;
                            cell_has_ccsn = true;
                        }
                        timing_arcs_with_ccsn++;
                        total_stages += timing.ccsn_stages->size();

                        for (const auto& stage : *timing.ccsn_stages) {
                            total_dc_current += stage.dc_current.size();
                            total_voltage_fall += stage.output_voltage_fall.size();
                            total_voltage_rise += stage.output_voltage_rise.size();
                            total_current_fall += stage.output_current_fall.size();
                            total_current_rise += stage.output_current_rise.size();
                            total_noise_high += stage.propagated_noise_high.size();
                            total_noise_low += stage.propagated_noise_low.size();
                        }
                    }
                }

                if (pin_has_ccsn) pins_with_ccsn++;
            }

            if (cell_has_ccsn) cells_with_ccsn++;
        }

        int total_vector_tables = total_voltage_fall + total_voltage_rise + total_current_fall +
                                 total_current_rise + total_noise_high + total_noise_low;

        std::cout << "\n--- CCSN STATISTICS ---" << std::endl;
        std::cout << "📊 Cells with CCSN: " << cells_with_ccsn << "/" << lib.cells.size() << std::endl;
        std::cout << "📊 Pins with CCSN: " << pins_with_ccsn << std::endl;
        std::cout << "📊 Timing arcs with CCSN: " << timing_arcs_with_ccsn << std::endl;
        std::cout << "📊 Total CCSN stages: " << total_stages << std::endl;
        std::cout << "📊 DC current tables: " << total_dc_current << std::endl;
        std::cout << "📊 Voltage fall tables: " << total_voltage_fall << std::endl;
        std::cout << "📊 Voltage rise tables: " << total_voltage_rise << std::endl;
        std::cout << "📊 Current fall tables: " << total_current_fall << std::endl;
        std::cout << "📊 Current rise tables: " << total_current_rise << std::endl;
        std::cout << "📊 Propagated noise high: " << total_noise_high << std::endl;
        std::cout << "📊 Propagated noise low: " << total_noise_low << std::endl;
        std::cout << "📊 TOTAL VECTOR TABLES: " << total_vector_tables << std::endl;

        // Performance summary
        monitor.report("Final statistics completed");

        auto end_time = std::chrono::steady_clock::now();
        auto total_elapsed = std::chrono::duration_cast<std::chrono::seconds>(end_time - std::chrono::steady_clock::now()).count();
        size_t final_memory = get_memory_usage_mb();

        std::cout << "\n--- PERFORMANCE SUMMARY ---" << std::endl;
        std::cout << "⏱️  Parse time: " << total_elapsed << " seconds" << std::endl;
        std::cout << "🧠 Peak memory: " << final_memory << " MB" << std::endl;
        std::cout << "⚡ Throughput: " << (file_size_mb * 1.0 / (total_elapsed > 0 ? total_elapsed : 1)) << " MB/sec" << std::endl;
        std::cout << "📈 Vector tables/sec: " << (total_vector_tables * 1.0 / (total_elapsed > 0 ? total_elapsed : 1)) << std::endl;

        // Expected vs actual validation
        std::cout << "\n--- VALIDATION ---" << std::endl;
        std::cout << "Expected ~1154 cells: " << (lib.cells.size() > 1000 ? "✅ PASS" : "❌ FAIL") << std::endl;
        std::cout << "Expected ~18847 stages: " << (total_stages > 15000 ? "✅ PASS" : "❌ FAIL") << std::endl;
        std::cout << "Expected ~358216 vectors: " << (total_vector_tables > 300000 ? "✅ PASS" : "❌ FAIL") << std::endl;

        alarm(0); // Cancel timeout
        return 0;

    } catch (const std::exception& e) {
        std::cout << "❌ Error: " << e.what() << std::endl;
        monitor.report("FAILED");
        alarm(0);
        return 1;
    }
}