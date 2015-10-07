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

    std::auto_ptr<mongo::DBClientCursor> query(mongo::Query query = mongo::Query(), int n_to_return = 0, int n_to_skip = 0, mongo::BSONObj* fields_to_return = 0, int query_options = 0, int batch_size = 0);
    
    void find(std::vector<mongo::BSONObj>& out, mongo::Query query = mongo::Query(), int n_to_return = 0, int n_to_skip = 0, mongo::BSONObj* fields_to_return = 0, int query_options = 0);
    
    void update(mongo::BSONObj object, mongo::Query query, bool upsert = false, bool multi = false);
    void insert(mongo::BSONObj object, unsigned int flags = 0);
    void remove(const mongo::Query& query, bool just_one = false);

  private:
    Database&                  db;
    mongo::DBClientConnection& connection;
    const std::string          name;
  };
}

#endif