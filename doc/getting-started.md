# Create a project
The first step to use the Crails Framewrok after having it installed is to use the create script to generate all
the files your project might need.

    crails new myapplication

This command creates a Crails appliatin in the directory myapplication.
It needs to be build and run from the buid directory.

You can use `crails new -h` to list the options.

# Configure a project
## Request Pipeline
The file `config/request_pipe.cpp` allows you to define the path that a request will follow inside the Crails server. Crails' pipeline is composed of two types of objects:

   - The request parsers, collecting data from the requests headers and body.
   - The request handlers, which have the responsability of providing a response to the request.

By default, Crails comes with a number of parsers and handlers:

```C++
void Server::initialize_request_pipe()
{
  add_request_parser(new RequestDataParser); // parses the URI parameters (?param=value)
  add_request_parser(new RequestFormParser); // parses a body with Content-Type application/x-www-form-urlencoded 
  add_request_parser(new RequestJsonParser); // parses a body with Content-Type application/json
  add_request_parser(new RequestMultipartParser); // parses a body with Content-Type multipart/form-data

  add_request_handler(new FileRequestHandler);   // provides an answer using files from the /public folder
  add_request_handler(new ActionRequestHandler); // uses the Crails::Router to redirect the query to your controllers
}
```

You may edit `config/request_pipe.cpp` to lighten your application's pipeline, by removing features you don't want to use.
You may also write your own request parsers, or handlers, and append them to the pipeline using this file (TODO: document creation of RequestParser/RequestHandlers).

## Databases
The file `config/db.json` is used to load and connect to the proper databases.

```JSON
{
  "production": {
    "mongodb": {
      "type": "mongodb",
      "host": "127.0.0.1",
      "database": "crails_db"
    }
  },
  "development": {
    "mongodb": {
      "type": "mongodb",
      "host": "127.0.0.1",
      "database": "crails_db_dev"
    }
  },
  "test": {
    "mongodb": {
      "type": "mongodb",
      "host": "127.0.0.1",
      "database": "crails_db_test"
    }
  }
}
```

The default configuration file looks like this.
On the first level is the environment: you may use different dbs when in production, development or test mode.
On the second level, you describe each databases needed, and give them a name: the name is used by the Models so that each model can refer to any database for its operations.

Notes:
- Authentication with mongodb is not yet ready.
- The SQL module only support MySQL for now. SQLite and PostgreSQL support is planned.

## Asset pipeline
The `Guardfile` provided by crails has two tasks for compiling javascripts and css assets (`sass` and `crailsjs`). The lookup pathes for your assets are `app/assets/javascripts` and `app/assets/stylesheets`.

### SCSS
The scss compiling is handled by the [guard-sass](https://github.com/hawx/guard-sass) gem.

By default, it will take any scss file from `app/assets/stylesheets`, compile it, and output it in the `public/stylesheets` directory. Partial scss files are not concerned by this, but can still be required by other scss files.

### Javascript
The crailsjs task looks for the `targets` (by default, `application.js` is the only target).
Every target is going to be built and redirected to the output directory.

By default, this is how the crailsjs task is configured:
```ruby
guard 'crails-js', input: 'app/assets/javascripts', output: 'public/javascripts', targets: ['application.js']
```

This will look for a file `app/assets/javascripts/application.js`, compile it, and publish the output as `public/javascripts/application.js`

Note that your `application.js` file can require other javascript files from the `app/assets/javascripts` folder, by using the `//= require myfile.js` syntax.

### Coffeescript
The crailsjs task also supports coffeescript. Note that, because coffeescript removes single line comments, you cannot use the `#= require` syntax. Instead, use the regular javascript syntax, and surround it with backquotes, such as this:

```coffee
`//= require myfile.coffee`
```

Feel free to mix javascript and coffeescript files as you wish: crailsjs doesn't care.

# Start developing
Guard will help you set everything up in your environment. Use the command `crails guard` to launch guard.
Once you've launched the `crails guard` command, you'll be taken to a prompt. Here's a list of the commands you may use:
#### crailsserver
- will start a server on port 3001 if none is already launched
- the logs will be redirected to `logs/out.log` and `logs/err.log`

#### crailscmake
- will recompile your code and run the tests.
- if a server is launched, crailscmake will restart it when the compilation and tests are successfull

#### crailsjs
- will compile your javascript and coffeescript files

#### sass
- will compile your stylesheets

#### all
- will run all the tasks
