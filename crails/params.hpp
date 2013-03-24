#ifndef  PARAMS_HPP
# define PARAMS_HPP

# include "server.hpp"
# include "session_store.hpp"

class Params : public DynStruct
{
  friend class CrailsServer;
public:
  Params(void) : handle(1), response_parsed(0)
  {
  }
  
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
  
  const File*     Upload(const std::string& key) const;

  void Lock(void)   { handle.Wait(); }
  void Unlock(void) { handle.Post(); }
  
  SessionStore    session;

private:
  Sync::Semaphore handle;
  Sync::Semaphore response_parsed;
  Files           files;
};

#endif
