module ::Guard
  class CrailsJs < Plugin
    def initialize options = {}
      options[:watchers] << ::Guard::Watcher.new(%r{^#{options[:input]}/(.+\.(js|coffee))$})
      super options
    end

    def run_all
      options[:targets].each do |target|
        text        = compile_file target
        output_file = options[:output] + '/' + target
        File.open(output_file, 'w') { |f| f.write text }
        puts ">> Generated #{output_file}"
      end
    end

    def run_on_modifications(paths)
      run_all
    end

  private
    def compile_file file
      current_dir       = file.split('/')[0...-1].join('/')
      file_content      = ''
      comment_character = "//"
      File.open (options[:input] + '/' + file), 'r' do |f|
        f.each_line do |line|
          matches = line.match /^#{comment_character}=\s+(require|include)\s+(.+)$/
          if matches.nil?
            file_content += line
          else
            required_filename = current_dir + '/' + matches[2]
            file_content     += (compile_file required_filename) + "\n"
          end
        end
      end
      file_content
    end
  end
end
