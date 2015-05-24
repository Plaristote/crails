#include "Boots/Utils/json.hpp"
#include <fstream>
#include <iostream>

using namespace std;
using namespace Json;

void Parser::ParseString(DataBranch* value)
{
  unsigned int start = ++_it;

  for (; _it < _str.length() ; ++_it)
  {
    if ((_str[_it] == '"') && (_str[_it - 1] != '\\'))
      break ;
  }
  if (_it < _str.length())
  {
    value->value = _str.substr(start, _it - start);
    for (unsigned int i = 0 ; i < value->value.length() ; ++i)
    {
      if (value->value[i] == '\\' && value->value[i + 1] == '"')
        value->value.erase(i, 1);
    }
  }
}

void Parser::ParseOther(DataBranch* value)
{
  unsigned int start = _it;
  unsigned int endSpace;

  for (; _it < _str.length() && (_str[_it] != ',' && _str[_it] != '}' && _str[_it] != ']') ; ++_it);
  endSpace = _it - 1;
  if (_str[endSpace] == ' ' || _str[endSpace] == '\n' || _str[endSpace] == '\r')
    for (; endSpace > start && (_str[endSpace] == ' ' || _str[endSpace] == '\n' || _str[endSpace] == '\r') ; --endSpace);
  if (_it < _str.length())
    value->value = _str.substr(start, (endSpace + 1) - start);
}

void Parser::ParseValue(DataBranch* data)
{
  for (; _it < _str.length() && _str[_it] == ' ' ; ++_it);
  if (_it < _str.length())
  {
    if (_str[_it] == '{')
      ParseObject(data);
    else if (_str[_it] == '[')
      ParseArray(data);
    else if (_str[_it] == '"')
      ParseString(data);
    else
      ParseOther(data);
  }
}

void Parser::ParseObject(DataBranch* value)
{
  int         begKey;

  ++_it;
  while (_it < _str.length())
  {
    for (; _it < _str.length() && (_str[_it] == ' ' || _str[_it] == '\n' || _str[_it] == '\r') ; ++_it);
    if (_it < _str.length())
    {
      std::string key;

      // OBJECT KEY - OBJECT END
      if (_str[_it] == '"') // Quoted key
      {
        for (begKey = ++_it ; _it < _str.length() && (_str[_it] != '"' || _str[_it] == '\\') ; ++_it);
        if (_it >= _str.length())
          throw "String: Syntax Error";
        key = _str.substr(begKey, _it - begKey);
        for (_it = _it + 1 ; _it < _str.length() && _str[_it] == ' ' ; ++_it);
        if (_it >= _str.length() || _str[_it] != ':')
          throw "Object: Syntax Error";
        ++_it;
      }
      else if (_str[_it] == '}') // End of object
      {
        ++_it;
        break ;
      }
      else // Unquoted key
      {
        for (begKey = _it ; _it < _str.length() && _str[_it] != ':' ; ++_it);
        if (_it >= _str.length())
          throw "Object: Syntax Error";
        key = _str.substr(begKey, _it - begKey);
      }

      // MAKE THE BRANCH
      DataBranch* child = new DataBranch;

      child->father = value;
      child->nil    = false;
      child->key    = key;
      value->children.push_back(child);

      // OBJECT VALUE
      for (; _it < _str.length() && (_str[_it] == ' ' || _str[_it] == '\n' || _str[_it] == '\r') ; ++_it);
      ParseValue(child);

      for (; _it < _str.length() && _str[_it] != ',' && _str[_it] != '}' ; ++_it);
      if (_str[_it] == ',') ++_it;
    }
  }
}

void Parser::ParseArray(DataBranch* value)
{
  ++_it;
  while (_it < _str.length())
  {
    for (; _it < _str.length() && (_str[_it] == ' ' || _str[_it] == '\n' || _str[_it] == '\r') ; ++_it);
    if (_it >= _str.length())
      throw "Unclosed array";
    if (_str[_it] == ']')
      break ;
    DataBranch* child = new DataBranch;

    child->father = value;
    child->nil    = false;
    value->children.push_back(child);
    ParseValue(child);

    for (; _it < _str.length() && _str[_it] != ',' && _str[_it] != ']' ; ++_it);
    if (_str[_it] == ',')
      ++_it;
  }
}

Parser::Parser(const string& filename, bool filepath)
{
  _fileLoaded = true;
  if (filepath)
  {
    std::ifstream file(filename.c_str());

    _source = filename;
    if ((_fileLoaded = file.is_open()))
    {
      long  begin, end;
      long  size;
      char* raw;

      begin     = file.tellg();
      file.seekg (0, ios::end);
      end       = file.tellg();
      file.seekg(0, ios::beg);
      size      = end - begin;
      raw       = new char[size + 1];
      file.read(raw, size);
      file.close();
      raw[size] = 0;
      _str      = raw;
      delete[] raw;
    }
    else
    {
      std::cerr << filename << ": can't open file." << std::endl;
      return ;
    }
  }
  else
    _str = filename;
    
  for (unsigned int i = 0 ; i < _str.size() ; ++i)
  {
    if (_str[i] == '\t')
      _str[i] = ' ';
    if (_str[i] == '"')
      for (; i < _str.size() && _str[i] != '"' && (i == 0 || _str[i] - 1 != '\\') ; ++i);
  }
}

DataTree* Parser::Run()
{
  if (_fileLoaded)
  {
    DataTree* data = new DataTree;

    data->source = _source;
    try
    {
      _it            = 0;
      ParseValue(data);
    }
    catch (const char* error)
    {
      std::cout << "/!\\ Parse Error at " << _it << " : " << error << std::endl;
      delete data;
      data           = 0;
    }
    return (data);
  }
  return (0);
}

DataTree* DataTree::Factory::JSON(const std::string& filename)
{
  Parser parser(filename);

  return (parser.Run());
}

DataTree* DataTree::Factory::StringJSON(const std::string& filename)
{
  Parser parser(filename, false);

  return (parser.Run());
}

//
// File Writer
//

static bool isNumeric(std::string& str)
{
  unsigned int i = 0;

  for (i = 0 ; i < str.size() ; ++i)
  {
    if ((str[i] < '0' || str[i] > '9') && str[i] != '.')
      return (false);
  }
  {
    std::stringstream stream;
    
    stream << atoi(str.c_str());
    str = stream.str();
  }
  atoi(str.c_str());
  return (true);
}

static std::string appendArray(Data data, unsigned short indent = 0);
static std::string appendObject(Data data, unsigned short indent = 0);

static std::string appendValue(Data data, unsigned short indent = 0)
{
    std::string toWrite;

    if (data.begin() != data.end())
    {
        // Object or Array
        bool isArray = true;

        Data::iterator arrChkIt  = data.begin();
        Data::iterator arrChkEnd = data.end();

        for (; arrChkIt != arrChkEnd ; ++arrChkIt)
        {
            if ((*arrChkIt).Key() != "")
            {
              isArray = false;
              break ;
            }
        }
        if (isArray)
          toWrite += appendArray(data, indent);
        else
          toWrite += appendObject(data, indent);
    }
    else
    {
        // String or numeric or char
        std::string value = data.Value();

        if (value.size() == 0)
          value = "\"\"";
        else if (!(isNumeric(value)))
        {
          std::string escaped_value = "";

          for (unsigned int i = 0 ; i < value.length() ; ++i)
          {
            if (value[i] == '"')
              escaped_value += '\\';
            escaped_value += value[i];
          }
          value = "\"" + escaped_value + "\"";
        }
        toWrite += value;
    }
    return (toWrite);
}

static std::string appendArray(Data data, unsigned short indent)
{
    std::string toWrite;
    Data::iterator it  = data.begin();
    Data::iterator end = data.end();

    toWrite = '[';
    while (it != end)
    {
        if ((*it).Nil())
        {
          ++it;
          continue ;
        }
        toWrite += appendValue(*it, indent + 2);
        ++it;
        if (it  != end)
          toWrite += ", ";
    }
    toWrite += ']';
    return (toWrite);
}

static std::string appendObject(Data data, unsigned short indent)
{
    std::string toWrite;
    Data::iterator it  = data.begin();
    Data::iterator end = data.end();

    toWrite = '{';
    while (it != end)
    {
        if ((*it).Nil())
        {
          ++it;
          continue ;
        }
        toWrite += '\n';
        for (unsigned short iIndent = indent + 2 ; iIndent ; --iIndent)
            toWrite += ' ';
        toWrite += "\"" + (*it).Key() + "\": ";
        toWrite += appendValue(*it, indent + 2);
        ++it;
        if (it  != end)
          toWrite += ',';
    }
    toWrite += '\n';
    for (unsigned short iIndent = indent ; iIndent ; --iIndent)
        toWrite += ' ';
    toWrite += '}';
    return (toWrite);
}

bool DataTree::Writers::StringJSON(Data data, string& str)
{
  str = appendValue(data);
  return (true);
}

bool DataTree::Writers::JSON(Data data, const string &filename)
{
  std::ofstream file(filename.c_str());

  if (file.is_open())
  {
    std::string toWrite;
    
    StringJSON(data, toWrite);
    file.write(toWrite.c_str(), toWrite.size());
    file.close();
  }
  else
    return (false);
  return (true);
}
