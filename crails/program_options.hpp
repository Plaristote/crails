#ifndef  PROGRAM_OPTIONS_HPP 
# define PROGRAM_OPTIONS_HPP

# include <boost/program_options.hpp>

namespace Crails
{
  class ProgramOptions
  {
  public:
    ProgramOptions(int argc, char** argv);

    template<typename T>
    T get_value(const std::string& option_name, const T& default_value)
    {
      if (vm.count(option_name))
        return vm[option_name].as<T>();
      return default_value;
    }
  private:
    boost::program_options::variables_map vm;
  };
}

#endif
