bundler_gemspec = Gem::Specification::find_all_by_name('bundler')
unless bundler_gemspec.any?
  puts "/!\\ Bundler gem not found. Please install it with `gem install bundler`."
  exit 2
end
bundler_gem = bundler_gemspec.first
bundler_bin = "#{bundler_gem.bin_dir}/#{bundler_gem.executables.first}"

arguments = $*.map {|i| "'#{i}'"}

puts "Using #{bundler_bin}"
exec "#{bundler_bin} exec guard #{arguments.join ' '}"
