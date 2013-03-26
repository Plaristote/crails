#!/usr/bin/ruby

bundle = ARGV[0]
name   = ARGV[1]

if bundle.nil? or name.nil?
  puts "usage: crails generate/layout bootsrap [name]"
  exit 1
end

if bundle != 'bootstrap'
  puts "sorry, only bootstrap is available for now"
  exit 2
end

javascripts = [ 'jquery',
                'boostrap-transition',
                'bootstrap-alert',
                'bootstrap-modal',
                'boostrap-dropdown',
                'boostrap-scrollspy',
                'bootstrap-tab',
                'bootstrap-tooltip',
                'bootstrap-popover',
                'bootstrap-button',
                'bootstrap-collapse',
                'bootstrap-carousel',
                'bootstrap-typeahead' ]

require 'net/http'
javascripts.each do |js|
  path = "public/js/#{js}.js"
  unless File.exists? path
    dl   = "http://twitter.github.com/bootstrap/assets/js/#{js}.js"
    url        = URI.parse dl
    request    = Net::HTTP::Get.new url.path
    response   = Net::HTTP.start url.host, url.port do |http|
      http.request request
    end
    File.open path, 'w' do |file|
      file.write response.body
    end
    puts "\033[32m[CREATE]\033[0m " + "File #{path}"
  end
end

html = <<HTML
string* @yield;
// END LINKING
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <title>Bootstrap, from Twitter</title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta name="description" content="">
    <meta name="author" content="">

    <!-- Le styles -->
    <link href="/css/bootstrap.min.css" rel="stylesheet">
    <style>
      body {
        padding-top: 60px; /* 60px to make the container go all the way to the bottom of the topbar */
      }
    </style>
    <link href="/css/bootstrap-responsive.css" rel="stylesheet">
  </head>

  <body>

    <div class="navbar navbar-inverse navbar-fixed-top">
      <div class="navbar-inner">
        <div class="container">
          <button type="button" class="btn btn-navbar" data-toggle="collapse" data-target=".nav-collapse">
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
          </button>
          <a class="brand" href="#">Project name</a>
          <div class="nav-collapse collapse">
            <ul class="nav">
              <li class="active"><a href="#">Home</a></li>
              <li><a href="#about">About</a></li>
              <li><a href="#contact">Contact</a></li>
            </ul>
          </div><!--/.nav-collapse -->
        </div>
      </div>
    </div>

    <div class="container">

      <%= (yield ? *yield : "") %>

    </div> <!-- /container -->

    <!-- Le javascript
    ================================================== -->
    <!-- Placed at the end of the document so the pages load faster -->
#{
  (
    value = String.new
    javascripts.each do |js|
      value += "<script src='/js/" + js + ".js'></script>\n"
    end
    value
  )
}  </body>
</html>
HTML

path = "app/views/layouts/#{name}.html.ecpp"

File.open path, 'w' do |file|
  file.write html
end

puts "\033[32m[CREATE]\033[0m " + "File #{path}" 
