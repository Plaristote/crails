vars = {}

case ARGV[0]
when 'production'
  vars['DEVELOPER_MODE:BOOL'] = 'OFF'
when 'development'
  vars['DEVELOPER_MODE:BOOL'] = 'ON'
when 'multithread'
  vars['USE_MULTITHREAD:BOOL'] = 'ON'
when 'synchronous'
  vars['USE_MULTITHREAD:BOOL'] = 'OFF'
else
  puts "Unknown environment #{ARGV[0]}"
  exit 1
end

cmakecache = 'build/CMakeCache.txt'

vars.each do |key,value|
  `sed -e s/#{key}=.*/#{key}=#{value}/ '#{cmakecache}' > .crails-set-env.tmp`
  `mv .crails-set-env.tmp '#{cmakecache}'`
end
