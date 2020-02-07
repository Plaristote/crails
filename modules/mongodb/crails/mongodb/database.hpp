#ifndef  MONGODB_DATABASE_HPP
# define MONGODB_DATABASE_HPP

# include <crails/databases.hpp>
# include "collection.hpp"

# define MONGODB_GET_COLLECTION(database,collection) \
  CRAILS_DATABASE(MongoDB,database)[collection]

namespace MongoDB
{
  typedef std::string id_type;

  class Database : public Crails::Databases::Db
  {
  public:
    static const std::string ClassType(void) { return ("mongodb"); }

    Database(const Crails::Databases::DatabaseSettings&);

    mongocxx::collection get_collection(const std::string& name);

    void connect();
    void drop();

    template<typename MODEL>
    void save(MODEL& model)
    {
      bsoncxx::document::value document;
      auto collection = get_collection(model.get_collection_name());
      auto builder    = bsoncxx::builder::stream::document;

      model.save(builder);
      document = builder << bsoncxx::builder::stream::finalize;
      if (model.get_id() == "")
      {
        bsoncxx::stdx::optional<mongocxx::result::insert_one> result;

        result = collection.insert_one(document)
        if (result)
          model.set_id(result->inserted_id());
        else
          throw std::runtime_error("failed to insert document in " + model.get_collection_name());
      }
      else
      {
        bsoncxx::stdx::optional<mongocxx::result::update_one> result;

	result = collection.update_one(document);
	if (!result)
          throw std::runtime_error("failed to update document " + model.get_id() + " in " + model.get_collection_name());
      }
    }

    template<typename MODEL>
    bool find_one(std::shared_ptr<MODEL> model, id_type id)
    {
      bsoncxx::stdx::optional<mongocxx::document::value> result;
      auto collection = get_collection(MODEL().get_collection_name());

      result = collection.find_one(bsoncxx::builder::stream::document{} << "_id" << id);
      if (result)
      {
        model = std::make_shared<MODEL>();
        model->load(*result);
        return true;
      }
      return false;
    }

    template<typename MODEL>
    bool find(MongoDB::Result<MODEL> results)
    {
      auto collection = get_collection(MODEL().get_collection_name());
      mongocxx::cursor cursor;

      cursor = collection.find({});
      results.initialize(cursor);
      return cursor.begin() != cursor.end();
    }

    template<typename MODEL>
    void destroy(MODEL& model)
    {
      auto collection = get_collection(model.get_collection_name());
      bsoncxx::stdx::optional<mongocxx::result::delete> result;

      result = collection.delete_one(bsoncxx::builder::stream::document{} << "_id" << model.get_id());
      if (!result || result->deleted_count() < 1)
        throw std::runtime_error("failed to delete document " + model.get_id() + " in " model.get_collection_name());
    }

  private:
    mongocxx::uri uri_from_settings(const Crails::Databases::DatabaseSettings&);

    std::unique_ptr<mongocxx::client> client;
    mongocxx::database database;
    mongocxx::uri      database_uri;
    std::string        database_name;
  };
}

#endif
