#include "ecpp_property_parser.hpp"

using namespace std;

static void initialize_property_status(EcppProperty& property)
{
  // - Move pointers symbols to the type
  // - Check for the @ symbol, marking the property as shared
  // - Check if the property is a pointer, reference, both
  for (unsigned short i = 0 ; i < property.name.length();)
  {
    if (property.name[i] == '*' || property.name[i] == '&')
    {
      property.type += property.name[i];
      property.name = property.name.substr(1);
    }
    else if (property.name[i] == '@')
    {
      property.shared = true;
      property.name = property.name.substr(1);
      break ;
    }
    else
      break ;
  }
  if (property.type[property.type.length() - 1] == '*')
    property.pointer = true;
  else if (property.type[property.type.length() - 1] == '&')
    property.reference = true;
}

EcppPropertyParser::EcppPropertyParser(const std::string& source, unsigned int& cursor) : source(source), cursor(cursor)
{
  pattern_begin = cursor - 1;
  context = Type;
  while (cursor < source.length() && context != Aborted)
  {
    if (context == Type)
      advance_type();
    else if (context == NameLookup)
      advance_name_lookup();
    else if (context == Name)
      advance_name();
    else if (context == ValueLookup)
      advance_value_lookup();
    else if (context == Value)
      advance_value();
  }
  initialize_property_status(property);
}

void EcppPropertyParser::advance_type()
{
  if (source[cursor] == '<')
    template_depth++;
  else if (source[cursor] == '(')
    parentheses_depth++;
  else if (template_depth > 0 || parentheses_depth > 0)
  {
    if (template_depth > 0 && source[cursor] == '>')
      template_depth--;
    else if (parentheses_depth > 0 && source[cursor] == ')')
      parentheses_depth--;
  }
  else if (source[cursor] == ' ' || source[cursor] == '\t')
  {
    string match = source.substr(pattern_begin, cursor - pattern_begin);
    if (match == "const" || match == "unsigned")
    {
      property.type += match + ' ';
      pattern_begin = cursor + 1;
    }
    else
    {
      property.type += match;
      context = NameLookup;
    }
  }
  cursor++;
}

void EcppPropertyParser::advance_name_lookup()
{
  if (source[cursor] == ' ' || source[cursor] == '\t' || source[cursor] == '\n' || source[cursor] == '\r')
    cursor++;
  else if (source[cursor] == ';')
  {
    context = Aborted;
    cursor++;
  }
  else
  {
    context = Name;
    pattern_begin = cursor;
    cursor++;
  }
}

void EcppPropertyParser::advance_name()
{
  if (source[cursor] == ' ' || source[cursor] == '\t' || source[cursor] == '=' || source[cursor] == ';' || source[cursor] == '(')
  {
    property.name = source.substr(pattern_begin, cursor - pattern_begin);
    if (source[cursor] == ';')
      context = Aborted;
    else if (source[cursor] == '=' || source[cursor] == '(')
    {
      pattern_begin = cursor + 1;
      context = Value;
    }
    else
      context = ValueLookup;
  }
  cursor++;
}

void EcppPropertyParser::advance_value_lookup()
{
  if (source[cursor] == ';')
    context = Aborted;
  else if (source[cursor] == '=' || source[cursor] == '(')
  {
    pattern_begin = cursor + 1;
    context = Value;
  }
  cursor++;
}

void EcppPropertyParser::advance_value()
{
  if (source[cursor] == '"')
    advance_string();
  if (source[cursor] == ';')
  {
    property.default_value = source.substr(pattern_begin, cursor - pattern_begin);
    context = Aborted;
  }
  cursor++;
}

void EcppPropertyParser::advance_string()
{
  bool next_character_escaped = false;

  cursor++;
  while (cursor < source.length())
  {
    if (next_character_escaped)
      next_character_escaped = false;
    else if (source[cursor] == '\\')
      next_character_escaped = true;
    else if (source[cursor] == '"')
    {
      cursor++;
      break ;
    }
    cursor++;
  }
}
