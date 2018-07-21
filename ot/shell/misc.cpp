#include <ot/shell/shell.hpp>

namespace ot {

// Procedure: echo
void echo(std::string_view cmd, std::istream& is, std::ostream& os, std::ostream& es) {
  if(::system(cmd.data()) == -1) {
    es << strerror(errno) << '\n';
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: which
void which(std::string_view cmd, std::istream& is, std::ostream& os, std::ostream& es) {
  if(::system(cmd.data()) == -1) {
    es << strerror(errno) << '\n';
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: clear
void clear(std::string_view cmd, std::istream& is, std::ostream& os, std::ostream& es) {
  if(::system(cmd.data()) == -1) {
    es << strerror(errno) << '\n';
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: tail
void tail(std::string_view cmd, std::istream& is, std::ostream& os, std::ostream& es) {
  if(::system(cmd.data()) == -1) {
    es << strerror(errno) << '\n';
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: head
void head(std::string_view cmd, std::istream& is, std::ostream& os, std::ostream& es) {
  if(::system(cmd.data()) == -1) {
    es << strerror(errno) << '\n';
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: cat
void cat(std::string_view cmd, std::istream& is, std::ostream& os, std::ostream& es) {
  if(::system(cmd.data()) == -1) {
    es << strerror(errno) << '\n';
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: cd
void cd(std::string_view cmd, std::istream& is, std::ostream& os, std::ostream& es) {

  size_t ptr = 0;
  for(; ptr + 1 < cmd.size(); ptr++) {
    if(cmd[ptr]=='c' && cmd[ptr+1]=='d') {
      ptr = ptr + 2;
      break;
    }
  }

  while(ptr < cmd.size() && cmd[ptr] == ' ') {
    ptr++;
  }

  if(::chdir(cmd.data()+ptr) == -1) {
    es << strerror(errno) << ": " << cmd.data()+ptr << '\n'; 
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: rm
void rm(std::string_view cmd, std::istream& is, std::ostream& os, std::ostream& es) {
  if(::system(cmd.data()) == -1) {
    es << strerror(errno) << '\n';
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: pwd
void pwd(std::string_view cmd, std::istream& is, std::ostream& os, std::ostream& es) {
  if(::system(cmd.data()) == -1) {
    es << strerror(errno) << '\n';
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: ls
void ls(std::string_view cmd, std::istream& is, std::ostream& os, std::ostream& es) {
  if(::system(cmd.data()) == -1) {
    es << strerror(errno) << '\n';
  }
}



};  // end of namespace ot. -----------------------------------------------------------------------
