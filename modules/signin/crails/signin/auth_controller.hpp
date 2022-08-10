#ifndef  SIGNIN_AUTH_CONTROLLER_HPP
# define SIGNIN_AUTH_CONTROLLER_HPP

# include <crails/controller.hpp>
# include <crails/signin/session.hpp>

namespace Crails
{
  template<typename USER, typename SUPER>
  class AuthController : public SUPER
  {
  public:
    AuthController(Crails::Context& context) : SUPER(context), user_session(SUPER::session)
    {
    }

    void initialize()
    {
      SUPER::initialize();
      initialize_required_user();
    }

    virtual bool require_authentified_user() const { return false; }

    void initialize_required_user()
    {
      if (require_authentified_user() && !initialize_current_user())
        SUPER::close();
    }

  protected:
    bool initialize_current_user()
    {
      if (user_session.get_current_user() == nullptr)
      {
        SUPER::respond_with(HttpStatus::unauthorized);
        return false;
      }
      return true;
    }

    Session<USER> user_session;
  };
}

#endif
