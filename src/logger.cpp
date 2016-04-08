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
  boost::thread_specific_ptr<Logger::Buffer> Logger::buffer;
}

Logger::Logger()
{
  stdout = &cout;
  stderr = &cerr;
}

Logger::Buffer& Logger::get_buffer()
{
  if (buffer.get() == 0)
    buffer.reset(new Buffer);
  return *buffer;
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
  switch (get_buffer().level)
  {
    case Info:
      *stdout log_prefix << get_buffer().stream.str();
      stdout->flush();
      break ;
    default:
      *stderr log_prefix << get_buffer().stream.str();
      stderr->flush();
      break ;
  }
  mutex.unlock();
  get_buffer().stream.str("");
  get_buffer().stream.clear();
}

Logger& Logger::operator<<(Symbol level)
{
  if (log_level <= get_buffer().level)
  {
    if (level == Logger::endl)
      *this << "\n\r";
    if (get_buffer().level != level)
      flush();
    if (level != Logger::endl)
      get_buffer().level = level;
  }
  return (*this);
}
