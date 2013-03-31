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

class ControllerBase
{
  friend class Router;
private:
  static void      BeforeFilter(Params& params)
  {
  }

  static DynStruct AfterFilter(DynStruct render_data, Params&)
  {
    return (render_data);
  }

  static DynStruct RescueFrom(std::function<DynStruct (Params&)> callback, Params& params)
  {
    return (callback(params));
  }

protected:
  static void RedirectTo(const std::string& uri);
};

#endif
