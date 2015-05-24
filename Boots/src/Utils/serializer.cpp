#include "Boots/Utils/serializer.hpp"
#include <sstream>

using namespace std;

namespace Utils
{
  // Sweet metaprog shit

  template Packet& Packet::operator<< <std::int32_t>(std::int32_t& i);
  template Packet& Packet::operator<< <float>(float& i);
  template Packet& Packet::operator<< <short>(short& i);
  template Packet& Packet::operator<< <char>(char& i);
  template Packet& Packet::operator<< <unsigned short>(unsigned short& i);
  template Packet& Packet::operator<< <unsigned char>(unsigned char& i);
  template Packet& Packet::operator<< <long>(long&);
  template Packet& Packet::operator<< <std::int32_t>(const std::int32_t& i);
  template Packet& Packet::operator<< <float>(const float& i);
  template Packet& Packet::operator<< <short>(const short& i);
  template Packet& Packet::operator<< <char>(const char& i);
  template Packet& Packet::operator<< <unsigned short>(const unsigned short& i);
  template Packet& Packet::operator<< <unsigned char>(const unsigned char& i);  

  template Packet& Packet::operator>> <std::int32_t>(std::int32_t& i);
  template Packet& Packet::operator>> <short>(short& i);
  template Packet& Packet::operator>> <char>(char& i);
  template Packet& Packet::operator>> <float>(float& i);
  template Packet& Packet::operator>> <unsigned short>(unsigned short& i);
  template Packet& Packet::operator>> <unsigned char>(unsigned char& i);

  template Packet& Packet::operator<< <std::int32_t>(const list<int>& list);
  template Packet& Packet::operator<< <float>(const list<float>& list);
  template Packet& Packet::operator<< <std::string>(const list<std::string>& list);
  template Packet& Packet::operator<< <short>(const list<short>& list);
  template Packet& Packet::operator<< <char>(const list<char>& list);

  template Packet& Packet::operator<< <std::int32_t>(const vector<int>& list);
  template Packet& Packet::operator<< <float>(const vector<float>& list);
  template Packet& Packet::operator<< <std::string>(const vector<std::string>& list);
  template Packet& Packet::operator<< <short>(const vector<short>& list);
  template Packet& Packet::operator<< <char>(const vector<char>& list);

  template Packet& Packet::operator<< <std::int32_t>(list<int>& list);
  template Packet& Packet::operator<< <float>(list<float>& list);
  template Packet& Packet::operator<< <std::string>(list<std::string>& list);
  template Packet& Packet::operator<< <short>(list<short>& list);
  template Packet& Packet::operator<< <char>(list<char>& list);

  template Packet& Packet::operator<< <std::int32_t>(vector<int>& list);
  template Packet& Packet::operator<< <float>(vector<float>& list);
  template Packet& Packet::operator<< <std::string>(vector<std::string>& list);
  template Packet& Packet::operator<< <short>(vector<short>& list);
  template Packet& Packet::operator<< <char>(vector<char>& list);
  
  template<> Packet& Packet::operator<< <std::string>(const std::string& str)
  {
    int         newSize = sizeBuffer;
    char*         typeCode;
    std::int32_t*       sizeString;
    char*               dupStr;
    const char* tmp = str.c_str();

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

  template<> Packet& Packet::operator<< <std::string>(std::string& str)
  {
    const std::string& ref = str;
    
    this->operator<< <std::string>(ref);
    return (*this);
  }

  template<> Packet& Packet::operator>> <std::string>(std::string& str)
  {
    std::int32_t        size;

    str.clear();
    checkType(Packet::String);
    if (!(canIHaz(sizeof(std::int32_t), 1)))
      throw CorruptPacket(this);
    size = *(reinterpret_cast<std::int32_t*>(reading));
    reading = reinterpret_cast<void*>((long)reading + sizeof(std::int32_t));
    if (!(canIHaz(sizeof(char), size)))
      throw CorruptPacket(this);
    str.append(static_cast<char*>(reading), size);
    reading = reinterpret_cast<void*>((long)reading + sizeof(char) * size);
    return (*this);
  }
  
  /*
  * Packet methods
  */
  Packet::Packet(void)
  {
    initializeAsEmpty();
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
    initializeFromBuffer(raw, size);
  }

  Packet::~Packet(void)
  {
    finalize();
  }
  
  void Packet::Clear(void)
  {
    finalize();
    initializeAsEmpty();
  }

  void Packet::finalize(void)
  {
    if (isDuplicate && buffer)
      delete[] reinterpret_cast<char*>(buffer);
  }

  size_t		Packet::size(void) const
  {
    return (sizeBuffer);
  }
  
  void Packet::initializeAsEmpty(void)
  {
    std::int32_t tmp_buffer = 0;

    buffer      = 0;
    sizeBuffer  = 0;
    reading     = 0;
    isDuplicate = true;
    *this << tmp_buffer;
  }
  
  void Packet::initializeFromBuffer(const char* raw, size_t size)
  {
    char* dupRaw = new char[size + 1];

    copy(raw, &raw[size + 1], dupRaw);
    buffer = reinterpret_cast<void*>(dupRaw);
    sizeBuffer = size;
    reading = buffer;
    realloc(size);
    updateHeader();
  }

  const char*   Packet::raw(void) const
  {
    return (reinterpret_cast<char*>(buffer));
  }

  void Packet::Serialize(Utils::Packet& packet) const
  {
    std::string string_buffer(raw(), size());

    packet << string_buffer;
  }
  
  void Packet::Unserialize(Utils::Packet& packet)
  {
    std::string string_buffer;

    packet >> string_buffer;
    finalize();
    isDuplicate = true;
    initializeFromBuffer(string_buffer.c_str(), string_buffer.length());
  }

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
      cerr << "[Serializer] Invalid Size: can't read on this Packet anymore." << endl;
      return (false);
    }
    return (true);
  }

  void		Packet::checkType(int assumedType)
  {
    char	        type = -1;

    read<char>(type);
    if (type != assumedType)
      throw UnserializeUnknownType(this, assumedType, (char)type);
  }
  
  void		Packet::realloc(int newSize)
  {
    char*		alloc = new char[newSize];

    if (buffer)
    {
      char*	toCopy = static_cast<char*>(buffer);

      copy(toCopy, &toCopy[sizeBuffer], alloc);
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
  
  std::string Packet::Exception::type_names[] = { "[unsupported type]", "std::string", "int", "float", "short", "char", "array", "unsigned int", "unsigned short", "unsigned char" };
  
  Packet::CorruptPacket::CorruptPacket(Packet* packet)
  {
    std::stringstream stream;

    assumed_type = provided_type = 0;
    this->packet = packet;
    offset       = (long)packet->reading - (long)packet->buffer;
    size         = packet->size();
    stream << "Packet is corrupted (packet's size do not match what it claims to be) at offset " << offset;
    message = stream.str();
  }
  
  Packet::SerializeUnknownType::SerializeUnknownType(Packet* packet)
  {
    std::stringstream stream;

    assumed_type = provided_type = 0;
    this->packet = packet;
    offset       = packet->sizeBuffer;
    size         = packet->size();
    stream << "Exception while serializing packet at offset: attempt to serialize an unsupported type.";
    message = stream.str();
  }
  
  Packet::UnserializeUnknownType::UnserializeUnknownType(Packet* packet, char assumed_type, char provided_type)
  {
    std::stringstream stream;
    
    this->packet        = packet;
    this->assumed_type  = assumed_type;
    this->provided_type = provided_type;
    offset              = (long)packet->reading - (long)packet->buffer;
    size                = packet->size();
    stream << "Exception while unserializing packet at offset " << offset << " : packet provided " << ProvidedType() << ", user tried to unserialize " << ExpectedType();
    message = stream.str();
  }
  
  const std::string& Packet::Exception::ExpectedType(void) const
  {
    return (type_names[(size_t)assumed_type]);
  }
  
  const std::string& Packet::Exception::ProvidedType(void) const
  {
    return (type_names[(size_t)provided_type]);
  }
}
