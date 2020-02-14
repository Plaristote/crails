#ifndef  CRAILS_FRONT_COMMENT_ELEMENT_HPP
# define CRAILS_FRONT_COMMENT_ELEMENT_HPP

# include "element.hpp"

namespace Crails
{
  namespace Front
  {
    class CommentElement : public Crails::Front::Element
    {
      static client::HTMLElement* makeCommentElement(const client::String&);
    public:
      CommentElement(const client::String& str) : Element(makeCommentElement(str)) {}
      CommentElement() : Element(makeCommentElement("")) {}
    };
  }
}

#endif
