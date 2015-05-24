#ifndef  _NETWORK_BUFFER_HPP_
# define _NETWORK_BUFFER_HPP_

# include "../Sync/mutex.hpp"

namespace Network
{
  class Buffer
  {
    friend class Socket;
  public:
    Buffer(void)
    {
      _buffer        = 0;
      _length        = 0;
      _buffer_length = 0;
    }

    ~Buffer(void)
    {
      Sync::Mutex::Lock lock(_sem);
      if (_buffer)
        delete[] _buffer;
    }

    const char*  Data(void)   const { return (_buffer); }
    unsigned int Length(void) const { return (_length); }

    void         Flush(unsigned int how_much);
    void         Push(const char* str, unsigned int length);

    void         Lock(void)   { _sem.Wait(); }
    void         Unlock(void) { _sem.Post(); }

  private:
    void         Realloc(unsigned int new_length);
    
    char*        _buffer;
    unsigned int _buffer_length;
    unsigned int _length;

    Sync::Mutex _sem;
  };
}

#endif
