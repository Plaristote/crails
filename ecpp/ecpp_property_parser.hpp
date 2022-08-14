#pragma once
#include <string>
#include "ecpp_types.hpp"

class EcppPropertyParser
{
  enum Context { Type, NameLookup, Name, ValueLookup, Value, Aborted };
public:
  EcppProperty property;

  EcppPropertyParser(const std::string& source, unsigned int& cursor);

private:
  void advance_type();
  void advance_name_lookup();
  void advance_name();
  void advance_value_lookup();
  void advance_value();
  void advance_string();

  const std::string& source;
  unsigned int&      cursor;
  unsigned int       pattern_begin;
  Context            context;
  int                template_depth = 0;
  int                parentheses_depth = 0;
};
