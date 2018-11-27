#ifndef  CRAILS_SIGNIN_MODEL_HPP
# define CRAILS_SIGNIN_MODEL_HPP

# include <string>
# include <ctime>

namespace Crails
{
  #pragma db object abstract
  class AuthenticableModel
  {
  public:
    static const std::time_t session_duration;

    const std::string& get_authentication_token() const { return authentication_token; }
    std::time_t        get_sign_in_at()           const { return sign_in_at; }

    void set_authentication_token(const std::string& v) { authentication_token = v; }
    void set_sign_in_at(std::time_t v)                  { sign_in_at = v; }

    void generate_authentication_token();
    std::time_t get_token_expires_in() const;

  private:
    std::string authentication_token;
    std::time_t sign_in_at;
  };
}

#endif
