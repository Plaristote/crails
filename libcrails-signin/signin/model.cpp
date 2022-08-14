#include "model.hpp"
#include "utils/string.hpp"
#include <chrono>

using namespace std;
using namespace Crails;

void AuthenticableModel::generate_authentication_token()
{
  static const string charset(
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHUJKLMNOPQRSTUVWXYZ"
    "1234567890"
    "!@$^*()"
    "`~-_{};:,<> "
  );

  set_authentication_token(Crails::generate_random_string(charset, 16));
}

time_t AuthenticableModel::get_token_expires_in() const
{
  if (sign_in_at != 0)
  {
    using namespace chrono;
    auto now = system_clock::now();

    return session_duration - (system_clock::to_time_t(now) - sign_in_at);
  }
  return session_duration;
}
