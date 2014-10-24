# MongoDB
## Configure your project for MongoDB
There are two ways to get MongoDB support into your crails project:

If you haven't created the application yet, then use the `--use-mongodb` option provided by the `crails create` command.

If the application already exists, then you need to add an option to `CmakeLists.txt`:

      option(USE_MONGODB "enables mongodb support" ON)

If the option was previously set to `OFF`, you MUST remove the `build/CMakeCache.txt` file after you've done it.

## Writing a model

```C++
#ifndef  USER_HPP
# define USER_HPP
        
# include <crails/mongodb/model.hpp>
# include <crails/mongodb/resultset.hpp>
# include <crails/password.hpp>
        
class User : public MongoDB::Model
{
  // MONGODB_MODEL generates constructors, destructor, and the Find and Create methods.
  MONGODB_MODEL(User);
  // MONGODB_FIELD generates a getter and a setter (respectively `type name() const` and `void set_name(type)`)
  MONGODB_FIELD(std::string, name,     "")
  MONGODB_FIELD(Password,    password, "")
};
```
## Create, update, delete
```C++
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
```

## Relations
Relations connect your models together, greatly simplifying the task of querying objects that are related to each other.

### Has many
A one-to-many relationship is represented using the MONGODB_HAS_MANY macro:
```C++
  class User : public MongoDB::Model
  {
    MONGODB_MODEL(User);
    MONGODB_FIELD(std::string, name,     "")
    MONGODB_FIELD(Password,    password, "")
    MONGODB_HAS_MANY(Comment, user)
  };
```

MONGODB_HAS_MANY takes two parameters: the type of the related model, and the name of the "inverse". In this example,
we create a case were MONGODB_HAS_MANY will look for a `user_id` field in the `Comment` model that matches the id of
the current object.

### Belongs to
Belongs to is the reverse of a "has_many" relationship. It helps you manage the field.
```C++
  class Comment : public MongoDB::Model
  {
    MONGODB_MODEL(Comment);
    MONGODB_FIELD(std::string, message,  "")
    MONGODB_BELONGS_TO(User)
  };
```

This helper will create the methods `void set_user(const User&)` and `User user()`.
You may also customize the name of the relationship, like this:

```C++
  class User : public MongoDB::Model
  {
    MONGODB_MODEL(User);
    MONGODB_FIELD(std::string, name,     "")
    MONGODB_FIELD(Password,    password, "")
    MONGODB_HAS_MANY(Comment, author)
  };

  class Comment : public MongoDB::Model
  {
    MONGODB_MODEL(Comment);
    MONGODB_FIELD(std::string, message,  "")
    MONGODB_BELONGS_TO_AS(User, author)
  };
```

Now, the generated methods will be `void set_author(const User&)` and `User author()`, and the field in the `comments`
collection will have an `author_id` field instead of the `user_id` field.

### Has and belongs to many
