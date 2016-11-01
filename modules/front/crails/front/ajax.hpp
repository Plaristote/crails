#ifndef  CRAILS_AJAX_HPP
# define CRAILS_AJAX_HPP

# include <cheerp/client.h>
# include <cheerp/clientlib.h>
# include "signal.hpp"
# include <map>
# include <string>

namespace Crails
{
  namespace Front
  {
    class Ajax
    {
      Ajax(const Ajax&) = delete;
      Ajax();
    public:
      typedef std::function<void (const Ajax&)> AjaxCallback;

      struct Callbacks
      {
        AjaxCallback success, error, complete, progress;
      };

      static Signal<const Ajax&> query_opened;
      static Signal<const Ajax&> query_completed;
      static Signal<const Ajax&> query_failed;

      static Ajax& query(const std::string& url);
      static Ajax& query(const std::string& method, const std::string& url);

      void operator()();
      client::XMLHttpRequest* operator->() const { return xhr; }

      Ajax& headers(const std::map<std::string, std::string>&);
      Ajax& data(const std::string& val)       { _data = val; return *this; }
      Ajax& callbacks(Callbacks vals)          { _callbacks = vals; return *this; }
      Ajax& on_success(AjaxCallback callback)  { _callbacks.success  = callback; return *this; }
      Ajax& on_error(AjaxCallback callback)    { _callbacks.error    = callback; return *this; }
      Ajax& on_progress(AjaxCallback callback) { _callbacks.progress = callback; return *this; }
      Ajax& on_complete(AjaxCallback callback) { _callbacks.complete = callback; return *this; }

    protected:
      void on_ready_state_changed();
      void on_done();
      void clear_callbacks();
      void clear_transport();
      void ensure_transport_exists();
      void open_transport();

      client::XMLHttpRequest* xhr;
      std::string _method, _url, _data;
      Callbacks _callbacks;
      bool opened;
    };
  }
}

#endif
