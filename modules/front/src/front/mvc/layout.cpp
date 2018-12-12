#include <crails/front/mvc/layout.hpp>

using namespace std;
using namespace Crails::Front;

void Layout::set_current_view(shared_ptr<View> view)
{
  if (current_view != nullptr)
    current_view->destroy();
  current_view = view;
  get_content() > (*current_view);
  current_view->on_attached();
}

