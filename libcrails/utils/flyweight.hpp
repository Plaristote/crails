#ifndef  FLYWEIGHT_HPP
# define FLYWEIGHT_HPP

# include <list>
# include <algorithm>
# include <functional>
# include <memory>

template<typename TYPE, typename KEY_TYPE>
class Flyweight
{
  struct Instance
  {
    bool operator==(KEY_TYPE comp) const { return key == comp; }
    bool should_garbage_collect()  const { return ptr == nullptr || ptr.use_count() == 1; }

    KEY_TYPE              key;
    std::shared_ptr<TYPE> ptr;
  };

  typedef std::list<Instance> Instances;

public:
  virtual std::shared_ptr<TYPE> create_instance(KEY_TYPE key) = 0;

  std::shared_ptr<TYPE> require(KEY_TYPE key)
  {
    auto iterator = std::find(instances.begin(), instances.end(), key);

    if (iterator == instances.end())
    {
      Instance instance;

      instance.key = key;
      instance.ptr = create_instance(key);
      instances.push_back(instance);
      return instance.ptr;
    }
    return iterator->ptr;
  }
  
  bool contains(KEY_TYPE key)
  {
    auto iterator = std::find(instances.begin(), instances.end(), key);
    
    return iterator != instances.end();
  }

  void garbage_collect(void)
  {
    auto it = instances.begin();

    while (it != instances.end())
    {
      if (it->should_garbage_collect())
        it = instances.erase(it);
      else
        ++it;
    }
  }

private:
  Instances instances;
};


#endif
