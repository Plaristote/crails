#ifndef  SELECTTHREAD_HPP
# define SELECTTHREAD_HPP

# include <Boots/Network/network.hpp>
# include <Boots/Sync/thread.hpp>

class SelectThread : public Network::Select, public Sync::Thread
{
public:
  void Run(void);
};

#endif
