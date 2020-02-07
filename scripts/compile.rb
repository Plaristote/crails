$: << ENV['CRAILS_SHARED_DIR']

require 'guard'
require 'guard/crails-base'

Guard.setup

groups = ARGV
groups = ['before_compile', 'compile'] if ARGV.length == 0

puts "[guard] Running guard tasks: #{groups.join ', '}"

begin
  groups.each do |arg|
    guards = Guard.state.session.plugins.all.select do |g|
      (not g.group.nil? and g.group.name.to_sym == arg.to_sym) ||
      g.name.to_sym == arg.to_sym
    end
    puts "[guard] Found #{guards.count} plugins"
    guards.each do |item|
      puts "[guard] Running plugin #{item.name}"
      item.start if item.methods.include? :start
      item.run_all
      item.stop  if item.methods.include? :stop
    end
  end
rescue Exception => e
  puts "crails/compile: caught exception: #{e.message}"
  exit 255
end

exit Guard::CrailsPlugin.exit_success if Guard::CrailsPlugin.exit_success != nil
