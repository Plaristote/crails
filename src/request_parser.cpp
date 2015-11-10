#include "crails/request_parser.hpp"
#include "crails/params.hpp"

using namespace std;
using namespace Crails;

bool RequestParser::content_type_matches(Params& params, const Regex regex)
{
  if (params["headers"]["Content-Type"].exists())
  {
    string type = params["headers"]["Content-Type"].as<string>();

    return (!(regex.Match(type)));
  }
  return false;
}

 
