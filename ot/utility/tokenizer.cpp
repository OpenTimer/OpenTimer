#include <ot/utility/tokenizer.hpp>

// TODO
// 1. Consider removing the utf-8 bom (https://github.com/zer4tul/utf8-bom-strip)

namespace ot {

// Function: to_lower
std::string to_lower(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(), [] (auto c) { 
    return std::tolower(c); 
  });
  return s;
}

// Function: to_upper
std::string to_upper(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(), [] (auto c) {
    return std::toupper(c);
  });
  return s;
}

// Functon : remove_quote
std::string remove_quote(std::string s) {
  s.erase(std::remove( s.begin(), s.end(), '\"'), s.end());
  return s;
}

// Function: unquoted
std::string unquoted(std::string s) {
  if(s.size() >= 2 && s.front() == '\"' && s.back() == '\"') {
    return s.substr(1, s.size() - 2);
  }
  else return s;
}

// Function: is_numeric
bool is_numeric(const std::string& token) {
  return std::regex_match(token, std::regex("(\\+|-)?[0-9]*(\\.?([0-9]+))$")); 
}

// Function: is_array
bool is_array(const std::string& token) {
  return std::regex_match(token, std::regex("[a-zA-Z_][a-zA-Z_0-9]*(\\[[0-9]+\\])+"));
}

// Function: is_word
bool is_word(const std::string& token) {
  return std::regex_match(token, std::regex("[a-zA-Z_][a-zA-Z_0-9]*"));
}

// ------------------------------------------------------------------------------------------------

// Function: tokenize
std::vector<std::string> split(const std::string& str, std::string_view dels) {

  // Parse the token.
  std::string token;
  std::vector<std::string> tokens;

  for(size_t i=0; i<str.size(); ++i) {
    bool is_del = (dels.find(str[i]) != std::string_view::npos);
    if(is_del || std::isspace(str[i])) {
      if(!token.empty()) {                            // Add the current token.
        tokens.push_back(std::move(token));
      }
    } else {
      token.push_back(str[i]);  // Add the char to the current token.
    }
  }

  if(!token.empty()) {
    tokens.push_back(std::move(token));
  }

  return tokens;
}

//-------------------------------------------------------------------------------------------------

// Function: tokenize:
std::vector<std::string> tokenize(
  const std::filesystem::path& path, 
  std::string_view dels,
  std::string_view exps
) {
  
  using namespace std::literals::string_literals;

  std::ifstream ifs(path, std::ios::ate);

  if(!ifs.good()) {
    //throw std::invalid_argument("failed to open the file '"s + path.c_str() + '\'');
    return {};
  }
  
  // Read the file to a local buffer.
  size_t fsize = ifs.tellg();
  ifs.seekg(0, std::ios::beg);
  std::vector<char> buffer(fsize + 1);
  ifs.read(buffer.data(), fsize);
  buffer[fsize] = 0;
  
  // Mart out the comment
  for(size_t i=0; i<fsize; ++i) {

    // Block comment
    if(buffer[i] == '/' && buffer[i+1] == '*') {
      buffer[i] = buffer[i+1] = ' ';
      for(i=i+2; i<fsize; buffer[i++]=' ') {
        if(buffer[i] == '*' && buffer[i+1] == '/') {
          buffer[i] = buffer[i+1] = ' ';
          i = i+1;
          break;
        }
      }
    }
    
    // Line comment
    if(buffer[i] == '/' && buffer[i+1] == '/') {
      buffer[i] = buffer[i+1] = ' ';
      for(i=i+2; i<fsize; ++i) {
        if(buffer[i] == '\n' || buffer[i] == '\r') {
          break;
        }
        else buffer[i] = ' ';
      }
    }
    
    // Pond comment
    if(buffer[i] == '#') {
      buffer[i] = ' ';
      for(i=i+1; i<fsize; ++i) {
        if(buffer[i] == '\n' || buffer[i] == '\r') {
          break;
        }
        else buffer[i] = ' ';
      }
    }
  }

  //std::cout << std::string_view(buffer.data()) << std::endl;

  // Parse the token.
  std::string token;
  std::vector<std::string> tokens;

  for(size_t i=0; i<fsize; ++i) {

    auto c = buffer[i];
    bool is_del = (dels.find(c) != std::string_view::npos);

    if(is_del || std::isspace(c)) {
      if(!token.empty()) {                            // Add the current token.
        tokens.push_back(std::move(token));
        token.clear();
      }
      if(is_del && exps.find(c) != std::string_view::npos) {
        token.push_back(c);
        tokens.push_back(std::move(token));
      }
    } else {
      token.push_back(c);  // Add the char to the current token.
    }
  }

  if(!token.empty()) {
    tokens.push_back(std::move(token));
  }

  return tokens;
}

};  // end of namespace ot. -----------------------------------------------------------------------


