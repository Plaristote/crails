#pragma once
#include <sstream>
#include <string_view>
#include <regex>

struct EcppMarkupBody : public std::stringstream
{
  enum EcppBodyContext { RawContext, CppContext, CppOutputContext, CppYieldContext };
public:
  EcppMarkupBody(const std::string& source, unsigned int header_lines = 0, std::string_view out_property_name = "ecpp_stream");

private:
  std::string get_end_delimiter_replacement() const;
  std::string get_block_delimiter_replacement() const;
  void advance_in_raw_context();
  void advance_in_cpp_context();
  bool try_to_replace_and_advance(std::string_view pattern, std::string_view replacement);
  bool try_to_replace_and_advance(std::regex, std::string_view replacement);
  bool try_to_start_cpp_context();

  const std::string& source;
  unsigned int       header_lines = 0;
  std::string_view   out_property_name;
  unsigned int       cursor = 0;
  EcppBodyContext    context = RawContext;
  bool               should_ignore_next_line_return = false;
  std::string        yield_begin, yield_end;
  unsigned int do_depth = 0, yield_depth = 0;
};
