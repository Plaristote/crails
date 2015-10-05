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
      Info,
      Debug,
      Warning,
      Error,
      endl
    };

    struct Buffer
    {
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
      buffer.stream << item;
      return *this;
    }

    void flush();
  private:
    static thread_local Buffer buffer;
    std::mutex    mutex;
    std::ostream* stdout;
    std::ostream* stderr;
  };

  extern Logger logger;
}

#endif
