#ifndef  CRAILS_PLATFORM_HPP
# define CRAILS_PLATFORM_HPP

# if defined(__APPLE__)
#  define DYNLIB_EXT "dylib"
# elif defined(__linux__)
#  define DYNLIB_EXT "so"
# else
#  define DYNLIB_EXT "dll"
# endif

#endif
