#ifndef  MONGODB_COLLECTION_HPP
# define MONGODB_COLLECTION_HPP

# include <mongo/client/dbclient.h>

namespace MongoDB
{
  class Database;

  class Collection
  {
  public:
    Collection(mongo::DBClientConnection& c, Database& db, const std::string& name) : db(db), connection(c), name(name)
    {}

    Database&          get_database(void)       { return (db); }
    unsigned int       count(const mongo::Query& query = mongo::Query());
    const std::string& get_name(void)     const { return (name); }
    const std::string  get_full_name(void) const;
    bool               operator==(const std::string& name) const { return (this->name == name); }

    std::auto_ptr<mongo::DBClientCursor> query(mongo::Query query = mongo::Query(), int n_to_return = 0, int n_to_skip = 0, mongo::BSONObj* fields_to_return = 0, int query_options = 0, int batch_size = 0)
    {
      return (connection.query(get_full_name(), query, n_to_return, n_to_skip, fields_to_return, query_options, batch_size));
    }
    
    void find(std::vector<mongo::BSONObj>& out, mongo::Query query = mongo::Query(), int n_to_return = 0, int n_to_skip = 0, mongo::BSONObj* fields_to_return = 0, int query_options = 0)
    {
      connection.findN(out, get_full_name(), query, n_to_return, n_to_skip, fields_to_return, query_options);
    }
    
    void update(mongo::BSONObj object, mongo::Query query, bool upsert = false, bool multi = false)
    {
      connection.update(get_full_name(), query, object, upsert, multi);
    }
    
    void insert(mongo::BSONObj object, unsigned int flags = 0)
    {
      connection.insert(get_full_name(), object, flags);
    }
    
    void remove(const mongo::Query& query, bool just_one = false)
    {
      connection.remove(get_full_name(), query, just_one);
    }

  private:
    Database&                  db;
    mongo::DBClientConnection& connection;
    const std::string          name;
  };
}

#endif