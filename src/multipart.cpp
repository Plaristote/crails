#include "crails/server.hpp"
#include <Boots/Utils/regex.hpp>
#include <Boots/Utils/directory.hpp>
#include "crails/cookie_data.hpp"
#include "crails/params.hpp"
#include "crails/multipart.hpp"
#include <fstream>

using namespace std;
using namespace Crails;

void MultipartParser::parse(Params& params)
{
  bool blocked;
  
  do
  {
    blocked = true;
    if (parsed_state == 0)
    {
      const Regex reg_header(boundary + "\r\nContent-Disposition: ([^;]+);", REG_EXTENDED);
      regmatch_t         matches[2];

      if (!(reg_header.Match(read_buffer, matches, 2)))
      {
        unsigned int disp_so  = matches[1].rm_so;
        unsigned int disp_len = matches[1].rm_eo - disp_so;
        unsigned int form_end = matches[0].rm_eo;

        content_disposition = read_buffer.substr(disp_so, disp_len);
        read_buffer.erase(0, form_end);
        parsed_state++;
        cout << "Parsed state 1" << endl;
      }
    }
    if (parsed_state == 1)
    {
      const Regex reg("([^\r]+)\r\n", REG_EXTENDED);
      regmatch_t          matches[2];
      
      if (!(reg.Match(read_buffer, matches, 2)))
      {
        unsigned int form_so  = matches[1].rm_so;
        unsigned int form_len = matches[1].rm_eo - form_so;
        unsigned int form_end = matches[0].rm_eo;

        content_data.unserialize(read_buffer.substr(form_so, form_len));
        for (unsigned short i = 0 ; i < content_data.Count() ; ++i)
        {
          Data   data = content_data[i];
          string str  = data.Value();

          if (str.size() > 0)
            str.erase(0, 1);
          if (str.size() > 0)
            str.erase(str.size() - 1, str.size());
          data = str;
        }
        content_data.Output();
        read_buffer.erase(0, form_end);
        parsed_state++;
          cout << "Parsed state 2" << endl;
      }
    }
    if (parsed_state == 2)
    {
      if (read_buffer.substr(0, 14) == "Content-Type: ")
      {
        size_t end = read_buffer.find("\r\n");
        
        if (end != string::npos)
        {
          mimetype = read_buffer.substr(14, end - 14);
          cout << "Found mimetype: " << mimetype << endl;
          read_buffer.erase(0, end + 4);
          parsed_state ++;
          cout << "Parsed state 3 (file)" << endl;
        }
      }
      else if (read_buffer.substr(0, 2) == "\r\n")
      {
        cout << '\'' << read_buffer.substr(0, 14) << '\'' << endl;
        read_buffer.erase(0, 2);
        parsed_state++;
        cout << "Parsed state 3 (form)" << endl;
      }
    }
    if (parsed_state == 3)
    {
      size_t pos = read_buffer.find(boundary);

      if (mimetype == "")
      {
        if (pos != string::npos)
        {
          string field = content_data["name"];

          params.lock();
          params[field] = read_buffer.substr(0, pos - 4);
          params.unlock();
          read_buffer.erase(0, pos);
          parsed_state = 0;
          blocked = false;
          cout << "Done" << endl;
        }
      }
      else
      {
        unsigned int to_copy, to_erase;
        bool         to_close = false;

        if (!(file.is_open()))
        {
          file.open("/tmp/" + content_data["filename"].Value());
        }
        if (pos != string::npos)
        {
          Params::File to_push;

          to_push.key            = content_data["name"].Value();
          to_push.name           = content_data["filename"].Value();
          to_push.mimetype       = mimetype;
          to_push.temporary_path = "/tmp/" + content_data["filename"].Value();
          params.files.push_back(to_push);
          to_erase               = pos;
          to_copy                = pos - 2;
          parsed_state           = 0;
          blocked                = read_buffer.size() <= pos + boundary.size();
          mimetype               = "";
          to_close               = true;
          cout << "Done" << endl;
        }
        else
          to_copy = to_erase = read_buffer.size();
        file.write(read_buffer.c_str(), to_copy - 2);
        //cout << "Reading: " << endl;
        //cout << read_buffer << endl;
        read_buffer.erase(0, to_erase - 2);
        if (to_close)
          file.close();
      }
    }
  } while (!blocked);
}

void MultipartParser::initialize(Params& params)
{
  string             type     = params["header"]["Content-Type"].Value();
  Regex              get_boundary("boundary=(.*)", REG_EXTENDED);
  regmatch_t         matches[2];

  get_boundary.Match(type, matches, 2);
  to_read      = params["header"]["Content-Length"];
  total_read   = 0;
  boundary     = type.substr(matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so);
  parsed_state = 0;
}
