#ifndef  NETWORK_HPP
# define NETWORK_HPP

# include "select.hpp"
# include "server.hpp"
# include "../Utils/exception.hpp"

namespace Network
{
  void        Initialize(void);
  void        Finalize(void);

  int         GetLastError(void);
  std::string GetLastErrorMessage(void);

  template<typename T>
  void        RaiseLastError(T* ptr = 0)
  {
    if (ptr)
      Exception<T>::Raise(ptr, Network::GetLastErrorMessage(), Network::GetLastError());
    else
      Exception<T>::Raise(Network::GetLastErrorMessage(), Network::GetLastError());
  }
}

#endif
