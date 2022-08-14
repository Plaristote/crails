#include "program_options.hpp"
#include "logger.hpp"
#include <memory>
#include <thread>

using namespace std;
using namespace boost;
using namespace Crails;

ProgramOptions::ProgramOptions(int argc, const char** argv)
{
  program_options::options_description desc("Options");

  desc.add_options()
    ("port,p",     program_options::value<std::string>(),    "listened port")
    ("hostname,h", program_options::value<std::string>(),    "listened host")
    ("threads,t",  program_options::value<unsigned short>(), "amount of threads")
    ("pidfile",    program_options::value<std::string>(),    "pid file")
    ;
  program_options::store(program_options::parse_command_line(argc, argv, desc), vm);
  program_options::notify(vm);
}

boost::asio::ip::tcp::endpoint ProgramOptions::get_endpoint() const
{
  return boost::asio::ip::tcp::endpoint{
    boost::asio::ip::make_address(get_value("hostname", std::string("0.0.0.0"))),
    get_value<unsigned short>("port", 3001)
  };
}

unsigned short ProgramOptions::get_thread_count() const
{
  return get_value("threads", (unsigned short)(std::thread::hardware_concurrency()));
}

std::string ProgramOptions::get_pidfile_path() const
{
  return get_value("pidfile", std::string("crails.pid"));
}
