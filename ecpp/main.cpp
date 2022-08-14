#include "utils/string.hpp"
#include "read_file.hpp"
#include <boost/program_options.hpp>
#include <iostream>
#include "ecpp_header_parser.hpp"
#include "ecpp_body.hpp"
#include "_libcrails_placeholder.hpp"

using namespace std;
using namespace boost;

std::string ecpp_generate(const std::string& source, const EcppOptions& options);

static string path_to_classname(const std::string& path)
{
  string result;
  bool uppercase = true;

  result = "Ecpp";
  for (unsigned short i = 0 ; i < path.size() ; ++i)
  {
    if (path[i] >= 'a' && path[i] <= 'z' && uppercase)
    {
      result += path[i] - 'a' + 'A';
      uppercase = false;
    }
    else if ((path[i] >= 'a' && path[i] <= 'z') || (path[i] >= 'A' && path[i] <= 'Z') || (path[i] >= '0' && path[i] <= '9'))
      result += path[i];
    else if (!uppercase)
      uppercase = true;
  }
  return result;
}

static EcppOptions options_from_command_line(program_options::variables_map options)
{
  EcppOptions result;

  if (options.count("name"))
    result.output_name = options["name"].as<string>();
  else
    result.output_name = path_to_classname(options["input"].as<string>());
  if (options.count("crails-include"))
  {
    result.crails_include = options["crails-include"].as<string>();
    result.parent_header = options["crails-include"].as<string>() + "template.hpp";
  }
  if (options.count("template-header"))
    result.parent_header = options["template-header"].as<string>();
  if (options.count("template-type"))
    result.parent_class = options["template-type"].as<string>();
  if (options.count("out_property_name"))
    result.out_property_name = options["out-property-name"].as<string>();
  if (options.count("render-mode"))
    result.body_mode = options["render-mode"].as<string>();
  return result;
}

int main(int argc, char** argv)
{
  program_options::variables_map options;
  program_options::options_description desc("Options");
  string input;

  desc.add_options()
    ("help,h",             "product help message")
    ("name,n",             program_options::value<std::string>(), "class name for your template")
    ("input,i",            program_options::value<std::string>(), "ecpp source to process")
    ("crails-include,c",   program_options::value<std::string>(), "include folder to crails (defaults to `crails`)")
    ("template-type,t",    program_options::value<std::string>(), "name of the template class (default to Crails::Template)")
    ("template-header,z",  program_options::value<std::string>(), "path to the header defining the parent class")
    ("render-mode,m",      program_options::value<std::string>(), "raw or markup (defaults to `markup`)");
  program_options::store(program_options::parse_command_line(argc, argv, desc), options);
  program_options::notify(options);
  if (options.count("help"))
    std::cout << "usage: " << argv[0] << " -i [filename] [options]" << std::endl << desc;
  else if (options.count("input") == 0)
    return -1;
  else if (Crails::read_file(options["input"].as<string>(), input))
  {
    try
    {
      std::cout << ecpp_generate(input, options_from_command_line(options));
    }
    catch (const std::runtime_error& e)
    {
      std::cerr << options["input"].as<string>() << ": " << e.what() << std::endl;
      return -1;
    }
  }
  return 0;
}
