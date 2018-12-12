#include <crails/front/mvc/controller.hpp>
#include <crails/front/globals.hpp>
#include <crails/front/exception.hpp>

using namespace std;
using namespace Crails::Front;

map<string, shared_ptr<Layout> > Controller::layouts;
string                           Controller::current_layout;

void Controller::render(shared_ptr<View> view)
{
  if (current_layout.length() > 0)
  {
    auto layout = layouts.at(current_layout);

    view->render();
    layout->set_current_view(view);
  }
}

void Controller::detach_layout()
{
  if (current_layout.length() > 0)
  {
    auto layout = layouts.at(current_layout);

    layout->destroy();
    current_layout = "";
  }
}

void Controller::attach_layout(const string& name)
{
  const Element& layout = *(layouts.at(name));

  body->appendChild(*layout);
  current_layout = name;
}

void Controller::set_layout(const string& name)
{
  if (current_layout != name)
  {
    detach_layout();
    if (layouts.count(name) > 0)
      attach_layout(name);
    else
      Crails::raise(std::runtime_error("no such layout `" + name + '`'));
  }
}
