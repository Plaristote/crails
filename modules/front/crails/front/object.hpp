#ifndef  CRAILS_FRONT_OBJECT_HPP
# define CRAILS_FRONT_OBJECT_HPP

# include <cheerp/client.h>
# include <cheerp/clientlib.h>
# include <map>
# include <vector>
# include <string>
# include <boost/lexical_cast.hpp>

#define js_object(...) \
  Crails::Front::Object(std::map<std::string, Crails::Front::Object>({__VA_ARGS__}))
#define js_array(...) \
  Crails::Front::Object(std::vector<Crails::Front::Object>({__VA_ARGS__}))

namespace Crails
{
  namespace Front
  {
    template<typename CLIENT_TYPE>
    class ObjectImpl;

    template<typename ARG>
    struct ApplyParamsToString;

    class Object
    {
    protected:
      client::Object* ptr;
    public:
      Object() { ptr = new client::Object; }
      Object(client::Object* ptr) : ptr(ptr) {}
      Object(client::String* ptr) : ptr((client::Object*)ptr) {}
      Object(const char* str)         { ptr = new client::String(str); }
      Object(const wchar_t* str)      { ptr = new client::String(str); }
      Object(const std::string& str)  { ptr = new client::String(str.c_str()); }
      Object(const std::wstring& str) { ptr = new client::String(str.c_str()); }
      Object(unsigned long number)    { ptr = new client::String(number); }
      Object(unsigned int number)     { ptr = new client::String(number); }
      Object(long number)             { ptr = new client::String(number); }
      Object(int number)              { ptr = new client::String(number); }
      Object(double number)           { ptr = new client::String(number); }
      Object(bool boolean)            { ptr = new client::String((int)boolean); }
      Object(const std::map<std::string, Object>& items) { ptr = new client::Object; set(items); }

      Object(const std::vector<Object>& items)
      {
        auto* array = new client::Array;

        for (auto item : items)
          array->push(*item);
        ptr = array;
      }

      template<typename FUNCTYPE>
      Object(FUNCTYPE func) { ptr = cheerp::Callback(func); }

      Object operator[](const char* key)
      {
        return Object(ptr->operator[](key));
      }

      template<typename CLIENT_TYPE>
      ObjectImpl<CLIENT_TYPE> get(const char* key)
      {
        return ObjectImpl<CLIENT_TYPE>(*(this->operator[](key)));
      }

      client::Object* operator*() const { return ptr; }

      operator std::string() const { return (std::string)(*static_cast<client::String*>(ptr)); }
      operator double()      const { return (double)(*ptr); }

      template<typename T>
      operator std::vector<T>() const
      {
        const client::Array& array = *(static_cast<client::Array*>(ptr));
        std::vector<T> result;
        result.resize(array.get_length());
        for (int i = 0 ; i < array.get_length() ; ++i)
          result[i] = (T)(Object(array[i]));
        return result;
      }

      void set(const std::string& str, Object object)
      {
        ptr->set_(str.c_str(), object.ptr);
      }

      void set(const std::map<std::string, Object>& items)
      {
        for (auto item : items)
          set(item.first, item.second);
      }

      template<typename ...ARGS>
      Object apply(const std::string& name, ARGS... args)
      {
        ((client::EventTarget*)&client::window)->set_("_crails_front_", ptr);

        std::string str      = "_crails_front_." + name + '(';
        std::string str_args = _apply_params('a', args...);

        str = str + str_args;
        str = str + ')';
        return client::eval(str.c_str());
      }

    private:
      std::string _apply_params(char) { return ""; }

      template<typename ARG>
      std::string _apply_params(char i, ARG arg)
      {
        return ApplyParamsToString<ARG>::func(arg, i);
      }

      template<typename ARG, typename ...ARGS>
      std::string _apply_params(char i, ARG arg, ARGS... args)
      {
        return ApplyParamsToString<ARG>::func(arg, i)
          + ',' + _apply_params<ARGS...>(i + 1, args...);
      }
    };

    template<typename ARG>
    struct ApplyParamsToString
    {
      static std::string func(ARG arg, char) { return boost::lexical_cast<std::string>(arg); }
    };

    template<>
    struct ApplyParamsToString<Object>
    {
      static std::string func(Object object, char i)
      {
        std::string varname = "_crails_front_arg_0_";

        varname[18] = i;
        ((client::EventTarget*)&client::window)->set_(varname.c_str(), *object);
        return varname;
      }
    };

    template<typename ARG>
    struct ApplyParamsToString<ObjectImpl<ARG> >
    {
      static std::string func(ObjectImpl<ARG> arg, char i)
      {
        return ApplyParamsToString<Object>::func(Object((client::Object*)(*arg)), i);
      }
    };

    template<>
    struct ApplyParamsToString<client::Object*>
    {
      static std::string func(client::Object* arg, char i)
      {
        return ApplyParamsToString<Object>::func(Object(arg), i);
      }
    };

    template<>
    struct ApplyParamsToString<client::String*>
    {
      static std::string func(client::String* arg, char i)
      {
        return ApplyParamsToString<Object>::func(Object((client::Object*)arg), i);
      }
    };

    template<>
    struct ApplyParamsToString<const char*>
    {
      static std::string func(const char* arg, char i)
      {
        std::string varval("\"");

        varval = varval + arg;
        varval = varval + "\"";
        return varval;
      }
    };

    template<>
    struct ApplyParamsToString<std::string>
    {
      static std::string func(std::string arg, char i)
      {
        return '"' + arg + '"';
      }
    };

    template<typename CLIENT_TYPE>
    class ObjectImpl : public Object
    {
    public:
      ObjectImpl() { ptr = new CLIENT_TYPE; }
      ObjectImpl(CLIENT_TYPE* ptr) { this->ptr = ptr; }
      ObjectImpl(client::Object* ptr) { this->ptr = ptr; }

      CLIENT_TYPE* operator*() const { return static_cast<CLIENT_TYPE*>(ptr); }
      CLIENT_TYPE* operator->() { return static_cast<CLIENT_TYPE*>(ptr); }
    };
  }
}

#endif
