Crails
======
Crails Framework is an MVC web development framework aiming to bring a Rails-like experience to C++
developers. Despite being inspired by Ruby on Rails, it is much lighter and simpler.

It's a cpp-netlib based HTTP server using a routing system, MVC design, C++-compiled templates, asset
precompilation and database abstraction.

Crails is modular, and comes by default with a bunch of modules:
- With crails-html and crails-json, you have the ability to write templates that are then compiled into C++
- With crails-mongodb and crails-odb, you get an object oriented database abstraction
- With crails-cache, you get a simple API to handle your caching using memcached
- With crails-mail, you get the ability to easily render and send mails
- With crails-image, you can store image in your models, and use Magick++ to generate thumbnails

Install Crails Framework
========
Compiling
--------
This project has been compiled using `gcc 4.9`. It should support `clang` as well, though this support is not actively maintained for now.

Here's a list of the dependecies:
- ruby >= 1.9 (development only)
- cpp-netlib
- segvcatch (optional)
- [ODB](http://www.codesynthesis.com/products/odb/) (optional, used by crails-odb)
- [mongo-cxx-driver](https://github.com/mongodb/mongo-cxx-driver/tree/legacy) (optional, used by crail-mongodb)
- [libmemcached](http://libmemcached.org) (optional, used by crails-cache)
- [Magick++](http://www.imagemagick.org/Magick++/) (optional, used by crails-image)

MacOSX users, you're out of luck (sort of): this project requires the `thread_local` feature from C++11, which Apple has removed from clang. You can still work using the `cedar14-compat` branch.

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

    # disable development mode, and use a thread pool to handle HTTP requests
    crails set-env production
    crails set-env multithread
    # enable development mode, and use a single thread to handle HTTP requests
    crails set-env development
    crails set-env synchronous

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

Now that everything is ready, you may launch your server using the `crails server` command:

    crails server -p 8080 -h 0.0.0.0

Note that `crails server` binds to port 3001 and host 127.0.0.1 by default.
