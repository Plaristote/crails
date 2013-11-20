#ifndef  SESSION_STORE_HPP
# define SESSION_STORE_HPP

# include "crails/cookie_data.hpp"
# include "crails/http_response.hpp"

# ifdef USE_MONGODB_SESSION_STORE
#  include "crails/session_store/mongo_store.hpp"
typedef MongoStore  SessionStore;
# else
#  include "crails/session_store/cookie_store.hpp"
typedef CookieStore SessionStore;
#endif

#endif