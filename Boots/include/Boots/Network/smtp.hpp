#ifndef  BOOTS_SMTP_HPP
# define BOOTS_SMTP_HPP

# include "socket.hpp"
# include "../Utils/helpers.hpp"
# include <map>

namespace Smtp
{
  class Server;
  
  class Mail
  {
    friend class Server;
  public:
    struct Recipient
    {
      Recipient() : type(0) {}

      bool          operator==(const std::string& address) const { return (this->address == address); }
      std::string   address;
      std::string   name;
      unsigned char type;
    };
    
    struct Sender
    {
      std::string address;
      std::string name;
    };

    typedef std::vector<Recipient> Recipients;
    
    enum RecipientType
    {
      CarbonCopy = 1,
      Blind      = 2
    };

    void        SetSender(const std::string& address, const std::string& name = "");
    void        AddRecipient(const std::string& address, const std::string& name = "", unsigned char flags = 0);
    void        DelRecipient(const std::string& address);
    const_attr_accessor(Sender&,      Sender,  sender)
    const_attr_accessor(std::string&, Subject, subject)
    const_attr_accessor(std::string&, Body,    body)
    const_attr_accessor(std::string&, ReplyTo, reply_to)

  private:
    Recipients  recipients;
    Sender      sender;
    std::string subject;
    std::string body;
    std::string reply_to;
  };
  
  class Server : private Network::Socket
  {
  public:
    enum AuthenticationProtocol
    {
      PLAIN,
      LOGIN,
      DIGEST_MD5,
      MD5,
      CRAM_MD5
    };
    
    Server();

    void StartTls(void);
    void Connect(const std::string& hostname, unsigned short port);
    void Connect(const std::string& hostname, unsigned short port, const std::string& username, const std::string& password, AuthenticationProtocol auth = PLAIN);
    void Disconnect(void);
    void Send(const Mail& mail);
    const_attr_getter(std::string&, ServerId, server_id)

  private:
    // SMTP Protocol Implementation
    std::string SmtpReadAnswer(unsigned short expected_return = 250);
    void        SmtpBody(const Mail& mail);
    void        SmtpRecipients(const Mail& mail);
    void        SmtpNewMail(const Mail& mail);
    void        SmtpHandshake();
    void        SmtpQuit();
    
    void        SmtpDataAddress(const std::string& field, const std::string& address, const std::string& name);
    void        SmtpDataAddresses(const std::string& field, const Mail& mail, int flag);
    
    std::string server_id;

    // Authentication Extension as defined in RFC 2554
    typedef void (Server::*SmtpAuthMethod)(const std::string& user, const std::string& password);
    void        SmtpAuthPlain    (const std::string&, const std::string&);
    void        SmtpAuthLogin    (const std::string&, const std::string&);
    void        SmtpAuthDigestMd5(const std::string&, const std::string&);
    void        SmtpAuthMd5      (const std::string&, const std::string&);
    void        SmtpAuthCramMd5  (const std::string&, const std::string&);
    
    static std::map<AuthenticationProtocol,SmtpAuthMethod> auth_methods;

    // StartTLS Extension as defined in RFC 2487
    void        SmtpStartTls(void);
    bool        tls_enabled;
  };
}

#endif