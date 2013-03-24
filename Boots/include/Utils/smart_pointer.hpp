#ifndef  SMART_POINTER
# define SMART_POINTER

# include <memory>

template<typename T>
class SmartPointer
{
public:
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
      delete pointer;
      delete counter;
    }
  }
  
  T* operator->()
  {
    return (pointer);
  }
  
  T& operator*(void)
  {
    return (*pointer);
  }
  
  const T& operator*(void) const
  {
    return (*pointer);
  }
  
private:
  T*            pointer;
  unsigned int* counter;
};

#endif