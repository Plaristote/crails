#include <crails/front/http.hpp>

using namespace std;
using namespace Crails::Front;
using namespace Crails::Front::Http;

/*
 * Response
 */
Response::Response()
{
}

void Response::initialize(client::XMLHttpRequest* xhr)
{
  status = xhr->get_status();
  if (xhr->get_responseText())
  {
    Object object(xhr->get_responseText());

    _has_body = true;
    body      = (std::string)(object);
  }
}

DataTree Response::get_response_data() const
{
  DataTree tree;

  if (_has_body)
    tree.from_json(body);
  return tree;
}

/*
 * Request
 */
Request::Request(const string& m, const string& p) : method(m), path(p)
{
  response = make_shared<Response>();
  xhr      = new client::XMLHttpRequest;
  xhr->open(method.c_str(), path.c_str());
}

void Request::set_headers(const map<string, string> headers)
{
  for (const auto header : headers)
    xhr->setRequestHeader(header.first.c_str(), header.second.c_str());
}

void Request::set_body(const std::string& value)
{
  body = value;
}

static client::EventListener* request_on_load(
  client::XMLHttpRequest* xhr,
  shared_ptr<Response>    response,
  function<void()>        resolve,
  function<void()>        reject)
{
  return cheerp::Callback([xhr, response, resolve, reject]()
  {
    if ((int)xhr->get_readyState() == 4)
    {
      response->initialize(xhr);
      resolve();
    }
  });
}

Promise Request::send()
{
  return Promise([this](function<void()> resolve, function<void()> reject)
  {
    xhr->set_onload(request_on_load(xhr, response, resolve, reject));
    if (body.length() > 0)
    {
      Crails::Front::String js_string(body.c_str());

      xhr->send(*js_string);
    }
    else
      xhr->send();
  });
}
