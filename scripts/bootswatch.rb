 
require 'net/http'

theme_name = ARGV[0]

path       = if ARGV[1].nil?
               'public/css/bootstrap.min.css'
             else
               ARGV[1]
             end

url        = "http://bootswatch.com/#{theme_name}/bootstrap.min.css"
url        = URI.parse url

request    = Net::HTTP::Get.new url.path
response   = Net::HTTP.start url.host, url.port do |http|
  http.request request
end

File.open path, 'w' do |file|
  file.write response.body
end

puts "\033[32m[CREATE]\033[0m " + "File #{path}"