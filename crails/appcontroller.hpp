#ifndef  APPCONTROLLER_HPP
# define APPCONTROLLER_HPP

# include <Boots/Utils/dynstruct.hpp>

# include <crails/databases.hpp>
# include <crails/shared_vars.hpp>
# include <crails/view.hpp>
# include <crails/params.hpp>

# define RESCUE_FROM(render_data) \
  static DynStruct RescueFrom(std::function<DynStruct (void)> callback) \
  { \
    DynStruct render_data; \
    \
    try \
    { \
      return (callback()); \
    }
    
# define END_RESCUE_FROM \
    return (render_data); \
  }
  
class Router;

class ControllerBase
{
  friend class Router;
protected:
  ControllerBase(Params& params) : params(params)
  {
    *vars["params"] = &params;
  }

private:
  static DynStruct RescueFrom(std::function<DynStruct (void)> callback)
  {
    return (callback());
  }

protected:
  static void RedirectTo(const std::string& uri);
  Params&     params;
  SharedVars  vars;
};

#endif
