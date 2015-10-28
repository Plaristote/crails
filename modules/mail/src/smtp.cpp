#include <Boots/Utils/exception.hpp>
#include <Boots/Utils/string.hpp>
#include <boost/array.hpp>
#include <crails/smtp.hpp>

using namespace std;

/*
 * Smtp::Mail
 */
void        Smtp::Mail::SetSender(const std::string& address, const std::string& name)
{
  sender.address = address;
  sender.name    = name;
}

void        Smtp::Mail::AddRecipient(const std::string& address, const std::string& name, unsigned char flags)
{
  Recipient recipient;

  recipient.address = address;
  recipient.name    = name;
  recipient.type    = flags;
  recipients.push_back(recipient);
}

void        Smtp::Mail::DelRecipient(const std::string& address)
{
  auto it = std::find(recipients.begin(), recipients.end(), address);
  
  if (it != recipients.end())
    recipients.erase(it);
}

/*
 * Smtp::Server
 */
using namespace boost::asio::ip;

Smtp::Server::Server() : sock(io_service)
{
}

void Smtp::Server::connect(const std::string& hostname, unsigned short port)
{
  tcp::resolver        resolver(io_service);
  tcp::resolver::query query(hostname, std::to_string(port));

  tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
  tcp::resolver::iterator end;

  boost::system::error_code error = boost::asio::error::host_not_found;
  while (error && endpoint_iterator != end)
  {
    sock.close();
    sock.connect(*endpoint_iterator++, error);
  }
  if (error)
    Exception<Smtp::Server>::Raise("Cannot connect to " + hostname);

  smtp_handshake();
  if (tls_enabled)
    smtp_start_tls();
}

void Smtp::Server::connect(const std::string& hostname, unsigned short port, const std::string& username, const std::string& password, AuthenticationProtocol protocol)
{
  this->connect(hostname, port);
  (this->*auth_methods[protocol])(username, password);
}

void Smtp::Server::disconnect(void)
{
  smtp_quit();
  sock.close();
}

void Smtp::Server::send(const Smtp::Mail& mail)
{
  smtp_new_mail(mail);
  smtp_recipients(mail);
  smtp_body(mail);
}

/*
 * SMTP Protocol Implementation (RFC 5321)
 */
std::string Smtp::Server::smtp_read_answer(unsigned short expected_return)
{
  std::string    answer;
  unsigned short return_value;

  // NetworkRead
  {
    boost::array<char,256> buffer;
    std::size_t    bytes_received = sock.receive(boost::asio::buffer(buffer));

    if (bytes_received == 0)
      Exception<Smtp::Server>::Raise("The server closed the connection");
    std::copy(buffer.begin(), buffer.begin() + bytes_received, std::back_inserter(answer));
  }
  //
  return_value = atoi(answer.substr(0, 3).c_str());
  if (return_value != expected_return)
  {
    std::stringstream error_message;

    error_message << "Expected answer status to be " << expected_return << ", received `" << answer << '`';
    Exception<Smtp::Server>::Raise(error_message.str());
  }
  return (answer);
}

void Smtp::Server::smtp_write_message()
{
  boost::system::error_code error;

  boost::asio::write(sock, boost::asio::buffer(server_message.str()), error);
  server_message.str(std::string());
}

void Smtp::Server::smtp_body(const Smtp::Mail& mail)
{
  // Notify that we're starting the DATA stream
  server_message << "DATA\r\n";
  smtp_write_message();
  smtp_read_answer(354);
  // Setting the headers
  smtp_data_addresses("To",       mail, 0);
  smtp_data_addresses("Cc",       mail, Smtp::Mail::CarbonCopy);
  smtp_data_addresses("Bcc",      mail, Smtp::Mail::CarbonCopy | Smtp::Mail::Blind);
  smtp_data_address  ("Reply-To", mail.GetReplyTo(),        "");
  smtp_data_address  ("From",     mail.GetSender().address, mail.GetSender().name);
  // Send the subject
  server_message << "Subject: " << mail.GetSubject() << "\r\n";
  // Send the body and finish the DATA stream
  server_message << mail.body   << "\r\n.\r\n";
  smtp_write_message();
  smtp_read_answer();
}

void Smtp::Server::smtp_data_addresses(const std::string& field, const Smtp::Mail& mail, int flag)
{
  auto it  = mail.recipients.begin();
  auto end = mail.recipients.end();
  int  i   = 0;
  
  for (; it != end ; ++it)
  {
    if (it->type == flag)
    {
      if (i != 0)
        server_message << ", ";
      else
        server_message << field << ": ";
      if (it->name.size() > 0)
        server_message << it->name << " <" << it->address << ">";
      else
        server_message << it->address;
      ++i;
    }
  }
}

void Smtp::Server::smtp_data_address(const std::string& field, const std::string& address, const std::string& name)
{
  server_message << field << ": " << address;
  if (name.size() > 0)
    server_message << " <" << name << ">";
  server_message << "\r\n";
}

void Smtp::Server::smtp_recipients(const Smtp::Mail& mail)
{
  auto it  = mail.recipients.begin();
  auto end = mail.recipients.end();
  
  for (; it != end ; ++it)
  {
    server_message << "RCPT TO: " << it->address << "\r\n";
    smtp_write_message();
    smtp_read_answer();
  }
}

void Smtp::Server::smtp_new_mail(const Smtp::Mail& mail)
{
  const Smtp::Mail::Sender& sender = mail.GetSender();

  server_message << "MAIL FROM: " << sender.address << "\r\n";
  smtp_write_message();
  smtp_read_answer();
}

void Smtp::Server::smtp_handshake()
{
  // Receiving the server identification
  server_id = smtp_read_answer(220);
  // Sending handshake
  server_message << "HELO " << sock.remote_endpoint().address().to_string() << "\r\n";
  smtp_write_message();
  smtp_read_answer(250);
}

void Smtp::Server::smtp_quit()
{
  server_message << "QUIT\r\n";
  smtp_write_message();
  smtp_read_answer(221);
}

/* 
 * SMTP StartTLS (RFC 2487) (TLS protocol is defined in RFC2246)
 */
void Smtp::Server::start_tls(void)
{
  tls_enabled = true;
}

void Smtp::Server::smtp_start_tls(void)
{
  server_message << "STARTTLS\r\n";
  smtp_write_message();
  smtp_read_answer(220);
}

/*
 * SMTP Authentication Extension (RFC 2554)
 */
std::map<Smtp::Server::AuthenticationProtocol,Smtp::Server::SmtpAuthMethod> Smtp::Server::auth_methods = {
  { Smtp::Server::PLAIN,      &Smtp::Server::smtp_auth_plain      },
  { Smtp::Server::LOGIN,      &Smtp::Server::smtp_auth_login      },
  { Smtp::Server::MD5,        &Smtp::Server::smtp_auth_md5        },
  { Smtp::Server::DIGEST_MD5, &Smtp::Server::smtp_auth_digest_md5 },
  { Smtp::Server::CRAM_MD5,   &Smtp::Server::smtp_auth_cram_md5   }
};

void Smtp::Server::smtp_auth_plain(const std::string& user, const std::string& password)
{
  string auth_hash = String::base64_encode('\000' + user + '\000' + password);

  server_message << "AUTH PLAIN " << auth_hash << "\r\n";
  smtp_write_message();
  smtp_read_answer(235);
}

void Smtp::Server::smtp_auth_login(const std::string& user, const std::string& password)
{
  string user_hash = String::base64_encode(user);
  string pswd_hash = String::base64_encode(password);

  server_message << "AUTH LOGIN\r\n";
  smtp_write_message();
  smtp_read_answer(334);
  server_message << user_hash << "\r\n";
  smtp_write_message();
  smtp_read_answer(334);
  server_message << pswd_hash << "\r\n";
  smtp_write_message();
  smtp_read_answer(235);
}

void Smtp::Server::smtp_auth_md5(const std::string& user, const std::string& password)
{
  throw "unsupported";
}

void Smtp::Server::smtp_auth_digest_md5(const string& user, const string& password)
{
  throw "unsupported";
}

void Smtp::Server::smtp_auth_cram_md5(const string& user, const string& password)
{
  throw "unsupported";
}

// END SMTP AUTHENTICATION EXTENSION
