#pragma once
#include <string_view>
#include <boost/program_options.hpp>
#include <iostream>
#include "project_configuration.hpp"

namespace Crails
{
  class Command
  {
  public:
    Command() {}
    virtual ~Command() {}

    virtual int run() { return 0; }
    virtual std::string_view description() const { return ""; }
    virtual void options_description(boost::program_options::options_description&) const {}
    virtual bool initialize(int argc, char** argv);

  protected:
    boost::program_options::variables_map options;
    ProjectConfiguration                  configuration;
  };
}
