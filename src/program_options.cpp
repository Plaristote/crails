#include "crails/program_options.hpp"

using namespace boost;
using namespace Crails;

ProgramOptions::ProgramOptions(int argc, char** argv)
{
  program_options::options_description desc("Options");

  desc.add_options()
    ("port,p",     program_options::value<std::string>()->required(),    "listened port")
    ("hostname,h", program_options::value<std::string>()->required(),    "listened host")
    ("thread,t",   program_options::value<unsigned short>()->required(), "amount of threads")
    ("ssl",        "enable SSL");
  program_options::store(program_options::parse_command_line(argc, argv, desc), vm);
}