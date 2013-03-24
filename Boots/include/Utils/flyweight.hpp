#ifndef  FLYWEIGHT_HPP
# define FLYWEIGHT_HPP

# include <list>
# include <algorithm>
# include <functional>

template<typename TYPE, typename KEY_TYPE>
class Flyweight
{
  struct Instance
  {
    Instance(TYPE* instance, KEY_TYPE key) : key(key), ref_count(0), instance(instance) {}

    bool operator==(KEY_TYPE comp) const { return (key == comp); }

    KEY_TYPE       key;
    unsigned short ref_count;
    TYPE*          instance;
  };

  typedef std::list<Instance> Instances;

public:
  class Item
  {
  public:
    Item(void) : instance(0) {}

    Item(Instance* instance) : instance(instance)
    {
      instance->ref_count++;
    }

    ~Item(void)
    {
      if (instance)
        instance->ref_count--;
    }

    const Item& operator=(const Item& other)
    {
      if (instance != 0)
        instance->ref_count--;
      instance = other.instance;
      if (instance != 0)
        instance->ref_count++;
      return (*this);
    }

    TYPE& operator*(void) { return (*(instance->instance)); }

  private:
    Instance* instance;
  };

  Flyweight() {}

  ~Flyweight()
  {
    std::for_each(instances.begin(), instances.end(), [](Instance& cur)
    {
      delete cur.instance;
    });
  }

  virtual TYPE* CreateInstance(KEY_TYPE key) = 0;

  Item      Require(KEY_TYPE key)
  {
    typename Instances::iterator cur, last;

    for (cur = instances.begin(), last = instances.end() ; cur != last ; ++cur)
    {
      if ((*cur) == key)
        return (Item(&(*cur)));
    }
    return (Item(&Instantiate(key)));
  }

  void      GarbageCollect(void)
  {
    typename Instances::iterator cur, last;

    for (cur = instances.begin(), last = instances.end() ; cur != last ;)
    {
      if (cur->ref_count == 0)
      {
        delete cur->instance;
        cur = instances.erase(cur);
      }
      else
        ++cur;
    }
  }

private:
  Instance&     Instantiate(KEY_TYPE key)
  {
    TYPE* ptr = CreateInstance(key);

    instances.push_back(Instance(ptr, key));
    return (*(instances.rbegin()));
  }

  Instances instances;
};


#endif
