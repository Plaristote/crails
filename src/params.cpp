#include <crails/params.hpp>
#include <Boots/Utils/directory.hpp>
#include <crails/cookie_data.hpp>
#include <crails/session_store.hpp>

using namespace std;

Params::Params(void) : handle(1), response_parsed(0)
{
  session = SessionStore::Factory();
}

Params::~Params(void)
{
  std::for_each(files.begin(), files.end(), [](const File& file)
  {
    Filesystem::FileRemove(file.temporary_path);
  });
}

const Params::File* Params::Upload(const std::string& key) const
{
  Files::const_iterator it = std::find(files.begin(), files.end(), key);
  
  if (it != files.end())
    return (&(*it));
  return (0);
}
