# MongoDB
## Configure your project for MongoDB
There are two ways to get MongoDB support into your crails project:

If you haven't created the application yet, then use the `--use-mongodb` option provided by the `crails create` command.

If the application already exists, then you need to add an option to `CmakeLists.txt`:

      option(USE_MONGODB "enables mongodb support" ON)

If the option was previously set to `OFF`, you MUST remove the `build/CMakeCache.txt` file after you've done it.

## Writing a model

        #ifndef  USER_HPP
        # define USER_HPP
        
        # include <crails/mongodb/model.hpp>
        
        class User : public MongoDB::Model
        {
          // MONGODB_MODEL generates constructors, destructor, and the Find and Create methods.
          MONGODB_MODEL(User);
          // MONGODB_FIELD generates a getter and a setter (respectively `type name() const` and `void set_name(type)`)
          MONGODB_FIELD(std::string, name,     "")
          MONGODB_FIELD(Password,    password, "")
        };

## Create, update, delete

        void UserController::create()
        {
          if (params["user"]["name"].Value()     != "" &&
              params["user"]["password"].Value() != "" &&
              password_matches_confirmation())
          {
            auto user = User::Create(params["user"]);

            user.Save();
          }
        }
        
        void UserController::update()
        {
          // Note that the SmartPointer throws an exception when trying to use null pointers.
          SmartPointer<User> user     = User::Find(params["id"]);
          bool               success = true;

          if (params["user"]["name"].Value() != "")
            user->set_name(params["user"]["name"]);
          if (params["user"]["password"].Value() != "")
          {
            if (password_matches_confirmation())
              user->set_password(params["user"]["password"]);
            else
              success = false;
          }
          if (success)
            user->save();
        }
        
        void UserController::delete()
        {
          SmartPointer<User> user = User::Find(params["id"]);

          user->Remove();
        }
        
        bool UserController::password_matches_confirmation()
        {
          return (params["password"].Value() && params["password-confirm"].Value());
        }

## Relations
### Has many
### Belongs to
### Has and belongs to many
