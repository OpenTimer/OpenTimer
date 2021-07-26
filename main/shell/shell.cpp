#include <ot/shell/shell.hpp>

int main(int argc, char* argv[]) {
  
  // ==========================================================================
  // Argument Definitions
  // ==========================================================================
  
  ot::ArgParse app {"OpenTimer Shell"};
  
  auto flag_quiet {false};
  auto flag_version {false};

  std::filesystem::path stdin_path;
  std::filesystem::path stdout_path;
  std::filesystem::path stderr_path;
  std::filesystem::path log_path;

  app.add_flag  ("-q,--quiet",   flag_quiet,   "do not print the welcome on startup");
  app.add_flag  ("-v,--version", flag_version, "print version information and exit");
  app.add_option("-i,--stdin",   stdin_path,   "redirect stdin to a file");
  app.add_option("-o,--stdout",  stdout_path,  "redirect stdout to a file");
  app.add_option("-e,--stderr",  stderr_path,  "redirect stderr to a file");
  app.add_option("--log",        log_path,     "redirect logging to a file");

  try {
    app.parse(argc, argv);
  }
  catch (const CLI::ParseError& e) {
    return app.exit(e);
  }

  // ==========================================================================
  // OpenTimer Shell
  // ==========================================================================

  // set up the stdin
  FILE* is = ::fopen(stdin_path.c_str(), "r");
  if(is == nullptr) {
    is = stdin;
  }

  // set up the stdout and stderr
  std::ofstream ofs(stdout_path);
  std::ofstream efs(stderr_path);
  
  std::ostream& os = ofs.good() ? ofs : std::cout;
  std::ostream& es = efs.good() ? efs : std::cerr;
  
  // Set the logging
  OT_LOGTO(log_path);

  // version
  if(flag_version) {
    os << OT_VERSION << '\n';
    return 0;
  }

  // Launch the OpenTimer shell
  ot::Shell(flag_quiet ? "" : ot::welcome, is, os, es)();

  // close the input stream
  if(is != stdin) {
    ::fclose(is);
  }

  return 0;
}


