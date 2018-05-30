#include <crails/front/router.hpp>
#include <crails/front/object.hpp>

using namespace Crails::Front;
using namespace std;
using namespace client;

Router::Router()
{
  window.addEventListener("hashchange", cheerp::Callback([this](Event* e) {
    on_hash_changed();
  }));
}

void Router::on_hash_changed()
{
  string hash = (string)(*window.get_location()->get_hash());

  if (hash != last_hash)
  {
    last_hash = hash;
    if (hash[0] == '#')
      hash = hash.substr(1, hash.size() - 1);
    navigate(hash);
  }
}

bool Router::navigate(const string& path, bool trigger)
{
  if ((string)(*window.get_location()->get_hash()) != path)
    window.get_location()->set_hash(path.c_str());
  if (trigger)
  {
    Params params;
    const Action* action = get_action("", path, params);

    if (action)
    {
      (*action)(params);
      on_route_executed.trigger(path);
      return true;
    }
    else
      on_route_not_found.trigger(path);
  }
  return false;
}

string Router::get_current_path() const
{
  Crails::Front::Object location_hash = window.get_location()->get_hash();
  string result = location_hash;

  result.erase(result.begin()); // remove the sharp character
  return result;
}

void Router::initialize()
{
  navigate(get_current_path(), true);
}
