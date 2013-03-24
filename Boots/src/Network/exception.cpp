#include <Network/network.hpp>
#include <string.h>

#ifdef _WIN32
#else
# include <errno.h>
#endif

using namespace std;

#ifdef _WIN32
int Network::GetLastError(void)
{
  return (WSAGetLastError());
}

string Network::GetLastErrorMessage(void)
{
  LPSTR  buffer = NULL;
  string result;

  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                0, GetLastError(), 0, (LPSTR)&buffer, 0, 0);
  result = buffer;
  LocalFree(buffer);
  return (result);
}
#else
int Network::GetLastError(void)
{
  return (errno);
}

string Network::GetLastErrorMessage(void)
{
  return (strerror(GetLastError()));
}
#endif

