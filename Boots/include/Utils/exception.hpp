#ifndef  MY_EXCEPTION_HPP
# define MY_EXCEPTION_HPP

# include <exception>
# include <string>
# include <sstream>
# include <thread>
# ifdef __GNUG__
#  include <cxxabi.h>
# endif

struct AllException : public std::exception
{
  enum Details
  {
    NoDetails = 0,
    Thread    = 1,
    Type      = 2,
    Code      = 4,
    Pointer   = 8
  };

  virtual ~AllException() throw() {}
  const char*   what(void)    const throw() { return (message.c_str()); }
  unsigned char GetCode(void) const         { return (code);            }

protected:
  AllException(const std::string& message, unsigned char code = 0) : message(message), code(code) {}

  std::string   message;
  unsigned char code;  
};

template<class T>
struct Exception : public AllException
{
  T* instance;

  Exception(T* pointer, const std::string& message, unsigned char code = 0, int details = Thread | Type) : AllException(message, code)
  {
    std::stringstream stream;

    instance = pointer;
    if (details & Thread)
    { stream << "[Thread::" << (std::this_thread::get_id()) << ']'; }
    if (details & Type)
    { stream << '[' << cpp_demangle_symbol(typeid(T).name()) << ']'; }
    if (details & Pointer)
    { stream << '[' << (pointer) << ']'; }
    if (details & Code)
    { stream << "[Code::" << code << ']'; }
    if (details != 0)
      stream << ' ';
    stream << message;
    this->message = stream.str();
  }

  static void Raise(const std::string& message, unsigned char code = 0)
  {
    throw Exception(0, message, code, Thread | Type | (code != 0 ? Code : 0));
  }
  
  static void Raise(T* instance, const std::string& message, unsigned char code = 0)
  {
    throw Exception(instance, message, code, Thread | Type | Pointer | (code != 0 ? Code : 0));
  }
  
private:
# ifdef __GNUG__
  std::string cpp_demangle_symbol(const char* name)
  {
    int         status;
    char*       demangled = abi::__cxa_demangle(name, 0, 0, &status);
    std::string result(demangled);

    free(demangled);
    return (result);
  }
# else
  std::string cpp_demangle_symbol(const char* name)
  {
    return (name);
  }
# endif
};


#endif
