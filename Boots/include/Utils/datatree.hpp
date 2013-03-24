#ifndef  DATABRANCH
# define DATABRANCH

# include <list>
# include <string>
# include <sstream>
# include <iostream>

# include "serializer.hpp"

/*! \class DataBranch
 * \brief Structure containing every data assigned to a branch of a DataTree. Best used wrapped in the Data class. */
struct DataBranch
{
  typedef std::list<DataBranch*> Children;
  
  DataBranch()
  {
    pointers = 0;
    father   = 0;
    nil      = true;
    root     = false;
  }

  virtual ~DataBranch();

  std::string  key;
  std::string  value;
  Children     children;
  DataBranch*  father;
  bool         nil, root;
  unsigned int pointers;
};

class DataTree;

/*! \class Data
 * \brief Wrapper for the DataBranch and DataTree classes, allowing easy manipulation of the corresponding data.
 */
class Data : public Utils::Serializable
{
  Data(const std::string&, DataBranch*);
public:
  typedef DataBranch::Children Children;

  Data(void)          : _data(0)       { }
  Data(DataTree*   d);
  Data(DataBranch* d) : _data(d)       { _data->pointers++; }
  virtual ~Data(void);

  /*! \brief Returns a Data containing the child DataBranch corresponding to key if it exists, or Nil Data otherwise (see the Nil method) */
  Data         operator[](const std::string& key);
  const Data   operator[](const std::string& key) const;
  Data         operator[](unsigned int it);

  std::string  Key(void)   const { return (_data ? _data->key             : ""); }
  std::string  Value(void) const { return (_data ? _data->value           : ""); }

  void         SetKey(const std::string& newKey) { if (_data) _data->key = newKey; }
  /*! \brief Copy all branches from var's tree and duplicate them under this branch */
  void         Duplicate(Data var);

  const Data& operator=(const Data& var);

  template<typename T>
  void operator=(const T& var)
  {
    if (_data)
    {
      std::stringstream stream;

      stream << var;
      _data->value = stream.str();
      _data->nil   = false;
    }
  }

  template<typename T>
  bool operator==(const T& var) const
  {
    if (!_data) return (false);
    std::stringstream stream;
    
    stream << var;
    return (_data->value == stream.str());
  }

  template<typename T>
  operator T() const
  {
    std::stringstream stream;
    T                 out;

    if (_data)
      stream << _data->value;
    stream >> out;
    return (out);
  }

  /*! \brief Sets the DataBranch to nil, thus removing it cleanly when Data is destroyed */
  void Remove(void)       { if (_data) _data->nil = true; }
  void CutBranch(void);
  /*! \brief Checks if the current Data is attached to a DataBranch */
  bool Nil(void) const    { return (!_data || _data->nil);          }
  bool NotNil(void) const { return (!(Nil()));                      }
  Data Parent(void)       { return (_data ? _data->father : _data); }
  /*! \brief Move the parameter Data into this Data's children. */
  void PushBack(Data d);
  /*! \brief For debug purposes, writes the content of the DataTree from the current branch to the standard output */
  //void Output(unsigned char indent = 0);
  void Output(std::ostream& stream = std::cout, unsigned char indent = 0);
  
  /*! \brief Returns the amount of children the branch has */
  unsigned int Count(void) const { return (_data ? _data->children.size() : 0); }
  
  class my_iterator : public std::iterator_traits<Children::iterator>
  {
  public:
    my_iterator(Children::iterator it)           { _it = it;               }
    void operator=(Children::iterator it)     { _it = it;               }
    bool operator==(const my_iterator& it) const { return (_it == it._it); }
    bool operator!=(const my_iterator& it) const { return (_it != it._it); }
    Data operator*() const                    { return (Data(*_it));    }
    void operator++()                         { ++_it;                  }
    void operator--()                         { --_it;                  }
  private:
    Children::iterator _it;
  };

  class const_my_iterator : public std::iterator_traits<Children::const_iterator>
  {
  public:
    const_my_iterator(Children::const_iterator it)     { _it = it;               }
    void operator=(Children::const_iterator it)     { _it = it;               }
    bool operator==(const const_my_iterator& it) const { return (_it == it._it); }
    bool operator!=(const const_my_iterator& it) const { return (_it != it._it); }
    Data operator*() const                          { return (Data(*_it));    }
    void operator++()                               { ++_it;                  }
    void operator--()                               { --_it;                  }
  private:
    Children::const_iterator _it;
  };

  my_iterator       begin()             { return (my_iterator(_data->children.begin()));       }
  const_my_iterator const_begin() const { return (const_my_iterator(_data->children.begin())); }
  my_iterator       end()               { return (my_iterator(_data->children.end()));         }
  const_my_iterator const_end() const   { return (const_my_iterator(_data->children.end()));   }

  void Serialize(Utils::Packet& packet) const;
  void Unserialize(Utils::Packet& packet);

protected:
  DataBranch* _data;
};

namespace Yaml
{
  class Parser;
}

namespace ShinyLang
{
  class Parser;
}

namespace Json
{
  class Parser;
}

/*! \class DataTree
 * \brief Represents the root DataBranch */
class DataTree : public DataBranch
{
  friend class Data;
  friend class Yaml::Parser;
  friend class ShinyLang::Parser;
  friend class Json::Parser;
public:
  DataTree() : DataBranch() { root = true; }

  /*! \class Factory
   * \brief Contains a set of procedures for loading DataTrees
   */
  struct Factory
  {
    /*! \brief Loads a DataTree from a Yaml file */
    static DataTree* Yaml(const std::string& filename);
    /*! \brief Loads a DataTree from a ShinyLang file */
    static DataTree* ShinyLang(const std::string& filename);
    /*! \brief Loads a DataTree from a JSON file */
    static DataTree* JSON(const std::string& filename);
    /*! \brief Loads a DataTree from a JSON string */
    static DataTree* StringJSON(const std::string& str);
  };

  struct Writers
  {
      static bool JSON(Data, const std::string& filename);
      static bool StringJSON(Data, std::string& str);
  };

  friend struct Factory;

  Data operator[](const std::string& string)
  {
    Data tmp(this);

    return (tmp[string]);
  }

  /*! \brief Saves the modification to the datas from this DataTree to the file it was loaded from  */
  bool Save(void) { return (Save(source)); }
  /*! \brief Saves the modification to the datas from this DataTree to the file specified as a parameter  */
  bool Save(const std::string& path);

  const std::string& GetSourceFile(void) { return (source); }

private:
  std::string source;
};

#endif
 
