#pragma once
#include "../command.hpp"
#include "../scaffolds/controller.hpp"

class Scaffold : public Crails::Command
{
  std::string type;
  ControllerScaffold* model = nullptr;
public:
  std::string_view description() const override { return "Generates basic files forming the basic structure of Crails."; }

  bool initialize(int argc, char** argv) override
  {
    configuration.initialize();
    if (argc > 0)
    {
      type = argv[1];
      if (type == "controller")
        model = new ControllerScaffold;
      else
      {
        std::cerr << "Unknown scaffold type " << type << std::endl;
        return false;
      }
      return Command::initialize(argc - 1, &argv[1]);
    }
    return false;
  }

  void options_description(boost::program_options::options_description& desc) const override
  {
    if (model)
      model->options_description(desc);
  }

  int run()
  {
    model->create(options);
    return 0;
  }

private:
};
