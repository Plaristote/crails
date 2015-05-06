#ifndef  PARAMS_HPP
# define PARAMS_HPP

# include "server.hpp"
# include "session_store.hpp"

struct MultipartParser;
class  ActionRequestHandler;

class Params : public DynStruct
{
  friend struct CrailsServer;
  friend struct MultipartParser; 
  friend class  ActionRequestHandler;
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
  const File*     operator[](const std::string& key) const { return (Upload(key)); }
#ifdef __llvm__
  Data            operator[](const char* key)       { return (DynStruct::operator[](std::string(key))); }
  const File*     operator[](const char* key) const { return (Upload(key)); }
#endif
  const File*     Upload(const std::string& key) const;

  void Lock(void)   { handle.Wait(); }
  void Unlock(void) { handle.Post(); }

  DynStruct&       Session(void)       { return ((*session).Session()); }
  const DynStruct& Session(void) const { return ((*session).Session()); }

private:
  SmartPointer<SessionStore> session;
  Sync::Semaphore            handle;
  Sync::Semaphore            response_parsed;
  Files                      files;
};

#endif
