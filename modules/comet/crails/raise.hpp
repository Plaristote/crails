#ifndef  CRAILS_RAISE_HPP
# define CRAILS_RAISE_HPP

# include <exception>
# ifdef __COMET_CLIENT__
#  define boost_ext std
# endif

namespace Crails
{
  void raise(const std::exception& e);
}

#endif
