#include <crails/session_store/mongo_store.hpp>

const std::string  MongoStore::SessionStore::collection_name    = "session_store";
const std::string  MongoStore::SessionStore::database_name      = "mongodb";
const unsigned int MongoStore::SessionStore::session_expiration = 60 * 30;

