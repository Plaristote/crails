def run_command cmd
  system cmd
  $?.exitstatus
end

def find_bundler
  bundler_gemspec = Gem::Specification::find_all_by_name('bundler')
  system_bundler  = `which bundle 2> /dev/null`
  if bundler_gemspec.any?
    bundler_gem = bundler_gemspec.first
    "#{bundler_gem.bin_dir}/#{bundler_gem.executables.first}"
  elsif system_bundler != ""
    system_bundler.split("\n").first
  else
    nil
  end
end

def bundle_install
  bundler_bin = find_bundler

  if bundler_bin.nil?
    puts "/!\\ Bundler gem not found. Please install it with `gem install bundler`."
    exit 2
  end

  puts "+ #{bundler_bin} install"
  run_command "#{bundler_bin} install"
end

def run_using_bundler cmd
  bundler_bin = find_bundler

  if bundler_bin.nil?
    puts "/!\\ Bundler gem not found. Please install it with `gem install bundler`."
    exit 2
  end

  puts "[crails] Using ruby bundler #{bundler_bin}"

  bundle_cmd = "#{bundler_bin} install"

  unless ENV['CRAILS_RUBY_BUNDLE_PATH'].nil?
    config_cmd = "#{bundler_bin} config set path #{ENV['CRAILS_RUBY_BUNDLE_PATH']}"
    run_command config_cmd
    bundle_cmd += " --path #{ENV['CRAILS_RUBY_BUNDLE_PATH']}"
  end

  cmd = "#{bundler_bin} exec #{cmd}"

  puts `echo "+ bundle check" && #{bundler_bin} check`
  cmd = "#{bundle_cmd} && #{cmd}" unless $?.success?
  run_command cmd
end
