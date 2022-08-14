#pragma once
#include "command.hpp"
#include <string_view>
#include <unordered_map>
#include <memory>

class CommandIndex : public Crails::Command
{
  typedef std::function<std::shared_ptr<Command>()>         CommandCtor;
  typedef std::unordered_map<std::string_view, CommandCtor> CommandCtors;

  CommandCtors             commands;
  std::shared_ptr<Command> command;
public:
  bool                     initialize(int argc, char ** argv) override;
  int                      run() override;
  void                     add_command(std::string_view name, CommandCtor ctor) { commands.emplace(name, ctor); }
private:
  void                     list_commands(int argc, char **argv);
  std::shared_ptr<Command> find_command(std::string_view);
};
