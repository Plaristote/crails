#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <iostream>
#include <string_view>
#include <unordered_map>
#include "command_index.hpp"
#include "commands/new.hpp"
#include "commands/scaffold.hpp"
#include "commands/build.hpp"
#include "templates/index.hpp"
#include "_libcrails_placeholder.hpp"

using namespace Crails;
using namespace std;

class Index : public CommandIndex
{
public:
  Index()
  {
    add_command("templates", []() { return make_shared<TemplatesIndex>(); });
    add_command("build",     []() { return make_shared<BuildManager>(); });
    add_command("scaffold",  []() { return make_shared<Scaffold>(); });
    add_command("new",       []() { return make_shared<New>(); });
  }
};

int main(int argc, char **argv)
{
  Index index;

  if (index.initialize(argc, argv))
    return index.run();
  return -1;
}
