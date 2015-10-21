#include "crails/sql/type_mapper.hpp"
#include <map>

namespace SQL
{
  template<>
  struct TypeMapper<std::string>
  {
    static std::string get_for_backend(const std::string& backend)
    {
      static std::map<std::string, std::string> mapper = {
        { "mysql",      "VARCHAR(256)" },
        { "postgresql", "VARCHAR(256)" },
        { "sqlite3",    "TEXT" }
      };

      return mapper[backend];
    }
  };

  template<>
  struct TypeMapper<int>
  {
    static std::string get_for_backend(const std::string& backend) { return "INTEGER"; }
  };

  template<>
  struct TypeMapper<unsigned int>
  {
    static std::string get_for_backend(const std::string& backend) { return "INTEGER"; }
  };

  template<>
  struct TypeMapper<double>
  {
    static std::string get_for_backend(const std::string& backend)
    {
      static std::map<std::string, std::string> mapper = {
        { "mysql",      "DOUBLE" },
        { "postgresql", "double" },
        { "sqlite3",    "REAL" }
      };

      return mapper[backend];
    }
  };
}