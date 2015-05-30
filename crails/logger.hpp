#ifndef  CRAILS_LOGGER_HPP
# define CRAILS_LOGGER_HPP

# include <Boots/Utils/singleton.hpp>
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
      friend class ThreadSingleton<Logger::Buffer>;
      typedef ThreadSingleton<Logger::Buffer> singleton;
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
      get_buffer()->stream << item;
      return *this;
    }

    Buffer* get_buffer() const;
    void flush();
  private:
    std::mutex    mutex;
    std::ostream* stdout;
    std::ostream* stderr;
  };

  extern Logger logger;
}

#endif