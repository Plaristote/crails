#ifndef  CRAILS_BASIC_AUTHENTICATION_CONTROLLER_HPP
# define CRAILS_BASIC_AUTHENTICATION_CONTROLLER_HPP

# include "action.hpp"

namespace Crails
{
  class BasicAuthenticationController : public ActionController
  {
  protected:
    BasicAuthenticationController(Context&);

    bool require_basic_authentication(const std::string& username, const std::string& password);
    bool require_basic_authentication(std::function<bool (const std::string&, const std::string&)>);
  private:
    bool check_basic_authentication_header(const std::string& header, std::function<bool (const std::string&, const std::string&)>);
  };
}

#endif
