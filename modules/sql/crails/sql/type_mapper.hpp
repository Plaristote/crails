#ifndef  SQL_TYPE_MAPPER_HPP
# define SQL_TYPE_MAPPER_HPP

# include <string>

namespace SQL
{
  template<typename T>
  struct TypeMapper
  {
    static std::string get_for_backend(const std::string& backend) { return ""; }
  };
}

#endif