#include <crails/server.hpp>
#include <crails/renderer.hpp>
#include <crails/cipher.h>

using namespace std;
using namespace Crails;

int main(int argc, char **argv)
{
  // Initializers
  Renderer::initialize();
  Cipher::initialize();

  // Application loop
  Server::Launch(argc, argv);

  // Finalizers
  Renderer::finalize();
  return (0);
}
