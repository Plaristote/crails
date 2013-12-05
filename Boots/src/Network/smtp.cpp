#include <Network/smtp.hpp>
#include <Utils/exception.hpp>
#include <Utils/string.hpp>

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
Smtp::Server::Server()
{
}

void Smtp::Server::Connect(const std::string& hostname, unsigned short port)
{
  if (Network::Socket::Connect(hostname, port))
    SmtpHandshake();
  else
    Exception<Smtp::Server>::Raise("Cannot connect to " + hostname);
  if (tls_enabled)
    SmtpStartTls();
}

void Smtp::Server::Connect(const std::string& hostname, unsigned short port, const std::string& username, const std::string& password, AuthenticationProtocol protocol)
{
  Connect(hostname, port);
  (this->*auth_methods[protocol])(username, password);
}

void Smtp::Server::Disconnect(void)
{
  SmtpQuit();
  Network::Socket::Disconnect();
}

void Smtp::Server::Send(const Smtp::Mail& mail)
{
  SmtpNewMail(mail);
  SmtpRecipients(mail);
  SmtpBody(mail);
}

/*
 * SMTP Protocol Implementation (RFC 5321)
 */
std::string Smtp::Server::SmtpReadAnswer(unsigned short expected_return)
{
  std::string    answer;
  unsigned short return_value;

  WaitForData();
  NetworkRead();
  *this >> answer;
  return_value = atoi(answer.substr(0, 3).c_str());
  if (return_value != expected_return)
  {
    std::stringstream error_message;
    
    error_message << "Expected answer status to be " << expected_return << ", received `" << answer << '`';
    Exception<Smtp::Server>::Raise(error_message.str());
  }
  return (answer);
}

void Smtp::Server::SmtpBody(const Smtp::Mail& mail)
{
  // Notify that we're starting the DATA stream
  *this << "DATA\r\n";
  NetworkWrite();
  SmtpReadAnswer(354);
  // Setting the headers
  SmtpDataAddresses("To",       mail, 0);
  SmtpDataAddresses("Cc",       mail, Smtp::Mail::CarbonCopy);
  SmtpDataAddresses("Bcc",      mail, Smtp::Mail::CarbonCopy | Smtp::Mail::Blind);
  SmtpDataAddress  ("Reply-To", mail.GetReplyTo(),        "");
  SmtpDataAddress  ("From",     mail.GetSender().address, mail.GetSender().name);
  // Send the subject
  *this << "Subject: " << mail.GetSubject() << "\r\n";
  // Send the body and finish the DATA stream
  *this << mail.body   << "\r\n.\r\n";
  NetworkWrite();
  SmtpReadAnswer();
}

void Smtp::Server::SmtpDataAddresses(const std::string& field, const Smtp::Mail& mail, int flag)
{
  auto it  = mail.recipients.begin();
  auto end = mail.recipients.end();
  int  i   = 0;
  
  for (; it != end ; ++it)
  {
    if (it->type == flag)
    {
      if (i != 0)
        *this << ", ";
      else
        *this << field << ": ";
      if (it->name.size() > 0)
        *this << it->name << " <" << it->address << ">";
      else
        *this << it->address;
      ++i;
    }
  }
}

void Smtp::Server::SmtpDataAddress(const std::string& field, const std::string& address, const std::string& name)
{
  *this << field << ": " << address;
  if (name.size() > 0)
    *this << " <" << name << ">";
  *this << "\r\n";
}

void Smtp::Server::SmtpRecipients(const Smtp::Mail& mail)
{
  auto it  = mail.recipients.begin();
  auto end = mail.recipients.end();
  
  for (; it != end ; ++it)
  {
    *this << "RCPT TO: " << it->address << "\r\n";
    NetworkWrite();
    SmtpReadAnswer();
  }
}

void Smtp::Server::SmtpNewMail(const Smtp::Mail& mail)
{
  const Smtp::Mail::Sender& sender = mail.GetSender();

  *this << "MAIL FROM: " << sender.address << "\r\n";
  NetworkWrite();
  SmtpReadAnswer();
}

void Smtp::Server::SmtpHandshake()
{
  std::stringstream command;

  // Receiving the server identification
  server_id = SmtpReadAnswer(220);
  // Sending handshake
  command << "HELO " << Ip() << "\r\n";
  *this << command.str();
  NetworkWrite();
  SmtpReadAnswer(250);
}

void Smtp::Server::SmtpQuit()
{
  *this << "QUIT\r\n";
  NetworkWrite();
  SmtpReadAnswer(221);
}

/* 
 * SMTP StartTLS (RFC 2487) (TLS protocol is defined in RFC2246)
 */
void Smtp::Server::StartTls(void)
{
  tls_enabled = true;
}

void Smtp::Server::SmtpStartTls(void)
{
  *this << "STARTTLS" << "\r\n";
  NetworkWrite();
  SmtpReadAnswer(220);
}

/*
 * SMTP Authentication Extension (RFC 2554)
 */
std::map<Smtp::Server::AuthenticationProtocol,Smtp::Server::SmtpAuthMethod> Smtp::Server::auth_methods = {
  { Smtp::Server::PLAIN,      &Smtp::Server::SmtpAuthPlain     },
  { Smtp::Server::LOGIN,      &Smtp::Server::SmtpAuthLogin     },
  { Smtp::Server::MD5,        &Smtp::Server::SmtpAuthMd5       },
  { Smtp::Server::DIGEST_MD5, &Smtp::Server::SmtpAuthDigestMd5 },
  { Smtp::Server::CRAM_MD5,   &Smtp::Server::SmtpAuthCramMd5   }
};

void Smtp::Server::SmtpAuthPlain(const std::string& user, const std::string& password)
{
  string auth_hash = String::base64_encode('\000' + user + '\000' + password);

  *this << "AUTH PLAIN " << auth_hash << "\r\n";
  NetworkWrite();
  SmtpReadAnswer(235);
}

void Smtp::Server::SmtpAuthLogin(const std::string& user, const std::string& password)
{
  string user_hash = String::base64_encode(user);
  string pswd_hash = String::base64_encode(password);

  *this << "AUTH LOGIN\r\n";
  NetworkWrite();
  SmtpReadAnswer(334);
  *this << user_hash << "\r\n";
  NetworkWrite();
  SmtpReadAnswer(334);
  *this << pswd_hash << "\r\n";
  NetworkWrite();
  SmtpReadAnswer(235);
}

void Smtp::Server::SmtpAuthMd5(const std::string& user, const std::string& password)
{
  throw "unsupported";
}

void Smtp::Server::SmtpAuthDigestMd5(const string& user, const string& password)
{
  throw "unsupported";
}

void Smtp::Server::SmtpAuthCramMd5(const string& user, const string& password)
{
  throw "unsupported";
}

// END SMTP AUTHENTICATION EXTENSION
