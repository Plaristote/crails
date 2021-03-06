#ifndef  SMART_POINTER
# define SMART_POINTER

# include <memory>
# include <crails/utils/backtrace.hpp>

struct NullPointerException : public boost_ext::exception
{
public:
  const char* what() const throw()
  {
    return ("A null pointer has been dereferenced.");
  }
};

template<typename T>
class safe_ptr : public std::shared_ptr<T>
{
public:
  safe_ptr()
  {
  }

  safe_ptr(T* ptr) : std::shared_ptr<T>(ptr)
  {
  }

  template<typename CPY>
  safe_ptr(CPY& cpy) : std::shared_ptr<T>(cpy)
  {
  }

  safe_ptr& operator=(const std::shared_ptr<T>& cpy)
  {
    std::shared_ptr<T>::operator=(cpy);
    return *this;
  }

  T* operator->() const
  {
    if (this->get() == 0)
      throw NullPointerException();
    return (this->get());
  }
  
  T& operator*(void) const
  {
    if (this->get() == 0)
      throw NullPointerException();
    return (*this->get());
  }
};

# define SP(T) ::safe_ptr<T>

#endif
