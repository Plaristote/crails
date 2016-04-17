#include <crails/logger.hpp>
#include <thread>
#include <iostream>

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
  mutex.lock();
  switch (buffer.level)
  {
    case Info:
      *stdout log_prefix << buffer.stream.str();
      stdout->flush();
      break ;
    default:
      *stderr log_prefix << buffer.stream.str();
      stderr->flush();
      break ;
  }
  mutex.unlock();
  buffer.stream.str("");
  buffer.stream.clear();
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
