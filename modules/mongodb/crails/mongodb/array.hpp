#ifndef  MONGODB_ARRAY_HPP
# define MONGODB_ARRAY_HPP

# include <mongo/client/dbclient.h>
# include <vector>

namespace MongoDB
{
  template<typename T>
  struct Array : public mongo::BSONArray
  {
    Array()
    {
    }
    
    Array(const std::vector<T>& array)
    {
      mongo::BSONArrayBuilder builder;
      
      for (unsigned short i = 0 ; i < array.size() ; ++i)
        builder.append(array[i]);
      mongo::BSONArray::operator=(builder.arr());
    }
    
    bool Contains(const T& value) const
    {
      std::vector<T> array = *this;

      return (std::find(array.begin(), array.end(), value) != array.end());
    }

    operator std::vector<T>() const
    {
      std::vector<mongo::BSONElement> elements;
      std::vector<T>                  array;

      this->elems(elements);
      for (unsigned short i = 0 ; i < elements.size() ; ++i)
      {
        if (elements[i].ok())
        {
          T val;
          
          elements[i].Val(val);
          array.push_back(val);
        }
      }
      return (array);
    }
  };
}

#endif