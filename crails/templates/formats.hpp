#pragma once
#include "../command.hpp"

class TemplateFormatsManager : public Crails::Command
{
public:
  int              run() override;
  std::string_view description() const override { return "add/remove renderers"; }
  void             options_description(boost::program_options::options_description& desc) const override;
};
