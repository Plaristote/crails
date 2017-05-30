#include <crails/server.hpp>
#include <crails/renderer.hpp>
#include <crails/cipher.h>
#include <crails/cookie_data.hpp>

using namespace std;
using namespace Crails;

int main(int argc, char **argv)
{
  // Initializers
  Renderer::initialize();
  if (CookieData::use_encryption)
    Cipher::initialize();

  // Application loop
  Server::Launch(argc, argv);

  // Finalizers
  Renderer::finalize();
  return (0);
}
