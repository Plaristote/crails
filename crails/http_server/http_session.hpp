#ifndef  CRAILS_HTTP_SESSION_HPP
# define CRAILS_HTTP_SESSION_HPP

# include <memory>
# include <boost/beast/core.hpp>
# include <boost/beast/http.hpp>
# include <boost/make_unique.hpp>
# include <boost/optional.hpp>

namespace Crails
{
  class HttpSession : public std::enable_shared_from_this<HttpSession>
  {
    static const unsigned int   body_limit;
    static const unsigned short timeout_in_seconds;

    class Queue
    {
      enum
      {
        limit = 8 // maximum number of queued responses
      };

      struct Work
      {
        virtual ~Work() = default;
        virtual void operator()() = 0;
      };

      template<bool isRequest, class Body, class Fields>
      struct WorkImpl : public Work
      {
        typedef typename boost::beast::http::message<isRequest, Body, Fields> MessageType;
        HttpSession& self;
        MessageType  message;

        WorkImpl(HttpSession& self, MessageType&& message) : self(self), message(std::move(message))
        {
        }

        void operator()()
        {
          boost::beast::http::async_write(
            self.stream,
            message,
            boost::beast::bind_front_handler(
              &HttpSession::on_write,
              self.shared_from_this(),
              message.need_eof()
            )
          );
        }
      };

      HttpSession& self;
      std::vector<std::unique_ptr<Work> > items;
    public:
      explicit Queue(HttpSession& self) : self(self)
      {
        static_assert(limit > 0, "queue limit must be positive");
        items.reserve(limit);
      }

      bool is_full() const { return items.size() >= limit; }

      bool on_write()
      {
	if (!items.empty()) // that should always be true
        {
          auto const was_full = is_full();
          items.erase(items.begin());
          if (!items.empty())
            (*items.front())();
          return was_full;
        }
        return false;
      }

      template<bool isRequest, class Body, class Fields>
      void operator()(boost::beast::http::message<isRequest, Body, Fields>&& message)
      {
        typedef WorkImpl<isRequest, Body, Fields> WorkType;
        auto new_item = boost::make_unique<WorkType>(self, std::move(message));

        items.push_back(new_item);
        if (items.size() == 1)
          (*items.front())();
      }
    };

    boost::beast::tcp_stream  stream;
    boost::beast::flat_buffer buffer;
    Queue                     queue;

    boost::optional<boost::beast::http::request_parser<boost::beast::http::string_body> > parser;

  public:
    HttpSession(boost::asio::ip::tcp::socket&& socket) : stream(std::move(socket)), queue(*this)
    {
    }

    void run();

  private:
    void wait_read();
    void on_read(boost::beast::error_code ec, std::size_t bytes_transferred);
    void on_write(bool close, boost::beast::error_code ec, std::size_t bytes_transferred);
    void close_connection();
  };
}

#endif
