#ifndef  CRAILS_ARCHIVE_HPP
# define CRAILS_ARCHIVE_HPP

# include <memory>
# include <string>
# include <vector>
# include <boost/lexical_cast.hpp>
# ifndef __CHEERP_CLIENT__
#  include <crails/utils/backtrace.hpp>
# endif

# define define_serializer(body) \
  template<typename ARCHIVE> \
  void serialize(ARCHIVE& archive) \
  { body } \
  void serialize(IArchive& a) { serialize<IArchive>(a); } \
  void serialize(OArchive& a) { serialize<OArchive>(a); }

class DataTree;

class Archive
{
public:
  static const std::string mimetype;

  const std::string& as_string() const { return str; }

  template<typename T>
  char typecode()
  {
    return '*';
  }

  void throw_unmatching_types()
  {
#ifdef __CHEERP_CLIENT__
    __asm__("throw 'Archive: unmatching types'");
#else
    throw boost_ext::runtime_error("Archive: unmatching types");
#endif
  }

  void throw_unimplemented_serializer()
  {
#ifdef __CHEERP_CLIENT__
    __asm__("throw 'Archive: unimplemented serializer'");
#else
    throw boost_ext::logic_error("Archive: unimplemented serializer");
#endif
  }

  void throw_unimplemented_unserializer()
  {
#ifdef __CHEERP_CLIENT__
    __asm__("throw 'Archive: unimplemented unserializer'");
#else
    throw boost_ext::logic_error("Archive: unimplemented unserializer");
#endif
  }

protected:
  std::string str;
};

class OArchive : public Archive
{
public:
  template<typename T>
  OArchive& operator&(const T& value)
  {
    str += typecode<T>();
    serialize(value);
    return *this;
  }

  template<typename T>
  OArchive& operator&(const std::vector<T>& value)
  {
    str += 'a';
    str += typecode<T>();
    serialize(value.size());
    for (auto it = value.begin() ; it != value.end() ; ++it)
      serialize<T>(*it);
    return *this;
  }

  template<typename T>
  OArchive& operator&(const std::shared_ptr<T>& value)
  {
    return this->operator&<T>(*value);
  }

  template<typename T>
  void serialize(const T& value) { throw_unimplemented_serializer(); }
};

class IArchive : public Archive
{
public:
  IArchive() : offset(0) {}

  void set_data(const std::string& str)
  {
    this->str = str;
    offset = 0;
  }

  template<typename T>
  IArchive& operator&(T& value)
  {
    if (str[offset] == typecode<T>())
      offset += 1;
    else
      throw_unmatching_types();
    unserialize<T>(value);
    return *this;
  }

  template<typename T>
  IArchive& operator&(std::vector<T>& value)
  {
    int length;

    if (str[offset] == 'a' && str[offset + 1] == typecode<T>())
      offset += 2;
    else
      throw_unmatching_types();
    unserialize_number<int>(length);
    value.resize(length);
    for (int i = 0 ; i < length ; ++i)
      unserialize(value[i]);
    return *this;
  }

  template<typename T>
  IArchive& operator&(std::shared_ptr<T>& value)
  {
    value = std::make_shared<T>();
    return this->operator&<T>(*value);
  }

  template<typename T>
  void unserialize(T& value) { throw_unimplemented_unserializer(); }

protected:
  unsigned int offset;

  template<typename NUMERICAL_TYPE>
  void unserialize_number(NUMERICAL_TYPE& value)
  {
    int length = 0;

    for (size_t i = offset ; i < str.size() && str[i] != ';' ; ++i)
      ++length;
    value = boost::lexical_cast<NUMERICAL_TYPE>(str.substr(offset, length));
    offset += length + 1;
  }

  template<typename CHAR_TYPE>
  void unserialize_char(CHAR_TYPE& value)
  {
    value = (CHAR_TYPE)(str[offset]);
    offset += 1;
  }
};

template<> char Archive::typecode<bool>();
template<> char Archive::typecode<int>();
template<> char Archive::typecode<long>();
template<> char Archive::typecode<unsigned int>();
template<> char Archive::typecode<unsigned long>();
template<> char Archive::typecode<std::string>();
template<> char Archive::typecode<short>();
template<> char Archive::typecode<unsigned short>();
template<> char Archive::typecode<char>();
template<> char Archive::typecode<unsigned char>();
template<> char Archive::typecode<DataTree>();

template<> void OArchive::serialize<bool>(const bool& value);
template<> void OArchive::serialize<char>(const char& value);
template<> void OArchive::serialize<int>(const int& value);
template<> void OArchive::serialize<long>(const long& value);
template<> void OArchive::serialize<short>(const short& value);
template<> void OArchive::serialize<unsigned char>(const unsigned char& value);
template<> void OArchive::serialize<unsigned long>(const unsigned long& value);
template<> void OArchive::serialize<unsigned int>(const unsigned int& value);
template<> void OArchive::serialize<unsigned short>(const unsigned short& value);
template<> void OArchive::serialize<std::string>(const std::string& value);
template<> void OArchive::serialize<DataTree>(const DataTree& value);

template<> void IArchive::unserialize<bool>(bool& value);
template<> void IArchive::unserialize<char>(char& value);
template<> void IArchive::unserialize<int>(int& value);
template<> void IArchive::unserialize<long>(long& value);
template<> void IArchive::unserialize<short>(short& value);
template<> void IArchive::unserialize<unsigned char>(unsigned char& value);
template<> void IArchive::unserialize<unsigned int>(unsigned int& value);
template<> void IArchive::unserialize<unsigned long>(unsigned long& value);
template<> void IArchive::unserialize<unsigned short>(unsigned short& value);
template<> void IArchive::unserialize<std::string>(std::string& value);
template<> void IArchive::unserialize<DataTree>(DataTree& value);

#endif
