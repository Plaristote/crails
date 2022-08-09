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
  class Request;
  template<typename CONTROLLER> class ControllerRoute;

  struct ExceptionCSRF : public std::exception
  {
    const char* what(void) const throw() { return ("The CSRF token was missing or invalid and your identity couldn't be verified."); }
  };

  class Controller : public std::enable_shared_from_this<Controller>
  {
    template<typename CONTROLLER>
    friend class ControllerRoute;
  protected:
    typedef Crails::HttpStatus ResponseStatus;

    Controller(Request& request);
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
    void            respond_with(Crails::HttpStatus);
    bool            require_basic_authentication(const std::string& username, const std::string& password);
    bool            require_basic_authentication(std::function<bool (const std::string&, const std::string&)>);

    void            render(const std::string& view);
    void            render(const std::string& view, SharedVars);
    void            render(RenderType type, Data value);
    void            render(RenderType type, const std::string& value);

    virtual bool    must_protect_from_forgery() const { return true; };

    std::string     get_action_name() const { return params["controller-data"]["action"].as<std::string>(); }

    Params&           params;
    Data              session;
    BuildingResponse& response;
    SharedVars        vars;
    DataTree          flash;
  private:
    void            protect_from_forgery(void);
    bool            check_csrf_token(void);// const;
    bool            check_basic_authentication_header(const std::string& header, std::function<bool (const std::string&, const std::string&)>);
    void            set_content_type(RenderType);
    void            set_content_type_from_extension(const std::string&);
    void            close();

    std::shared_ptr<Request> request;
    std::function<void()>    callback;
    bool                     close_on_deletion = false;
    Utils::Timer             timer;
  };
}

#endif
