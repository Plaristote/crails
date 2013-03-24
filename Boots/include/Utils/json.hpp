#ifndef  DATATREE_JSON_HPP
# define DATATREE_JSON_HPP

# include "datatree.hpp"

namespace Json
{
  class Parser
  {
  public:
    Parser(const std::string&, bool filename = true);

    DataTree*     Run(void);

  private:
    void          ParseValue(DataBranch*);
    void          ParseString(DataBranch*);
    void          ParseOther(DataBranch*);
    void          ParseObject(DataBranch*);
    void          ParseArray(DataBranch*);

    std::string   _source;
    std::string   _str;
    unsigned int  _it;
    bool          _fileLoaded;
  };
}

#endif
