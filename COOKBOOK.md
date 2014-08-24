Crails Cookbook
=====
This tutorial is intented to show you step by step how to create a Crails application from scratch.

# Create a project
The first step to use the Crails Framewrok after having it installed is to use the create script to generate all
the files your project might need.

    crails create myapplication --use-sql --use-mongodb --debug-mode

This command creates a Crails appliatin in the directory myapplication.
It needs to be build and run from the buid directory.

You can use crails create -h to list the options.

# Configure a project
## Databases
The file conf/db.json is used to load and connct the prope databases.

      {
        "mongodb": {
          "type": "mongodb",
          "host": "127.0.0.1",
          "database": "crails_db"
        },
      
        "mysql": {
          "type": "sql",
          "host": "127.0.0.1",
          "database": "crails_db",
          "user": "root"
        }
      }

The default conf file looks like this.
The keys to the first objects are the name of your dbs which you will use to get a handle of them at runtime.

Notes:
- Authentication with mongodb is not yet ready.
- The SQL module only support MySQL for now. SQLite and PostgreSQL support is planned.

# Write a controller
This is what a Crails controller looks like:

    #ifndef  CRM_ACCOUNTS_HPP
    # define CRM_ACCOUNTS_HPP
    
    # include <crais/appcontroller.hpp>
    
    class CrmAccountsController : public ControllerBase
    {
    public:
      CrmAccountsController(Params& params) : ControllerBase(params)
      {
      }
    
      void index  ();
      void show   ();
      void _new   ();
      void create ();
      void edit   ();
      void update ();
      void _delete();
    };
    
    #endif

Some explanations about what you're seeing right now:
First of all, the crail/appcontroller.hpp is a header that includes pretty much everything needed to write contollers
with Crail: (DynStruct, Params, ControllerBase, SharedVars and more).

The controller class must inherits ControllerBase, which will implement some default behaviors (by default,
no exception catcher or filters are set, but you can override these behaviors).
Since the server might run in asynchronous mode, you must also be careful not to use any global variables or static
attibutes unless you know what you're doing (the Boots lib implements some asynchronous development tools that you
might wanna use if you need global stuff. Check out Boots/Sync/semaphore.hpp and Boots/Sync/mutex.hpp).

## Controller method
We'll now see how to implement a controller method and how to link it to a route.

      void CrmAccountsController::show()
      {
        std::string body;
    
        body  = "Hello World<br />";
        body += params["id"].Value(); // params is a protected reference to the request's params
        response["headers"]["Content-Type"] = "text/html"; // use this to set the response's headers, default is text/html
        response["body"]                    = body;        // use this to set the response's body
      }

Here's a not-quite-minimal method for a controller.

Let's first talk about the DynStruct and Params objets. They are quite similar since Params inheits DynStruct,
however Params implements other tools (for handling file upload for instance).

### DynStruct
DynStruct is an object that serializes/unserializes data in text mode. Any node from the root of the DynStruct can
contain a value or a set of other nodes. Unexisting nodes are created on the fly and garbage collected when they go
out of scope. This means you can do stuff like this:

      if ((render_data["whatever"]["something"]["something_else"].Nil()))
        std::cout << "This node does not exist" << std::endl;

If you set a value on an unexisting node, it will be saved along with all the unexisting parent nodes. This means that
this is a correct use of the DynStruct:

      render_data["nonexisting-node"]["akey"] = "Fuck yeah";

They also support any type that is suppoted by std streams. So this s correct as well:

      render_data["key"] = 42.f;

It also automatically cast to the expected type:

      unsigned int number = render_data["key"];

I have however experienced compilation issues with using this method with string and MSVC, so the correct way to get
a string value from a DynStruct node is actually this (if you wish to be compliant with something else than GCC):

      std::string mystring = render_data["key"].Value();

### Returning DynStruct
Your controller method must always return a DynStruct.
The DynStruct will be used to genrate the HTTP response. The body node (render_data["body"]) must contain the body
of the response.
Status code and setting headers / mimetypes is not yet supported, but it's in the works.

### Params
The param object is also a DynStruct, but it contains everything you need to know about the client's request:
All the GET, form, multipart and routing parameters are stored at the root of the params object.
Say your route is /crmacconts/:id, you would access the id parameter by using params["id"].
There's also an header node (params["headers"]) containing the header parameters of the request.

### Session
The session is managed directly by Crails. It might not be a good idea to send "Set-Cookie" headers, since Crails is going
to handle those on its own.
The session variable can be accessed through the Params object. For instance, if you wish to record the ID of a connected user:

        params.Session()["current_user"] = "user_id";
        
And if you wish to remove it:

        params.Session()["current_user"].Remove();

### Flash
Flash is a particular part of the session which gets erased at the beginning of each request. It's a way of communicating
between one request and the next one.
For instance, to record a notification that needs to be displayed to an user:

        void SessionsController::destroy()
        {
          params.Session()["current_user"].Remove();
          params.Session()["flash"]["info"] = "You've been disconnected"; // Sets a message into flash
          RedirectTo(params["headers"]["Referer"]); // Redirects to previous page
        }

The flash variables aren't accessed the same way they're set. If you want to display the message:

        void SessionsController::show()
        {
          std::cout << flash["info"].Value() << std::endl; // displays the message
        }

## File Upload
Uploaded files are exposed to the developer in the Param object. You can access them using the Upload method:

      const Params::File* Upload(const std::string& key) const;

The returned File struct implements the following attributes:

      struct File
      {
        std::string key;            // attribute 'name' in the file input html field
        std::string temporary_path; // absolute path where the file was uploaded
        std::string name;           // original name of the file
        std::string mimetype;       // mimetype
      };

Here's an example of contoller method using uploaded files:

      #include <Boots/Utils/directory.hpp> // For filesystem operations

      void MyController::file_upload()
      {
        const Params::File* file = params.upload("file_upload[upfile]");
        std::stringstream   html;

        // If a file was uploaded, display it
        if (file)
        {
          std::string display_file;
          
          Filesystem::FileContent(file->temporary_path, display_file);
          html << "<pre>" << display_file << "</pre>";
        }
        // Display the form to upload files
        html << "<form mehod='post' action='/file_upload' enctype='multipart/form-data'>";
        html << "<input type='file' name='file_upload[upfile]' /><br />";
        html << "<input type='submit' value='Upload' />";
        html << "</form>";
        render(HTML, html.str());
      }

This controller method will generate an upload form. If the query contains an uploaded file, it will be
displayed at the top of the page.
Create the routes with:

      SetRoute("GET",  "/file_upload", MyController, file_upload)
      SetRoute("POST", "/file_upload", MyController, file_upload)

And test it by uploading text files.

# Write a view
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

# Scaffolding
Scaffolding is a way of getting setting up a code base for new elements you wish to add to your application.
Scaffolding scripts support generation of controllers, models, views, layouts.

## Layouts
All applictions need layout in which your views will be rendered. Crails incorporate some scripts to download and
generate whatever files you need to start a new layout.
It uses an HTML Engine system which currntly supports Bootstrap and HTML Kickstart.

Say you want to start your brand new website with a kickstart theme, you would type the following:

      crails generate/layout kickstart application install

The command is composed of three parameters:
- HTML Engine to use (currently supported: 'bootstrap' and 'kickstart' for Twitter's Bootstrap and HTML Kickstart)
- Layout's name
- Command (install / uninstall / nothing)

The command 'intall' will ensure that kickstart requirements (js, css and pictures) are already installed. If you do not
specify this parameter, the script will simply generate a layout without cheking the requirements.

The layout's name will be used to create the file. In our use case, the file will be:
/app/views/layouts/application.html.ecpp

## Views

## Controller

## Model

## All at once

# Notes on asset precompile
Note: CSS and SCSS assets aren't supported yet, but it's planned.

Asset precompilation use coffeescript and uglifyJS. You'll need nodejs to install them.
Coffeescript is not mandatory unless you have coffee files in your app/assets/javascripts folder.
UglifyJS is not used in debug mode.
So if you're in debug mode and don't use coffeescript, you needn't worry about nodejs.
