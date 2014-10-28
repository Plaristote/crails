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
MacOS
--------
You may use a brew formula to install crails on Mac:

    brew tap Plaristote/libs
    brew install crails

Compiling
--------
This project was only compiled on an Archlinux. It should work fine with any unix-like OS, but there might be some
unknown compiling issue. Please let me know if you meet any issues with building.

Here's a list of the dependecies:
- Boots (included in the repository)
- cpp-netlib
- segvcatch (optional, add "-DUSE_SEGVCATCH" and uncomment the CMakeList linking line to use)
- soci (if you want to compile crails-sql)
- mongoclient (if you want to compile crail-mongodb)
- nodejs (you'll need uglifyjs, and coffeescript if you put coffee files in /app/assets/javascripts)
- ruby 1.9+

Bulding Boots
--------
Go to the Boots folder, create a build directory, go in the build directory and type:

    cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr .. && make && sudo make install

Building the Crails Framework
--------
To the root of the project's directory, create a build directory, go in there, and once again, type:

    cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr .. && make && sudo make install

In debug mode, the Crails Framework provides a number of tools (disabled caching of views and assets, displaying exceptions as HTTP response...) that you simply need while working on a Crails application. On your development machine, you would want to compile the framework with this command line:

    cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX:PATH=/usr .. && make && sudo make install

Creating a Crails Application
========
The installation of the Crails Framework should have added a ruby script named 'crails' in your bin directory.
You can use it to create application this way:

    crails create -h # Check out the options first (support for databases and all that)
    crails create application_name --use-mongodb --use-mongo-session-store --debug-mode

Now that your crails application is ready, check out the [COOKBOOK](COOKBOOK.md) to see all the neat things you can do with it !
