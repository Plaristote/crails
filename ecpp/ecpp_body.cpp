#include "ecpp_body.hpp"
#include <iostream>

using namespace std;

const regex do_pattern("^\\s+do\\s*-?%>");
const regex end_pattern("^\n?<%\\s*end\\s+");
const regex yield_begin_pattern("^\\)\\s+yields\\s*-?%>");
const regex yield_end_pattern("^\n?<%\\s*y(ields-e)?nd\\s+");
const regex output_delimiter("^\n?<%=");
const regex begin_delimiter("^\n?<%");
const string_view   trim_end_delimiter = "-%>";
const string_view   end_delimiter      = "%>";

EcppMarkupBody::EcppMarkupBody(const string& source, unsigned int header_lines, string_view out_property_name) : source(source), header_lines(header_lines), out_property_name(out_property_name)
{
  yield_begin = ", [&]() -> std::string { std::stringstream " + string(out_property_name) + get_end_delimiter_replacement();
  yield_end   = "\";\n  return " + string(out_property_name) + ".str();\n  }))";
  *this << out_property_name << " << \"";
  while (cursor < source.length())
  {
    if (context == RawContext)
      advance_in_raw_context();
    else // if (context == CppContext || context == CppOutputContext)
      advance_in_cpp_context();
  }
  if (context == RawContext) *this << "\";";
}

static void throw_template_error(std::string_view error_desc, const std::string& source, unsigned int cursor, unsigned int header_lines)
{
  std::stringstream message;
  unsigned int line = 1 + header_lines;
  unsigned int col = 0;

  for (unsigned int i = 0 ; i < cursor ; ++i, ++col)
  { if (source[i] == '\n') { line++; col = 0; } }
  message << "line " << line << ':' << col << " > " << error_desc;
  throw std::runtime_error(message.str());
}

bool EcppMarkupBody::try_to_start_cpp_context()
{
  static const std::string_view output_replacement("\" << (");
  static const std::string_view begin_replacement("\";\n");

  if      (try_to_replace_and_advance(output_delimiter, output_replacement))
    context = CppOutputContext;
  else if (try_to_replace_and_advance(begin_delimiter,  begin_replacement))
    context = CppContext;
  else
    return false;
  return true;
}

void EcppMarkupBody::advance_in_raw_context()
{
  if (try_to_replace_and_advance("\"", "\\\""))
    ;
  else if (try_to_replace_and_advance(end_pattern, "\";\n }"))
  {
    if (do_depth == 0) throw_template_error("extra `end`", source, cursor, header_lines);
    context = CppContext;
    do_depth--;
  }
  else if (try_to_replace_and_advance(yield_end_pattern, yield_end))
  {
    if (yield_depth == 0) throw_template_error("extra `yield-ends`", source, cursor, header_lines);
    context = CppContext;
    yield_depth--;
  }
  else if (try_to_start_cpp_context())
    return ;
  else if (source[cursor] == '\n')
  {
    if (!should_ignore_next_line_return) *this << "\\n";
    should_ignore_next_line_return = false;
    cursor++;
  }
  else
    *this << source[cursor++];
}

void EcppMarkupBody::advance_in_cpp_context()
{
  if (try_to_replace_and_advance(trim_end_delimiter, get_end_delimiter_replacement()))
  {
    context = RawContext;
    should_ignore_next_line_return = true;
  }
  else if (try_to_replace_and_advance(end_delimiter, get_end_delimiter_replacement()))
    context = RawContext;
  else if (try_to_replace_and_advance(yield_begin_pattern, yield_begin))
  {
    context = RawContext;
    yield_depth++;
  }
  else if (try_to_replace_and_advance(do_pattern, get_block_delimiter_replacement()))
  {
    context = RawContext;
    do_depth++;
  }
  else
    *this << source[cursor++];
}

bool EcppMarkupBody::try_to_replace_and_advance(string_view pattern, string_view replacement)
{
  if (source.substr(cursor, pattern.length()) == pattern)
  {
    *this << replacement;
    cursor += pattern.length();
    return true;
  }
  return false;
}

bool EcppMarkupBody::try_to_replace_and_advance(regex pattern, string_view replacement)
{
  auto match = cregex_iterator(&source[cursor], &source[source.length()], pattern);

  if (match != cregex_iterator())
  {
    *this << replacement;
    cursor = cursor + match->position() + match->length();
    if (source.length() > cursor + match->length() - 3 &&
        source.substr(cursor + match->length() - 3, 3) == "-%>")
      should_ignore_next_line_return = true;
    return true;
  }
  return false;
}

string EcppMarkupBody::get_end_delimiter_replacement() const
{
  string base(";\n  " + string(out_property_name) + " << \"");

  if (context == CppOutputContext)
    return ")" + base;
  return base;
}

string EcppMarkupBody::get_block_delimiter_replacement() const
{
  string base = get_end_delimiter_replacement();
  base[0] = '{';
  return base;
}


