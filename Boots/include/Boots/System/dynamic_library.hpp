#ifndef  DYNAMIC_LIBRARY_HPP
# define DYNAMIC_LIBRARY_HPP

# include <dlfcn.h>
# include <string>

class DynamicLibrary
{
public:
  DynamicLibrary(void);
  ~DynamicLibrary(void);

  bool               Load(const std::string& path);

#ifdef _WIN32
  template<typename FUNC_PTR>
  FUNC_PTR           Resolve(const std::string& symbol)
  {
    if (lib_handle)
    {
      FUNC_PTR ptr = (FUNC_PTR)GetProcAddress(lib_handle, symbol.c_str());

      if (!ptr)
        GetError();
      return (ptr);
    }
    return (0);
  }
#else
  template<typename FUNC_PTR>
  FUNC_PTR           Resolve(const std::string& symbol)
  {
    if (lib_handle)
    {
      FUNC_PTR ptr = (FUNC_PTR)dlsym(lib_handle, symbol.c_str());

      GetError();
      return (ptr);
    }
    return (0);
  }
#endif

  void               Unload(void);

  const std::string  Error(void) const;
  const std::string& Filename(void) const;

private:
  void GetError(void);

#ifdef _WIN32
  HINSTANCE   lib_handle
#else  
  void        *lib_handle;
#endif
  std::string filename;
  std::string last_error;
};

#endif
