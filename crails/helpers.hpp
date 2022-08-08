#ifndef  CRAILS_HELPERS_HPP
# define CRAILS_HELPERS_HPP

# include "crails/datatree.hpp"
# include <string>

void        cgi2params(Data params, const std::string& encoded_str);
std::string get_mimetype(const std::string& filename);

#endif
