#ifndef  DATABASE_CONTROLLER_HPP
# define DATABASE_CONTROLLER_HPP

# include <crails/controller.hpp>
# include "connection.hpp"

namespace ODB
{
  class Controller : public Crails::Controller
  {
  protected:
    ODB::Connection database;

  public:
    Controller(Crails::Context& params);

    void finalize() override;
  };
}

#endif
