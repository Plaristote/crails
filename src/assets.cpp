#include "crails/assets.hpp"
#include <vector>
#include <string>
#include <algorithm>
#include <Boots/System/process.hpp>
#include <iostream>

using namespace std;
using namespace System;

void Assets::UglifyJS(void)
{
  Directory directory;
  string    relative_path = COMPILED_ASSETS_PATH;
  
  if ((directory.OpenDir(relative_path)))
  {
    Regex              match;
    Directory::Entries files = directory.GetEntries();

    match.SetPattern("^(.+)\\.js$", REG_EXTENDED);
    for_each(files.begin(), files.end(), [match, relative_path](Dirent& dirent)
    {
      if (!(match.Match(dirent.d_name)))
      {
        string  fullpath = relative_path + '/' + dirent.d_name;
        Process process("/usr/bin/uglifyjs", { fullpath, "-o", fullpath });

        cout << "Assets::Precompile - UglifyJS - " << dirent.d_name << endl;
        if (process.Execute())
        {
          if ((process.Join()) != 0)
          {
            stringstream stream;
            
            process.Stderr(stream);
            throw Assets::Exception(fullpath, stream);
          }
        }
      }
    });
  }
}

void Assets::Coffeescript(void)
{
  Directory   directory;
  std::string relative_path = ASSETS_PATH + "/javascript";

  if (!(Filesystem::FileExists(COMPILED_ASSETS_PATH)))
  {
    if (!(Directory::MakeDir(COMPILED_ASSETS_PATH)))
      throw Assets::Exception("Couldn't create assets directory '" + COMPILED_ASSETS_PATH + '\'');
  }
  
  if ((directory.OpenDir(relative_path)))
  {
    Directory::Entries files = directory.GetEntries();
    Regex              match;
    vector<string>     coffeescript_files;

    match.SetPattern("^(.+)\\.coffee$", REG_EXTENDED);

    for_each(files.begin(), files.end(), [&coffeescript_files, match, relative_path](Dirent& dirent)
    {
      if (dirent.d_type == DT_DIR);
      else if (!(match.Match(dirent.d_name)))
        coffeescript_files.push_back(dirent.d_name);
      else
        Filesystem::FileCopy(relative_path + '/' + dirent.d_name, COMPILED_ASSETS_PATH + '/' + dirent.d_name);
    });

    for_each(coffeescript_files.begin(), coffeescript_files.end(), [&relative_path, match](const string& filename)
    {
      string  filepath = relative_path + '/' + filename;
      string  dest     = COMPILED_ASSETS_PATH;
      Process process("/usr/bin/coffee", { "-c", filepath });

      if (process.Execute())
      {
        if ((process.Join() != 0))
        {
          stringstream stream;

          process.Stderr(stream);
          throw Assets::Exception(filepath, stream);
        }
        else
        {
          std::cout << "Compiled coffeescript file " << filename << std::endl;
          regmatch_t matches[2];
          string compiled_name;

          match.Match(filename, matches, 2);
          compiled_name = filename.substr(0, matches[1].rm_eo);
          if (!(Filesystem::FileMove(relative_path + '/' + compiled_name + ".js", dest + compiled_name)))
            throw Assets::Exception("Couldn't move '" + relative_path + '/' + compiled_name + "' to the asset directory");
        }
      }
    });
  }
}

void Assets::Cleanup(void)
{
  Directory compiled_directory;

  if ((compiled_directory.OpenDir(COMPILED_ASSETS_PATH)))
  {
    Directory::Entries files = compiled_directory.GetEntries();
    
    for_each(files.begin(), files.end(), [](Dirent entry)
    {
      Filesystem::FileRemove(COMPILED_ASSETS_PATH + '/' + entry.d_name);
    });
  }
}

void Assets::Precompile(void)
{
  //cout << "## Asset Precompiling ##" << endl;
  Cleanup();
  Coffeescript();
#ifndef SERVER_DEBUG
  UglifyJS();
#else
  //cout << ">> Debug Server: Skipping minimization of javascript" << endl;
#endif
  //cout << "## Done Precompiling ##" << endl;
}