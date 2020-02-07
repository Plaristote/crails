#include <memory>
#include <mongocxx/instance.hpp>

using namespace std;

namespace MongoDB
{
  unique_ptr<mongocxx::instance> instance;

  void initialize()
  {
    instance = unique_ptr<mongocxx::instance>(new mongocxx::instance);
  }

  void finalize()
  {
    instance.reset();
  }
}
