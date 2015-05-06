#include "crails/request_parser.hpp"
#include "crails/params.hpp"

using namespace std;

bool RequestParser::content_type_matches(Params& params, const Regex regex)
{
  string type = params["header"]["Content-Type"].Value();

  return (regex.Match(type));
}

 
