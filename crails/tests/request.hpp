#ifndef  CRAILS_TESTS_REQUEST_HPP
# define CRAILS_TESTS_REQUEST_HPP

# include "../router.hpp"
# include "../params.hpp"

namespace Crails
{
  namespace Tests
  {
    class Request
    {
    public:
      Request(const std::string& method, const std::string& uri)
      {
        params["method"] = method;
        params["uri"]    = uri;
      }

      void run()
      {
        const Router* router = Router::singleton::Get();
        if (router)
        {
          const Router::Action* action = router->get_action(params["method"].Value(), params["uri"].Value(), params);

          if (action != 0)
            response.Duplicate((*action)(params));
        }
        else
          std::cerr << "Router not initialized" << std::endl;
      }

      Params    params;
      DynStruct response;
    private:
    };
  }
}

#endif
