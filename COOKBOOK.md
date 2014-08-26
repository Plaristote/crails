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

# Controllers
* [Controllers](doc-controllers.md#controllers)
* [Write a controller](doc-controllers.md#controllers)
* [Router](doc-controllers.md#controllers)
* [Controller methods](doc-controllers.md#controllers)
  * [DynStruct (type used for storing parameters)](doc-controllers.md#dynstruct)
  * [Parameters](doc-controllers.md#params)
  * [Session](doc-controllers.md#session)
* [CSRF security](doc-controllers.md#controllers)
* [File upload](doc-controllers.md#controllers)

# Views
* [Views](doc-view.md#views)
* [Writing a view](doc-view.md#writing-a-view)
* [Render a view from a controller](doc-view.md#render-a-view-from-a-controller)
* [Using helpers within a view](doc-view.md#using-helpers-within-a-view)

# Models
* [MongoDB-based models](doc-models-mongo.md)
  * [Configure your project for MongoDB](doc-models-mongo.md#configure-your-project-for-mongodb)
  * [Writing a model](doc-models-mongo.md#writing-a-model)
  * [Create, update, delete](doc-models-mongo.md#create-update-delete)
  * [Relations](doc-models-mongo.md#relations)
    * [has many](doc-models-mongo.md#has-many)
    * [belongs to](doc-models-mongo.md#belongs-to)
    * [has and belongs to many](doc-models-mongo.md#has-and-belongs-to-many)
  * [Querying](doc-models-mongo.md#querying)
    * [find by id](doc-models-mongo.md#find-by-id)
    * [find many](doc-models-mongo.md#find-many)

* [SQL-based models](#)

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
