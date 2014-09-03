# Create a project
The first step to use the Crails Framewrok after having it installed is to use the create script to generate all
the files your project might need.

    crails create myapplication --use-sql --use-mongodb --debug-mode

This command creates a Crails appliatin in the directory myapplication.
It needs to be build and run from the buid directory.

You can use `crails create -h` to list the options.

# Configure a project
## Databases
The file conf/db.json is used to load and connct the prope databases.

```JSON
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
```

The default configuration file looks like this.
The keys to the first objects are the name of your dbs which you will use to get a handle of them at runtime.

Notes:
- Authentication with mongodb is not yet ready.
- The SQL module only support MySQL for now. SQLite and PostgreSQL support is planned.

## Asset pipeline
The `Gulpfile.js` provided by crails has two tasks for compiling javascripts and css assets. The lookup pathes for your assets are `app/assets/javascripts` and `app/assets/stylesheets`.

You may configure the asset pipeline's behavior by editing the `conf/assets.json` file. By default, it looks like this:

```JSON
{
  "javascripts": {
    "application": [ '**/*.js', '**/*.coffee' ]
  },
  "stylesheets": {
    "application": [ '**/*.css', '**/*.scss' ]
  }
}
```

The file is composed of three levels: the first level describes the type of assets (javascript/stylesheets), the second level creates a target file (in this case, `public/assets/application.js` and `public/assets/application.css`), and the third level contains an array of file matchers, specifying which files should be compiled into the target.

The order of the matchers matters during the compilation, which allows you to use this configuration to specify which files should be compiled before the others. For instance, if you have a dependency for jquery, you may make this change to the configuration:

```JSON
{
  "javascripts": {
    "application": [ 'jquery.js', '**/*.js', '**/*.coffee' ]
  },
  "stylesheets": {
    "application": [ '**/*.css', '**/*.scss' ]
  }
}
```

This would ensure that `/app/assets/javascripts/jquery.js` would be at the top of the `/public/assets/application.js`.

# Start developing
## Gulp
Before you start developing, you must launch `gulp` from your application directory: this service takes care of some code generation needed for the views and models, and compiles your assets and views.

## Launch the server
You may launch the server using the `crails server` command from your application directory. This will compile the server if needed, and launch it.
