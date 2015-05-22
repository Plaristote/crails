# Create a project
The first step to use the Crails Framewrok after having it installed is to use the create script to generate all
the files your project might need.

    crails create myapplication --use-mongodb

This command creates a Crails appliatin in the directory myapplication.
It needs to be build and run from the buid directory.

You can use `crails create -h` to list the options.

# Configure a project
## Databases
The file `conf/db.json` is used to load and connect to the proper databases.

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

## Javascript
The crailsjs task looks for the `targets` (by default, `application.js` is the only target).
Every target is going to be built and redirected to the output directory.

By default, this is how the crailsjs task is configured:
```ruby
guard 'crails-js', input: 'app/assets/javascripts', output: 'public/javascripts', targets: ['application.js']
```

This will look for a file `app/assets/javascripts/application.js`, compile it, and publish the output as `public/javascripts/application.js`

Note that your `application.js` file can require other javascript files from the `app/assets/javascripts` folder, by using the `//= require myfile.js` syntax.

## Coffeescript
The crailsjs task also supports coffeescript. Note that, because coffeescript removes single line comments, you cannot use the `#= require` syntax. Instead, use the regular javascript syntax, and surround it with backquotes, such as this:

```coffee
`//= require myfile.coffee`
```

Feel free to mix javascript and coffeescript files as you wish: crailsjs doesn't care.

# Start developing
## Guard
Before you start developing, you must launch `crails guard` from your application directory: this service takes care of some code generation needed for the views and models, and compiles your assets and views.
