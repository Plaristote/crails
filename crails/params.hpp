#ifndef  PARAMS_HPP
# define PARAMS_HPP

# include <Boots/Utils/smart_pointer.hpp>
# include <Boots/Sync/semaphore.hpp>
# include <Boots/Utils/dynstruct.hpp>
# include "session_store.hpp"

namespace Crails
{
  struct Server;
  struct MultipartParser;
  class  ActionRequestHandler;
  class  BodyParser;
  class  RequestMultipartParser;

  class Params : public DynStruct
  {
    friend struct Server;
    friend struct MultipartParser; 
    friend class  ActionRequestHandler;
    friend class  BodyParser;
    friend class  RequestMultipartParser;
  public:
    Params(void);
    ~Params(void);

    //
    struct File
    {
      bool operator==(const std::string& comp) const { return (key == comp); }
      
      std::string temporary_path;
      std::string name;
      std::string key;
      std::string mimetype;
    };

    typedef std::list<File> Files;
    //
    
    Data            operator[](const std::string& key)       { return (DynStruct::operator[](key)); }
    const File*     operator[](const std::string& key) const { return (get_upload(key)); }
  #ifdef __llvm__
    Data            operator[](const char* key)       { return (DynStruct::operator[](std::string(key))); }
    const File*     operator[](const char* key) const { return (get_upload(key)); }
  #endif
    const File*     get_upload(const std::string& key) const;

    void lock(void)   { handle.Wait(); }
    void unlock(void) { handle.Post(); }

    DynStruct&       get_session(void)       { return (session->to_data()); }
    const DynStruct& get_session(void) const { return (session->to_data()); }

  private:
    std::unique_ptr<SessionStore> session;
    Sync::Semaphore               handle;
    Sync::Semaphore               response_parsed;
    Files                         files;
  };
}

#endif
