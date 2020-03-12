#ifndef  FRONTEND_COLLECTION_HPP
# define FRONTEND_COLLECTION_HPP

# include <comet/mvc/collection.hpp>
# include <crails/archive.hpp>

namespace Comet
{
  template<typename MODEL>
  class ArchiveCollection : public Collection<MODEL>
  {
  public:
    virtual std::string get_mimetype() const
    {
      return Archive::mimetype;
    }

  protected:
    virtual void parse(const std::string& str)
    {
      IArchive      archive;
      unsigned long size;

      archive.set_data(str);
      archive & size;
      for (unsigned long i = 0 ; i < size ; ++i)
      {
        auto ptr = std::make_shared<MODEL>();
        ptr->serialize(archive);
        Collection<MODEL>::models.emplace(ptr->get_id(), ptr);
      }
    }
  };
}

#endif
