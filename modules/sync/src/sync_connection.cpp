#include "../crails/odb/sync_connection.hpp"

thread_local safe_ptr<ODB::Sync::Connection> ODB::Sync::Connection::instance;
