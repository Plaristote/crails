#include "../crails/sidekic.hpp"
#include <fstream>
#include <crails/utils/string.hpp>
#include <crails/environment.hpp>

using namespace std;
using namespace Crails;

std::string Sidekic::async_task(const string& name, Data params)
{
  if (Crails::environment == "test") return "dummy-async-task";
  string   uid      = generate_random_string("abcdefghijklmnopqrstwxyz0123456789", 10);
  string   filename = tasks_path + '/' + uid;
  ofstream file(filename.c_str());

  if (file.is_open())
  {
    params["sidekic"]["task_uid"] = uid;
    params["sidekic"]["type"] = name;
    file << params.to_json();
    file.close();
  }
  else
    throw boost_ext::runtime_error("sidekic cannot create file " + filename);
  return uid;
}

std::string Sidekic::schedule_task(time_t timestamp, const string& name, Data params)
{
  params["sidekic"]["run_at"] = timestamp;
  return async_task(name, params);
}

