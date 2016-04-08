#ifndef  CRAILS_LOGGER_HPP
# define CRAILS_LOGGER_HPP

# include <sstream>
# include <thread>
# include <mutex>
# include <boost/thread/tss.hpp>

namespace Crails
{
  class Logger
  {
  public:
    enum Symbol
    {
      Info = 0,
      Debug,
      Warning,
      Error,
      endl
    };

    struct Buffer
    {
      Buffer() : level(Logger::endl) {}

      std::stringstream stream;
      Symbol            level;
    };

    Logger();

    static Buffer& get_buffer();

    void set_stdout(std::ostream& stream);
    void set_stderr(std::ostream& stream);

    Logger& operator<<(Symbol level);

    template<typename T>
    Logger& operator<<(const T item)
    {
      if (log_level <= get_buffer().level)
        get_buffer().stream << item;
      return *this;
    }

    void flush();
  private:
    static const Symbol log_level;
    static boost::thread_specific_ptr<Buffer> buffer;
    std::mutex    mutex;
    std::ostream* stdout;
    std::ostream* stderr;
  };

  extern Logger logger;
}

#endif
