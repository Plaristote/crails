#include <crails/session_store/mongodb.hpp>
#include <crails/session_store.hpp>

using namespace Crails;

USE_SESSION_STORE(MongoStore)

const std::string  MongoStore::SessionStore::collection_name    = "session_store";
const std::string  MongoStore::SessionStore::database_name      = "mongodb";
const unsigned int MongoStore::SessionStore::session_expiration = 60 * 30;
