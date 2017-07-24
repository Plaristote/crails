#ifndef  CRAILS_CONTROLLER_HPP
# define CRAILS_CONTROLLER_HPP

# include <crails/utils/timer.hpp>
# include <crails/databases.hpp>
# include <crails/shared_vars.hpp>
# include <crails/params.hpp>
# include <crails/logger.hpp>

namespace Crails
{
  class Router;
  class Mailer;

  struct ExceptionCSRF : public std::exception
  {
    const char* what(void) const throw() { return ("The CSRF token was missing or invalid and your identity couldn't be verified."); }
  };

  class Controller
  {
  protected:
    Controller(Params& params);
  public:
    virtual ~Controller();

    void initialize(void);
    void finalize(void) {}
    
    enum RenderType
    {
      JSON,
      JSONP,
      HTML,
      XML,
      TEXT
    };

    void            redirect_to(const std::string& uri);

    void            render(const std::string& view);
    void            render(RenderType type, Data value);
    void            render(RenderType type, const std::string& value);

    virtual bool    must_protect_from_forgery(void) const { return (true); };

    Params&         params;
    Data            session;
    DataTree        response;
    SharedVars      vars;
    DataTree        flash;
  private:
    void            protect_from_forgery(void);
    bool            check_csrf_token(void);// const;
    void            set_content_type(RenderType);
    void            set_content_type_from_extension(const std::string&);

    Utils::Timer    timer;
  };
}

#endif
