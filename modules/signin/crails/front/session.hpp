#ifndef  FRONT_SIGNIN_SESSION_HPP
# define FRONT_SIGNIN_SESSION_HPP

# include <crails/front/signal.hpp>
# include <crails/front/ajax.hpp>
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

      void connect(const std::string& login, const std::string& password)
      {
        DataTree params;

        params[DESC::login_key]    = login;
        params[DESC::password_key] = password;
        Ajax::query("POST", DESC::path)
          .headers(get_headers())
          .data(params.to_json())
          .on_success(std::bind(&Session::on_connect_success, this, std::placeholders::_1))
          .on_error  (std::bind(&Session::on_connect_failure, this, std::placeholders::_1))
          ();
      }

      void disconnect()
      {
        Ajax::query("DELETE", DESC::path)
          .on_success(std::bind(&Session::on_disconnect_success, this, std::placeholders::_1))
          ();
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

      virtual void on_connect_success(const Ajax& ajax)
      {
        DataTree response(ajax.get_response_as_json());

        cookies.set("auth_token", response["auth_token"].as<std::string>());
        cookies.set("cuid",       response["cuid"].as<ODB::id_type>());
        on_connect.trigger();
      }

      virtual void on_connect_failure(const Ajax&)
      {
      }

      bool is_connected() const
      {
        return cookies.has("auth_token") && cookies.has("cuid");
      }

      virtual void on_disconnect_success(const Ajax&)
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
