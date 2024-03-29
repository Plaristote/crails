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
    typedef Crails::Server::HttpCodes ResponseStatus;

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
    void            respond_with(Crails::Server::HttpCode);
    bool            require_basic_authentication(const std::string& username, const std::string& password);
    bool            require_basic_authentication(std::function<bool (const std::string&, const std::string&)>);

    void            render(const std::string& view);
    void            render(const std::string& view, SharedVars);
    void            render(RenderType type, Data value);
    void            render(RenderType type, const std::string& value);

    virtual bool    must_protect_from_forgery() const { return true; };

    std::string     get_action_name() const { return params["controller-data"]["action"].as<std::string>(); }

    Params&         params;
    Data            session;
    DataTree        response;
    SharedVars      vars;
    DataTree        flash;
  private:
    void            protect_from_forgery(void);
    bool            check_csrf_token(void);// const;
    bool            check_basic_authentication_header(const std::string& header, std::function<bool (const std::string&, const std::string&)>);
    void            set_content_type(RenderType);
    void            set_content_type_from_extension(const std::string&);

    Utils::Timer    timer;
  };
}

#endif
