#ifndef  APPCONTROLLER_HPP
# define APPCONTROLLER_HPP

# include <Boots/Utils/dynstruct.hpp>

# include <crails/databases.hpp>
# include <crails/shared_vars.hpp>
# include <crails/view.hpp>
# include <crails/params.hpp>

# define RESCUE_FROM(render_data) \
  static DynStruct RescueFrom(std::function<DynStruct (Params&)> callback, Params& params) \
  { \
    DynStruct render_data; \
    \
    try \
    { \
      return (callback(params)); \
    }
    
# define END_RESCUE_FROM \
    return (render_data); \
  }
  
class Router;

struct ExceptionCSRF : public std::exception
{
};

class ControllerBase
{
  friend class Router;
protected:
  ControllerBase(Params& params, bool must_protect_from_forgery = true);
  
  static DynStruct RescueFrom(std::function<DynStruct ()> callback)
  {
    return (callback());
  }

  static void     RedirectTo(const std::string& uri);
  
  bool            protect_from_forgery(void);

  Params&    params;
  SharedVars vars;
  Data       flash;
};

#endif