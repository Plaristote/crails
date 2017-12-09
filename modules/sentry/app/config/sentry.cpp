#include <crails/sentry.hpp>
#include <crails/getenv.hpp>

using namespace std;

thread_local Sentry sentry;

const string Sentry::project_id      = Crails::getenv("SENTRY_PROJECT_ID");
const string Sentry::sentry_key      = Crails::getenv("SENTRY_KEY");
const string Sentry::sentry_secret   = Crails::getenv("SENTRY_SECRET");
const string Sentry::server_url      = Crails::getenv("SENTRY_PRIVATE_URL");
const string Sentry::server_protocol = Crails::getenv("SENTRY_PROTOCOL", "http");
