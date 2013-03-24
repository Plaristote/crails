#include <Utils/serializer.hpp>
#include <Utils/exception.hpp>

using namespace std;

namespace Utils
{
  // Sweet metaprog shit

  template Packet& Packet::operator<< <std::int32_t>(std::int32_t& i);
  template Packet& Packet::operator<< <float>(float& i);
  template Packet& Packet::operator<< <short>(short& i);
  template Packet& Packet::operator<< <char>(char& i);
  template Packet& Packet::operator<< <unsigned int>(unsigned int& i);
  template Packet& Packet::operator<< <unsigned short>(unsigned short& i);
  template Packet& Packet::operator<< <unsigned char>(unsigned char& i);
  template Packet& Packet::operator<< <long>(long&);
  template Packet& Packet::operator<< <std::int32_t>(const std::int32_t& i);
  template Packet& Packet::operator<< <float>(const float& i);
  template Packet& Packet::operator<< <short>(const short& i);
  template Packet& Packet::operator<< <char>(const char& i);
  template Packet& Packet::operator<< <unsigned int>(const unsigned int& i);
  template Packet& Packet::operator<< <unsigned short>(const unsigned short& i);
  template Packet& Packet::operator<< <unsigned char>(const unsigned char& i);  
  template Packet& Packet::operator<< <long>(const long&);
  // => There's also a String specialization.

  template Packet& Packet::operator>> <int>(int& i);
  template Packet& Packet::operator>> <short>(short& i);
  template Packet& Packet::operator>> <char>(char& i);
  template Packet& Packet::operator>> <float>(float& i);
  template Packet& Packet::operator>> <unsigned int>(unsigned int& i);
  template Packet& Packet::operator>> <unsigned short>(unsigned short& i);
  template Packet& Packet::operator>> <unsigned char>(unsigned char& i);
  template Packet& Packet::operator>> <long>(long&);
  // => Same shit. Also a String specialization.

  template Packet& Packet::operator<< <int>(list<int>& list);
  template Packet& Packet::operator<< <float>(list<float>& list);
  template Packet& Packet::operator<< <std::string>(list<std::string>& list);
  template Packet& Packet::operator<< <short>(list<short>& list);
  template Packet& Packet::operator<< <char>(list<char>& list);

  template Packet& Packet::operator<< <int>(vector<int>& list);
  template Packet& Packet::operator<< <float>(vector<float>& list);
  template Packet& Packet::operator<< <std::string>(vector<std::string>& list);
  template Packet& Packet::operator<< <short>(vector<short>& list);
  template Packet& Packet::operator<< <char>(vector<char>& list);

  /*
  * Packet methods
  */
  Packet::Packet(void)
  {
    buffer      = 0;
    sizeBuffer  = 0;
    reading     = 0;
    isDuplicate = true;
    *this << (std::int32_t&)sizeBuffer;
  }
  
  Packet::Packet(std::ifstream& file)
  {
    int            begin, end;
    int            size;
    char*          raw;

    isDuplicate = true;
    begin       = file.tellg();
    file.seekg(0, std::ios::end);
    end         = file.tellg();
    file.seekg(0, std::ios::beg);
    size        = end - begin;
    raw         = new char[size + 1];
    file.read(raw, size);
    file.close();
    raw[size]   = 0;
    buffer      = raw;
    sizeBuffer  = size;
    realloc(size);
    updateHeader();
  }

  Packet::Packet(char* raw, size_t size, bool duplicate) : isDuplicate(duplicate)
  {
    if (duplicate)
    {
      char*	dupRaw = new char[size + 1];

      copy(raw, &raw[size + 1], dupRaw);
      buffer = reinterpret_cast<void*>(dupRaw);
    }
    else
      buffer = raw;
    sizeBuffer = size;
    reading = buffer;
    realloc(size);
    updateHeader();
  }

  Packet::Packet(const char* raw, size_t size) : isDuplicate(true)
  {
      char* dupRaw = new char[size + 1];

      copy(raw, &raw[size + 1], dupRaw);
      buffer = reinterpret_cast<void*>(dupRaw);
      sizeBuffer = size;
      reading = buffer;
      realloc(size);
      updateHeader();
  }

  Packet::~Packet(void)
  {
    if (isDuplicate && buffer)
      delete[] reinterpret_cast<char*>(buffer);
  }

  const char*	Packet::raw(void) const
  {
    return (reinterpret_cast<char*>(buffer));
  }

  size_t		Packet::size(void) const
  {
    return (sizeBuffer);
  }

  /*
  * Serializer
  */

  /*template<typename T>
  Packet&		Packet::operator<<(T& i)
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
  }*/
  
  /*template<typename T>
  Packet&		Packet::operator<<(const T& i)
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
  }*/

  template<typename T>
  void				Packet::SerializeArray(T& tehList)
  {
    typename T::iterator		current = tehList.begin();
    typename T::iterator		end = tehList.end();
    int				newSize = sizeBuffer;
    char*			        typeCode;
    std::int32_t*			sizeArray;

    newSize += sizeof(char) + sizeof(std::int32_t);
    realloc(newSize);
    typeCode = reinterpret_cast<char*>((long)buffer + sizeBuffer);
    sizeArray = reinterpret_cast<std::int32_t*>((long)typeCode + sizeof(char));
    *typeCode = Packet::Array;
    *sizeArray = tehList.size();
    sizeBuffer = newSize;
    while (current != end)
    {
      *this << *current;
      current++;
    }
    updateHeader();
  }


  /*template<typename T>
  Packet&		Packet::operator<<(list<T>& tehList)
  {
    SerializeArray(tehList);
    return (*this);
  }

  template<typename T>
  Packet&		Packet::operator<<(vector<T>& tehList)
  {
    SerializeArray(tehList);
    return (*this);
  }*/

  /*
  * Unserializer
  */
  /*template<typename T>
  Packet&		Packet::operator>>(T& v)
  {
    checkType(TypeToCode<T>::TypeCode);
    v = 0;
    read<T>(v);
    return (*this);
  }*/

  template<>
  Packet& Packet::operator<< <std::string>(const std::string& str)
  {
    int		newSize = sizeBuffer;
    char*         typeCode;
    std::int32_t*	sizeString;
    char*		dupStr;
    const char*	tmp = str.c_str();

    newSize += (sizeof(std::int32_t));
    newSize += sizeof(char) * str.size() + 1;
    realloc(newSize);
    typeCode = reinterpret_cast<char*>((long)buffer + sizeBuffer);
    sizeString = reinterpret_cast<std::int32_t*>((long)typeCode + sizeof(char));
    dupStr = reinterpret_cast<char*>((long)sizeString + sizeof(std::int32_t));
    *typeCode = Packet::String;
    *sizeString = str.size();
    std::copy(tmp, &tmp[str.size()], dupStr);
    sizeBuffer = newSize;
    updateHeader();
    return (*this);
  }

  template<>
  Packet& Packet::operator<< <std::string>(std::string& str)
  {
    const std::string& ref = str;
    
    this->operator<< <std::string>(ref);
    return (*this);
  }

  template<>
  Packet& Packet::operator>> <std::string>(std::string& str)
  {
    std::int32_t	size;

    str.clear();
    checkType(Packet::String);
    if (!(canIHaz(sizeof(std::int32_t), 1)))
      return (*this);
    size = *(reinterpret_cast<std::int32_t*>(reading));
    reading = reinterpret_cast<void*>((long)reading + sizeof(std::int32_t));
    if (!(canIHaz(sizeof(char), size)))
      return (*this);
    str.append(static_cast<char*>(reading), size);
    reading = reinterpret_cast<void*>((long)reading + sizeof(char) * size);
    return (*this);
  }
  // TODO: find a way to template this bullshit (list & vector unserializer)
  /*template<typename T>
  Packet&		Packet::operator>>(list<T>& list)
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

  template<typename T>
  Packet&		Packet::operator>>(vector<T>& list)
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
  }*/

  /*
  * Utility Methods for Packet
  */
  bool		Packet::canIHaz(size_t size, int n)
  {
    int		endBuffer, request;

    endBuffer = (long)buffer + sizeBuffer;
    request = (long)reading + (size * n);
    if (endBuffer < request)
    {
      //cerr << "[Serializer] Invalid Size: can't read on this Packet anymore." << endl;
      Exception<Packet>::Raise(this, "Invalid size: can't read on this Packet anymore.", InvalidSize);
      return (false);
    }
    return (true);
  }

  void		Packet::checkType(int assumedType)
  {
    char	        type = -1;

    read<char>(type);
    if (type != assumedType)
    {
      int offset = (long)reading - (long)buffer;

      // Hex debug
      int   it  = 0;
      char* hex = (char*)buffer;

      while (it < offset)
      {
          cerr << (int)(hex[it]) << " ";
          ++it;
      }
      cerr << endl;

      stringstream msgerr;
      msgerr << "Type Mismatch: waiting for " << assumedType << ", getting " << (int)type << " at offset " << offset << endl;
      Exception<Packet>::Raise(this, msgerr.str(), TypeMismatch);
    }
  }
  
  void          Packet::PrintRawContent(void)
  {
    char*        hex = (char*)buffer;
    unsigned int it  = 0;

    while (it < sizeBuffer)
      cout << (int)(hex[it++]) << ' ';
    cout << endl;
  }

  void		Packet::realloc(int newSize)
  {
    char*		alloc = new char[newSize];

    if (buffer)
    {
      char*	toCopy = static_cast<char*>(buffer);

      copy(toCopy, &toCopy[sizeBuffer], alloc);
      // WARNING may need to be commented:
      delete[] (char*)buffer;
    }
    buffer  = reinterpret_cast<void*>(alloc);
    reading = reinterpret_cast<void*>((long)buffer + sizeof(std::int32_t) + sizeof(char));
  }

  void		Packet::updateHeader(void)
  {
    if (sizeBuffer >= sizeof(std::int32_t))
      *(static_cast<std::int32_t*>(buffer)) = sizeBuffer;
  }

  /*
  * Less important stuff
  */
  void		Packet::PrintContent(void)
  {
    std::cout << "[Packet::PrintContent] isn't implemented yet" << std::endl;
  }
}
