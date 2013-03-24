#include <iostream>
#include <crails/server.hpp>
#include <crails/assets.hpp>

using namespace std;

int main(int argc, char **argv)
{
  Assets::Precompile();
  CrailsServer::Launch(argc, argv);
  return (0);
}
