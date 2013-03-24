#include "crails/server.hpp"
#include <Boots/Utils/regex.hpp>
#include <Boots/Utils/directory.hpp>
#include "crails/cookie_data.hpp"
#include "crails/params.hpp"
#include <fstream>

using namespace std;

void CrailsServer::ParseMultipart(Response response, Params& params)
{
  cout << "Going for multipart/form-data parsing" << endl;
  Sync::Semaphore    sem(1);
  Sync::Semaphore    end(0);
  string             type       = params["header"]["Content-Type"].Value();
  unsigned int       to_read    = params["header"]["Content-Length"];
  unsigned int       total_read = 0;
  std::string        read_buffer;
  Regex              get_boundary;
  regmatch_t         matches[2];

  get_boundary.SetPattern("boundary=(.*)", REG_EXTENDED);
  get_boundary.Match(type, matches, 2);
  string boundary      = type.substr(matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so);
  short  parsed_state  = 0;

  string     content_disposition;
  CookieData content_data;
  string     mimetype;
  ofstream   file;

  callback = [this, &sem, &end,
                           &to_read, &total_read, &read_buffer,
                           &response, boundary,
                           &parsed_state, &content_disposition, &content_data, &mimetype, &file,
                           &params](boost::iterator_range<char const*> range,
                      boost::system::error_code error_code,
                      size_t size_read,
                      Server::connection_ptr connection_ptr)
  {
    bool blocked;
    
    sem.Wait();
    total_read += size_read;
    for (unsigned int i = 0 ; i < size_read ; ++i)
      read_buffer += range[i];
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

          content_data.Unserialize(read_buffer.substr(form_so, form_len));
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

            params.Lock();
            params[field] = read_buffer.substr(0, pos - 4);
            params.Unlock();
            read_buffer.erase(0, pos);
            parsed_state = 0;
            blocked = false;
            cout << "Done" << endl;
          }
        }
        else
        {
          unsigned int to_copy, to_erase;

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
            cout << "Done" << endl;
          }
          else
            to_copy = to_erase = read_buffer.size();
          file.write(read_buffer.c_str(), to_copy - 2);
          read_buffer.erase(0, to_erase - 2);
        }
      }
    } while (!blocked);
    if (total_read < to_read)
      response->read(callback);
    else
      params.response_parsed.Post();
    sem.Post();
  };
  response->read(callback);
  params.response_parsed.Wait();
}

void CrailsServer::ParseForm(Response response, Params& params)
{
  cout << "Going for form-data parsing" << endl;
  Sync::Semaphore    sem(0);
  string             type       = params["header"]["Content-Type"].Value();
  unsigned int       to_read    = params["header"]["Content-Length"];
  unsigned int       total_read = 0;
  std::string        read_buffer;
  Regex              get_boundary;
  
  callback = [this, &sem, &to_read, &total_read, &read_buffer, &response](boost::iterator_range<char const*> range,
                        boost::system::error_code error_code,
                        size_t size_read,
                        Server::connection_ptr connection_ptr)
  {
    for (unsigned int i = 0 ; i < size_read ; ++i)
      read_buffer += range[i];
    total_read += size_read;
    if (total_read == to_read)
      sem.Post();
    else
      response->read(callback);
  };
  response->read(callback);
  sem.Wait();

  // Getting post parameters
  if (read_buffer.size() > 0)
  {
    cgi2params(params, read_buffer);
  }
}

void CrailsServer::ParseResponse(Response response, Params& params)
{
  static const Regex is_multipart("^multipart/form-data", REG_EXTENDED);
  string             type       = params["header"]["Content-Type"].Value();

  if (!(is_multipart.Match(type)))
    ParseMultipart(response, params);
  else
    ParseForm(response, params);
}