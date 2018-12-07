#ifndef  CRAILS_FRONT_TABLE_HPP
# define CRAILS_FRONT_TABLE_HPP

# include <crails/front/element.hpp>
# include <map>
# include <functional>

namespace Crails
{
  namespace Front
  {
    template<typename CONTENT_TYPE>
    class Table : public Crails::Front::Element
    {
      typedef std::function<Crails::Front::Element (const CONTENT_TYPE&)>  ColumnCallback;
      typedef std::vector<std::pair<std::string, ColumnCallback> >         ColumnMap;
      typedef std::vector<std::pair<std::string, Crails::Front::Element> > ColumnHeaders;

      ColumnMap     _columns;
      ColumnHeaders _header;
    public:
      Table() : Crails::Front::Element("table")
      {
      }

      Table& columns(const ColumnMap& value) { _columns = value; return *this; }
      Table& column(const std::string& name, ColumnCallback callback) { _columns.push_back(std::pair<std::string, ColumnCallback>(name, callback)); return *this; }
      Table& header_columns(const ColumnHeaders& value) { _header = value; return *this; }
      Table& header_column(const std::string& name, Crails::Front::Element el) { _header.push_back(std::pair<std::string, Crails::Front::Element>(name, el)); return *this; }

      void set_content(const std::vector<CONTENT_TYPE>& content)
      {
        std::vector<Crails::Front::Element> rows = make_rows(content);

        rows.insert(rows.begin(), make_header_row());
        html("").inner(rows);
      }
      
    private:      
      Crails::Front::Element make_header_row()
      {
        Crails::Front::Element tr("tr");
        std::vector<Crails::Front::Element> ths;

        for (const auto& th : _header)
          ths.push_back(th.second);
        return tr.inner(ths);
      }
      
      Crails::Front::Element make_row(const CONTENT_TYPE& value)
      {
        Crails::Front::Element tr("tr");
        std::vector<Crails::Front::Element> tds;

        for (const auto& td : _columns)
          tds.push_back(td.second(value));
        return tr.inner(tds);
      }

      std::vector<Crails::Front::Element> make_rows(const std::vector<CONTENT_TYPE>& content)
      {
        std::vector<Crails::Front::Element> rows;

        for (const auto& entry : content)
          rows.push_back(make_row(entry));
        return rows;
      }
    };
  }
}

#endif
