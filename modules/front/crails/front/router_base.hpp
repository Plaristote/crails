#ifndef  ROUTER_BASE_HPP
# define ROUTER_BASE_HPP

# include <functional>
# include <vector>
# include <string>
# define ROUTER_PARAM_PATTERN "[a-zA-Z0-9_-]*"
# include <crails/front/object.hpp>

namespace Crails
{
  template<typename PARAMS, typename ACTION>
  class RouterBase
  {
  public:
    typedef ACTION Action;
    typedef Crails::Front::ObjectImpl<client::RegExp> JsRegExp;

    struct Item
    {
      Item() : regexp(new client::RegExp("")) {}

      Action                   run;
      std::string              method;
      std::string              regexp_src, regexp_original;
      JsRegExp                 regexp;
      std::vector<std::string> param_names;
    };

    typedef std::vector<Item> Items;

    const Action* get_action(const std::string& method, const std::string& uri, PARAMS& query_params) const
    {
      using namespace std;
      Crails::Front::String js_uri(uri);

      for (const Item& item : routes)
      {
        bool does_match = item.regexp->test(*js_uri);

        if (does_match)
        {
          auto matches = Crails::Front::Object(js_uri->match(*item.regexp)).to_vector<std::string>();

          for (unsigned short i = 1 ; i < matches.size() ; ++i)
          {
            const string& param_name  = item.param_names[i - 1];
            const string& param_value = matches[i];

            query_params[param_name] = param_value;
          }
          return &(item.run);
        }
      }
      return 0;
    }

    void      match(const std::string& route, Action callback)
    {
      match("", route, callback);
    }

    void      match(const std::string& method, const std::string& route, Action callback)
    {
      Item item;

      item_initialize_regex(item, '^' + route + '$');
      item.method = method;
      item.run    = callback;
      routes.push_back(item);
    }

  private:
    void      item_initialize_regex(Item& item, const std::string& route)
    {
      using namespace std;
      string   find_params_str(':' + std::string(ROUTER_PARAM_PATTERN));
      JsRegExp find_params(new client::RegExp(find_params_str.c_str()));
      string   regexified_route;
      auto     matches = Crails::Front::Object(
        Crails::Front::String(route)->match(*find_params)
      ).to_vector<string>();
      size_t   last_position = 0;

      for (unsigned short i = 0 ; i < matches.size() ; ++i)
      {
        const string& match = matches[i];
        size_t match_position = route.find(match);

        regexified_route += route.substr(last_position, match_position - last_position);
        regexified_route  = regexified_route + '(' + ROUTER_PARAM_PATTERN + ')';
        item.param_names.push_back(route.substr(match_position + 1, match.length() - 1));
        last_position = match_position + match.length();
      }
      regexified_route += route.substr(last_position);
      item.regexp_src = regexified_route;
      item.regexp_original = route;
      item.regexp = new client::RegExp(regexified_route.c_str());
    }

  public:
    Items routes;
  };
}

#endif

