#pragma once
#include "../command.hpp"

class BuildManager : public Crails::Command
{
public:
  std::string_view description() const override { return "Builds a Crails server."; }
  int              run() override;

private:
  bool prebuild_renderers();
  bool build_with_cmake();
};
