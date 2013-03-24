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

    Database&          Db(void) { return (db); }
    unsigned int       Count(mongo::BSONObj query = mongo::BSONObj());
    const std::string& GetName(void)     const { return (name); }
    const std::string  GetFullName(void) const;
    bool               operator==(const std::string& name) const { return (this->name == name); }

    std::auto_ptr<mongo::DBClientCursor> Query(mongo::Query query = mongo::Query(), int n_to_return = 0, int n_to_skip = 0, mongo::BSONObj* fields_to_return = 0, int query_options = 0, int batch_size = 0)
    {
      return (connection.query(GetFullName(), query, n_to_return, n_to_skip, fields_to_return, query_options, batch_size));
    }
    
    void Find(std::vector<mongo::BSONObj>& out, mongo::Query query = mongo::Query(), int n_to_return = 0, int n_to_skip = 0, mongo::BSONObj* fields_to_return = 0, int query_options = 0)
    {
      connection.findN(out, GetFullName(), query, n_to_return, n_to_skip, fields_to_return, query_options);
    }
    
    void Update(mongo::BSONObj object, mongo::Query query, bool upsert = false, bool multi = false)
    {
      connection.update(GetFullName(), query, object, upsert, multi);
    }
    
    void Insert(mongo::BSONObj object, unsigned int flags = 0)
    {
      connection.insert(GetFullName(), object, flags);
    }
    
    void Remove(mongo::Query query, bool just_one = false)
    {
      connection.remove(GetFullName(), query, just_one);
    }

  private:
    Database&                  db;
    mongo::DBClientConnection& connection;
    const std::string          name;
  };
}

#endif