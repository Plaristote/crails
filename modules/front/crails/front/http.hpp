#ifndef  CRAILS_FRONT_HTTP_HPP
# define CRAILS_FRONT_HTTP_HPP

#include <crails/front/promise.hpp>
#include <crails/datatree.hpp>

namespace Crails
{
  namespace Front
  {
    namespace Http
    {
      class Request;
      
      class Response
      {
        friend class Request;

        bool        _has_body = false;
        std::string body;
        double      status = 0;
      public:
        Response();
        
        void initialize(client::XMLHttpRequest* xhr);

        bool   has_body()   const { return _has_body; }          
        bool   ok()         const { return status >= 200 && status < 300; }
        double get_status() const { return status; }

        DataTree    get_response_data() const;
        std::string get_response_text() const { return body; }
      };

      class Request
      {
        const std::string          method,  path;
        std::string                body;
        std::shared_ptr<Response>  response;
        client::XMLHttpRequest*    xhr;

      public:
        Request(const std::string& m, const std::string& p);

        typedef Crails::Front::Promise       PromiseType;
        typedef std::shared_ptr<PromiseType> PromisePtr;
        typedef std::shared_ptr<Request>     Ptr;

        static Ptr get    (const std::string& path) { return make("get", path); }
        static Ptr post   (const std::string& path) { return make("post", path); }
        static Ptr put    (const std::string& path) { return make("put", path); }
        static Ptr patch  (const std::string& path) { return make("patch", path); }
        static Ptr _delete(const std::string& path) { return make("delete", path); }

        static Ptr make(const std::string& method, const std::string& path)
        {
          return std::make_shared<Request>(method, path);
        }

        std::shared_ptr<Response> get_response() const { return response; }

        void set_headers(const std::map<std::string, std::string> value);
        void set_body(const std::string& value);

        Crails::Front::Promise send();
      };
    }
  }
}

#endif
