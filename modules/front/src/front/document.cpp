#include <iostream>
#include <crails/front/document.hpp>
#include <crails/front/globals.hpp>

using namespace Crails::Front;
using namespace std;

Crails::Front::Document Crails::Front::document;

Document::Document() : ObjectImpl(&(client::document))
{
}

void Document::on_ready(function<void ()> callback)
{
  std::string readyState = get<client::String>("readyState");

  if (readyState != "loading")
    callback();
  else
  {
    operator*()->addEventListener("DOMContentLoaded", cheerp::Callback([callback](client::Event* event_ptr)
    {
      callback();
    }));
  }
}
