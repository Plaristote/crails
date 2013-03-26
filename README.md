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

Compiling
======
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
=====
Go to the Boots folder, create a build directory, go in the build directory and type:

cmake .. && make && sudo make install

If the headers and librairies were installed in /usr/local, create symbolic links using these commands:
ln -s /usr/local/include/Boots /usr/include
ln -s /usr/local/lib/libBoots.so /usr/lib/libBoots.so

Building the Crails Framework
======
To the root of the project's directory, create a build directory, go in there, and type:

cmake .. && make && sudo make install

Again, make sure the installed files are accessible through the lib, include, binary and shared path.

Creating a Crails Application
=====
The installation of the Crails Framework should have added a ruby script named 'crails' in your bin directory.
You can use it to create application this way:

crails new application_name --use-mongodb --use-sql --debug-mode

The script will tell you how to compile and launch the server next.

N.B: You cannot compile if you don't use the --use-mongodb and --use-sql flags right now. It's a flaw that I'm working on.
