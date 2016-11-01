#include <crails/front/mvc/model.hpp>

using namespace Crails::Front;
using namespace client;
using namespace std;

void Model::fetch(Ajax::Callbacks callbacks)
{
  Ajax::query(get_url())
    .headers({{"Accept", Archive::mimetype}})
    .callbacks(callbacks)
    .on_success([this, callbacks](const Ajax& ajax)
    {
      if (ajax->get_responseText())
        parse((std::string)(*ajax->get_responseText()));
      if (callbacks.success)
        callbacks.success(ajax);
      synced.trigger();
    })();
}

void Model::save(Ajax::Callbacks callbacks)
{
  OArchive archive;

  serialize(archive);
  Ajax::query(get_id() == 0 ? "POST" : "PUT", get_url())
    .headers({{"Content-Type", Archive::mimetype}})
    .data(archive.as_string())
    .callbacks(callbacks)();
}

void Model::parse(const std::string& str)
{
  IArchive archive;

  archive.set_data(str);
  serialize(archive);
}
