#ifndef  MONGODB_RESULT_HPP
# define MONGODB_RESULT_HPP

namespace MongoDB
{
  template<typename MODEL>
  class Result
  {
    class iterator
    {
    public:
      iterator() {}
      iterator(mongocxx::cursor::iterator cursor_it) { it = cursor_it; }
      iterator(const iterator& other) { this->it = other.it; }
      iterator& operator=(const iterator& other) { this->it = other.it; return *this; }
      iterator& operator++() { it++; return *this; }

      MODEL operator*() const
      {
        MODEL model;

        model.load(*it);
        return model;
      }

    private:
      mongocxx::cursor::iterator it;
    };

  public:
    void initialize(mongocxx::cursor cursor) { this->cursor = cursor; }

    iterator begin() { return iterator(cursor.begin()); }
    iterator end()   { return iterator(cursor.end());   }

  private:
    mongocxx::cursor cursor;
  };
}

#endif
