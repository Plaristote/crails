Crails Cookbook
=====
This tutorial is intented to show you step by step how to create a Crails application from scratch.

## Create a project
The first step to use the Crails Framewrok after having it installed is to use the create script to generate all
the files your project might need.

    crails create myapplication --use-sql --use-mongodb --debug-mode

This command creates a Crails appliatin in the directory myapplication.
It needs to be build and run from the buid directory.

You can use crails create -h to list the options.
Right now the optios use sql and mongodb are actually not optional. Your project won't compile without both.

## Configure a project
# Databases
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

### Write a controller
This is what a Crails controller looks like:

    #ifndef  CRM_ACCOUNTS_HPP
    # define CRM_ACCOUNTS_HPP
    
    # include <crais/appcontroller.hpp>
    
    class CrmAccountsController : public ControllerBase
    {
    public:
      static DynStruct index (Params& params)
      static DynStruct show  (Params& params);
      static DynStruct new   (Params& params);
      static DynStruct create(Params& params);
      static DynStruct edit  (Params& params);
      static DynStruct update(Params& params);
      static DynStruct delete(Params& params);
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

# DynStruct
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

  unsigned int numbe = render_data["key"];

I have however experienced compilation issues with using this method with string and MSVC, so the correct way to get
a string value from a DynStruct node is actually this (if you wish to be compliant with something else than GCC):

  std::string mystring = render_data["key"].Value();

# Returning DynStruct
Your controller method must always return a DynStruct.
The DynStruct will be used to genrate the HTTP response. The body node (render_data["body"]) must contain the body
of the response.
Status code and setting headers / mimetypes is not yet supported, but it's in the works.

# Params
The param object is also a DynStruct, but it contains everything you need to know about the client's request:
All the GET, form, multipart and routing parameters are stored at the root of the params object.
Say your route is /crmacconts/:id, you would access the id parameter by using params["id"].
There's also an header node (params["headers"]) containing the header parameters of the request.

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

### Write MongoDB based model

### Write a MySQL based model

### Write a view

### Notes on asset precompile
Note: CSS and SCSS assets aren't supported yet, but it's planned.

Asset precompilation use coffeescript and uglifyJS. You'll need nodejs to install them.
Coffeescript is not mandatory unless you have coffee files in your app/assets/javascripts folder.
UglifyJS is not used in debug mode.
So if you're in debug mode and don't use coffeescript, you needn't worry about nodejs.
