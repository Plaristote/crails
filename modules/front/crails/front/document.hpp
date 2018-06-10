#ifndef  FRONT_DOCUMENT_HPP
# define FRONT_DOCUMENT_HPP

# include "object.hpp"

namespace Crails
{
  namespace Front
  {
    class Document : public ObjectImpl<client::Document>
    {
    public:
      Document();

      void on_ready(std::function<void ()> callback);
    };
  }
}

#endif
