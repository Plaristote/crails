vars = {}

case ARGV[0]
when 'production'
  vars['DEVELOPER_MODE:BOOLEAN'] = 'OFF'
when 'development'
  vars['DEVELOPER_MODE:BOOLEAN'] = 'ON'
else
  puts "Unknown environment #{ARGV[0]}"
  exit 1
end

cmakecache = 'build/CMakeCache.txt'

vars.each do |key,value|
  `sed -e s/#{key}=.*/#{key}=#{value} '#{cmakecache}' > .crails-set-env.tmp`
  `mv .crails-set-env.tmp '#{cmakecache}'`
end
