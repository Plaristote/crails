#include <crails/server.hpp>
#include <crails/databases.hpp>

using namespace std;
using namespace Crails;

int main(int argc, char **argv)
{
  // Initializers
  Renderer::initialize();

  // Application loop
  Server::Launch(argc, argv);

  // Finalizers
  Renderer::finalize();
  return (0);
}
