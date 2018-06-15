#ifndef  DB_MODEL_TIMESTAMPS_HPP
# define DB_MODEL_TIMESTAMPS_HPP

# include "soft_delete.hpp"
# include <ctime>

namespace ODB
{
  #pragma db object abstract
  class ModelTimestamps : public ModelSoftDelete
  {
    friend class odb::access;
    typedef ModelSoftDelete BaseType;
  public:
    std::time_t get_created_at() const { return created_at; }
    std::time_t get_udpated_at() const { return updated_at; }
    void update_timestamps();

    void save(odb::database&);
    void destroy(odb::database&);

  private:
    #pragma db null
    std::time_t created_at;
    #pragma db null
    std::time_t updated_at;
  };
}

#endif
