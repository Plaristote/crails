#ifndef PACKET_HPP
# define PACKET_HPP

# include <string>
# include <list>
# include <vector>
# include <algorithm>
# include <iostream>
# include <fstream>

# ifdef _WIN32
#  include <cstdint>
# endif

# include "helpers.hpp"
# include "exception.hpp"

/*! \namespace Utils
 * \brief Various essentials utilities for all-purpose programming
 */
namespace Utils
{
  class Packet;

  struct Serializable
  {
    virtual void Serialize(Packet&) const = 0;
    virtual void Unserialize(Packet&)     = 0;
  };

/*! \class Packet
 * \brief Serialization utility.
 */
class		Packet
{
public:
  enum Types
  {
    String = 1,
    Int    = 2,
    Float  = 3,
    Short  = 4,
    Char   = 5,
    Array  = 6,
    UInt   = 7,
    UShort = 8,
    UChar  = 9,
    Long   = 10
  };

  enum Errors
  {
    TypeMismatch,
    InvalidSize,
    UnsupportedType
  };

  /*! \brief Use to create an empty Packet. */
  Packet(void);
  /*! \brief Use to create a packet from a file */
  Packet(std::ifstream&);
  /*! \brief Use to create a packet from some raw data.
   * \param raw : The raw data in the state of a char string.
   * \param size : The size of that same char string.
   * \param duplicate : Define if the Packet must use the raw pointer or a copy of the char string it's pointing to. */
  Packet(char* raw, size_t size, bool duplicate = true);
  Packet(const char* raw, size_t size);
  /*! \brief Use to create a packet from another packet */
  Packet(const Packet& cpy);
  virtual ~Packet(void);

  // Utils
  /*! \brief Returns raw data as a char tab */
  const char*	raw(void) const;
  /*! \brief Returns size of the char tab */
  size_t	size(void) const;
  /*! \brief Prints the content of the packet in an human-readable form */
  void		PrintContent(void);
  void          PrintRawContent(void);
  
  Packet&	operator=(const Packet& cpy);

  // Serialization
  template<typename T> Packet& operator<<(T& v)
  {
    SelectSerializer<TypeToCode<T>::TypeCode != 0, std::is_base_of<Utils::Serializable, T>::value>::Func(*this, v);
    return (*this);
  }

  template<typename T> Packet& operator<<(T* i)
  {
    return (this->operator<<(*i));
  }

  template<bool is_serializable_natively, bool is_serializable>
  struct SelectSerializer   { template<typename T> static void Func(Utils::Packet& packet, T& v) { packet.Serialize(v); } };

  template<typename T> Packet&	Serialize(T& i)
  {
    int	    newSize = sizeBuffer;
    char*   typeCode;
    T*	    copy;

    newSize   += sizeof(T) + sizeof(char);
    realloc(newSize);
    typeCode   = reinterpret_cast<char*>((long)buffer + sizeBuffer);
    copy       = reinterpret_cast<T*>((long)typeCode + sizeof(char));
    *typeCode  = TypeToCode<T>::TypeCode;
    *copy      = i;
    sizeBuffer = newSize;
    updateHeader();
    return (*this);
  }

  template<typename T> Packet&	operator<<(const T& i)
  {
    int	    newSize = sizeBuffer;
    char*   typeCode;
    T*	    copy;

    newSize += sizeof(T) + sizeof(char);
    realloc(newSize);
    typeCode = reinterpret_cast<char*>((long)buffer + sizeBuffer);
    copy = reinterpret_cast<T*>((long)typeCode + sizeof(char));
    *typeCode = TypeToCode<T>::TypeCode;
    *copy = i;
    sizeBuffer = newSize;
    updateHeader();
    return (*this);
  }

  template<typename T> Packet&	operator<<(std::list<T>& tehList)
  {
    SerializeArray(tehList);
    return (*this);
  }

  template<typename T> Packet&	operator<<(std::vector<T>& tehList)
  {
    SerializeArray(tehList);
    return (*this);
  }

  // Unserialization
  template<typename T> Packet& operator>>(T& v)
  {
    SelectUnserializer<TypeToCode<T>::TypeCode != 0, std::is_base_of<Utils::Serializable, T>::value>::Func(*this, v);
    return (*this);
  }

  template<bool is_serializable_nativelly, bool is_serializable>
  struct SelectUnserializer { template<typename T> static void Func(Utils::Packet& packet, T& v) { packet.Unserialize(v); } };

  template<typename T> void Unserialize(T& v)
  {
    checkType(TypeToCode<T>::TypeCode);
    v = 0;
    read<T>(v);
  }

  template<typename T> Packet& operator>>(std::list<T>& list)
  {
    unsigned int	size, it;
    std::int32_t	tmp = 0;

    list.clear();
    checkType(Packet::Array);
    read<std::int32_t>(tmp);
    size = tmp;
    for (it = 0 ; it < size ; ++it)
    {
      T		reading;

      *this >> reading;
      list.push_back(reading);
    }
    return (*this);
  }

  template<typename T> Packet& operator>>(std::vector<T>& list)
  {
    unsigned int	size, it;
    std::int32_t	tmp = 0;

    list.clear();
    checkType(Packet::Array);
    read<std::int32_t>(tmp);
    size = tmp;
    for (it = 0 ; it < size ; ++it)
    {
      T		reading;

      (*this).operator>> <T>(reading);
      list.push_back(reading);
    }
    return (*this);
  }

private:
  template<typename T> void	SerializeArray(T& tehList);

  bool		canIHaz(size_t sizeType, int howMany); // Checks if the buffer is big enough for Packet to read size_t
  void		checkType(int assumedType);            // Check if the next type in buffer match the assumed type
  void		realloc(int newsize);                  // Realloc the buffer (used in serializing)
  void		updateHeader(void);                    // Update the size of the packet at the very front of the packet.

  template<typename T>
  void		read(T& tmp)
  {
    if ((canIHaz(sizeof(T), 1)))
      tmp = *(reinterpret_cast<T*>(reading));
    reading = reinterpret_cast<void*>((long)reading + sizeof(T));
  }

  // Association Type/TypeCode
  template<typename type> struct TypeToCode { enum { TypeCode = 0 }; };

  bool          isDuplicate;
  void*		buffer;
  size_t	sizeBuffer;
  void*		reading;
};

  template<>
  struct Packet::SelectSerializer<false, true>  { template<typename T> static void Func(Utils::Packet& packet, T& v) { v.Serialize(packet); } };

  template<>
  struct Packet::SelectSerializer<false, false> { template<typename T> static void Func(Utils::Packet& packet, T& v) { Exception<Packet>::Raise(&packet, std::string("Unsupported type ") + typeid(T).name(), UnsupportedType); } };

  template<>
  struct Packet::SelectUnserializer<false, true> { template<typename T> static void Func(Utils::Packet& packet, T& v) { v.Unserialize(packet); } };

  template<>
  struct Packet::SelectUnserializer<false, false> { template<typename T> static void Func(Utils::Packet& packet, T& v) { Exception<Packet>::Raise(&packet, std::string("Unsupported type ") + typeid(T).name(), UnsupportedType); } };

  template<> Packet& Packet::operator<< <std::string>(const std::string& str);
  template<> Packet& Packet::operator<< <std::string>(std::string& str);
  template<> Packet& Packet::operator>> <std::string>(std::string& str);

  template<> struct Packet::TypeToCode<std::string>    { enum { TypeCode = Packet::String }; };
  template<> struct Packet::TypeToCode<int>            { enum { TypeCode = Packet::Int    }; };
  template<> struct Packet::TypeToCode<float>          { enum { TypeCode = Packet::Float  }; };
  template<> struct Packet::TypeToCode<short>          { enum { TypeCode = Packet::Short  }; };
  template<> struct Packet::TypeToCode<char>           { enum { TypeCode = Packet::Char   }; };
  template<> struct Packet::TypeToCode<unsigned int>   { enum { TypeCode = Packet::UInt   }; };
  template<> struct Packet::TypeToCode<unsigned short> { enum { TypeCode = Packet::UShort }; };
  template<> struct Packet::TypeToCode<unsigned char>  { enum { TypeCode = Packet::UChar  }; };
  template<> struct Packet::TypeToCode<long>           { enum { TypeCode = Packet::Long   }; };
}

#endif
