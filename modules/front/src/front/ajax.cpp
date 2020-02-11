#include <crails/front/ajax.hpp>

using namespace Crails::Front;
using namespace client;
using namespace std;

Crails::Signal<const Ajax&> Ajax::query_opened;
Crails::Signal<const Ajax&> Ajax::query_completed;
Crails::Signal<const Ajax&> Ajax::query_failed;

Ajax::Ajax() : xhr(0), _method("GET"), opened(false)
{
}

Ajax& Ajax::query(const string& url)
{
  Ajax* ajax = new Ajax;

  ajax->_url = url;
  return *ajax;
}

Ajax& Ajax::query(const string& method, const string& url)
{
  Ajax& ajax = Ajax::query(url);

  ajax._method = method;
  return ajax;
}

void Ajax::clear_transport()
{
  if (xhr)
  {
    xhr = nullptr; // xhr has a client type (client::XMLHttpRequest) and is managed by the JavaScript GC, no delete required
    opened = false;
  }
}

void Ajax::ensure_transport_exists()
{
  if (!xhr)
  {
    xhr = new XMLHttpRequest;
    xhr->set_onload(cheerp::Callback([this](Event*) { on_ready_state_changed(); }));
  }
}

void Ajax::open_transport()
{
  if (!opened)
  {
    ensure_transport_exists();
    xhr->open(_method.c_str(), _url.c_str());
    opened = true;
  }
}

Ajax& Ajax::headers(const map<string, string>& vals)
{
  open_transport();
  for (auto val : vals)
    xhr->setRequestHeader(val.first.c_str(), val.second.c_str());
  return *this;
}

void Ajax::operator()()
{
  open_transport();
  if (_data.size() > 0)
  {
    Crails::Front::String js_string(_data.c_str());

    xhr->send(*js_string);
  }
  else
    xhr->send();
  query_opened.trigger(*this);
}

void Ajax::on_ready_state_changed()
{
  switch ((int)xhr->get_readyState())
  {
  case 1: // OPENED
  case 2: // HEADERS_RECEIVED
  case 3: // LOADING
    if (_callbacks.progress)
      _callbacks.progress(*this);
    break;
  case 4: // DONE
    on_done();
  default:
    break;
  }
}

void Ajax::on_done()
{
  if (xhr->get_status() == 200)
  {
    if (_callbacks.success)
      _callbacks.success(*this);
  }
  else
  {
    if (_callbacks.error)
      _callbacks.error(*this);
    query_failed.trigger(*this);
  }
  if (_callbacks.complete)
    _callbacks.complete(*this);
  query_completed.trigger(*this);
  clear_transport();
  delete this;
}

void Ajax::clear_callbacks()
{
  _callbacks = Callbacks();
}

string Ajax::get_response_text() const
{
  Crails::Front::Object object(xhr->get_responseText());

  if (object.is_undefined())
    return "";
  return object;
}

Crails::Front::Object Ajax::get_response_as_json() const
{
  ObjectImpl<client::String> object(xhr->get_responseText());

  if (object.is_undefined())
    return object;
  return Crails::Front::Object::from_json(*object);
}
