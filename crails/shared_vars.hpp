#ifndef SHARED_VARS_HPP
# define SHARED_VARS_HPP

# include <string>
# include <list>
# include <vector>

namespace Crails
{
  class SharedVars
  {
    struct SharedVar
    {
      SharedVar(const std::string& name) : name(name), ptr(0)
      {}

      template<typename T>
      T& operator*(void)
      {
        return (*(reinterpret_cast<T*>(ptr)));
      }
      
      void operator=(void* ptr)
      {
        this->ptr = ptr;
      }
      
      bool operator==(const std::string& comp) const
      {
        return (name == comp);
      }
      
      std::string name;
      void*       ptr;
    };
    
    typedef std::list<SharedVar> List;
  public:

    void** operator[](const std::string& name)
    {
      List::iterator it, end;
      
      for (it = vars.begin(), end = vars.end() ; it != end ; ++it)
      {
        if (*it == name)
          return (&(it->ptr));
      }
      vars.push_back(SharedVar(name));
      return (&(vars.back().ptr));
    }
    
  private:
    List vars;
  };
}

#endif
