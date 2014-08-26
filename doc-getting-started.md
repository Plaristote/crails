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
