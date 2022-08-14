#include "formats.hpp"
#include "utils/string.hpp"

using namespace std;

void TemplateFormatsManager::options_description(boost::program_options::options_description& desc) const
{
  desc.add_options()
    ("add,a",    boost::program_options::value<string>(), "list of renderers to add (ex: html,json)")
    ("remove,r", boost::program_options::value<string>(), "list of renderers to remove");
}

int TemplateFormatsManager::run()
{
  configuration.initialize();
  if (!options.count("add") && !options.count("remove"))
  {
    cout << Crails::join(configuration.renderers(), ',') << endl;
    return 0;
  }
  if (options.count("add"))
  {
    for (const auto& entry : Crails::split(options["add"].as<string>(), ','))
      configuration.add_renderer(entry);
  }
  if (options.count("remove"))
  {
    for (const auto& entry : Crails::split(options["remove"].as<string>(), ','))
      configuration.remove_renderer(entry);
  }
  configuration.save();
  return 0;
}
