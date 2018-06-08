#include <crails/getenv.hpp>
#include <crails/sync/client.hpp>

thread_local Faye::Client faye({
  Crails::getenv                   ("FAYE_PROTOCOL", "http"),
  Crails::getenv                   ("FAYE_HOST",     "localhost"),
  Crails::getenv_as<unsigned short>("FAYE_PORT",     9292),
  [](Data message) {
    message["ext"]["secret"] = Crails::getenv("FAYE_SECRET");
  }
});
