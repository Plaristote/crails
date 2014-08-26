# Controllers
Crails controllers are located in `app/controllers`. They inherits the ControllerBase class, which allows the Router
to interact with them, and brings a few features along (csrf security, view rendering features, redirecting methods,
and exception filtering).

They are compiled along with the application, which means you must recompile and restart the server at every new revision.

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
First of all, the crails/appcontroller.hpp is a header that includes pretty much everything needed to write contollers
with Crail: (DynStruct, Params, ControllerBase, SharedVars and more).

The controller class must inherits ControllerBase, which will implement some default behaviors (by default,
no exception catcher or filters are set, but you can override these behaviors).
Since the server might run in asynchronous mode, you must also be careful not to use any global variables or static
attibutes unless you know what you're doing (the Boots lib implements some asynchronous development tools that you
might wanna use if you need global stuff. Check out Boots/Sync/semaphore.hpp and Boots/Sync/mutex.hpp).

## Router
The `Router::Initialize()` method located in the `app/router.cpp` file: this is where you explain Crails in which
controllers and which methods each HTTP requests should head to.
Let's see what it looks like:

      #include <crails/router.hpp>
      #include "controllers/crm_accounts.hpp" // don't forget to include your controllers

      void Router::Initialize(void)
      {
        SetRoute("GET",    "/crm_accounts",      CrmAccountsController, index);
        SetRoute("GET",    "/crm_accounts/new",  CrmAccountsController, new);
        SetRoute("GET",    "/crm_accounts/edit", CrmAccountsController, edit);
        SetRoute("GET",    "/crm_accounts/:id",  CrmAccountsController, show);
        SetRoute("DELETE", "/crm_accounts/:id",  CrmAccountsController, _delete);
        SetRoute("PUT",    "/crm_accounts/:id",  CrmAccountsController, update);
        SetRoute("POST",   "/crm_accounts",      CrmAccountsController, create);
      }
      
SetRoute takes four parameters: the request's type, the URL matcher, the controller's name and the method's name.
The routes are checked in the same order you wrote them in. This means that when you're using variables (`:id` is the parameter in this exemple), you must make sure any route that might match the variable (in this exemple, `/crm_accounts/new` and `/crm_accounts/update`) is specified before the one using a variable.

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

## CSRF Security
CSRF is a system that use our encrypted cookie to authentify requests and guarantee the origin of POST, PUT and
DELETE requests. An exception is thrown when the request cannot be identified.

Crails takes care of generating CSRF tokens (using a salt value, generated in `config/salt.cpp`), and storing it in
the encrypted cookie. From the views, the token is inserted in a <meta> element, and inserted in forms using the `public/js/crails.js` script.

Here's what an ecpp template must include to make the csrf authentication work:

        #include <crails/params.hpp>

        Params* @params;
        string* @yield;
        // END LINKING
        <!DOCTYPE html>
        <html>
        <head>
            <!-- insert the csrf token in the view' --!>
            <meta name="csrf-token" content="<%= params->Session()["csrf_token"].Value() %>"/>
            <!-- include the crails.js script --!>
            <script type="text/javascript" src="/js/crails.js"/></script>

### Disable CSRF Security
You may disable CSRF security by overloading the `virtual bool must_protect_from_forgery() const` method.
Example:

      class MyController : public BaseController
      {
        bool must_protect_from_forgery() const
        {
          return (false);
        }
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
