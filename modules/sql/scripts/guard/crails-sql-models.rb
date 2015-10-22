require 'guard/crails-base'

module ::Guard
  class CrailsSqlModels < CrailsPlugin
    def run_all
      run_on_modifications watched_files
    end

    def run_on_modifications(paths)
      paths.each do |path|
        compile_model path
      end
    end

  private
    def compile_model filepath
      file_content            = (File.open filepath).read
      filename                = filepath.match(/app\/models\/(.*).h(pp)?$/)[1]
      relative_path           = filename + '.cpp'
      classname               = nil
      fields                  = []
      has_many                = []
      has_one                 = []
      belongs_to              = []
      has_and_belongs_to_many = []
      requires                = []

      # SCAN CLASS
      file_content.scan /SQL_MODEL\s*\([a-zA-Z0-9_]+\)/ do |param|
        if not classname.nil?
          puts "/!\\ #{filename} hosts several SQL_MODEL. Don't do that." if not classname.nil?
          raise [:task_has_failed]
        end
        classname  = param[10...param.length-1]
      end

      return if classname.nil?

      # SCAN REQUIRES
      file_content.scan /SQL_REQUIRE\s*\(([a-zA-Z0-9_\/]+)\)/ do |param|
        requires << param.first
      end

      # SCAN FIELDS
      file_content.scan /SQL_FIELD\s*\(([a-zA-Z0-9_:<>]+),\s*([a-zA-Z0-9_]+),(.*)\)/ do |param1, param2,param3|
        field_data = {
            type:    param1,
            name:    param2,
            default: param3
          }
        fields << field_data
      end

      template = ERB.new (File.new "#{File.dirname(__FILE__)}/templates/sql_model.cpp.erb").read, nil, '-'
      path     = Pathname.new "lib/models/#{relative_path}"

      FileUtils.mkdir_p path.dirname
      File.open path.to_s, 'w' do |file|
        file.write template.result binding
      end
    end
  end
end
