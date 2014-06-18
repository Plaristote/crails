#include <Network/serializer.hpp>
#include <Network/socket.hpp>
#include <iostream>

using namespace std;

void Network::WritePacket(Utils::Packet& packet, Network::Socket* socket)
{
  unsigned short size;

  size = packet.size();
  socket->WriteBuffer().Push((const char*)&size, sizeof(size));
  socket->WriteBuffer().Push(packet.raw(), size);
  socket->AskingToWrite.Emit();
}
