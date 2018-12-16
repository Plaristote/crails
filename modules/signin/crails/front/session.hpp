#ifndef  FRONT_SIGNIN_SESSION_HPP
# define FRONT_SIGNIN_SESSION_HPP

# include <crails/front/signal.hpp>
# include <crails/front/ajax.hpp>
# include <crails/front/http.hpp>
# include <crails/front/cookies.hpp>
# include <crails/datatree.hpp>
# include <crails/odb/id_type.hpp>

namespace Crails
{
  namespace Front
  {
    template<typename DESC>
    class Session
    {
      typedef std::shared_ptr<typename DESC::UserType> UserPtr;
    public:
      Crails::Signal<void> on_connect, on_disconnect;

      Crails::Front::Promise connect(const std::string& login, const std::string& password)
      {
        auto request = Http::Request::post(DESC::path);
        DataTree params;

        params[DESC::login_key]    = login;
        params[DESC::password_key] = password;
        request->set_headers(get_headers());
        request->set_body(params.to_json());
        return request->send().then([this, request]()
        {
          auto response = request->get_response();

          if (response->ok())
            on_connect_success(*response);
          else
            on_connect_failure(*response);
        });
      }

      Crails::Front::Promise disconnect()
      {
        auto request = Http::Request::_delete(DESC::path);
        
        return request->send().then([this, request]()
        {
          auto response = request->get_response();
          
          if (response->ok())
            on_disconnect_success(*response);
        });
      }

      void get_current_user(std::function<void (UserPtr)> callback)
      {
        if (!current_user && is_connected())
          fetch_current_user(callback);
        else
          callback(current_user);
      }

      ODB::id_type get_current_user_id()
      {
        return cookies.get<ODB::id_type>("cuid");
      }

      UserPtr get_current_user() const
      {
        return current_user;
      }

      std::map<std::string, std::string> get_headers() const
      {
        return {
          {"Content-Type", "application/json"},
          {"Accept",       "application/json"}
        };
      }

      void set_current_user(UserPtr ptr) { current_user = ptr; }

      virtual void on_connect_success(const Http::Response& response)
      {
        DataTree data = response.get_response_data();
        time_t expires_in = data["expires_in"].as<std::time_t>();

        cookies.set("auth_token", data["auth_token"].as<std::string>(), expires_in);
        cookies.set("cuid",       data["cuid"].as<ODB::id_type>(),      expires_in);
        on_connect.trigger();
      }

      virtual void on_connect_failure(const Http::Response&)
      {
      }

      bool is_connected()
      {
        return cookies.has("auth_token") && cookies.has("cuid");
      }

      virtual void on_disconnect_success(const Http::Response&)
      {
        cookies.remove("auth_token");
        cookies.remove("cuid");
        current_user = nullptr;
        on_disconnect.trigger();
      }

      virtual void fetch_current_user(std::function<void (UserPtr)>) = 0;

    protected:
      UserPtr current_user;
      Cookies cookies;
    };
  }
}

#endif
