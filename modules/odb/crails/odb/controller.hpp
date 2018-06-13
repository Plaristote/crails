#ifndef  DATABASE_CONTROLLER_HPP
# define DATABASE_CONTROLLER_HPP

# include <crails/controller.hpp>
# include "connection.hpp"

namespace Odb
{
  class Controller : public Crails::Controller
  {
  protected:
    Db::Connection database;

  public:
    Controller(Crails::Params& params);

    void finalize();
  };
}

#endif
