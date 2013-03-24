#ifndef  SELECTTHREAD_HPP
# define SELECTTHREAD_HPP

# include <Network/network.hpp>
# include <Sync/thread.hpp>

class SelectThread : public Network::Select, public Sync::Thread
{
public:
  void Run(void);
};

#endif