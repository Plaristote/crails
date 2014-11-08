#ifndef  SMART_POINTER
# define SMART_POINTER

# include <memory>

struct NullPointerException : public std::exception
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
    counter  = new unsigned int;
    *counter = 1;
  }

  SmartPointer(T* pointer) : pointer(pointer)
  {
    counter  = new unsigned int;
    *counter = 1;
  }

  SmartPointer(const std::unique_ptr<T>& ptr)
  {
    pointer  = ptr.get();
    counter  = new unsigned int;
    *counter = 1;
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
    *counter -= 1;
    if (*counter == 0)
    {
      if (pointer)
        delete pointer;
      delete counter;
    }
  }

  SmartPointer& operator=(T* n_pointer)
  {
    *counter -= 1;
    if (*counter == 0)
    {
      delete pointer;
      delete counter;
    }
    pointer  = n_pointer;
    counter  = new unsigned int;
    *counter = 1;
    return (*this);
  }

  SmartPointer& operator=(const SmartPointer& smart_ptr)
  {
    *counter -= 1;
    if (*counter == 0)
    {
      delete pointer;
      delete counter;
    }
    counter   = smart_ptr.counter;
    pointer   = smart_ptr.pointer;
    *counter += 1;
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
  const T* Pointer(void) const { return (pointer); }

private:
  T*            pointer;
  unsigned int* counter;
};

#endif
