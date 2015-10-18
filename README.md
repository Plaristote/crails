Crails
======

The Crails Framework for C++ MVC development of web applications.

WARNING: Ths is still under heavy development. You wouldn't want to use this for any seious work right now,
it's still quite into the experimenting stage.

What is Crails Framework
======
Crails Framework is an MVC web development framework aiming to bring the joy of a Rails-like experience to C++
developers.
It's a cpp-netlib based HTTP server using a routing system, MVC design, C++-compiled views templates, asset
precompilation and database abstraction.

Tutorials and samples of code will be made available at some point in the futur to intoduce you to the capabilities
of Crails Framework.

Install Crails Framework
========
Compiling
--------
This project has been compiled using `gcc 4.9`. It should support `clang` as well, though this support is not actively maintained for now.

Here's a list of the dependecies:
- ruby >= 1.9 (development only)
- cpp-netlib
- segvcatch (optional)
- [soci](http://soci.sourceforge.net) (optional, used by crails-sql)
- [mongo-cxx-driver](https://github.com/mongodb/mongo-cxx-driver/tree/legacy) (optional, used by crail-mongodb)
- [libmemcached](http://libmemcached.org) (optional, used by crails-cache)

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

Creating a Crails Application
========
The installation of the Crails Framework should have added a ruby script named 'crails' in your bin directory.
You can use it to create application this way:

    crails new -h # Check out the options first (support for databases and all that)
    crails new application_name

By default, the project will build in developer mode, which means it'll link to the debug server. Use `crails set-env production` to link to the production libraries instead. If you want to link with the debug libraries again, use `crails set-env development`.

You can switch between multithreaded and single threaded environment as well with the commands `crails set-env multithread` and `crails set-env synchronous`.

Now that your crails application is ready, check out the [COOKBOOK](COOKBOOK.md) to see all the neat things you can do with it !
