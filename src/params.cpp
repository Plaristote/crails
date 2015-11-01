#include <crails/params.hpp>
#include <crails/session_store.hpp>
#include <boost/filesystem.hpp>

using namespace std;
using namespace Crails;

Params::Params(void)
{
  session = SessionStore::Factory();
}

Params::~Params(void)
{
  for (const File& file : files)
    boost::filesystem::remove(file.temporary_path);
}

const Params::File* Params::get_upload(const string& key) const
{
  Files::const_iterator it = find(files.begin(), files.end(), key);

  if (it != files.end())
    return (&(*it));
  return (0);
}
