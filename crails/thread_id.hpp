#ifndef  CRAILS_THREAD_ID_HPP
# define CRAILS_THREAD_ID_HPP

# include <atomic>

struct ThreadId
{
public:
  ThreadId()
  {
    static std::atomic<unsigned long> value(0);

    id = ++value;
  }

  operator unsigned long() const { return id; }

private:
  unsigned long id;
};

#endif
