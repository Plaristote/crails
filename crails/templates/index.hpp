#pragma once
#include "../command_index.hpp"

class TemplatesIndex : public CommandIndex
{
public:
  TemplatesIndex();
  virtual std::string_view description() const override { return "manages ecpp templates and renderers."; }
};
