#ifndef  CRAILS_LOGGER_HPP
# define CRAILS_LOGGER_HPP

# include <sstream>
# include <thread>
# include <mutex>

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
      Buffer();

      std::stringstream stream;
      Symbol            level;
    };

    Logger();

    void set_stdout(std::ostream& stream);
    void set_stderr(std::ostream& stream);

    Logger& operator<<(Symbol level);

    template<typename T>
    Logger& operator<<(const T item)
    {
      if (log_level <= buffer.level)
        buffer.stream << item;
      return *this;
    }

    void flush();
  private:
    static const Symbol log_level;
    static thread_local Buffer buffer;
    std::mutex    mutex;
    std::ostream* stdout;
    std::ostream* stderr;
  };

  extern Logger logger;
}

#endif
