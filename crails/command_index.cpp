#include "command_index.hpp"
#include <iostream>

using namespace std;
using namespace Crails;

bool CommandIndex::initialize(int argc, char ** argv)
{
  if (argc > 1)
  {
    string_view command_name(argv[1]);

    if (command_name == "-h" || command_name == "--help")
    {
      list_commands(argc, argv);
      return true;
    }
    else if (command = find_command(command_name))
      return command->initialize(argc - 1, &argv[1]);
    else
      cerr << "Command `" << argv[1] << "` not found." << endl;
  }
  list_commands(argc, argv);
  return false;
}

void CommandIndex::list_commands(int argc, char **argv)
{
  size_t max_length = 0;

  cout << "usage: " << argv[0] << " [command] [options]" << endl
      << "Available commands:" << endl;
  for (auto it = commands.begin() ; it != commands.end() ; ++it)
    max_length = max(max_length, it->first.length());
  for (auto it = commands.begin() ; it != commands.end() ; ++it)
    cout << " - " << it->first << string(max_length - it->first.length() + 1, ' ') << "> " << it->second()->description() << endl;
}

shared_ptr<Command> CommandIndex::find_command(string_view name)
{
  for (auto it = commands.begin() ; it != commands.end() ; ++it)
  {
    if (it->first == name)
      return it->second();
  }
  return nullptr;
}

int CommandIndex::run()
{
  if (command)
    return command->run();
  return 0;
}
