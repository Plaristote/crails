require 'guard'

Guard.setup

puts "Running guard groups #{ARGV.join ', '}"

ARGV.each do |arg|
  guards = Guard.state.session.plugins.all.select {|g| g.group.name == arg.to_sym}
  puts "Found #{guards.count} plugins"
  guards.each(&:run_all)
end
