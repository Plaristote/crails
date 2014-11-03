#ifndef  RACKABLES_HPP
# define RACKABLES_HPP

# include <Boots/Utils/singleton.hpp>
# include <Boots/System/dynamic_library.hpp>
# include <Boots/Utils/directory.hpp>
# include <functional>
# include <iostream>
# include "platform.hpp"

template<typename TYPE>
class Rackables
{
  SINGLETON(Rackables)

  struct Rackable
  {
    bool operator==(const std::string& comp) const { return (name == comp); }

    std::string     name;
    TYPE            callback;
    DynamicLibrary* library;
  };

  typedef std::string (*HookName)(void);

  Rackables()
  {
  }

  ~Rackables()
  {
    std::for_each(constructors.begin(), constructors.end(), [](Rackable& rackable)
    {
      delete rackable.library;
    });
  }

  static bool IsDynamicallyLoadableLibrary(const Dirent dirent)
  {
    if (dirent.d_type == DT_REG)
    {
      std::string filename(dirent.d_name);
      std::string extension(DYNLIB_EXT);

      return (filename.substr(filename.length() - extension.length()) == extension);
    }
    return (false);
  }

public:

  TYPE GetByName(const std::string& name) const
  {
    TYPE result = 0;
    auto it     = std::find(constructors.begin(), constructors.end(), name);

    if (it  != constructors.end())
      result = it->callback;
    return (result);
  }

  void Load(const std::string& directory_path)
  {
    Directory directory;

    if ((directory.OpenDir(directory_path)))
    {
      Directory::Entries& entries = directory.GetEntries();
      auto it  = entries.begin();
      auto end = entries.end();

      for (; it != end ; ++it)
      {
        Dirent dirent = *it;

        if (IsDynamicallyLoadableLibrary(dirent))
        {
          DynamicLibrary* loader = new DynamicLibrary;

          if (loader->Load(directory_path + '/' + dirent.d_name))
          {
            HookName hook_name     = loader->Resolve<HookName>("hook_name");
            TYPE     hook_callback = loader->Resolve<TYPE>    ("hook_callback");

            if (!hook_name)
              std::cerr << "[Rackable] Couldn't load hook_name in '" << dirent.d_name << '\'' << std::endl;
            else if (!hook_callback)
              std::cerr << "[Rackable] Couldn't load hook_callback in '" << dirent.d_name << '\'' << std::endl;
            else
            {
              Rackable rackable;

              rackable.name     = hook_name();
              rackable.callback = hook_callback;
              rackable.library  = loader;
              constructors.push_back(rackable);
              std::cout << "[Rackable] Loaded rackable " << rackable.name << std::endl;
              continue ;
            }
          }
          else
            std::cerr << "[Rackable] Couldn't load loader '" << dirent.d_name << "': " << loader->Error() << std::endl;
          delete loader;
        }
      }
    }
  }

private:
  std::list<Rackable> constructors;
};

#endif
