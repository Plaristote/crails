#include <crails/front/comment_element.hpp>
#include <crails/front/globals.hpp>

using namespace Crails::Front;

client::HTMLElement* CommentElement::makeCommentElement(const client::String& str)
{
  client::Comment* comment = Crails::Front::document->createComment(str);

  return reinterpret_cast<client::HTMLElement*>(comment);
}
