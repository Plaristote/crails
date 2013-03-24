#include <System/dynamic_library.hpp>

using namespace std;

DynamicLibrary::DynamicLibrary(void) : lib_handle(0) {}

DynamicLibrary::~DynamicLibrary(void)
{
  Unload();
}

#ifdef _WIN32

bool DynamicLibrary::Load(const std::string& path)
{
  Unload();
  lib_handle = LoadLibrary(TEXT(path.c_str()));
  if (!lib_handle)
    GetError();
  return (lib_handle != 0);
}

void DynamicLibrary::Unload(void)
{
  if (lib_handle)
    FreeLibrary(lib_handle);
  lib_handle = 0;
  filename   = "";
}

void DynamicLibrary::GetError(void)
{
  last_error = GetLastError();
}

#else

bool DynamicLibrary::Load(const std::string& path)
{
  Unload();
  lib_handle = dlopen(path.c_str(), RTLD_LAZY);
  GetError();
  filename   = path;
  return (lib_handle != 0);
}

void DynamicLibrary::Unload(void)
{
  if (lib_handle)
    dlclose(lib_handle);
  lib_handle = 0;
  filename   = "";
}

void DynamicLibrary::GetError(void)
{
  char* err = dlerror();
  
  if (err)
    last_error = err;
}

#endif

const std::string  DynamicLibrary::Error(void) const
{
  return (last_error);
}

const std::string& DynamicLibrary::Filename(void) const
{
  return (filename);
}
