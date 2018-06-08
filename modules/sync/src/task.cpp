#include "../crails/sync/task.hpp"
#include "../crails/sync/client.hpp"
#include <crails/utils/string.hpp>

using namespace Sync;
using namespace std;

extern thread_local Faye::Client faye;

Task::Task(unsigned int task_count) : task_count(task_count)
{
  id = Crails::generate_random_string("abcdefghijklmnopqrstuvwxyz0123456789", 8);
  metadata["id"] = id;
}

Task::Task(const std::string& id, unsigned int task_count) : task_count(task_count)
{
  metadata["id"] = id;
}

Task::~Task()
{
  if (task_count > task_progress)
  {
    metadata["status"] = "abort";
    faye.publish("/tasks", metadata.as_data());
  }
}

unsigned int Task::notification_step() const
{
  return std::ceil(notification_step_perc / 100.f * task_count);
}

void Task::increment(unsigned int progress)
{
  task_progress += progress;
  if (task_progress - last_notification >= notification_step()
   || task_progress >= task_count)
    notify();
}

void Task::notify(const std::string& message)
{
  metadata["message"] = message;
  notify();
  metadata["message"].destroy();
}

void Task::notify()
{
  float progress = task_progress;

  progress /= task_count;
  if (progress > 1)
    progress = 1;
  metadata["progress"]      = progress;
  metadata["item_count"]    = task_count;
  metadata["item_progress"] = task_progress;
  last_notification = task_progress;
  faye.publish("/tasks", metadata.as_data());
}

void Task::set_task_count(unsigned int count)
{
  if (count != task_count)
  {
    task_count = count;
    notify();
  }
}
