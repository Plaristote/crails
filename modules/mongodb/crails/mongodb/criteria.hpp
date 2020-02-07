#ifndef  MONGODB_CRITERIA_HPP
# define MONGODB_CRITERIA_HPP

namespace MongoDB
{
  class Condition
  {
  };

  class Field : public std::string
  {
  public:
    template<typename VAL>
    bsoncxx::builder::stream::document operator==(const VAL& val) const
    {
      bsoncxx::builder::stream::document document;

      document << c_str() << val;
      return document;
    }

    template<typename VAL>
    bsoncxx::builder::stream::document operator>(const VAL val) const
    {
      bsoncxx::builder::stream::document document;

      document << c_str() << "$gt" << val;
      return document;
    }
  };

  class Criteria
  {
  public:
    Criteria& where(

  private:
    bsoncxx::builder::stream::document document;
  };
}

#endif
