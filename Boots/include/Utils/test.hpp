#ifndef  TEST_HPP
# define TEST_HPP

# include <string>
# include <list>
# include <functional>

class UnitTest
{
  struct Test
  {
    std::string                       description;
    std::function<std::string (void)> callback;
  };

  struct Category
  {
    Category(const std::string& name) : name(name) {}
    bool operator==(const std::string& name) const { return (name == this->name); }
    std::string     name;
    std::list<Test> tests;
  };

  typedef std::list<Category> Categories;

public:

  UnitTest(void) : _longest_desc(0)
  {
  }

  void       AddTest(const std::string& category, const std::string& description, std::function<std::string (void)> callback);
  void       Execute(void);

private:
  Categories     _categories;
  unsigned short _longest_desc;
};

#endif

