#pragma once
#include "ecpp_types.hpp"
#include <string>

class EcppHeaderParser : public EcppHeader
{
  enum Context { Lookup, Property, Namespace, Typedef, Preprocessor, Comment, MultilineComment };
public:
  EcppHeaderParser(std::string_view name, const std::string& source);

private:
  void advance_lookup();
  void advance_comment();
  void advance_preprocessor();
  void advance_typedef_or_namespace();
  void advance_property();

  const std::string& source;
  unsigned int       cursor = 0;
  Context            context = Lookup;
  unsigned int       pattern_begin = 0;
};
