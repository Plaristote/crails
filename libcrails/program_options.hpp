#ifndef  PROGRAM_OPTIONS_HPP 
# define PROGRAM_OPTIONS_HPP

# include <boost/program_options.hpp>
# include <boost/asio/ip/tcp.hpp>

namespace Crails
{
  class ProgramOptions
  {
  public:
    ProgramOptions(int argc, const char** argv);

    boost::asio::ip::tcp::endpoint get_endpoint() const;
    unsigned short                 get_thread_count() const;
    std::string                    get_pidfile_path() const;
    
  private:
    template<typename T>
    T get_value(const std::string& option_name, const T& default_value) const
    {
      if (vm.count(option_name))
        return vm[option_name].as<T>();
      return default_value;
    }
 
    boost::program_options::variables_map vm;
  };
}

#endif
