#include <ot/shell/shell.hpp>

int main(int argc, char* argv[]) {
  
  ot::ArgParse app {"ot-shell"};
  
  auto flag_quiet {false};
  auto flag_version {false};

  std::filesystem::path out_path;
  std::filesystem::path err_path;
  std::filesystem::path log_path;

  app.add_flag  ("-q,--quiet",   flag_quiet,   "Do not print the welcome on startup");
  app.add_flag  ("-v,--version", flag_version, "Print version information and exit");
  app.add_option("-o,--ostream", out_path,     "Redirect output stream to a file");
  app.add_option("-e,--estream", err_path,     "Redirect error stream to a file");
  app.add_option("--log",        log_path,     "Redirect logging to a file");

  try {
    app.parse(argc, argv);
  }
  catch (const CLI::ParseError& e) {
    return app.exit(e);
  }

  // Set up the stdout and stderr
  std::ofstream ofs(out_path);
  std::ofstream efs(err_path);

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
  ot::Shell(flag_quiet ? "" : ot::welcome, os, es)();

  return 0;
}


