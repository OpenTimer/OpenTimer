#include <ot/shell/shell.hpp>

namespace ot {

// welcome message
inline const std::string welcome = "\
  ____              _______              \n\
 / __ \\___  ___ ___/_  __(_)_ _  ___ ____\n\
/ /_/ / _ \\/ -_) _ \\/ / / /  ' \\/ -_) __/\n\
\\____/ .__/\\__/_//_/_/ /_/_/_/_/\\__/_/       v"s + OT_VERSION + "\n" + "\
    /_/                                     \n\
MIT License: type \"license\" to see more details.\n\
For help, type \"help\".\n\
For bug reports, issues, and manual, please see:\n\
<https://github.com/OpenTimer/OpenTimer>.\n";

// ------------------------------------------------------------------------------------------------

// Procedure: shell
int shell(int argc, char *argv[]) {

  ArgParse app {"ot-shell"};
  
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

  // ---------- Preparing shell run ----------

  // Create a timer
  ot::Timer timer;

  // Create a prompt object
  prompt::Prompt prompt(
    flag_quiet ? ""s : ot::welcome,
    "ot> ",
    ot::user_home() / ".ot_history"
  );

  // Add auto-complete
  for(const auto& kvp : ot::commands) {
    prompt.autocomplete(kvp.first);
  }

  for(const auto& kvp : ot::syscmds) {
    prompt.autocomplete(kvp.first);
  }

  prompt.autocomplete("quit");
  prompt.autocomplete("exit");

  // welcome message
  //OT_LOG_IF(!flag_quiet, ot::welcome);

  std::string line;

  bool quit {false};
  
  while(!quit && prompt.readline(line)) {

    // Remove the comment
    if(auto cpos = line.find('#'); cpos != std::string::npos) {
      line.erase(cpos);
    }

    std::istringstream iss(line);
    std::string op;
    iss >> op;
    
    // Nothing to do with empty line
    if(op.empty()) {
    }
    // leave
    else if(op == "quit" || op == "exit") {
      quit = true; 
    }
    // built-in command
    else if(auto itr = ot::commands.find(op); itr != ot::commands.end()) {
      std::invoke(itr->second, timer, iss, os, es);
    } 
    // system command
    else if(auto itr = ot::syscmds.find(op); itr != ot::syscmds.end()) {
      std::invoke(itr->second, line, iss, os, es);
    }
    else {
      es << "undefined command: " << std::quoted(line) << '\n';
    }

    // flush
    os.flush();
    es.flush();
  }
  
  return 0;
}

};  // end of namespace ot. -----------------------------------------------------------------------




