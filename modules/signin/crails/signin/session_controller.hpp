#ifndef  CRAILS_SESSION_CONTROLLER_HPP
# define CRAILS_SESSION_CONTROLLER_HPP

# include <crails/params.hpp>
# include <crails/signin/session.hpp>

# define signin_routes(path, controller) \
  SetRoute("GET",    path, controller, show); \
  SetRoute("POST",   path, controller, create); \
  SetRoute("DELETE", path, controller, destroy);

namespace Crails
{
  template<typename USER, typename SUPER>
  class SessionController : public SUPER
  {
  public:
    typedef std::shared_ptr<USER> UserPtr;

    SessionController(Crails::Params& params) : SUPER(params), user_session(SUPER::session)
    {
    }

    void show()
    {
      auto user = user_session.get_current_user();
      
      if (user)
      {
        DataTree response_body;

        response_body["auth_token"] = user->get_authentication_token();
        response_body["cuid"]       = user->get_id();
        response_body["expires_in"] = user->get_token_expires_in();
        SUPER::render(SUPER::JSON, response_body.as_data());
      }
      else
        SUPER::respond_with(SUPER::ResponseStatus::forbidden);
    }

    void create()
    {
      UserPtr user = find_user();

      if (user != nullptr)
      {
        user_session.set_current_user(user);
        show();
      }
      else
        SUPER::respond_with(SUPER::ResponseStatus::bad_request);
    }

    void destroy()
    {
      if (user_session.get_current_user())
        user_session.destroy();
      else
        SUPER::respond_with(SUPER::ResponseStatus::forbidden);
    }

  private:
    virtual UserPtr find_user() = 0;

    Session<USER> user_session;
  };
}

#endif
