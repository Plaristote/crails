#ifndef  CRAILS_ENVIRONMENT_HPP
# define CRAILS_ENVIRONMENT_HPP

# ifdef SERVER_DEBUG
#  define ENVIRONMENT "development"
# elif defined(CRAILS_TESTS)
#  define ENVIRONMENT "test"
# else
#  define ENVIRONMENT "production"
# endif

#endif