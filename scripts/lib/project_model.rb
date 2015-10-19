require 'fileutils'
require 'erb'

class ProjectModel
  include FileUtils
  
  def base_directory template_path, path, &block
    @template_path = template_path
    directory path do block.call end
  end

  def directory path, &block
    makedir path
    cd path.to_s, verbose: false do
      block.call
    end
  end

  def file name, source = nil
    action = "CREATE"
    if File.exists? name
      print "\033[35m[?]\033[0m File #{name} exists. Overwrite ? [y/n] "
      answer = STDIN.gets.chomp
      action = if answer =~ /^y(es){0,1}$/
        "REPLACE"
      else
        "EXISTS"
      end
    end
    if action != "EXISTS"
      source = name if source.nil?
      cp "#{@template_path}/#{source}", name.to_s
    end
    puts "\033[32m[#{action}]\033[0m " + "File #{name}"
  end
  
  def generate_erb target, source, options
    path = "#{@template_path}/#{source}"
    tpl  = ERB.new (File.new path).read, nil, '-'
    File.open target, 'w' do | f |
      f.write (tpl.result binding)
    end
    puts "\033[32m[GENERATED]\033[0m " + "File #{target}"
  end

private
  def makedir path
    begin
      mkdir path.to_s
      puts "\033[32m[CREATE]\033[0m " + "Directory #{path}"
    rescue Errno::EEXIST
      puts "\033[37m[EXISTS]\033[0m " + "Directory #{path}"
    end
  end
end

