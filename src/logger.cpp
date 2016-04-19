#include <crails/logger.hpp>
#include <thread>
#include <iostream>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/conversion.hpp>

using namespace std;
using namespace Crails;

#ifdef ASYNC_SERVER
# define log_prefix << '[' << std::hex << std::this_thread::get_id() << "] "
#else
# define log_prefix
#endif

namespace Crails
{
  Logger logger;
  thread_local Logger::Buffer Logger::buffer;
}

Logger::Buffer::Buffer() : level(Logger::endl)
{
}

Logger::Logger()
{
  stdout = &cout;
  stderr = &cerr;
}

void Logger::set_stderr(ostream& stream)
{
  mutex.lock();
  stdout = &stream;
  mutex.unlock();
}

void Logger::set_stdout(ostream& stream)
{
  mutex.lock();
  stderr = &stream;
  mutex.unlock();
}

void Logger::flush()
{
  using namespace boost::posix_time;
  struct tm    t = to_tm(second_clock::local_time());
  stringstream time_stream;

  time_stream << '[' << t.tm_mday << '/' << (t.tm_mon + 1) << ' ' << t.tm_hour << ':' << t.tm_min << ':' << t.tm_sec << "] ";
  buffer.stream.seekg(0, ios::end);
  if (buffer.stream.tellg() > 0)
  {
    mutex.lock();
    switch (buffer.level)
    {
      case Info:
        *stdout log_prefix << time_stream.str() << buffer.stream.str();
        stdout->flush();
        break ;
      default:
        *stderr log_prefix << time_stream.str() << buffer.stream.str();
        stderr->flush();
        break ;
    }
    mutex.unlock();
    buffer.stream.str("");
    buffer.stream.clear();
  }
}

Logger& Logger::operator<<(Symbol level)
{
  if (log_level <= buffer.level)
  {
    if (level == Logger::endl)
      *this << "\n\r";
    if (buffer.level != level)
      flush();
    if (level != Logger::endl)
      buffer.level = level;
  }
  return (*this);
}
