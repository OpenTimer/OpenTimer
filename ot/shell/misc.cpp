#include <ot/shell/shell.hpp>

namespace ot {

// Procedure: echo
void Shell::_echo() {
  if(::system(_line.data()) == -1) {
    _es << strerror(errno) << '\n';
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: which
void Shell::_which() {
  if(::system(_line.data()) == -1) {
    _es << strerror(errno) << '\n';
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: clear
void Shell::_clear() {
  if(::system(_line.data()) == -1) {
    _es << strerror(errno) << '\n';
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: tail
void Shell::_tail() {
  if(::system(_line.data()) == -1) {
    _es << strerror(errno) << '\n';
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: head
void Shell::_head() {
  if(::system(_line.data()) == -1) {
    _es << strerror(errno) << '\n';
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: cat
void Shell::_cat() {
  if(::system(_line.data()) == -1) {
    _es << strerror(errno) << '\n';
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: cd
void Shell::_cd() {

  size_t ptr = 0;
  for(; ptr + 1 < _line.size(); ptr++) {
    if(_line[ptr]=='c' && _line[ptr+1]=='d') {
      ptr = ptr + 2;
      break;
    }
  }

  while(ptr < _line.size() && _line[ptr] == ' ') {
    ptr++;
  }

  if(::chdir(_line.data()+ptr) == -1) {
    _es << strerror(errno) << ": " << _line.data()+ptr << '\n'; 
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: rm
void Shell::_rm() {
  if(::system(_line.data()) == -1) {
    _es << strerror(errno) << '\n';
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: pwd
void Shell::_pwd() {
  if(::system(_line.data()) == -1) {
    _es << strerror(errno) << '\n';
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: ls
void Shell::_ls() {
  if(::system(_line.data()) == -1) {
    _es << strerror(errno) << '\n';
  }
}



};  // end of namespace ot. -----------------------------------------------------------------------
