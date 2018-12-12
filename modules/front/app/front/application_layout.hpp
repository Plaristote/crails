#ifndef  FRONT_APPLICATION_LAYOUT_HPP
# define FRONT_APPLICATION_LAYOUT_HPP

# include <crails/front/mvc/layout.hpp>

class ApplicationLayout : public Crails::Front::Layout
{
  Crails::Front::Element content;
public:
  Crails::Front::Element get_content() const { return content; }

  ApplicationLayout()
  {
    using namespace Crails::Front;

    // The content attribute will be the container for the views attached to this layout
    content = Element("section");
    content.add_class("main-content");

    // Set up the application layout
    inner({
      Element("header").inner({
        Element("h1").text("Crails Front Application")
      }),
      content
    });
  }
};

#endif
