#ifndef  CRAILS_FRONT_ELEMENT_HPP
# define CRAILS_FRONT_ELEMENT_HPP

# include <cheerp/client.h>
# include <cheerp/clientlib.h>
# include <map>

namespace Crails
{
  namespace Front
  {
    struct Element
    {
    public:
      Element();
      Element(const client::String& str, const std::map<std::string, std::string>& attrs = {});
      Element(client::HTMLElement*);

      Element& inner(const std::vector<Element>& children);
      Element& attr(const std::map<std::string, std::string>& attrs);
      Element& attr(const std::map<std::string, std::wstring>& attrs);
      Element& attr(const std::map<std::string, client::String*>& attrs);

      Element& html(client::String* content)      { el->set_innerHTML(content); return *this; }
      Element& html(const std::string& content)   { _html<std::string>(content); return *this; }
      Element& html(const std::wstring& content)  { _html<std::wstring>(content); return *this; }
      Element& text(client::String* content)      { el->set_textContent(content); return *this; }
      Element& text(const std::string& content)   { _text<std::string>(content); return *this; }
      Element& text(const std::wstring& content)  { _text<std::wstring>(content); return *this; }
      Element& value(client::String* val)         { static_cast<client::HTMLInputElement*>(el)->set_value(val); return *this; }
      Element& value(const std::string& val)      { _value<std::string>(val); return *this; }
      Element& value(const std::wstring& val)     { _value<std::wstring>(val); return *this; }
      Element& checked(bool val)                  { static_cast<client::HTMLInputElement*>(el)->set_checked(val); return *this; }

      Element& operator>(const std::vector<Element>& children);
      Element& operator>(Element child);
      Element& empty();

      void append_to(client::HTMLElement* el);
      void destroy();

      bool    has_parent() const;
      Element get_parent();
      Element get_next();

      client::HTMLElement* operator->() { return el; }
      client::HTMLElement* operator*()  { return el; }

      std::vector<Element> find(const std::string& selector);
      bool                 contains(const client::HTMLElement*);
      void                 each(std::function<bool (Element&)>);

      void                 toggle_class(const std::string& str, bool set);
      void                 add_class(const std::string& str);
      void                 remove_class(const std::string& str);
      bool                 has_class(const std::string& str) const;

      bool                 has_attribute(const std::string& key) const;
      std::string          get_attribute(const std::string& key) const;
      void                 remove_attribute(const std::string& key);

      bool                 is_visible() const;

      std::string          get_value() const;
      bool                 get_checked() const { return static_cast<client::HTMLInputElement*>(el)->get_checked(); }

    private:
      template<typename TYPE>
      void _html(const TYPE& content)
      {
        el->set_innerHTML(content.c_str());
      }

      template<typename TYPE>
      void _text(const TYPE& content)
      {
        el->set_textContent(content.c_str());
      }

      template<typename TYPE>
      void _value(const TYPE& val)
      {
        static_cast<client::HTMLInputElement*>(el)->set_value(val.c_str());
      }

      client::HTMLElement* el;
    };
  }
}

#endif

