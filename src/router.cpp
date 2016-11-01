#include "crails/router.hpp"
#include "crails/session_store.hpp"
#include "crails/http.hpp"
#include <memory>

using namespace std;
using namespace Crails;

const Router::Action* Router::get_action(const string& method, const string& uri, Params& query_params) const
{
  for (const Item& item : routes)
  {
    auto matches = sregex_iterator(uri.begin(), uri.end(), item.regexp);

    if ((item.method == "" || item.method == method) &&
         distance(matches, sregex_iterator()) > 0)
    {
      smatch match = *matches;

      for (size_t i = 1 ; i < match.size() ; ++i)
      {
        std::string param_name  = item.param_names[i - 1];
        std::string param_value = uri.substr(match.position(i), match.length(i));

        query_params[param_name] = param_value;
      }
      return &(item.run);
    }
  }
  return 0;
}

void Router::match(const std::string& route, Router::Action callback)
{
  Item item;

  item_initialize_regex(item, '^' + route + '$');
  item.run = callback;
  routes.push_back(item);
}

void Router::match(const std::string& method, const std::string& route, Router::Action callback)
{
  Item item;

  item_initialize_regex(item, '^' + route + '$');
  item.method = method;
  item.run    = callback;
  routes.push_back(item);
}

void Router::item_initialize_regex(Item& item, const std::string& route)
{
  regex find_params(":[a-zA-Z0-9_-]*", regex_constants::ECMAScript);
  string regexified_route;
  auto matches = sregex_iterator(route.begin(), route.end(), find_params);
  size_t last_position = 0;

  for (auto it = matches ; it != sregex_iterator() ; ++it)
  {
    smatch match = *it;

    regexified_route += route.substr(last_position, match.position(0));
    regexified_route += "([a-zA-Z0-9_-]*)";
    item.param_names.push_back(route.substr(match.position(0) + 1, match.length(0) - 1));
    last_position = match.position(0) + match.length(0);
  }
  regexified_route += route.substr(last_position);
  item.regexp = regex(regexified_route, regex_constants::ECMAScript);
}
