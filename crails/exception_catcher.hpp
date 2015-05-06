#ifndef  EXCEPTION_CATCHER_HPP
# define EXCEPTION_CATCHER_HPP

# include "http_server.hpp"
# include "backtrace.hpp"
# include <sstream>

class Params;

namespace Crails
{
  class ExceptionCatcher
  {
    struct Context
    {
      Context(BuildingResponse& out, Params& params) : out(out), params(params) {}
      
      unsigned short        iterator;
      BuildingResponse&     out;
      Params&               params;
      std::function<void()> callback;
    };

    typedef std::function<void (Context)> Function;
    typedef std::vector<Function> Functions;
  public:
    ExceptionCatcher();

    void run(BuildingResponse& out, Params& params, std::function<void()> callback);

    template<typename EXCEPTION>
    void add_exception_catcher(const std::string exception_name)
    {
      add_exception_catcher<EXCEPTION>([this, exception_name](BuildingResponse& out, Params& params, const EXCEPTION e)
      {
        std::stringstream stream;

        stream << boost_ext::trace(e);
        default_exception_handler(out,
          params, exception_name, e.what(), stream.str());
      });
    }
    
    template<typename EXCEPTION>
    void add_exception_catcher(std::function<void (BuildingResponse&, Params&, const EXCEPTION)> handler)
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
          handler(context.out, context.params, e);
        }
      });
    }

  private:
    void response_exception(BuildingResponse& out, std::string exception_name, std::string message, Params&);
    void default_exception_handler(BuildingResponse& out, Params& params, const std::string exception_name, const std::string message, const std::string& trace);

    Functions     functions;
  };
}


#endif
