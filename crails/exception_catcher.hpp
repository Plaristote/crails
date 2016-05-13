#ifndef  EXCEPTION_CATCHER_HPP
# define EXCEPTION_CATCHER_HPP

# include "http_server.hpp"
# include <crails/utils/backtrace.hpp>
# include <sstream>

namespace Crails
{
  class Request;
  class Server;

  class ExceptionCatcher
  {
    friend class Request;
    friend class Server;

    struct Context
    {
      Context(Request& request) : request(&request) {}
      Context() : request(0) {}

      unsigned short        iterator;
      Request*              request;
      std::thread::id       thread_id;
      std::function<void()> callback;
    };

    typedef std::function<void (Context)> Function;
    typedef std::vector<Function> Functions;
  public:
    ExceptionCatcher();

    void run(Request&, std::function<void()> callback) const;
    void run_protected(Request&, std::function<void()> callback) const;

    template<typename EXCEPTION>
    void add_exception_catcher(const std::string exception_name)
    {
      add_exception_catcher<EXCEPTION>([this, exception_name](Request& request, const EXCEPTION e)
      {
        std::stringstream stream;

        stream << boost_ext::trace(e);
        default_exception_handler(request,
          exception_name, e.what(), stream.str());
      });
    }

    template<typename EXCEPTION>
    void add_exception_catcher(std::function<void (Request&, const EXCEPTION)> handler)
    {
      functions.push_back([this, handler](Context context)
      {
        try
        {
          context.iterator++;
          if (context.iterator < functions.size())
            functions[context.iterator](context);
          else
            context.callback();
        }
        catch (const EXCEPTION e)
        {
          handler(*(context.request), e);
        }
      });
    }

  private:
    void response_exception(Request&, std::string exception_name, std::string message) const;
    void default_exception_handler(Request&, const std::string exception_name, const std::string message, const std::string& trace);

    Functions     functions;
  };
}


#endif
