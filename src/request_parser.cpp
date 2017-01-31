#include "crails/request_parser.hpp"
#include "crails/params.hpp"

using namespace std;
using namespace Crails;

bool RequestParser::content_type_matches(Params& params, const regex regexp)
{
  Data           headers = params["headers"];
  vector<string> supported_headers = {"Content-Type","content-type"};

  for (string header_key : supported_headers)
  {
    if (params["headers"][header_key].exists())
    {
      string type = params["headers"][header_key].as<string>();

      return regex_search(type, regexp);
    }
  }
  return false;
}

 
