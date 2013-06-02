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
      static DynStruct index  (Params& params);
      static DynStruct show   (Params& params);
      static DynStruct _new   (Params& params);
      static DynStruct create (Params& params);
      static DynStruct edit   (Params& params);
      static DynStruct update (Params& params);
      static DynStruct _delete(Params& params);
    };
    
    #endif

Some explanations about what you're seeing right now:
First of all, the crail/appcontroller.hpp is a header that includes pretty much everything needed to write contollers
with Crail: (DynStruct, Params, ControllerBase, SharedVars and more).

The controller class must inherits ControllerBase, which will implement some default behaviors (by default,
no exception catcher or filters are set, but you can override these behaviors).
All the mehods need to be static. Since the server might run in asynchronous mode, you must also be careful not to
use any global variables or static attibutes unless you know what you're doing (the Boots lib implements some
asynchronous development tools that you might wanna use if you need global stuff. Check out Boots/Sync/semaphore.hpp
and Boots/Sync/mutex.hpp).

## Controller method
We'll now see how to implement a controller method and how to link it to a route.

      DynStruct CrmAccountsController::show(Params& params)
      {
        DynStruct render_data;
        std::string body;
    
        body  = "Hello World<br />";
        body += params["id"].Value();
        render_data["body"] = body;
        return (render_data);
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

## Filters
There are two methods in the controller base class that can be overloaded to implement before and after filters
behaviours.

      static void      BeforeFilter(Params&);
      static DynStruct AfterFilter(DynStruct, Params&);

Note that the AfterFilter 

If you overload these methods, you MUST call the parent's class filter method, like this:

      static void      BeforeFilter(Params& params)
      {
        ControllerBase::BeforeFilter(params);
        if (params["header"]["Content-Type"] != "image/png")
          RedirectTo("/unsuported-file-format.html");
      }

      static DynStruct AfterFilter(DynStruct render_data, Params& params)
      {
        render_data["body"] = "content was after filtered !";
        return (ControllerBase::AfterFilter(render_data, params));
      }

Note that the order is important. In the before filter, we call the parent's method first, and in the
after filter, we call it last. You don't have to do it this way but it's highly recommended.

## Rescue From
You can also use macro to overload the global 'rescue from' of your contoller. This will allow you to catch
exceptions thrown in any routes implemented by your controller.

Here's how to implement a rescue from in you controller:

      class CrmAccountsController : public ControllerBase
      {
      public:
        static DynStruct index     (Params& params);
        
      RESCUE_FROM(render_data)
        catch (const std::exception e)
        {
          std::cout << "Rescue From catched exception: " << std::endl;
        }
        catch (const char* str)
        {
          std::cout << "Catched a string " << str << std::endl;
        }
      END_RESCUE_FROM
      };

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

      DynStruct MyController::file_upload(Params& params)
      {
        DynStruct           render_data;
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
        return (render_data);
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

      string* some_string = @some_string;
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
