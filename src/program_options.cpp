#include "crails/program_options.hpp"
#include "crails/logger.hpp"
#include <boost/asio.hpp>
#include <memory>
#ifdef CRAILS_WITH_SSL
# include <boost/asio/ssl.hpp>
#endif

using namespace std;
using namespace boost;
using namespace Crails;

#ifdef CRAILS_WITH_SSL
namespace Crails {
  std::string get_ssl_password(std::size_t max_length, asio::ssl::context_base::password_purpose purpose);
}
#endif

ProgramOptions::ProgramOptions(int argc, char** argv)
{
  program_options::options_description desc("Options");

  desc.add_options()
    ("port,p",     program_options::value<std::string>(),    "listened port")
    ("hostname,h", program_options::value<std::string>(),    "listened host")
    ("threads,t",  program_options::value<unsigned short>(), "amount of threads")
#ifdef CRAILS_WITH_SSL
    ("ssl",        "enable SSL")
    ("ssl-cert",   program_options::value<std::string>(), "path to the ssl certificate file")
    ("ssl-key",    program_options::value<std::string>(), "path to the ssl key file")
    ("ssl-dh",     program_options::value<std::string>(), "path to a tmp_dh_file")
#endif
    ;
  program_options::store(program_options::parse_command_line(argc, argv, desc), vm);
  program_options::notify(vm);
}

HttpServer::options ProgramOptions::get_server_options(Server& handler) const
{
  HttpServer::options server_options(handler);

  server_options.reuse_address(true);
  initialize_interface(server_options);
  initialize_thread_pool(server_options);
  if (vm.count("ssl"))
    initialize_ssl_context(server_options);
  return server_options;
}

void ProgramOptions::initialize_interface(HttpServer::options& options) const
{
  std::string    address = get_value("hostname", std::string("127.0.0.1"));
  std::string    port    = get_value("port",     std::string("3001"));

  logger << ">> Initializing server" << Logger::endl;
  logger << ">> Listening on " << address << ":" << port << Logger::endl;
  options.address(address.c_str()).port(port.c_str());
}

void ProgramOptions::initialize_thread_pool(HttpServer::options& options) const
{
  unsigned short threads = get_value("threads", (unsigned short)(std::thread::hardware_concurrency()));

  logger << ">> Pool Thread Size: " << threads << Logger::endl;
  options.thread_pool(
    std::make_shared<boost::network::utils::thread_pool>(threads)
  );
}

void ProgramOptions::initialize_ssl_context(HttpServer::options& options) const
{
#ifdef CRAILS_WITH_SSL
  auto ctx = std::make_shared<asio::ssl::context>(asio::ssl::context::sslv23);
  std::string certificate_chain_file = get_value("ssl-cert", std::string("server.pem"));
  std::string private_key_file       = get_value("ssl-key",  std::string("server.pem"));

  logger << ">> Using SSL\n";
  logger << ">> Use certificate file: " << certificate_chain_file << '\n';
  logger << ">> Use key file: " << private_key_file << Logger::endl;
  ctx->set_options(asio::ssl::context::default_workarounds |
                   asio::ssl::context::no_sslv2 |
                   asio::ssl::context::single_dh_use);
  ctx->set_password_callback(Crails::get_ssl_password);
  ctx->use_certificate_chain_file(certificate_chain_file);
  ctx->use_private_key_file(private_key_file, asio::ssl::context::pem);
  if (vm.count("ssl-dh"))
    ctx->use_tmp_dh_file(vm["ssl-dh"].as<std::string>());
  options.context(ctx);
#endif
}
