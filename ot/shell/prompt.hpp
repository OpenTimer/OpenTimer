//  MIT License
//  
//  Copyright (c) 2018 Chun-Xun Lin, Tsung-Wei Huang and Martin D. F. Wong
//  
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//  
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//  
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  SOFTWARE.

#ifndef PROMPT_HPP_
#define PROMPT_HPP_

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <pwd.h>
#include <errno.h>
#include <unistd.h>
#include <algorithm>
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <memory>
#include <list>
#include <vector>
#include <cstring>
#include <fstream>
#include <string_view>
#include <experimental/filesystem>
#include <cassert>


namespace std {

namespace filesystem = experimental::filesystem;

};

// ------------------------------------------------------------------------------------------------

namespace prompt {

// Procedure: read_line
// Read one line from an input stream
template <typename C, typename T, typename A>
std::basic_istream<C, T>& read_line(
  std::basic_istream<C, T>& is, 
  std::basic_string<C, T, A>& line
) {

  line.clear();

  typename std::basic_istream<C, T>::sentry se(is, true);        
  std::streambuf* sb = is.rdbuf();

  for(;;) {
    switch (int c = sb->sbumpc(); c) {

      // case 1: newline
      case '\n':
        return is;
      break;

      // case 2: carriage return
      case '\r':
        if(sb->sgetc() == '\n'){
          sb->sbumpc();
        }
        return is;
      break;
      
      // case 3: eof
      case std::streambuf::traits_type::eof():
        // Also handle the case when the last line has no line ending
        if(line.empty()) {
          is.setstate(std::ios::eofbit | std::ios::failbit);
        }
        return is;
      break;

      default:
        line.push_back(static_cast<char>(c));
      break;
    }
  } 

  return is;
}

// ------------------------------------------------------------------------------------------------

// Function: count_prefix  
// Count the the length of same prefix between two strings
template <typename C>
constexpr size_t count_prefix(
  std::basic_string_view<typename C::value_type, typename C::traits_type> s1, 
  std::basic_string_view<typename C::value_type, typename C::traits_type> s2){
  size_t i {0};
  size_t len {std::min(s1.size(), s2.size())};
  for(; i<len; ++i){
    if(s1[i] != s2[i]){
      break;
    }
  }
  return i;
}

// ------------------------------------------------------------------------------------------------

// Class: RadixTree 
template <typename C>
class RadixTree{

  using value_type     = typename C::value_type;
  using traits_type    = typename C::traits_type;
  using allocator_type = typename C::allocator_type;

  public:
  
    struct Node {
      bool is_word {false};
      std::list<std::pair<C, std::unique_ptr<Node>>> children;
    };

   RadixTree() = default;
   RadixTree(const std::vector<C>&);
   
   bool exist(std::basic_string_view<value_type, traits_type>) const;
   void insert(const C&);
  
   std::vector<C> match_prefix(const C&) const;
   std::vector<C> all_words() const;
   C dump() const;

   const Node& root() const;

  private:

   Node _root;

   void _insert(std::basic_string_view<value_type, traits_type>, Node&);
   void _match_prefix(std::vector<C>&, const Node&, const C&) const;
   void _dump(const Node&, size_t, C&) const;
  
   std::pair<const Node*, C> _search_prefix_node(
     std::basic_string_view<value_type, traits_type>
   ) const;
};


// Procedure: Ctor 
template <typename C>
RadixTree<C>::RadixTree(const std::vector<C>& words){
  for(const auto& w: words){
    insert(w);
  }
}


// Procedure: dump 
// Dump the radix tree into a string 
template <typename C>
C RadixTree<C>::dump() const {
  C t;
  _dump(_root, 0, t);      
  t.append(1, '\n');
  return t;
}

// Procedure: _dump 
// Recursively traverse the tree and append each level to string 
template <typename C>
void RadixTree<C>::_dump(const Node& n, size_t level, C& s) const {
  for(auto &[k,v]: n.children){
    s.append(level, '-');
    s.append(1, ' ');
    s += k;
    s.append(1, '\n');
    _dump(*v, level+1, s);
  }
}

// Procedure: _match_prefix 
// Find all words that match the given prefix 
template <typename C>
void RadixTree<C>::_match_prefix(
  std::vector<C> &vec, 
  const Node& n, 
  const C& s
) const {
  if(n.is_word){
    vec.emplace_back(s);
  }
  for(auto& [k,v]:n.children){
    _match_prefix(vec, *v, s+k);
  }
}

// Procedure: all_words 
// Extract all words in the radix tree 
template <typename C>
std::vector<C> RadixTree<C>::all_words() const {
  std::vector<C> words;
  for(auto &[k, v]: _root.children){
    _match_prefix(words, *v, k);
  }
  return words;
}

// Procedure: match_prefix 
// Collect all words that match the given prefix 
template <typename C>
std::vector<C> RadixTree<C>::match_prefix(const C& prefix) const {
  if(auto [prefix_node, suffix] = _search_prefix_node(prefix); prefix_node == nullptr){
    return {};
  }
  else{
    std::vector<C> matches;
    if(prefix_node->is_word){
      matches.emplace_back(prefix + suffix);
    }
    for(auto &[k, v]: prefix_node->children){
      _match_prefix(matches, *v, prefix+suffix+k);
    }
    return matches;
  }
}

// Procedure: _search_prefix_node 
// Find the node that matches the given prefix 
template <typename C>
std::pair<const typename RadixTree<C>::Node*, C> RadixTree<C>::_search_prefix_node(
  std::basic_string_view<value_type, traits_type> s
) const {

  Node const *n = &_root;
  std::basic_string<value_type, traits_type> suffix;
  for(size_t pos=0; pos<s.size(); ){ // Search until full match
    bool match = {false};
    for(const auto& [k, v]: n->children){
      if(auto num = count_prefix<C>(k, s.data()+pos); num>0){
        pos += num;
        if(pos == s.size()){
          suffix = k.substr(num, k.size()-num);
        }
        n = v.get();
        match = true;
        break;
      }
    }
    if(not match){
      return {nullptr, suffix};
    }
  }
  return {n, suffix};
}

// Procedure: exist 
// Check whether the given word is in the radix tree or not 
template <typename C>
bool RadixTree<C>::exist(std::basic_string_view<value_type, traits_type> s) const {

  size_t pos {0};
  Node const *n = &_root;
  while(not n->children.empty()){ // Search until reaching the leaf
    bool match {false};
    for(const auto& [k, v]: n->children){
      auto match_num = count_prefix<C>(k, s.data()+pos);
      if(match_num > 0){
        if(pos += match_num; pos == s.size()){
          if(match_num == k.size() and v->is_word){
            return true;
          }
        }
        else{
          n = v.get();
          match = true;
        }
        break;
      }
    }
    if(not match){
      return false;
    }
  }
  return false;
}

// Procedure: insert 
// Insert a word into radix tree 
template <typename C>
void RadixTree<C>::insert(const C& s){
  if(s.empty()){  // Empty string not allowed
    return;
  }
  _insert(s, _root);
}

// Procedure: _insert 
// Insert a word into radix tree 
template <typename C>
void RadixTree<C>::_insert(std::basic_string_view<value_type, traits_type> sv, Node& n){

  // base case
  if(sv.empty()) {
    n.is_word = true;
    return;
  }

  size_t match_num {0};

  auto itr = std::find_if(n.children.begin(), n.children.end(), [&] (const auto& kv) {
    if(match_num = count_prefix<C>(std::get<0>(kv), sv); match_num > 0) {
      return true;
    }
    else return false;
  });

  if(match_num == 0){   // Base case 1 
    assert(itr == n.children.end());
    auto& child = std::get<1>(n.children.emplace_back(std::make_pair(sv, new Node))); 
    child->is_word = true;
  }
  else {
    
    if(match_num == itr->first.size()) {
      _insert(sv.data() + match_num, *itr->second);
    }
    else {
    
      auto& par = std::get<1>(n.children.emplace_back(
        std::make_pair(
          std::basic_string_view<value_type, traits_type>{sv.data(), match_num}, new Node))
      );

      par->children.emplace_back(
        std::make_pair(itr->first.data()+match_num, std::move(itr->second))
      );

      n.children.erase(itr);
      
      _insert(sv.data() + match_num, *par);
    }
  }
}


// Procedure: root
// Return the root of RadixTree 
template <typename C>
const typename RadixTree<C>::Node& RadixTree<C>::root() const{
  return _root;
}

// ------------------------------------------------------------------------------------------------


// http://www.physics.udel.edu/~watson/scen103/ascii.html
enum class KEY{
  KEY_NULL = 0,       /* NULL      */
  CTRL_A   = 1,       /* Ctrl+a    */
  CTRL_B   = 2,       /* Ctrl-b    */
  CTRL_C   = 3,       /* Ctrl-c    */
  CTRL_D   = 4,       /* Ctrl-d    */
  CTRL_E   = 5,       /* Ctrl-e    */
  CTRL_F   = 6,       /* Ctrl-f    */
  CTRL_H   = 8,       /* Ctrl-h    */
  TAB      = 9,       /* Tab       */
  CTRL_K   = 11,      /* Ctrl+k    */
  CTRL_L   = 12,      /* Ctrl+l    */
  ENTER    = 13,      /* Enter     */
  CTRL_N   = 14,      /* Ctrl-n    */
  CTRL_P   = 16,      /* Ctrl-p    */
  CTRL_T   = 20,      /* Ctrl-t    */
  CTRL_U   = 21,      /* Ctrl+u    */
  CTRL_W   = 23,      /* Ctrl+w    */
  ESC      = 27,      /* Escape    */
  BACKSPACE =  127    /* Backspace */
};

enum class COLOR{
  BLACK = 30,
  RED,
  GREEN,
  YELLOW,
  BLUE,
  MAGNETA,
  CYAN,
  WHITE
};

class Prompt {

  struct LineInfo{
    
    std::string buf;
    int history_trace {0};
    size_t cur_pos {0};   // cursor position

    void operator = (const LineInfo&);
    inline void reset(){ 
      cur_pos = history_trace = 0; 
      buf.clear();
    }
  };

  public:

    Prompt(
      const std::string&,   // Welcome message 
      const std::string&,   // prompt
      const std::filesystem::path& = std::filesystem::current_path()/".prompt_history",  // history log path
      std::istream& = std::cin, 
      std::ostream& = std::cout, 
      std::ostream& = std::cerr,
      int = STDIN_FILENO
    );

    ~Prompt();

    bool readline(std::string&);

    void set_history_size(size_t);
    size_t history_size() const { return _history.size(); };
    
    void autocomplete(const std::string&);

  private: 
  
    std::string _prompt;  
    std::filesystem::path _history_path;
    std::istream& _cin;
    std::ostream& _cout;
    std::ostream& _cerr;

    
    int _infd;
    size_t _columns {80};   // default width of terminal is 80
    
    RadixTree<std::string> _tree;  // Radix tree for command autocomplete
  
    std::string _obuf;      // Buffer for _refresh_single_line

    bool _unsupported_term();
    void _stdin_not_tty(std::string &);
    bool _set_raw_mode();
    void _disable_raw_mode();

    // History  
    size_t _max_history_size {100};
    std::list<std::string> _history;
    void _add_history(const std::string&);
    void _save_history();
    void _load_history();

    termios _orig_termios;
    bool _has_orig_termios {false};
    bool _save_orig_termios(int);

    int _get_cursor_pos();
    void _clear_screen();
    size_t _terminal_columns();

    void _edit_line(std::string&);

    void _refresh_single_line(LineInfo&);

    LineInfo _line;
    LineInfo _line_save;

    int _autocomplete_iterate_command();
    void _autocomplete_command();
    void _autocomplete_folder();

    std::vector<std::string> _files_in_folder(const std::filesystem::path&) const;
    std::vector<std::string> _files_match_prefix(const std::filesystem::path&) const;
    std::string _dump_files(const std::vector<std::string>&, const std::filesystem::path&);
    std::string _dump_options(const std::vector<std::string>&);
    std::string _next_prefix(const std::vector<std::string>&, const size_t);


    std::filesystem::path _user_home() const;
    bool _has_read_access(const std::filesystem::path&) const;

    // Key handling subroutine
    void _key_backspace(LineInfo&);
    void _key_delete(LineInfo&);
    void _key_delete_prev_word(LineInfo&);

    void _key_prev_history(LineInfo&);
    void _key_next_history(LineInfo&);
    void _key_history(LineInfo&, bool);
    bool _key_handle_CSI(LineInfo&);

    bool _append_character(LineInfo&, char);
};


// Copy assignment
inline void Prompt::LineInfo::operator = (const LineInfo& l){
  buf = l.buf;
  cur_pos = l.cur_pos;
  history_trace = l.history_trace;   
}

// Procedure: Ctor
inline Prompt::Prompt(
  const std::string& welcome_msg, 
  const std::string& pmt, 
  const std::filesystem::path& path,
  std::istream& in, 
  std::ostream& out, 
  std::ostream& err,
  int infd
):
  _prompt(pmt), 
  _history_path(path),
  _cin(in),
  _cout(out),
  _cerr(err),
  _infd(infd)
{
  if(::isatty(_infd)){
    _cout << welcome_msg;
    _columns = _terminal_columns();
    if(std::filesystem::exists(_history_path)){
      if(std::error_code ec; not std::filesystem::is_regular_file(_history_path, ec)){
        _cerr << "The history file is not a regular file\n";
      }
      else{
        _load_history();
      }
    }
  }
}


// Procedure: Dtor
inline Prompt::~Prompt(){
  // Restore the original mode if has kept
  if(_has_orig_termios){
    ::tcsetattr(_infd, TCSAFLUSH, &_orig_termios);
  }
  if(not _history.empty()){
    _save_history();
  }
}

// Procedure: autocomplete
// This function adds the word into radix tree
inline void Prompt::autocomplete(const std::string& word){
  _tree.insert(word);
}

// Procedure: history_size 
// Change the max history size
inline void Prompt::set_history_size(size_t new_size){
  _max_history_size = new_size;
}


// Procedure: _save_history 
// Save history commands to a file
inline void Prompt::_save_history(){
  std::ofstream ofs(_history_path);
  if(not ofs.good()){
    return;
  }
  
  for(const auto& c: _history){
    ofs << c << '\n';
  }
  ofs.close();
}


// Procedure: _load_history 
// Load history commands from a file
inline void Prompt::_load_history(){
  if(std::filesystem::exists(_history_path)){
    std::ifstream ifs(_history_path);
    std::string placeholder;
    while(std::getline(ifs, placeholder)){
      if(_history.size() == _max_history_size){
        _history.pop_front();
      }
      _history.emplace_back(std::move(placeholder));
    }
  }
}

// Procedure: _add_history 
// Add command to history list
inline void Prompt::_add_history(const std::string &hist){
  // hist cannot be empty and cannot be the same as the last one
  if(hist.empty() or (not _history.empty() and _history.back() == hist)){
    return ;
  }
  if(_history.size() == _max_history_size){
    _history.pop_front();
  }
  _history.emplace_back(hist);
}

// Procedure: _stdin_not_tty
// Store input in a string if stdin is not from tty (from pipe or redirected file)
inline void Prompt::_stdin_not_tty(std::string& s){
  read_line(_cin, s);  // Read until newline, CR or EOF.
}

// Procedure: _unsupported_term
// Check the terminal is supported or not
inline bool Prompt::_unsupported_term(){
    
  static auto _unsupported_terms = {"dumb", "cons25", "emacs"};

  if(char* term = getenv("TERM"); term == NULL){
    return false;
  }
  else if(std::string_view str(term); 
          std::find(_unsupported_terms.begin(), 
                    _unsupported_terms.end() , str) != _unsupported_terms.end()){
    return true;
  }
  else {
    return false;
  }
}

// Procedure: readline 
// This is the main entry of Prompt
inline bool Prompt::readline(std::string& s) {
  if(not ::isatty(_infd)) {
    // not a tty, either from file or pipe; we don't limit the line size
    _stdin_not_tty(s);
    return _cin.eof() ? false : true;
  }
  else if(_unsupported_term()){
    read_line(_cin, s);
    return _cin.eof() ? false : true;
  }
  else{
    if(not _set_raw_mode()){
      return {};
    } 
    _edit_line(s);
    _add_history(s);
    _disable_raw_mode();
    std::cout << '\n';
    return errno == EAGAIN ? false : true;
  }
}

// Procedure: _save_orig_termios
// Save the original termios for recovering before exit
inline bool Prompt::_save_orig_termios(int fd){
  if(not _has_orig_termios){
    if(::tcgetattr(fd, &_orig_termios) == -1){
      return false;
    }
    _has_orig_termios = true;
  }
  return true;
}


// Procedure: _get_cursor_pos
// Return the current position of cursor
inline int Prompt::_get_cursor_pos(){
  /* Report cursor location */  
  // x1b[6n : DSR - Device Status Report  https://en.wikipedia.org/wiki/ANSI_escape_code 
  // The output will be like : ESC[n;mR, where "n" is the row and "m" is the column.
  // Try this in ur terminal : echo -en "abc \x1b[6n"
  if(not (_cout << "\x1b[6n")){
    return -1;
  }

  char buf[32];
  int cols, rows;

  /* Read the response: ESC [ rows ; cols R */
  for(size_t i=0; i<sizeof(buf)-1; i++){
    if(not(_cin >> buf[i]) or buf[i] == 'R'){
      buf[i] = '\0';
      break;
    }
  }

  /* Parse it. */
  if (static_cast<KEY>(buf[0]) != KEY::ESC or 
      buf[1] != '[' or 
      ::sscanf(buf+2, "%d;%d", &rows, &cols) != 2){
    return -1;
  }
  return cols;
}



// Procedure: _clear_screen (ctrl + l) 
inline void Prompt::_clear_screen() {
  // https://en.wikipedia.org/wiki/ANSI_escape_code 
  // CSI n ;  m H : CUP - Cursor Position
  // CSI n J      : Erase in Display
  //if(_cout.write("\x1b[H\x1b[2J",7); !_cout.good()){
  if(not (_cout << "\x1b[H\x1b[2J")){
    /* nothing to do, just to avoid warning. */
  }
}

// Procedure: _set_raw_mode 
// Set the fd to raw mode
inline bool Prompt::_set_raw_mode(){
  if(::isatty(_infd) and _save_orig_termios(_infd) == true){
    struct termios raw;  
    raw = _orig_termios;  /* modify the original mode */
    /* input modes: no break, no CR to NL, no parity check, no strip char,
     * no start/stop output control. */
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    /* output modes - disable post processing */
    raw.c_oflag &= ~(OPOST);
    /* control modes - set 8 bit chars */
    raw.c_cflag |= (CS8);
    /* local modes - choing off, canonical off, no extended functions,
     * no signal chars (^Z,^C) */
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    /* control chars - set return condition: min number of bytes and timer.
     * We want read to return every single byte, without timeout. */
    raw.c_cc[VMIN] = 1; raw.c_cc[VTIME] = 0; /* 1 byte, no timer */

    /* put terminal in raw mode after flushing */
    if (::tcsetattr(_infd, TCSAFLUSH, &raw) >= 0){
      return true;
    }
  }
  errno = ENOTTY;
  return false;
}


// Procedure: _disable_raw_mode  
// Recover the termios to the original mode
inline void Prompt::_disable_raw_mode(){
  if(_has_orig_termios){
    ::tcsetattr(_infd, TCSAFLUSH, &_orig_termios);
  }
}


// Procedure: _terminal_columns
// Get the number of columns of current line buf
inline size_t Prompt::_terminal_columns(){
  // Use ioctl to get Window size 
  if(winsize ws; ::ioctl(1, TIOCGWINSZ, &ws) == -1 or ws.ws_col == 0){
    int start = _get_cursor_pos();
    if(start == -1 or not (_cout << "\x1b[999c")){
      return 80;
    }
    int cols = _get_cursor_pos();
    if(cols == -1){
      return 80;
    }
    if(cols > start){
      char seq[32];
      // Move cursor back
      ::snprintf(seq, 32, "\x1b[%dD", cols-start);
      if(_cout.write(seq, strlen(seq)); not _cout.good()){
        /* Fail to move cursor back*/
      }
    }
    return cols;
  }
  else{
    return ws.ws_col;
  }
}


// Procedure: user_home
// Return the home folder of user
inline std::filesystem::path Prompt::_user_home() const{
  auto home = ::getenv("HOME");
  if(home == nullptr) {
    home = ::getpwuid(::getuid())->pw_dir;
  }
  return home ? home : std::filesystem::current_path();
}


// Procedure: _has_read_access 
// Check a file has read access grant to the user
inline bool Prompt::_has_read_access(const std::filesystem::path &path) const {
  if(auto rval=::access(path.c_str(), F_OK); rval == 0){
    if(rval = ::access(path.c_str(), R_OK); rval == 0){
      return true;
    }
  }
  return false;
}


// Procedure: _next_prefix
// Find the prefix among a set of strings starting from position n
inline std::string Prompt::_next_prefix(const std::vector<std::string>& words, const size_t n){
  if(words.empty()){
    return {};
  }
  else if(words.size() == 1){
    return words[0].substr(n, words[0].size()-n);
  }
  else{
    for(size_t end=n ;;++end){
      if(words[0].size() <= end){
        return words[0].substr(n, end-n);
      }
      for(size_t i=1; i<words.size(); i++){
        if(words[i].size() <= end or words[i][end] != words[0][end]){
          return words[i].substr(n, end-n);
        }
      }
    }
  }
}


// Procedure: _autocomplete_iterate_command
// This is the main entry for autocomplete iterating commands
inline int Prompt::_autocomplete_iterate_command(){

  if(auto words = _tree.match_prefix(_line.buf); words.empty()){
    return 0;
  }
  else{
    char c {0}; 
    bool stop {false};
    for(size_t i=0; not stop;){
      if(i < words.size()){
        _line_save = _line;
        _line.cur_pos = words[i].size();
        _line.buf = words[i];
        _refresh_single_line(_line);
        _line = _line_save;
      }
      else{
        _refresh_single_line(_line);
      }

      if(_cin.read(&c, 1); not _cin.good()){
        return -1;
      }

      switch(static_cast<KEY>(c)){
        case KEY::TAB:
          i = (i+1) % words.size();
          break;
        case KEY::ESC: // refresh the same thing again 
          _refresh_single_line(_line);
          stop = true;
          break;
        default:
          if(i < words.size()){
            _line.buf = words[i];
            _line.cur_pos = words[i].size();
          }
          stop = true;
          break;
      }
    }
    return c;
  }
}


inline std::string Prompt::_dump_options(const std::vector<std::string>& opts){
  if(opts.empty()){
    return {};
  }

  const size_t col_width = std::max_element(opts.begin(), opts.end(), 
    [](const auto& a, const auto& b){ 
      return a.size() < b.size();
    }
  )->size() + 4;

  auto col_num = std::max(size_t{1}, _terminal_columns() / col_width);

  std::string s;
  for(size_t i=0; i<opts.size(); ++i){
    if(i % col_num == 0) {
      s.append("\n\r\x1b[0K");
    }

    s.append(opts[i]);

    if(opts[i].size() < col_width){
      s.append(col_width - opts[i].size(), ' ');
    }
  }
  return s;
}


// Procedure: _autocomplete_command
// This is the main entry for command autocomplete
inline void Prompt::_autocomplete_command(){
  if(auto words = _tree.match_prefix(_line.buf); words.empty()){
  }
  else{
    if(auto suffix = _next_prefix(words, _line.cur_pos); not suffix.empty()){
      _line.buf.insert(_line.cur_pos, suffix);
      _line.cur_pos += suffix.size();
    }
    if(auto s = _dump_options(words); s.size() > 0){
      s.append("\x1b[0K\n");
      _cout << s;
    }
    _refresh_single_line(_line);
  }
}


// Procedure: _files_match_prefix
// Find all the files in a folder that match the prefix
inline std::vector<std::string> Prompt::_files_match_prefix(
  const std::filesystem::path& path
) const {
  // Need to check in case path is a file in current folder (parent_path = "")
  auto folder = path.filename() == path ? std::filesystem::current_path() : path.parent_path();
  std::string prefix(path.filename());

  std::vector<std::string> matches; 
  if(std::error_code ec; _has_read_access(folder) and std::filesystem::is_directory(folder, ec)){
    for(const auto& p: std::filesystem::directory_iterator(folder)){
      std::string fname {p.path().filename()};
      if(fname.compare(0, prefix.size(), prefix) == 0){
        matches.emplace_back(std::move(fname));
      }
    }
  }
  return matches;
}


// Procedure: _files_in_folder
// List all files in a given folder
inline std::vector<std::string> Prompt::_files_in_folder(
  const std::filesystem::path& path
) const {
  auto p = path.empty() ? std::filesystem::current_path() : path;
  std::vector<std::string> matches;
  // Check permission 
  if(_has_read_access(p)){
    for(const auto& p: std::filesystem::directory_iterator(p)){
      matches.emplace_back(p.path().filename());
    }
  }
  return matches;
}


// Procedure: _dump_files 
// Format the strings for pretty print in terminal.
inline std::string Prompt::_dump_files(
  const std::vector<std::string>& v, 
  const std::filesystem::path& path)
{
  if(v.empty()){
    return {};
  }

  const size_t col_width = std::max_element(v.begin(), v.end(), 
    [](const auto& a, const auto& b){ 
      return a.size() < b.size();
    }
  )->size() + 4;

  auto col_num = std::max(size_t{1}, _terminal_columns() / col_width);

  std::string s;

  char seq[64];
  snprintf(seq, 64, "\033[%d;1;49m", static_cast<int>(COLOR::BLUE));
  std::error_code ec;
  for(size_t i=0; i<v.size(); ++i){

    if(i % col_num == 0) {
      s.append("\n\r\x1b[0K");
    }

    if(std::filesystem::is_directory(path.native() + "/" + v[i], ec)){
      // A typical color code example : \033[31;1;4m 
      //   \033[ : begin of color code, 31 : red color,  1 : bold,  4 : underlined
      s.append(seq, strlen(seq));
      s.append(v[i]);
      s.append("\033[0m");
    }
    else{
      s.append(v[i]);
    }
    s.append(col_width - v[i].size(), ' ');
  }
  return s;
}

// Procedure: _autocomplete_folder 
// The entry for folder autocomplete
inline void Prompt::_autocomplete_folder(){
  std::string s;
  size_t ws_index = _line.buf.rfind(' ', _line.cur_pos) + 1;

  std::filesystem::path p(
    _line.buf[ws_index] != '~' ? 
    _line.buf.substr(ws_index, _line.cur_pos - ws_index) : 
    _user_home().native() + _line.buf.substr(ws_index+1, _line.cur_pos-ws_index-1)
  );

  if(std::error_code ec; p.empty() or std::filesystem::is_directory(p, ec)) {
    s = _dump_files(_files_in_folder(p), p);
  }
  else{
    auto match = _files_match_prefix(p);  
    if(not match.empty()){
      s = _dump_files(match, p.parent_path());
      if(auto suffix = _next_prefix(match, p.filename().native().size()); not suffix.empty()){
        _line.buf.insert(_line.cur_pos, suffix);
        _line.cur_pos += suffix.size();
        p += suffix;
        // Append a '/' if is a folder and not the prefix of other files
        if(std::filesystem::is_directory(p, ec) and 
            std::count_if(match.begin(), match.end(), 
              [p = p.filename().native()](const auto& m)
              { return (m.size() >= p.size() and m.compare(0, p.size(), p) == 0); }) == 1){
          _line.buf.insert(_line.cur_pos, 1, '/');
          _line.cur_pos += 1;
        }
      }
    }
  }
  if(s.size() > 0){
    s.append("\x1b[0K\n");
    _cout << s;
  }

  _refresh_single_line(_line);
}

// Procedure: _key_delete_prev_word
// Remove the word before cursor (including the whitespace between cursor and the word)
inline void Prompt::_key_delete_prev_word(LineInfo& line){
  // Remove all whitespace before cursor
  auto iter = std::find_if_not(line.buf.rbegin()+line.buf.size()-line.cur_pos, line.buf.rend(), 
                               [](const auto& c){ return c == ' ';});
  auto offset = std::distance(iter.base(), line.buf.begin()+line.cur_pos);
  line.buf.erase(iter.base(), line.buf.begin()+line.cur_pos);
  line.cur_pos -= offset;

  // Remove the word before cursor and stop at the first ws
  iter = std::find_if_not(line.buf.rbegin()+line.buf.size()-line.cur_pos, line.buf.rend(), 
                           [](const auto& c){ return c != ' ';});
  offset = std::distance(iter.base(), line.buf.begin()+line.cur_pos);
  line.buf.erase(iter.base(), line.buf.begin()+line.cur_pos);
  line.cur_pos -= offset;
}

// Procedure: _key_delete 
// Remove the character at cursor
inline void Prompt::_key_delete(LineInfo &line){
  if(line.buf.size() > 0){
    line.buf.erase(line.cur_pos, 1);
  }
}

// Procedure: _key_backspace 
// Remove the character before curosr
inline void Prompt::_key_backspace(LineInfo &line){
  if(line.cur_pos > 0){
    line.buf.erase(line.cur_pos-1, 1);
    line.cur_pos--;
  }
}

// Procedure: _key_prev_history
// Set line buffer to previous command in history
inline void Prompt::_key_prev_history(LineInfo& line){
  _key_history(line, true);
}

// Procedure: _key_next_history
// Set line buffer to next command in history
inline void Prompt::_key_next_history(LineInfo& line){
  _key_history(line, false);
}

// Procedure:_key_history
// Set the line buffer to previous/next history command
inline void Prompt::_key_history(LineInfo &line, bool prev){
  if(_history.size() > 1){
    *std::next(_history.begin(), _history.size()-1-line.history_trace) = line.buf;

    if(line.history_trace += prev ? 1 : -1; line.history_trace < 0){
      line.history_trace = 0;
    }
    else if(line.history_trace >= static_cast<int>(_history.size())){
      line.history_trace = _history.size()-1;
    }
    else{
      auto it = std::next(_history.begin(), _history.size()-1-line.history_trace);
      line.buf = *it;
      line.cur_pos = line.buf.size();
    }
  }
}

// Procedure: _append_character
// Insert a character to the cursor position in line buffer
inline bool Prompt::_append_character(LineInfo& line, char c){
  try{
    line.buf.insert(line.cur_pos, 1, c);
  }
  catch (const std::exception& e){
    _cerr << "append failed: " << e.what() << '\n';
    assert(false);
  }
  
  if(line.cur_pos++; line.buf.size() == line.cur_pos){
    if(not (_cout << c)){
      return false;
    }
  }
  else{
    _refresh_single_line(line);
  }
  return true;
}

// Procedure: _key_handle_CSI
// Handle Control Sequence Introducer
inline bool Prompt::_key_handle_CSI(LineInfo& line){ 
  char seq[3];
  if(_cin.read(seq,1), _cin.read(seq+1,1); not _cin.good() ){
    return false;
  }
  if(seq[0] == '['){
    if(seq[1] >= '0' and seq[1] <= '9'){
      if(_cin.read(seq+2, 1); not _cin.good()){
        return false;
      }
      if(seq[2] == '~' and seq[1] == '3'){
        _key_delete(line);
      }
    }
    else{
      switch(seq[1]){
        case 'A':  // Prev history
          _key_prev_history(line);
          break;
        case 'B':  // Next history
          _key_next_history(line);
          break;
        case 'C':  // Move cursor to right 
          if(line.cur_pos != line.buf.size()){
            line.cur_pos ++;
          }
          break;
        case 'D':  // Move cursor to left 
          if(line.cur_pos > 0){
            line.cur_pos --;
          }
          break;
        case 'H':  // Home : move cursor to the starting
          line.cur_pos = 0;
          break;
        case 'F':  // End  : move cursor to the end
          line.cur_pos = line.buf.size();
          break;
      }
    }
  }
  else if(seq[0] == 'O'){
    switch(seq[1]){
      case 'H':  // Home : move cursor to the starting
        line.cur_pos = 0;
        break;
      case 'F':  // End  : move cursor to the end
        line.cur_pos = line.buf.size();
        break;
    }
  }
  return true;
}


// Procedure: _edit_line 
// Handle the character input from the user
inline void Prompt::_edit_line(std::string &s){
  if(not (_cout << _prompt)){
    return;
  }

  _history.emplace_back("");
  _line.reset();
  s.clear();
  for(char c;;){
    if(_cin.read(&c, 1); not _cin.good()){
      s = _line.buf;
      return ;
    }

    // if user hits tab
    if(static_cast<KEY>(c) == KEY::TAB){
      if(_line.buf.empty()){
        continue;
      }
      else if(_line.buf.rfind(' ', _line.cur_pos) != std::string::npos){
        _autocomplete_folder();
        continue;
      }
      else{
        _autocomplete_command();
        continue;
        //if(c=_autocomplete_iterate_command(); c<0){
        //if(c=_autocomplete_command(); c<0){
        //  // something wrong happened
        //  return;
        //}
        //else if(c == 0){
        // continue;
        //}
      }
    }

    // Proceed to process character
    switch(static_cast<KEY>(c)){
      case KEY::ENTER:
        _history.pop_back(); // Remove the emptry string history added in beginning  
        s =  _line.buf;
        return ;
      case KEY::CTRL_A:    // Go to the start of the line 
        if(_line.cur_pos != 0){
          _line.cur_pos = 0;
        }
        _refresh_single_line(_line);
        break;
      case KEY::CTRL_B:    // Move cursor to left
        if(_line.cur_pos > 0){
          _line.cur_pos --;
        }
        _refresh_single_line(_line);
        break;
      case KEY::CTRL_C:
        _history.pop_back(); // Remove the emptry string history added in beginning  
        errno = EAGAIN;
        return ;
      case KEY::CTRL_D:    // Remove the char at the right of cursor. 
                           // If the line is empty, act as end-of-file
        if(_line.buf.size() > 0){
          _key_delete(_line);
          _refresh_single_line(_line);
        }
        else{
          _history.pop_back();
          return;
        }
        break;
      case KEY::CTRL_E:    // Move cursor to end of line 
        if(_line.cur_pos != _line.buf.size()){
          _line.cur_pos = _line.buf.size();
        }
         _refresh_single_line(_line);       
        break;
      case KEY::CTRL_F:    // Move cursor to right
        if(_line.cur_pos != _line.buf.size()){
          _line.cur_pos ++;
        }
        _refresh_single_line(_line);
        break;
      case KEY::BACKSPACE:  // CTRL_H is the same as BACKSPACE 
      case KEY::CTRL_H:
        _key_backspace(_line);
        _refresh_single_line(_line);
        break;
      case KEY::CTRL_K:    // Delete from current to EOF  
        _line.buf.erase(_line.cur_pos, _line.buf.size()-_line.cur_pos);
        _refresh_single_line(_line);
        break;
      case KEY::CTRL_L:    // Clear screen 
        _clear_screen();
        _refresh_single_line(_line);
        break;
      case KEY::CTRL_N:    // Next history command
        _key_next_history(_line);
        _refresh_single_line(_line);
        break;
      case KEY::CTRL_P:    // Previous history command
        _key_prev_history(_line);
        _refresh_single_line(_line);
        break;
      case KEY::CTRL_T:    // Swap current char with previous
        if(_line.cur_pos > 0){
          std::swap(_line.buf[_line.cur_pos], _line.buf[_line.cur_pos-1]);
          if(_line.cur_pos < _line.buf.size()){
            _line.cur_pos ++;
          }
          _refresh_single_line(_line);
        }
        break;
      case KEY::CTRL_U:    // Delete whole line
        _line.cur_pos = 0;
        _line.buf.clear();
        _refresh_single_line(_line);
        break;
      case KEY::CTRL_W:    // Delete previous word 
        _key_delete_prev_word(_line);
        _refresh_single_line(_line);
        break;
      case KEY::ESC:
        if(not _key_handle_CSI(_line)){
          _history.pop_back(); // Remove the emptry string history added in beginning  
          return;
        }
        _refresh_single_line(_line);
        break;
      default:
        _append_character(_line, c);
        break;
    }
  }
}


// Procedure: _refresh_single_line
// Flush the current line buffer on screen
inline void Prompt::_refresh_single_line(LineInfo &l){
  // 1. Append "move cursor to left" in the output buffer
  // 2. Append buf to output buffer
  // 3. Append "erase to  the right" to the output buffer 
  // 4. Append "forward cursor" to the output buffer : Adjust cursor to correct pos
  // 5. Write output buffer to fd  
  
  static const std::string CR {"\r"};       // Carriage Return (set cursor to left)
  static const std::string EL {"\x1b[0K"};  // Erase in Line (clear from cursor to the end of the line)

  auto len {l.buf.size()};
  auto pos {l.cur_pos};
  size_t start {0};

  if(_prompt.length()+pos >= _columns){
    start += _prompt.length()+pos-_columns-1;
    len -= _prompt.length()+pos-_columns-1;
    pos += (_prompt.length()+pos-_columns-1);
  }

  if(_prompt.length()+len > _columns){
    len -= (_prompt.length()+len-_columns);
  }

  char seq[64];
  ::snprintf(seq, 64, "\r\x1b[%dC", (int)(pos+_prompt.length()));

  _obuf.clear();
  _obuf.reserve(CR.length()+_prompt.length()+len+EL.length()+strlen(seq));
  _obuf.append(CR);
  _obuf.append(_prompt);
  _obuf.append(l.buf.data() + start, len);
  _obuf.append(EL);
  _obuf.append(seq, strlen(seq));

  if(not (_cout << _obuf)){
    _cerr << "Refresh line fail\n";
  }
}

};  // end of namespace prompt. -------------------------------------------------------------------

#endif 

