#include "../crails/sync/transaction.hpp"
#include "../crails/sync/client.hpp"

extern thread_local Faye::Client faye;

using namespace Sync;
using namespace std;

namespace Sync
{
  bool enabled = true;
  function<void (DataTree&)> Transaction::on_commit;
}

bool Transaction::is_enabled()
{
  return Sync::enabled;
}

void Transaction::commit()
{
  if (updates.size() + removals.size() > 0)
  {
    DataTree message;
    vector<string> removal_uids;

    for (auto update : updates)
      update->render(message["updates"]);
    for (auto removal : removals)
      removal_uids.push_back(removal->uid());
    message["removals"].from_vector<string>(removal_uids);
    if (on_commit)
      on_commit(message);
    faye.publish("/sync", message.as_data());
    rollback();
  }
}

void Transaction::rollback()
{
  updates.clear();
  removals.clear();
}
