# Views
Views in Crails are dynamically loaded libraries. In debug mode, they're reloaded at each requests (while in release mode, they're loaded once, the first time they're needed, and are only released when the server stops).
The compilation is accomplished using the `gulp` command (gulp also compiles coffeescript, javascript and scss assets). It
also watches for change and re-compiles when necessary.

The views are written in ecpp, an erb-like template format:
- they may share variables along with the controller that called them (using the `SharedVars vars` attribute of the controller classes).
- they use a few syntaxic features to allow the use of C++ helpers

# Writing a view
Views templates in Crails use a syntax similar to Ruby's ERB, with some tricks to make it more accessible to C++
development.
Views are stored in the /app/views folder, and the file extension is 'ecpp' (for an index html render, you would
call your file index.html.ecpp).

The templates are divided in two parts: 

- The first one is called the Linking part. You can use it to include your
headers, declare variable or write code like you would in any regular C++ code. It ends when the line // END LINKING is met.
- The second one is the actual view template, much like ERB, but with C++.

This is what a view template looks like:

      string* @some_string;
      // END LINKING
      <h2>Some page</h2>
      <% if (some_string != NULL) do %>
        <%= *some_string %>
      <% end else do %>
        some_string was null !
      <% end %>

You proably noticed that we use words instead of braces. For readability purpose, in Crails views templates, you have the
possibility to replace { and } with do and end respectively.

You must also note that code between <% %> will not output anything. However, code between <%= %> will, and the return value
of the expression between these symbols must be a type handled by std's streams.

There's also something quite interesting happening in the first line. As you can see, we declare a pointer on a string and
name it 'some_string'. The interesting part is the rvalue, which start with the character '@'.

When you render views in Crails, you need to pass them a SharedVar object containing all the variables that must be shared
between the controller, the layout and the partials that will be rendered. This '@' is how you get these variables back
inside a template.
If the variable wasn't stored in the SharedVar object, the pointer will be NULL, so don't forget to check that value to
avoid unnecessary segmentation faults.

## Render a view from a controller
Here are a few demonstrations of how a view can be rendered from a controller:

    void MyController::index()
    {
      // Will render the ecpp view within the default layout (layouts/application.html.ecpp)
      render("mycontroller/index.html.ecpp");
    }
    
    void MyController::index()
    {
      // Will render the ecpp view within the specified layout (layouts/mylayout.html.ecpp)
      render("mycontroller/index.html.ecpp", "layouts/mylayout");
    }
    
    void MyController::index()
    {
      // Will render the ecpp view without using any layouts
      render("mycontroller/index.html.ecpp", "");
    }

    void MyController::index()
    {
      string some_string = "some string was *not* null";

      *vars["some_string"] = &some_string; // Will make 'some_string' visible from the ECPP template
      render("mycontroller/index.html.ecpp", "layouts/mylayout");
    }

## Using helpers within a view
The folder `app/helpers` is supposed to host functions that you would want to use in your views in order to factorize your HTML. Here is an exemple using two helpers ('breadcrumb' and 'box'):

      #include "helpers/bootstrap.hpp"
      string* @some_string;
      // END LINKING
      <%= breadcrumb({ { "Home", "/home" } { "MyController", "/my_controller" } }) %>
      
      <%= box("Box Title", "info-sign") yields %>
        <h2>Some page</h2>
        <% if (some_string != NULL) do %>
          <%= *some_string %>
        <% end else do %>
          some_string was null !
        <% end %>
      <% yend %>

The first helper is pretty easy to understand: it's a simple function taking a map<string,string> object as a parameter.

The second helper use the "yields" syntax, which allows you to render ECPP inside of a helper. A "yielding" helper is a function that takes a lambda as a last parameter. If you want the lambda to render ECPP code, you should not declare the lambda parameter, and instead append the function call with the "yields" clause. Note that the created block won't end with the "end" word, as usual, but with the "yend" word (yield ends).

Now, this is how such helpers could look like in "helpers/bootstrap.hpp"

        // Breadcrumb Helper
        std::string breadcrumb(std::map<std::string, std::string> elements)
        {
          std::stringstream html;
        
          html << "<div class='breadcrumb'>";          
          for (auto it = elements.begin() ; it != elements.end() ; ++it)
            html << "<a href='" << it->second << "'>" << it->first << "</a>";
          html << "</div>";
          return (html.str());
        }
        
        // Box Helper
        std::string box(std::string title, std::string icon, std::function<std::string ()> yield)
        {
          std::stringstream html;
          
          html << "<div class='box'>;
          html <<   "<h2><i class='glyphicon glyphicon-" << icon << "'></i> " << title << "</h2>";
          html <<   "<div class='content'>";
          html <<     yield(); // Append the rendered ECPP to the stream
          html <<   "</div>";
          html << "</div>";
          return (html.str());
        }
