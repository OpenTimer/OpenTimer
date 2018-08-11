#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include <ot/utility/utility.hpp>

// Testcase: Tokenizer.IsWord
TEST_CASE("Tokenizer.IsWord") {
  REQUIRE(ot::is_word(" ") == false);
  REQUIRE(ot::is_word("f") == true);
  REQUIRE(ot::is_word("f_") == true);
  REQUIRE(ot::is_word("f_ ") == false);
  REQUIRE(ot::is_word("f1_f") == true);
  REQUIRE(ot::is_word("1_f") == false);
}

// Testcase: Tokenizer.IsArray
TEST_CASE("Tokenizer.IsArray") {
  REQUIRE(ot::is_array("af") == false);
  REQUIRE(ot::is_array("a[0]") == true);
  REQUIRE(ot::is_array("a[]") == false);
  REQUIRE(ot::is_array(" []") == false);
  REQUIRE(ot::is_array(" [98745]") == false);
  REQUIRE(ot::is_array("adf_[98745]") == true);
  REQUIRE(ot::is_array("_[98745]") == true);
  REQUIRE(ot::is_array("as f_[98745]") == false);
  REQUIRE(ot::is_array("B[1][0]") == true);
}

// Testcase: Tokenizer.IsNumeric
TEST_CASE("Tokenizer.IsNumeric") {
  REQUIRE(ot::is_numeric("123") == true);
  REQUIRE(ot::is_numeric("123 ") == false);
  REQUIRE(ot::is_numeric(" 123") == false);
  REQUIRE(ot::is_numeric("123.4") == true);
  REQUIRE(ot::is_numeric("123.") == false);
  REQUIRE(ot::is_numeric("+789") == true);
  REQUIRE(ot::is_numeric("+789 ") == false);
  REQUIRE(ot::is_numeric("+ 789") == false);
  REQUIRE(ot::is_numeric("+789.4") == true);
  REQUIRE(ot::is_numeric("+789.") == false);
  REQUIRE(ot::is_numeric("-456") == true);
  REQUIRE(ot::is_numeric("-456 ") == false);
  REQUIRE(ot::is_numeric("- 456") == false);
  REQUIRE(ot::is_numeric("-456.4") == true);
  REQUIRE(ot::is_numeric("-456.") == false);
}

// Testcase: Tokenizer.Quote
TEST_CASE("Tokenizer.Quote") {
  
  std::string str1 = "no quote";
  std::string str2 = "one quote\"";
  std::string str3 = "one \"quote";
  std::string str4 = "\"quote\"";
  std::string str5 = "1234\"_____\"5678";

  auto [b1, e1] = ot::find_quote_pair(str1.begin(), str1.end());
  REQUIRE((b1 == e1 && b1 == str1.end()));

  auto [b2, e2] = ot::find_quote_pair(str2.begin(), str2.end());
  REQUIRE((b2 == e2 && b2 == str2.end()));

  auto [b3, e3] = ot::find_quote_pair(str3.begin(), str3.end());
  REQUIRE((b3 == e3 && b3 == str3.end()));
  
  auto [b4, e4] = ot::find_quote_pair(str4.begin(), str4.end());
  REQUIRE((b4 == str4.begin() && e4 == str4.begin() + str4.size() - 1));
  
  auto [b5, e5] = ot::find_quote_pair(str5.begin(), str5.end());
  REQUIRE((b5 == str5.begin() + 4 && e5 == str5.begin() + str5.size() - 5));
}

// Testcase: Tokenizer.Brace
TEST_CASE("Tokenizer.Brace") {

  std::string str1 = "{}";
  std::string str2 = "{";
  std::string str3 = "}";
  std::string str4 = "{{[][]{}}}";
  std::string str5 = "56{{[][]{}}}ab";
  std::string str6 = "xyz{[][]{}}}456";

  auto [b1, e1] = ot::find_brace_pair(str1.begin(), str1.end());
  REQUIRE((b1 == str1.begin() && e1 == str1.begin() + 1));

  auto [b2, e2] = ot::find_brace_pair(str2.begin(), str2.end());
  REQUIRE((b2 == str2.end() && e2 == str2.end()));
  
  auto [b3, e3] = ot::find_brace_pair(str3.begin(), str3.end());
  REQUIRE((b3 == str3.end() && e3 == str3.end()));
  
  auto [b4, e4] = ot::find_brace_pair(str4.begin(), str4.end());
  REQUIRE((b4 == str4.begin() && e4 == str4.begin() + str4.size() - 1));
  
  auto [b5, e5] = ot::find_brace_pair(str5.begin(), str5.end());
  REQUIRE((b5 == str5.begin() + 2 && e5 == str5.begin() + str5.size() - 3));
  
  auto [b6, e6] = ot::find_brace_pair(str6.begin(), str6.end());
  REQUIRE((b6 == str6.begin() + 3 && e6 == str6.begin() + str6.size() - 5));
}

// Testcase: Tokenizer.Bracket
TEST_CASE("Tokenizer.Bracket") {

  std::string str1 = "[]";
  std::string str2 = "[";
  std::string str3 = "]";
  std::string str4 = "[[{}{}[]]]";
  std::string str5 = "56[[{}{}[]]]ab";
  std::string str6 = "xyz[{}{}[]]]456";

  auto [b1, e1] = ot::find_bracket_pair(str1.begin(), str1.end());
  REQUIRE((b1 == str1.begin() && e1 == str1.begin() + 1));

  auto [b2, e2] = ot::find_bracket_pair(str2.begin(), str2.end());
  REQUIRE((b2 == str2.end() && e2 == str2.end()));
  
  auto [b3, e3] = ot::find_bracket_pair(str3.begin(), str3.end());
  REQUIRE((b3 == str3.end() && e3 == str3.end()));
  
  auto [b4, e4] = ot::find_bracket_pair(str4.begin(), str4.end());
  REQUIRE((b4 == str4.begin() && e4 == str4.begin() + str4.size() - 1));
  
  auto [b5, e5] = ot::find_bracket_pair(str5.begin(), str5.end());
  REQUIRE((b5 == str5.begin() + 2 && e5 == str5.begin() + str5.size() - 3));
  
  auto [b6, e6] = ot::find_bracket_pair(str6.begin(), str6.end());
  REQUIRE((b6 == str6.begin() + 3 && e6 == str6.begin() + str6.size() - 5));
}







