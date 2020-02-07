#ifndef  MONGODB_MODEL_HPP
# define MONGODB_MODEL_HPP

namespace MongoDB
{
  class Model
  {
  public:
    virtual std::string get_collection_name()           = 0;
    virtual void        save(bsoncxx::builder::stream&) = 0;
    virtual void        load(bsoncxx::document::value&) = 0;
  private:
  };
}

#endif
