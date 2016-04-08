#ifndef CRAILS_MEMCACHED_HPP
# define CRAILS_MEMCACHED_HPP

# include <string>
# include <functional>
# include <libmemcached-1.0/memcached.h>
# include <crails/logger.hpp>
# include <crails/utils/backtrace.hpp>
# include <boost/thread/tss.hpp>

namespace Crails
{
  class Cache
  {
  public:
    struct Exception : public boost_ext::exception
    {
      Exception(const std::string& message) : message(message) {}
      const char* what() const throw() { return message.c_str(); }
      const std::string message;
    };

    struct Buffer
    {
      Buffer() : data(0), size(0)
      {
      }

      ~Buffer()
      {
        if (data)
          free(data);
      }

      char*  data;
      size_t size;
    };

    Cache();
    ~Cache();

    static Cache& singleton();

    static void discard(const std::string& key);

    template<typename TYPE>
    static TYPE record(const std::string& key, std::function<TYPE ()> func)
    {
      auto& instance = singleton();

      instance.ensure_initialization();
      if (memcached_exist(instance.memc, key.c_str(), key.size()) == MEMCACHED_SUCCESS)
	return instance.existing_record<TYPE>(key, func);
      return instance.new_record<TYPE>(key, func);
    }

  private:
    template<typename TYPE>
    TYPE new_record(const std::string& key, std::function<TYPE ()> func)
    {
      Buffer buffer;
      time_t expiration = 0;
      TYPE result = func();

      serialize<TYPE>(result, (const char**)(&buffer.data), buffer.size);
      if (memcached_add(memc, key.c_str(), key.size(), buffer.data, buffer.size, expiration, 0) != MEMCACHED_SUCCESS)
        Crails::logger << Crails::Logger::Warning << "memcached failed to store value for " << key;
      buffer.data = 0;
      return (result);
    }

    template<typename TYPE>
    TYPE existing_record(const std::string& key, std::function<TYPE ()> func)
    {
      Buffer buffer;
      memcached_return_t memcached_return;
      uint32_t flags = 0;
      buffer.data = memcached_get(memc, key.c_str(), key.size(), &(buffer.size), &flags, &memcached_return);

      if (memcached_return != MEMCACHED_SUCCESS)
      {
        Crails::logger << Crails::Logger::Warning << "memcached failed to retrieve value for " << key;
        return new_record<TYPE>(key, func);
      }
      return unserialize<TYPE>(buffer.data, buffer.size);
    }

    template<typename TYPE>
    TYPE unserialize(const char* buffer, size_t size)
    {
      if (size != sizeof(TYPE))
        throw Cache::Exception("Value stored did not contain the expected type of object");
      return TYPE(*(reinterpret_cast<const TYPE*>(buffer)));
    }

    template<typename TYPE>
    void serialize(TYPE source, const char** buffer, size_t& size)
    {
      *buffer = (const char*)(&source);
      size    = sizeof(TYPE);
    }

    void ensure_initialization();

    static const std::string config;
    static boost::thread_specific_ptr<Cache> instance;
    memcached_st* memc;
  };
}

#endif
