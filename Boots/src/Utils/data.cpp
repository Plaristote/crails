#include "Utils/datatree.hpp"
#include <fstream>
#include <iostream>

using namespace std;

void      Data::Output(std::ostream& stream, unsigned char indent)
{
  Data::iterator it  = this->begin();
  Data::iterator end = this->end();

  for (unsigned char toIndent = indent ; toIndent ; --toIndent)
    stream << ' ';
  stream << "[" << _data->key << "] -> '" << _data->value << "'" << std::endl;
  for (; it != end ; ++it)
    (*it).Output(stream, indent + 2);
}

static void WriteFile(ofstream& stream, const Data data, unsigned char indent)
{
  Data::const_iterator it  = data.const_begin();
  Data::const_iterator end = data.const_end();

  for (unsigned char toIndent = indent ; toIndent ; --toIndent)
    stream << ' ';
  stream << data.Key() << ": " << data.Value() << endl;
  for (; it != end ; ++it)
    (*it).Output(stream, indent + 2);
}

bool DataTree::Save(const string& path)
{
  ofstream file;
  
  file.open(path.c_str());
  if (file.is_open())
  {
    WriteFile(file, this, 0);
    file.close();
    return (true);
  }
  return (false);
}

Data::Data(DataTree* d) : _data(d)
{
  d->pointers++;
}

Data::Data(const std::string& key, DataBranch* father)
{
  _data = new DataBranch();
  _data->father   = father;
  _data->key      = key;
  _data->pointers = 1;
  _data->nil      = true;
  if (father)
    father->children.push_back(_data);
}

Data::~Data()
{
  if (_data)
  {
    if (_data->pointers > 0)
      _data->pointers--;
    if ((_data->nil || !_data->father) && _data->root == false && _data->pointers == 0)
      delete _data;
  }
}

void Data::PushBack(Data d)
{
  if (_data)
  {
    DataBranch* parent = _data;

    _data->children.push_back(d._data);
    d._data->father = _data;
    d._data->nil    = false;
    while (parent)
    {
      parent->nil   = false;
      parent = parent->father;
    }
  }
}

Data Data::operator[](unsigned int n)
{
  DataBranch::Children::iterator it  = _data->children.begin();
 
  std::advance(it, n);
  if (it != _data->children.end())
    return (Data((*it)));
  return (Data());
}

Data Data::operator[](const std::string& key)
{
  if (_data)
  {
    DataBranch::Children::iterator it  = _data->children.begin();
    DataBranch::Children::iterator end = _data->children.end();

    _data->nil = false;
    for (; it != end ; ++it)
    {
      if ((*it)->key == key)
	return (Data((*it)));
    }
  }
  return (Data(key, _data));  
}

const Data Data::operator[](const std::string& key) const
{
  if (_data)
  {
    DataBranch::Children::const_iterator it  = _data->children.begin();
    DataBranch::Children::const_iterator end = _data->children.end();

    _data->nil = false;
    for (; it != end ; ++it)
    {
      if ((*it)->key == key)
	return (Data((*it)));
    }
  }
  return (Data(key, _data));
}

void        Data::Duplicate(Data var)
{
  Data::iterator it  = var.begin();
  Data::iterator end = var.end();

  if (_data == 0)
  {
    _data = new DataBranch;
    _data->pointers++;
  }
  _data->key   = var.Key();
  _data->value = var.Value();
  _data->nil   = false;

  for (; it != end ; ++it)
  {
    Data        children = *it;
    DataBranch* tmp      = new DataBranch;
    
    tmp->father = _data;
    _data->children.push_back(tmp);
    Data(tmp).Duplicate(children);
  }
}

const Data& Data::operator=(const Data& var)
{
  if (var.Nil() && !Nil())
  {
    if (_data->pointers > 0)
      _data->pointers--;
    _data = 0;
  }
  else if (Nil())
  {
    if (_data && _data->pointers == 1)
      delete _data;
    _data = var._data;
    if (_data)
      _data->pointers++;
  }
  else
    _data->value = var.Value();
  return (*this);
}

void Data::CutBranch(void)
{
  DataBranch::Children::iterator it  = _data->children.begin();
  DataBranch::Children::iterator end = _data->children.end();
  
  while (it != end)
  {
    (*it)->nil    = true;
    (*it)->father = 0;
    if ((*it)->pointers == 0)
      delete (*it);
    it = _data->children.erase(it);
  }
  Remove();
}

DataBranch::~DataBranch()
{
  Children::iterator it;
  Children::iterator end;

  //std::cout << "Deleting databranch" << endl;
  if (father)
  {
    it  = father->children.begin();
    end = father->children.end();
    
    for (; it != end ; ++it)
    {
      if ((*it) == this)
      {
	father->children.erase(it);
	break ;
      }
    }
  }
  while (children.begin() != children.end())
  {
    DataBranch* child = *children.begin();

    child->father = 0;
    if (child->pointers == 0)
      delete child;
    children.erase(children.begin());
  }
}

void Data::Serialize(Utils::Packet& packet) const
{
  std::string json;

  DataTree::Writers::StringJSON(*this, json);
  packet << json;
}

void Data::Unserialize(Utils::Packet& packet)
{
  std::string json;

  packet >> json;
  _data           = DataTree::Factory::StringJSON(json);
  if (_data)
  {
    _data->root     = false;
    _data->nil      = true;
    _data->pointers = 1;
  }
}

std::string Data::ToJson(void) const
{
  std::string result;

  DataTree::Writers::StringJSON(*this, result);
  return (result);
}
