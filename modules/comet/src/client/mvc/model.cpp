#include <crails/comet/mvc/model.hpp>

using namespace Comet;

std::string ArchiveModel::get_payload()
{
  OArchive archive;

  serialize(archive);
  return archive.as_string();
}

void ArchiveModel::parse(const std::string& str)
{
  IArchive archive;

  archive.set_data(str);
  serialize(archive);
}

std::string ArchiveModel::get_content_type() const
{
  return Archive::mimetype;
}
