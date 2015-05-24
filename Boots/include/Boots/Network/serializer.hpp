#ifndef  _NETWORK_SERIALIZER_HPP
# define _NETWORK_SERIALIZER_HPP

# include "../Utils/serializer.hpp"

namespace Network
{
  class Socket;
  void WritePacket(Utils::Packet& packet, Network::Socket* socket);
}

#endif
