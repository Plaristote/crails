#ifndef  CL_OPTIONS_HPP
# define CL_OPTIONS_HPP

# include <string>
# include <sstream>

namespace Utils
{
  class ClOptions
  {
  public:
    ClOptions(int argc, char** argv) : _argc(argc), _argv(argv)
    {
    }

    template<typename OPT_TYPE>
    OPT_TYPE GetValue(const std::string& shortcut, const std::string& name, OPT_TYPE default_value)
    {
      for (unsigned short i = 1 ; i < _argc ; ++i)
      {
        if (_argv[i] == shortcut || _argv[i] == name)
        {
          std::stringstream stream;
          OPT_TYPE          value;

          if (i + 1 < _argc)
          {
            stream << _argv[i + 1];
            stream >> value;
            return (value);
          }
        }
      }
      return (default_value);
    }

  private:
    int    _argc;
    char** _argv;
  };
}

#endif