$: << ENV['CRAILS_SHARED_DIR']

require 'guard'
require 'guard/crails-base'

Guard.setup

puts "Running guard groups #{ARGV.join ', '}"

begin
  ARGV.each do |arg|
    guards = Guard.state.session.plugins.all.select {|g| g.group.name == arg.to_sym}
    puts "Found #{guards.count} plugins"
    guards.each do |item|
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
