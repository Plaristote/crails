#ifndef  SMART_POINTER
# define SMART_POINTER

# include <memory>
# include "backtrace.hpp"

struct NullPointerException : public boost_ext::exception
{
public:
  const char* what() const throw()
  {
    return ("A null pointer has been dereferenced.");
  }
};

# define SP(T) SmartPointer<T>

template<typename T>
class SmartPointer
{
public:
  SmartPointer(void) : pointer(nullptr)
  {
    initialize_counter();
  }

  SmartPointer(T* pointer) : pointer(pointer)
  {
    initialize_counter();
  }

  SmartPointer(const std::unique_ptr<T>& ptr)
  {
    pointer  = ptr.get();
    initialize_counter();
    ptr.release();
  }

  SmartPointer(const SmartPointer& smart_ptr)
  {
    pointer   = smart_ptr.pointer;
    counter   = smart_ptr.counter;
    *counter += 1;
  }

  ~SmartPointer(void)
  {
    cleanup();
  }

  SmartPointer& operator=(T* n_pointer)
  {
    if (n_pointer != pointer)
    {
      cleanup();
      pointer  = n_pointer;
      counter  = new unsigned int;
      *counter = 1;
    }
    return (*this);
  }
  
  SmartPointer& operator=(const SmartPointer& smart_ptr)
  {
    if (smart_ptr.pointer != pointer)
    {
      cleanup();
      counter   = smart_ptr.counter;
      pointer   = smart_ptr.pointer;
      *counter += 1;
    }
    return (*this);
  }

  bool Null(void) const
  {
    return (pointer == 0);
  }

  bool NotNull(void) const
  {
    return (pointer != 0);
  }

  T* operator->()
  {
    if (pointer == 0) { throw NullPointerException(); }
    return (pointer);
  }
  
  const T* operator->() const
  {
    if (pointer == 0) { throw NullPointerException(); }
    return (pointer);
  }

  T& operator*(void)
  {
    if (pointer == 0) { throw NullPointerException(); }
    return (*pointer);
  }

  const T& operator*(void) const
  {
    if (pointer == 0) { throw NullPointerException(); }
    return (*pointer);
  }

  T*       Pointer(void) const { return (pointer); }

private:
  void initialize_counter()
  {
    counter  = new unsigned int;
    *counter = 1;
  }
  
  void cleanup()
  {
    *counter -= 1;
    if (*counter == 0)
    {
      if (pointer)
        delete pointer;
      delete counter;
    }
  }
  
  T*            pointer;
  unsigned int* counter;
};

#endif
