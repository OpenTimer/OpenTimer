#ifndef OT_UTILITY_TOKENIZER_HPP_
#define OT_UTILITY_TOKENIZER_HPP_

#include <vector>
#include <string>
#include <string_view>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <regex>
#include <experimental/filesystem>

namespace std {
  namespace filesystem = experimental::filesystem;
};

namespace ot {

// string conversion
std::string to_lower(std::string);
std::string to_upper(std::string);

std::string remove_quote(std::string);
std::string unquoted(std::string);

// check string syntax
bool is_numeric(const std::string&);
bool is_array(const std::string&);
bool is_word(const std::string&);

// ------------------------------------------------------------------------------------------------

// Function: find_quote_pair
template <typename I>
auto find_quote_pair(const I b, const I e) {
  
  if(auto l = std::find(b, e, '"'); l == e) {
    return std::make_pair(e, e);
  }
  else {
    if(auto r = std::find(std::next(l), e, '"'); r == e) {
      return std::make_pair(e, e);
    }
    else {
      return std::make_pair(l, r);
    }
  }
}

// Function: find_brace_pair
template <typename I>
auto find_brace_pair(const I b, const I e) {
  
  auto l = std::find(b, e, '{');
  auto r = l;

  int stack = 0;

  while(r != e) {
    if(*r == '{') ++stack;
    else if(*r == '}') --stack;
    if(stack == 0) break;
    ++r;
  }

  if(l == e || r == e) {
    return std::make_pair(e, e);
  }
  
  return std::make_pair(l, r);
}

// Function: find_bracket_pair
template <typename I>
auto find_bracket_pair(const I b, const I e) {

  auto l = std::find(b, e, '[');
  auto r = l;

  int stack = 0;

  while(r != e) {
    if(*r == '[') ++stack;
    else if(*r == ']') --stack;
    if(stack == 0) break;
    ++r;
  }

  if(l == e || r == e) {
    return std::make_pair(e, e);
  }
  
  return std::make_pair(l, r);
}

// Function: on_next_parentheses
template <typename I, typename C>
auto on_next_parentheses(const I b, const I e, C&& c) {

  auto l = std::find(b, e, "(");
  auto r = l;
  //std::find(l, e, ")");

  int stack = 0;

  while(r != e) {
    if(*r == "(") {
      ++stack;
    }
    else if(*r == ")") {
      --stack;
    }
    if(stack == 0) {
      break;
    }
    ++r;
  }
  
  if(l == e || r == e) return e;

  for(++l; l != r; ++l) {
    c(*l);
  }

  return r;
}


// Function: tokenize
std::vector<std::string> tokenize(const std::filesystem::path&, std::string_view="", std::string_view="");

// Function: split
std::vector<std::string> split(const std::string&, std::string_view="");


};  // end of namespace ot. -----------------------------------------------------------------------

#endif
