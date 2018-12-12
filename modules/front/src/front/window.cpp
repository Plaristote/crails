#include <crails/front/window.hpp>
#include <crails/front/globals.hpp>

using namespace Crails::Front;
using namespace std;

Crails::Front::Window Crails::Front::window;

Window::Window() : ObjectImpl((client::Window*)&client::window), events(make_shared<JavascriptEvents>((client::EventTarget*)&client::window))
{
  events->on("resize", [this]() { resized.trigger(); });
}
