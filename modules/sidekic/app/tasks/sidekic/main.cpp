#include <iostream>
#include <fstream>
#include <csignal>
#include <functional>
#include <chrono>
#include <atomic>
#include <crails/params.hpp>
#include <crails/utils/timer.hpp>
#include <crails/renderer.hpp>
#include <crails/logger.hpp>
#include <crails/sidekic.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include "ctpl.h"

#ifndef THREAD_POOL_SIZE
# define THREAD_POOL_SIZE 2
#endif

using namespace std;
using namespace boost::filesystem;
using namespace Crails;

extern map<string, function<void (Params&)> > sidetasks;

atomic_bool should_continue(true);

static void shutdown(int) { should_continue = false; }

static vector<string> get_tasks()
{
  vector<string> tasks;
  path p(Sidekic::tasks_path);
  directory_iterator it(p);
  directory_iterator end_it;

  for (; it != end_it ; ++it)
  {
    if (is_regular_file(it->path()))
      tasks.push_back(it->path().string());
  }
  return tasks;
}

static vector<pair<string,string> > get_commands(vector<string> tasks)
{
  vector<pair<string,string> > commands;

  for (auto filename : tasks)
  {
    std::ifstream file(filename.c_str());

    if (file.is_open())
    {
      string command, line;

      while (getline(file, line))
        command += line + '\n';
      file.close();
      commands.push_back(pair<string,string>(filename, command));
    }
  }
  return commands;
}

static void run_tasks(ctpl::thread_pool& pool, vector<string> tasks)
{
  vector<pair<string,string> > commands = get_commands(tasks);
  auto timestamp = chrono::system_clock::to_time_t(chrono::system_clock::now());

  for (auto command : commands)
  {
    string json = command.second;
    Params data; data.from_json(json);
    Data   sidedata = data["sidekic"];
    string type     = sidedata["type"];
    auto   runner   = sidetasks[type];

    if (sidedata["run_at"].exists() && sidedata["run_at"].as<time_t>() > timestamp)
      continue ;
    remove(path(command.first));
    pool.push([runner, json, type](int id)
    {
      try
      {
        Params data;
        Utils::Timer timer;

        data.from_json(json);
        logger << Logger::Info
          << "# Running task of type " << type << " in " << id << Logger::endl;
        runner(data);
        logger << Logger::Info
          << "# Done (duration: " << timer.GetElapsedSeconds() << "s)"
          << Logger::endl;
      }
      catch (const std::exception& e)
      {
        logger << Logger::Error <<
          "[!] Catched exception: " << e.what() << Logger::endl;
      }
    });
  }
}

void initialize_pid_file(const std::string& filepath)
{
  std::ofstream output(filepath.c_str());

  if (output.is_open())
  {
    pid_t pid = getpid();

    logger << ">> PID " << pid << " (stored in " << filepath << ')' << Logger::endl;
    output << pid;
    output.close();
  }
  else
    logger << "!! Failed to open PID file " << filepath << Logger::endl;
}

int main(int argc, char** argv)
{
  ctpl::thread_pool taskpool(THREAD_POOL_SIZE);

  {
    using namespace boost;
    program_options::variables_map vm;
    program_options::options_description desc("Options");

    desc.add_options()
      ("pidfile", program_options::value<std::string>(), "pid file");
    program_options::store(program_options::parse_command_line(argc, argv, desc), vm);
    program_options::notify(vm);
    if (vm.count("pidfile"))
      initialize_pid_file(vm["pidfile"].as<std::string>());
  }

  Renderer::initialize();
  signal(SIGINT, &shutdown);
  do {
    run_tasks(taskpool, get_tasks());
    sleep(5);
  } while (should_continue == true);
}
