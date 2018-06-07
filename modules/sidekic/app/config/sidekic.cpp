#include <crails/sidekic.hpp>
#include <boost/filesystem.hpp>

using namespace std;

const string Sidekic::tasks_path = boost::filesystem::current_path().string() + "/.pending_tasks";
