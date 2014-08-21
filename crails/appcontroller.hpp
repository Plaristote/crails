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
  const char* what(void) const throw() { return ("The CSRF token was missing or invalid and your identity couldn't be verified."); }
};

class ControllerBase
{
  friend class Router;
protected:
  ControllerBase(Params& params);
  
  static DynStruct RescueFrom(std::function<DynStruct ()> callback)
  {
    return (callback());
  }

  static void     RedirectTo(const std::string& uri);
  
  enum RenderType
  {
    JSON,
    JSONP,
    HTML,
    XML
  };

  void            render(RenderType type, DynStruct value);
  void            render(RenderType type, const std::string& value);
  void            render(const std::string& view, const std::string& layout = "layouts/application");

  virtual bool    must_protect_from_forgery(void) const { return (true); };

  Params&        params;
  DynStruct      response;
  SharedVars     vars;
  Data           flash;
private:
  void            protect_from_forgery(void) const;
  bool            check_csrf_token(void) const;
  void            set_content_type(RenderType);
  void            set_content_type_from_extension(const std::string&);
};

#endif