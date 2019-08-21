#ifndef  PROGRAM_OPTIONS_HPP 
# define PROGRAM_OPTIONS_HPP

# include <boost/program_options.hpp>
# include <crails/server.hpp>

namespace Crails
{
  class ProgramOptions
  {
    typedef boost::network::http::server_options<boost::network::http::tags::http_server, Crails::Server> ServerOptions;
  public:
    ProgramOptions(int argc, const char** argv);

    HttpServer::options get_server_options(Crails::Server&) const;

    template<typename T>
    T get_value(const std::string& option_name, const T& default_value) const
    {
      if (vm.count(option_name))
        return vm[option_name].as<T>();
      return default_value;
    }

  private:
    void initialize_interface(HttpServer::options&) const;
    void initialize_thread_pool(HttpServer::options&) const;
    void initialize_ssl_context(HttpServer::options&) const;
    void initialize_pid_file(HttpServer::options&) const;
    
    boost::program_options::variables_map vm;
  };
}

#endif
