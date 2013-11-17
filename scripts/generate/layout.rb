#!/usr/bin/ruby

bundle  = ARGV[0]
name    = ARGV[1]
command = if ARGV[2].nil? then :install else ARGV[2].to_sym end

if bundle.nil? or name.nil?
  puts "usage: crails generate/layout [bootsrap|kickstart] [name] (install/uninstall)"
  exit 1
end

cmakefile      = (File.open "CMakeLists.txt").read rescue ""
project_name   = "Crails Framework"

results        = cmakefile.scan /project\(([a-z0-9_-]+)\)/i
if results.count > 0
  project_name = results.first.first
end

PROJECT_NAME   = project_name

require 'net/http'
require 'fileutils'

def download_file url, output
  url        = URI.parse url
  request    = Net::HTTP::Get.new url.path
  response   = Net::HTTP.start url.host, url.port do |http|
    http.request request
  end
  File.open output, 'w' do |file|
    file.write response.body
  end
end

class ThemeEngine
  def downloaded_files url, &block
    @git_url = url
    @js      = Array.new
    @css     = Array.new
    @dirs    = Array.new
    @files   = Array.new
    block.call
  end
  
  def javascript desc
    @js  << desc
  end
  
  def css desc
    @css << desc
  end
  
  def file path
    @files << path
  end
  
  def directory path
    @dirs << path
  end
  
  def html &block
    css   = String.new
    js    = String.new
    @js.each  { |item| css += "<script type='text/javascript' src='/js/#{item[:name]}.js'></script>\n"               if item[:include?] != false }
    @css.each { |item| js  += "<link rel='stylesheet' type='text/css' href='/css/#{item[:name]}.css' media='all' />" if item[:include?] != false }
    @html = block.call css, js
  end
  
  def install
    @dirs.each do |dir|
      path = dir.split '/'
      i    = 0
      while i < path.count
	tmp_path = 'public/' + (path[0..i].join '/')
	unless File.exists? tmp_path
          FileUtils.mkdir tmp_path unless File.exists? tmp_path
	  puts "\033[32m[CREATE]\033[0m " + "Directory #{tmp_path}"
	end
	i += 1
      end
    end
    
    @files.each do |path|
      url  = @git_url  + path
      path = 'public/' + path 
      unless File.exists? path 
        system "wget \"#{url}\" -O \"#{path}\" -o /dev/null"
        puts "\033[32m[CREATE]\033[0m " + "Downloaded file #{path}"	
      end
    end
    
    @css.each do |desc|
      path  = "public/css/" + desc[:name] + '.css'
      unless File.exists? path
	url = @git_url + desc[:url]
	system "wget #{url} -O #{path} -o /dev/null"
        #download_file url, path
        puts "\033[32m[CREATE]\033[0m " + "Downloaded file #{path}"
      end
    end
    
    @js.each do |desc|
      path  = "public/js/" + desc[:name] + '.js'
      unless File.exists? path
	url = @git_url + desc[:url]
	system "wget #{url} -O #{path} -o /dev/null"
        #download_file url, path
        puts "\033[32m[CREATE]\033[0m " + "Downloaded file #{path}\tfrom\t#{url}"
      end
    end

    File.open LAYOUT_OUTPUT_PATH, 'w' do |file|
      file.write @html
    end

    puts "\033[32m[CREATE]\033[0m " + "File #{LAYOUT_OUTPUT_PATH}" 
  end

  def uninstall
    # TODO
    puts "Not implemented yet"
  end
end

LAYOUT_OUTPUT_PATH = "app/views/layouts/#{name}.html.ecpp"

html = ""

if bundle == 'bootstrap'

javascripts = [ 'jquery',
                'bootstrap-transition',
                'bootstrap-alert',
                'bootstrap-modal',
                'bootstrap-dropdown',
                'bootstrap-scrollspy',
                'bootstrap-tab',
                'bootstrap-tooltip',
                'bootstrap-popover',
                'bootstrap-button',
                'bootstrap-collapse',
                'bootstrap-carousel',
                'bootstrap-typeahead' ]

javascripts.each do |js|
  path = "public/js/#{js}.js"
  unless File.exists? path
    dl   = "http://twitter.github.com/bootstrap/assets/js/#{js}.js"
    download_file dl, path
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
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta name="description" content="">
    <meta name="author" content="">
    <link rel="shortcut icon" href="../../docs-assets/ico/favicon.png">

    <title>#{PROJECT_NAME} - Powered by Crails Framework</title>

    <link href="/css/bootstrap.min.css" rel="stylesheet">
    <link href="/css/bootstrap-responsive.css" rel="stylesheet">
  </head>
  <body>
    <div class="navbar navbar-inverse navbar-fixed-top" role="navigation">
      <div class="container">
        <div class="navbar-header">
          <button type="button" class="navbar-toggle" data-toggle="collapse" data-target=".navbar-collapse">
            <span class="sr-only">Toggle navigation</span>
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
          </button>
          <a class="navbar-brand" href="#">#{PROJECT_NAME}</a>
        </div>
        <div class="collapse navbar-collapse">
          <ul class="nav navbar-nav">
            <li class="active"><a href="#">Home</a></li>
            <li><a href="#about">About</a></li>
            <li><a href="#contact">Contact</a></li>
          </ul>
        </div><!--/.nav-collapse -->
      </div>
    </div>

    <div class="container">
      <%= (yield ? *yield : "") %>
    </div><!-- /.container -->

#{
  (
    value = String.new
    javascripts.each do |js|
      value += "<script src='/js/" + js + ".js'></script>\n"
    end
    value
  )
}    <script src="/js/crails.js"></script>
  </body>
</html>
HTML

File.open LAYOUT_OUTPUT_PATH, 'w' do |file|
  file.write html
end

puts "\033[32m[CREATE]\033[0m " + "File #{LAYOUT_OUTPUT_PATH}" 

elsif bundle == 'kickstart'

engine = ThemeEngine.new

engine.downloaded_files "http://raw.github.com/joshuagatcke/HTML-KickStart/master/" do
  engine.javascript url: 'js/kickstart.js',             name: 'kickstart',            include?: true
  engine.css        url: 'css/kickstart-buttons.css',   name: 'kickstart-buttons',    include?: false
  engine.css        url: 'css/kickstart-forms.css',     name: 'kickstart-forms',      include?: false
  engine.css        url: 'css/kickstart-grid.css',      name: 'kickstart-grid',       include?: false
  engine.css        url: 'css/kickstart-menus.css',     name: 'kickstart-menus',      include?: false
  engine.css        url: 'css/kickstart-slideshow.css', name: 'kickstart-slideshow',  include?: false
  engine.css        url: 'css/kickstart.css',           name: 'kickstart',            include?: true
  engine.css        url: 'css/prettify.css',            name: 'prettify',             include?: false
  engine.css        url: 'css/tiptip.css',              name: 'tiptip',               include?: false
  engine.css        url: 'style.css',                   name: 'kickstart.theme',      include?: true

  engine.directory 'css/img'
  engine.directory 'css/fonts/fontawesome/css'
  engine.directory 'css/fonts/fontawesome/font'
  engine.file 'css/img/breadcrumbs-bg.gif'
  engine.file 'css/img/bx_loader.gif'
  engine.file 'css/img/controls.png'
  engine.file 'css/img/gary_jean.png'
  engine.file 'css/img/icon-arrow-right.png'
  engine.file 'css/fonts/fontawesome/css/font-awesome.min.css'
  engine.file 'css/fonts/fontawesome/css/font-awesome.css'
  engine.file 'css/fonts/fontawesome/css/font-awesome.ie7.min.css'
  engine.file 'css/fonts/fontawesome/font/fontawesome-webfont.woff'
  engine.file 'css/fonts/fontawesome/font/fontawesome-webfont.eot'
  engine.file 'css/fonts/fontawesome/font/fontawesome-webfont.ttf'
end

engine.html do |css, js|
html = <<HTML
string* yield = @yield;
// END LINKING
<!DOCTYPE html>
<html><head>
<title>#{PROJECT_NAME} - Powered by Crails Framework</title>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0"/>
<meta name="description" content="" />
<meta name="copyright" content="" />
<script src="//ajax.googleapis.com/ajax/libs/jquery/1.9.1/jquery.min.js"></script>
<script src="/js/crails.js"></script>
#{js}
#{css}
</head><body>

<!-- Menu Horizontal -->
<ul class="menu">
<li class="current"><a href="">#{PROJECT_NAME}</a></li>
<li><a href="">Item 2</a></li>
<li><a href=""><span class="icon" data-icon="R"></span>Item 3</a>
	<ul>
	<li><a href=""><span class="icon" data-icon="G"></span>Sub Item</a></li>
	<li><a href=""><span class="icon" data-icon="A"></span>Sub Item</a>
		<ul>
		<li><a href=""><span class="icon" data-icon="Z"></span>Sub Item</a></li>
		<li><a href=""><span class="icon" data-icon="k"></span>Sub Item</a></li>
		<li><a href=""><span class="icon" data-icon="J"></span>Sub Item</a></li>
		<li><a href=""><span class="icon" data-icon="="></span>Sub Item</a></li>
		</ul>
	</li>
	<li class="divider"><a href=""><span class="icon" data-icon="T"></span>li.divider</a></li>
	</ul>
</li>
<li><a href="">Item 4</a></li>
</ul>

<div class="grid">
	
<!-- ===================================== END HEADER ===================================== -->
	 
<div class="col_12">
	<div class="col_9">
	<%= (yield ? *yield : "") %>
	</div>

	<div class="col_3">
	<h5>Icon List</h5>
	<ul class="icons">
	<li><i class="icon-ok"></i> Apple</li>
	<li><i class="icon-ok"></i> Banana</li>
	<li><i class="icon-ok"></i> Orange</li>
	<li><i class="icon-ok"></i> Pear</li>
	</ul>
	
	<h5>Sample Icons</h5>
	<i class="icon-twitter-sign icon-4x"></i> 
	<i class="icon-facebook-sign icon-4x"></i>
	<i class="icon-linkedin-sign icon-4x"></i>
	<i class="icon-github-sign icon-4x"></i>
	
	<span class="icon social x-large darkgray" data-icon="1"></span>
	<span class="icon social x-large black" data-icon="w"></span>
	<span class="icon x-large pink" data-icon="*"></span>
	<span class="icon social x-large green" data-icon="v"></span>
	
	<h5>Button w/Icon</h5>
	<a class="button orange small" href="#"><i class="icon-rss"></i> RSS</a>
	</div>
	
	<hr />
	
	<div class="col_3">
	<h4>Column</h4>
	<p>Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore 
	magna aliquam erat volutpat. Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis</p>
	</div>
	
	<div class="col_3">
	<h4>Column</h4>
	<p>Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore 
	magna aliquam erat volutpat. Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis</p>
	</div>
	
	<div class="col_3">
	<h4>Column</h4>
	<p>Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore 
	magna aliquam erat volutpat. Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis</p>
	</div>
	
	<div class="col_3">
	<h4>Column</h4>
	<p>Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore 
	magna aliquam erat volutpat. Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis</p>
	</div>
</div>

</div><!-- END GRID -->

<!-- ===================================== START FOOTER ===================================== -->
<div class="clear"></div>
<div id="footer">
This website was built with <a href="http://www.99lime.com">HTML KickStart</a> over Crails Framework
</div>

</body></html>
HTML
end

engine.send command

else
  puts "Bundle #{bundle} isn't supported."
  exit 2
end

