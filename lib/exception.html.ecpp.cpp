  #include <sstream>
  #include <iostream>
  #include <crails/shared_vars.hpp>

  #include <Boots/Utils/dynstruct.hpp>
#include <algorithm>

  using namespace std;

  extern "C"
  {
    std::string generate_view(SharedVars&);
    std::string render_view(const std::string& name, SharedVars&);
  }

  std::string generate_view(SharedVars& vars)
  {
    stringstream html_stream;
    
    // BEGIN LINKING

string* exception_name = (string*)*(vars["exception_name"]);
string* exception_what = (string*)*(vars["exception_what"]);
DynStruct* params = (DynStruct*)*(vars["params"]);

DynStruct& lparams = *params;

string     backtrace = lparams["backtrace"].Value();
lparams["backtrace"].Remove();
// END LINKING

    html_stream << "\n<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n  <meta charset=\"utf-8\" />\n  <title>Crails Server: Exception caught</title>\n  <style>\n    body { background-color: #fff; color: #333; }\n\n    body, p, ol, ul, td {\n      font-family: helvetica, verdana, arial, sans-serif;\n      font-size:   13px;\n      line-height: 18px;\n    }\n\n    pre {\n      background-color: #eee;\n      padding: 10px;\n      font-size: 11px;\n    }\n\n    a { color: #000; }\n    a:visited { color: #666; }\n    a:hover { color: #fff; background-color:#000; }\n  </style>\n</head>\n<body>\n\n<h1>" <<  *exception_name ;
html_stream << "</h1>\n<pre>" <<  *exception_what ;
html_stream << "</pre>\n\n\n<p><code>Rails.root: /home/plaristote/tmp/NoaERP-app</code></p>\n\n<div id=\"traces\">\n    <a href=\"#\" onclick=\"document.getElementById('Framework-Trace').style.display='none';document.getElementById('Full-Trace').style.display='none';document.getElementById('Application-Trace').style.display='block';; return false;\">Application Trace</a> |\n    <a href=\"#\" onclick=\"document.getElementById('Application-Trace').style.display='none';document.getElementById('Full-Trace').style.display='none';document.getElementById('Framework-Trace').style.display='block';; return false;\">Framework Trace</a> |\n    <a href=\"#\" onclick=\"document.getElementById('Application-Trace').style.display='none';document.getElementById('Framework-Trace').style.display='none';document.getElementById('Full-Trace').style.display='block';; return false;\">Full Trace</a> \n\n    <div id=\"Application-Trace\" style=\"display: block;\">\n      <pre><code>" <<  backtrace ;
html_stream << "</code></pre>\n    </div>\n    <div id=\"Framework-Trace\" style=\"display: none;\">\n      <pre><code>Dolor site ahmet</code></pre>\n    </div>\n    <div id=\"Full-Trace\" style=\"display: none;\">\n      <pre><code>Lorem ipsum</code></pre>\n    </div>\n</div>\n\n\n\n<h2 style=\"margin-top: 30px\">Request</h2>\n<p><b>Parameters</b>:\n<pre>\n";
 if (params == 0) { ;
html_stream << "\n  The parameter object wasn't sent to the exception view.\n";
 ; } else if (lparams.Count() == 0) { ;
html_stream << "\n  This request didn't contain any parameters.\n";
 ; } else { ;
html_stream << "\n  ";
 lparams.Output(html_stream); ;
html_stream << "\n";
 ; } ;
html_stream << "\n</pre></p>\n\n<p><a href=\"#\" onclick=\"document.getElementById('session_dump').style.display='block'; return false;\">Show session dump</a></p>\n<div id=\"session_dump\" style=\"display:none\">\n<pre>\n  Soon the session dump here\n</pre></div>\n\n<h2 style=\"margin-top: 30px\">Response</h2>\n<p><b>Headers</b>: <pre>None</pre></p>\n\n\n</body>\n</html>";
    return (html_stream.str());
  }
