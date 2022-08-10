Crails
======
Crails Framework is an MVC web development framework aiming to bring a Rails-like experience to C++
developers. Despite being inspired by Ruby on Rails, it is much lighter, faster, and much more C++.

It's a Boost.Beast powered HTTP server using a routing system, MVC design, C++-compiled templates, asset
precompilation, database abstraction and testing tools.

Crails is modular, and comes by default with a bunch of modules:
- crails-html and crails-json: template engines
- crails-mongodb and crails-odb: ORMs for SQL and MongoDB databases
- crails-docker: building tools to ease deployment on the production platform 
- crails-sidekic: task scheduler
- crails-cache: memcached powered caching
- crails-mail: sending emails
- crails-attachment: storing files in your models
- crails-image: use Magick++ to generate thumbnails
- crails-xmlrpc: implements xmlrpc server and clients 
- crails-proxy: redirects requests or acts as a proxy
- crails-sentry: monitor server exceptions using [Sentry](http://sentry.io)
- crails-selenium: automated browser tests for your test suite

Used with [Comet.cpp](https://github.com/crails-framework/comet.cpp), you can also use Crails to write
both backend and frontend code in C++.

The [MetaRecord](https://github.com/crails-framework/meta-record) code generator may also help you
generate code for your models. It provides backends for Crails, Comet.cpp, Qt and JavaScript.

Install Crails Framework
========
Compiling
--------
This project has been compiled using `gcc 11`. It should support `clang` as well, though this support is not actively maintained for now.

Here's a list of the dependecies:
- ruby >= 1.9 (development only)
- Boost
- segvcatch (optional)
- [ODB](http://www.codesynthesis.com/products/odb/) (optional, used by crails-odb)
- [mongo-cxx-driver](https://github.com/mongodb/mongo-cxx-driver/tree/legacy) (optional, used by crail-mongodb)
- [libmemcached](http://libmemcached.org) (optional, used by crails-cache)
- [Magick++](http://www.imagemagick.org/Magick++/) (optional, used by crails-image)

MacOSX users: this project requires the `thread_local` feature from C++11, which isn't available on Apple's clang.

Building the Crails Framework
--------
To the root of the project's directory, create a build directory, go in there, and once again, type:

    cmake ..
    make
    sudo make install

When the developer mode is on, the Crails Framework provides a number of tools (disabled caching of assets, using renderers to display exceptions) that you simply need while working on a Crails application. On your development machine, you would want to compile the framework with this command line:

    cmake -DDEVELOPER_MODE=ON

Developer and Production server do not conflict: you may want to compile the project twice, with and without the developer mode option, which would give you the ability to swap between developer and production mode simply by changing the CMAKE_BUILD_TYPE variable to Release or Debug in your project's CMakeCache.txt.

There are two types of server available: aysnchronous and synchronous. To compile one or the other, use the `-DUSE_MULTITHREAD=[ON|OFF]` option with cmake.

If you want to swap between environments, make sure you have installed every flavour of Crails that you are going to use.

Walkthrough
=======
The Crails Framework comes with the `crails` command, which is a Ruby toolset giving you a large set of commands to create and manage your application.
The first step you'll have to go through is the creation of your application:

    crails new my_crails_app

This will generate all the necessary files in the folder `./my_crails_app`.
Once you've done that, the next thing you should do is pick which modules from crails you are going to be using. To do that, use the `crails module` command.

    cd my_crails_app
    crails module # lists the available modules
    crails module html install
    crails module mongodb install

At any point, you may also change some of the framework internal settings, using `crails set-env`:

    # disable development mode
    crails set-env production
    # enable development mode
    crails set-env development

Once your application is created and configured, you may launch the `crails guard` tool. It has several applications:
  - The task `assets` compiles your javascript and stylesheet assets;
  - The task `before_compile` generates code for modules that need it (crails-html, crails-json, ...);
  - The task `compile` compiles your code and run the tests;
  - It watches your files to automatically re-run the tasks when needed;

Here's how you'd use `crails guard`:

    crails guard # launches the guard shell
    guard> assets
    guard> before_compile
    guard> compile

Now that everything is ready, you may launch your server by running the binary:

    build/server -p 8080 -h 0.0.0.0

Note that by defaults, the server binds to port 3001 and host 127.0.0.1.
