#include "ecpp_header_parser.hpp"
#include "ecpp_body.hpp"
#include "utils/string.hpp"
#include <sstream>

using namespace std;

static bool property_has_initializer(EcppProperty property) { return property.shared || property.default_value.has_value(); }

static string ecpp_result(const EcppHeader& header, const string& body, const EcppOptions& options)
{
  stringstream result;

  result
    << "#include <sstream>" << endl
    << "#include \"" << options.crails_include << "shared_vars.hpp\"" << endl
    << "#include \"" << options.parent_header << '"' << endl;
   for (auto preprocessor_line : header.preprocessor)
    result << preprocessor_line << endl;
  result << endl;

  result
    << "class " << header.name << " : public " << options.parent_class << endl
    << '{' << endl
    << "public:" << endl;

  // Constructor and property initializers
  result
    << "  " << header.name << "(const Crails::Renderer* renderer, Crails::SharedVars& vars) :" << endl
    << "    " << options.parent_class << "(renderer, vars)";
  for (auto property : header.properties)
  {
    if (property_has_initializer(property))
    {
      result << ", " << endl << "    ";
      if (property.shared)
      {
        string casting_type = property.type;

        result << property.name << '(';
        if (property.reference)
        {
          casting_type[casting_type.length() - 1] = '*';
          result << "reinterpret_cast<" << property.type << ">(*";
        }
        if (property.default_value.has_value())
          result << "Crails::cast<" << casting_type << ">(vars, \"" << property.name << "\", " << property.default_value.value() << ')';
        else
          result << "Crails::cast<" << casting_type << ">(vars, \"" << property.name << "\")";
        if (property.reference)
          result << ')';
        result << ')';
      }
      else if (property.default_value.has_value())
        result << property.name << '(' << property.default_value.value() << ')';
    }
  }
  result << endl << "  {}" << endl << endl;

  // Render
  result
    << "  std::string render()" << endl
    << "  {" << endl
    << body << endl
    << "    return " << options.out_property_name << ".str();" << endl
    << "  }" << endl;

  // Properties
  result << "private:" << endl
    << "  std::stringstream " << options.out_property_name << ';' << endl;
  for (auto property : header.properties)
    result << "  " << property.type << ' ' << property.name << ';' << endl;
  result << "};" << endl << endl;

  // Exported function
  result
    << "std::string render_" << Crails::underscore(header.name.data())
    << "(const Crails::Renderer* renderer, Crails::SharedVars& vars)" << endl
    << '{' << endl
    << "  return " << header.name << "(renderer, vars).render();" << endl
    << '}';
  return result.str();
}

static pair<string, string> ecpp_split_template_header_and_body(const string& source)
{
  regex zone_delimiter("^//\\s+END\\s+LINKING\\s*$", regex_constants::multiline | regex_constants::icase);
  auto  match = sregex_iterator(source.begin(), source.end(), zone_delimiter);

  if (match != sregex_iterator())
  {
    return {
      source.substr(0, match->position(0)), // header
      source.substr(match->position(0) + match->length() + 1) // body
    };
  }
  return {"", source};
}

string ecpp_generate(const string& source, const EcppOptions& options)
{
  auto       parts = ecpp_split_template_header_and_body(source);
  string     body;
  EcppHeader header = EcppHeaderParser(options.output_name, parts.first);

  if (parts.first.length() > 0)
    header.line_count++;
  body = options.body_mode == "markup"
    ? EcppMarkupBody(parts.second, header.line_count, options.out_property_name).str()
    : parts.second;
  return ecpp_result(header, body, options);
}
