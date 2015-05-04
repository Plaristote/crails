# MongoDB
## Configure your project for MongoDB
There are two ways to get MongoDB support into your crails project:

If you haven't created the application yet, then use the `--use-mongodb` option provided by the `crails create` command.

If the application already exists, then you need to make the following changes to the `CMakeLists.txt` file:

```cmake
# Add this line at the beginning of the file
set(crails_mongodb_dep mongoclient crails-mongodb)
# Add ${crails_mongodb_dep} to the call to `target_link_library`
target_link_libraries(crails-app
                  crails-core ${Boost_LIBRARIES} pthread dl crypto Boots ${CPPNETLIB_LIBRARIES}
                  ${crails_mongodb_dep})
```

## Writing a model

```C++
#ifndef  USER_HPP
# define USER_HPP
        
# include <crails/mongodb/model.hpp>
# include <crails/mongodb/resultset.hpp>
# include <crails/password.hpp>
        
class User : public MongoDB::Model
{
  // MONGODB_COLLECTION defines the names of the database (as specified in `conf/db.json`) and collection to use.
  MONGODB_COLLECTION(mongodb, users);
  // MONGODB_MODEL generates constructors, destructor, and the Find and Create methods.
  MONGODB_MODEL(User);
  // MONGODB_FIELD generates a getter and a setter (respectively `type name() const` and `void set_name(type)`)
  MONGODB_FIELD(std::string, name,     "")
  MONGODB_FIELD(Password,    password, "")
};
```

## Using models
We will now see how to use the model we just created by writing the code for a UserController.

### Find
```C++
void UserController::show()
{
  std::string id   = params["id"].Value();
  SP(User)    user = User::Find(id);
  
  if (user.Null())
  {
    response["status"] = 404;
    return ;
  }
  *shared_vars["user"] = user.Pointer();
  render("layouts/application", "users/show");
}
```

### Create
```C++
void UserController::create()
{
  if (params["user"]["name"].Value()     != "" &&
      params["user"]["password"].Value() != "" &&
      password_matches_confirmation())
  {
    auto user = User::Create(params["user"]);

    user.Save();
    *shared_vars["user"] = &user;
    render("layouts/application", "users/show");
  }
}
```

### Update
```C++
void UserController::update()
{
  SP(User) user    = User::Find(params["id"]);
  bool     success = true;

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
  {
    user->save();
    *shared_vars["user"] = user.Pointer();
    render("layouts/application", "users/show");
  }
  else
    response["status"] = 422;
}

bool UserController::password_matches_confirmation()
{
  return (params["password"].Value() && params["password-confirm"].Value());
}
```

### Delete
```C++
void UserController::delete()
{
  SP(User) user = User::Find(params["id"]);

  user->Remove();
}
```
        
## Relations
Relations connect your models together, greatly simplifying the task of querying objects that are related to each other.

### Has many
A one-to-many relationship is represented using the MONGODB_HAS_MANY macro:
```C++
  class User : public MongoDB::Model
  {
    MONGODB_COLLECTION(mongodb, users);
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
    MONGODB_COLLECTION(mongodb, comments);
    MONGODB_MODEL(Comment);
    MONGODB_FIELD(std::string, message,  "")
    MONGODB_BELONGS_TO(User)
  };
```

This helper will create the methods `void set_user(const User&)` and `SP(User) get_user() const`.
You may also customize the name of the relationship, like this:

```C++
  class User : public MongoDB::Model
  {
    MONGODB_COLLECTION(mongodb, users)
    MONGODB_MODEL(User)
    MONGODB_FIELD(std::string, name,     "")
    MONGODB_FIELD(Password,    password, "")
    MONGODB_HAS_MANY(Comment, author)
  };

  class Comment : public MongoDB::Model
  {
    MONGODB_COLLECTION(mongodb, comments)
    MONGODB_MODEL(Comment)
    MONGODB_FIELD(std::string, message,  "")
    MONGODB_BELONGS_TO_AS(User, author)
  };
```

Now, the generated methods will be `void set_author(const User&)` and `SP(User) get_author() const`, and the field in the `comments` collection will have an `author_id` field instead of the `user_id` field.

### Has and belongs to many

Has and belongs to many relationships are similar, but one of the model has to host the relationship.

```C++
  class Player : public MongoDB::Model
  {
    MONGODB_COLLECTION(mongodb, players)
    MONGODB_MODEL(Player)
    MONGODB_HAS_AND_BELONGS_TO_MANY(Game,game,player) // typename, relation name, foreign relation name
  }

  class Game : public MongoDB::Model
  {
    MONGODB_COLLECTION(mongodb, games)
    MONGODB_MODEL(Game)
    MONGODB_HAS_AND_BELONGS_TO_MANY_HOST(Player,player) // typename, relation name
  }
```

The generated methods will be:
```
  SmartPointer<ResultSet<Game> >   Player::get_games();
  void                             Player::add_to_games(Game&) const;
  void                             Player::remove_from_games(Game&) const;

  SmartPointer<ResultSet<Player> > Game::get_players();
  void                             Game::add_to_players(const Player&);
  void                             Game::remove_from_players(const Player&);
  MongoDB::Array<mongo::OID>       Game::get_players_ids() const;
  void                             Game::set_players_ids(MongoDB::Array<mongo::OID>);
```

Don't forget that no matter on which object you call the `add_to_...` and `remove_from_...` methods, the modifications are always made on the host (`Game` in this case). When you want to save your modifications, do not forget to call `Save` on the host object.

### Custom requests

When your requirements are a bit more specific, you'll need to make your own requests. You may use helpers from the `ResulSet` template to achieve this:

#### Querying
```C++
  #include <Boots/Utils/smart_pointer.hpp> // defines the SP macro

  void MyController::get_by_name()
  {
    std::string         name    = params["name"];
    SP(ResultSet<User>) results = ResultSet<User>::prepare(MONGO_QUERY("name" << name));
    std::stringstream   text;

    text << "Found " << results->count() << " users with name `" << name '`' << std::endl;
    render(TEXT, text);
  }
```

A simple documentation for the `mongo::Query` object is available [here](https://github.com/mongodb/mongo-cxx-driver/wiki/Tutorial#query).

#### Iterating
You may also iterate on the results using `ResultSet<T>::Each(std::function<bool(T&)>)`.

```C++
  void MyController::get_by_name()
  {
    std::string         name    = params["name"];
    SP(ResultSet<User>) results = ResultSet<User>::prepare(MONGO_QUERY("name" << name));
    std::stringstream   text;

    results->each([&text](User& user) -> bool {
      text << "User(" << user.Id() << ") matching name" << std::endl;
      return (true); // you may return 'false' if you wish to interrupt the loop
    });
    render(TEXT, text);
  }
```

#### Pagination
You may also only fetch a subset of the results, using pagination:

```C++
  std::list<User> MyController::get_user_page(int page, int items_per_pages)
  {
    SP(ResultSet<User>) results = ResultSet<User>::prepare();

    results->set_skip(page * items_per_pages);
    results->set_limit(items_per_pages);
    return results->entries();
  }
```
