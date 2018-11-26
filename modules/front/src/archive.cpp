#include <crails/front/archive.hpp>
#include <crails/datatree.hpp>

using namespace std;

const std::string Archive::mimetype = "crails/archive";

template<> char Archive::typecode<bool>()           { return 'b'; }
template<> char Archive::typecode<int>()            { return 'c'; }
template<> char Archive::typecode<long>()           { return 'd'; }
template<> char Archive::typecode<unsigned int>()   { return 'e'; }
template<> char Archive::typecode<unsigned long>()  { return 'f'; }
template<> char Archive::typecode<std::string>()    { return 'g'; }
template<> char Archive::typecode<short>()          { return 'h'; }
template<> char Archive::typecode<unsigned short>() { return 'i'; }
template<> char Archive::typecode<char>()           { return 'j'; }
template<> char Archive::typecode<unsigned char>()  { return 'k'; }
template<> char Archive::typecode<DataTree>()       { return 'l'; }

template<> void OArchive::serialize<char>(const char& value)                   { str += value; }
template<> void OArchive::serialize<unsigned char>(const unsigned char& value) { str += value; }
template<> void OArchive::serialize<bool>(const bool& value)     { str += (value ? '1' : '0'); }
template<> void OArchive::serialize<int>(const int& value)       { str += boost::lexical_cast<std::string>(value) + ';'; }
template<> void OArchive::serialize<long>(const long& value)     { str += boost::lexical_cast<std::string>(value) + ';'; }
template<> void OArchive::serialize<short>(const short& value)   { str += boost::lexical_cast<std::string>(value) + ';'; }
template<> void OArchive::serialize<unsigned long>(const unsigned long& value)   { str += boost::lexical_cast<std::string>(value) + ';'; }
template<> void OArchive::serialize<unsigned int>(const unsigned int& value)     { str += boost::lexical_cast<std::string>(value) + ';'; }
template<> void OArchive::serialize<unsigned short>(const unsigned short& value) { str += boost::lexical_cast<std::string>(value) + ';'; }

template<> void OArchive::serialize<std::string>(const std::string& value)
{
  size_t      len;
  const char* cstr = value.c_str();

  for (len = 0 ; cstr[len] ; ++len);
  str += boost::lexical_cast<std::string>(len) + ';';
  str += value;
}

template<> void OArchive::serialize<DataTree>(const DataTree& value)
{
  serialize(value.as_data().to_json());
}

template<> void IArchive::unserialize<bool>(bool& value)
{
  value = str[offset] == '1';
  offset += 1;
}

template<> void IArchive::unserialize<char>(char& value)                   { unserialize_char<char>(value); }
template<> void IArchive::unserialize<unsigned char>(unsigned char& value) { unserialize_char<unsigned char>(value); }
template<> void IArchive::unserialize<int>(int& value) { unserialize_number<int>(value); }
template<> void IArchive::unserialize<long>(long& value) { unserialize_number<long>(value); }
template<> void IArchive::unserialize<short>(short& value) { unserialize_number<short>(value); }
template<> void IArchive::unserialize<unsigned int>(unsigned int& value) { unserialize_number<unsigned int>(value); }
template<> void IArchive::unserialize<unsigned long>(unsigned long& value) { unserialize_number<unsigned long>(value); }
template<> void IArchive::unserialize<unsigned short>(unsigned short& value) { unserialize_number<unsigned short>(value); }

template<> void IArchive::unserialize<std::string>(std::string& value)
{
  int length;

  unserialize_number<int>(length);
#ifdef __CHEERP_CLIENT__ // wide characters in std::string do not behave as expected with Cheerp
  int end = offset + length;
  value = "";
  while (offset < end)
  {
    unsigned int char_value = (unsigned int)str[offset];

    if (char_value > 255)
      end--;
    value = value + str[offset];
    offset++;
  }
#else
  value   = str.substr(offset, length);
  offset += length;
#endif
}

template<> void IArchive::unserialize<DataTree>(DataTree& value)
{
  std::string json;

  unserialize(json);
  value.from_json(json);
}
