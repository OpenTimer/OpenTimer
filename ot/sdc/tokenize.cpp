#include <ot/sdc/sdc.hpp>

namespace ot::sdc {

// Function: tokenize
std::vector<std::string> tokenize(const std::string& line) {

  // Parse the token.
  std::string token;
  std::vector<std::string> tokens;

  auto add_token = [] (auto& tokens, auto& token) {
    if(!token.empty()) {
      tokens.push_back(std::move(token));
    }
  };

  for(auto itr = line.begin(); itr != line.end(); ++itr) {

    if(*itr == '#' || *itr == '\n' || *itr == '\r') {
      break;
    }

    if(*itr == ' ') {
      add_token(tokens, token);
    }
    // "quotation string"
    else if(*itr == '"') {

      OT_LOGF_IF(!token.empty(), "syntax error in parsing '\"'");

      if(auto [b, e] = find_quote_pair(itr, line.end()); b != e) {
        while(++b != e) {
          token.push_back(*b);
        }
        add_token(tokens, token);
        itr = e;
      }
      else {
        OT_LOGF("can't find quote pair \" \"");
      }
    }
    // [ ... ]
    else if(*itr == '[') {
      
      OT_LOGF_IF(!token.empty(), "syntax error in parsing '['");

      token.push_back('[');
      add_token(tokens, token);

      auto [b, e] = find_bracket_pair(itr, line.end());

      OT_LOGF_IF(b==line.end(), "can't find bracket pair [ ]");
      
      for(auto i=std::next(b); i != e; ++i) {
        if(*i == '{' || *i == '}') continue;
        if(*i == ' ') {
          add_token(tokens, token);
        }
        else {
          token.push_back(*i);
        }
      }
      
      add_token(tokens, token);
      token.push_back(']');
      add_token(tokens, token);

      itr = e;
    }
    // { ... }
    else if(*itr == '{') {

      OT_LOGF_IF(!token.empty(), "syntax error in parsing '{'");

      token.push_back('{');
      add_token(tokens, token);

      auto [b, e] = find_brace_pair(itr, line.end());

      OT_LOGF_IF(b == line.end(), "can't find brace pair { }");

      for(auto i=std::next(b); i!=e; ++i) {
        if(*i == '{' || *i == '}') continue;
        if(*i == ' ') {
          add_token(tokens, token);
        }
        else {
          token.push_back(*i);
        }
      }

      add_token(tokens, token);
      token.push_back('}');
      add_token(tokens, token);

      itr = e;
    }
    else {
      token.push_back(*itr);
    }
  }

  add_token(tokens, token);

  return tokens;
}

};  // end of namespace ot::sdc. ------------------------------------------------------------------
