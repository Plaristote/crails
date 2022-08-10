#ifndef  CRAILS_CSRF_CONTROLLER_HPP
# define CRAILS_CSRF_CONTROLLER_HPP

# include "action.hpp"
# include "basic_authentication.hpp"

namespace Crails
{
  struct ExceptionCSRF : public std::exception
  {
    const char* what(void) const throw() override;
  };

  class CsrfController : public BasicAuthenticationController
  {
  protected:
    CsrfController(Context&);

    void         initialize() override;
    virtual bool must_protect_from_forgery() const { return true; }

  private:
    void         protect_from_forgery(void);
    bool         check_csrf_token(void);// const;
  };
}

#endif
