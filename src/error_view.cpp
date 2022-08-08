#include "crails/http_response.hpp"
#include "crails/params.hpp"
#include "crails/request_handlers/file.hpp"
#include "crails/shared_vars.hpp"
#include "crails/renderer.hpp"

using namespace std;

namespace Crails
{
  static void render_custom_error_view(string view_name, BuildingResponse& out, HttpStatus code, Params& params)
  {
    out.set_status_code(code);
    if (Renderer::can_render(view_name, params.as_data()))
    {
      SharedVars vars;
      string     content_type;

      Renderer::render(view_name, params.as_data(), out, vars);
    }
    out.send();
  }

  void render_error_view(BuildingResponse& out, HttpStatus code, Params& params)
  {
    FileRequestHandler* file_handler = reinterpret_cast<FileRequestHandler*>(Server::get_request_handler("file"));
    stringstream file_name;
    stringstream view_name;

    file_name << (unsigned int)(code);
    view_name << "errors/" << file_name.str();
    if (file_handler &&
        file_handler->send_file("public/" + file_name.str() + ".html", out, code))
      return ;
    else
      render_custom_error_view(view_name.str(), out, code, params);
  }
}
