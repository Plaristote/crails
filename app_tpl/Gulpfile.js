var os              = require('os');
var gulp            = require('gulp');
var coffee          = require('gulp-coffee');
var concat          = require('gulp-concat');
var uglify          = require('gulp-uglify');
var sass            = require('gulp-sass');
var gulpif          = require('gulp-if');
var exec            = require('gulp-exec');
var child           = require('child_process');
var require_nocache = require('require-without-cache');
var assets          = require_nocache('./config/assets.json', require);

var debug            = true;
var dynlib_extension = 'so';

switch (os.platform())
{
  case 'darwin':
    dynlib_extension = 'dylib';
    break ;
  case 'linux':
    dynlib_extension = 'so';
    break ;
  case 'win32':
    dynlib_extension = 'dll';
    break ;
}

function command_compile(command)
{
  var compiler    = 'c++';
  var opts        = [ '-fPIC', '--std=c++11', '-shared', '-Wall', '-Wno-return-type-c-linkage', '-Wno-unused-private-field' ];
  if (debug)                     { opts.push('-g'); }
  if (os.platform() == 'darwin') { opts.push('-undefined dynamic_lookup'); }

  return (compiler + ' ' + opts.join(' ') + ' ' + command);
}

var paths = {
  cmake:   [ 'build/CMakeCache.txt' ],
  scripts: [ 'app/assets/javascripts/**/*.js', 'app/assets/javascripts/**/*.coffee' ],
  css:     [ 'app/assets/stylesheets/**/*.scss', 'app/assets/stylesheets/**/*.css' ],
  views:   [ 'app/views/**/*.ecpp', 'lib/*.ecpp' ],
  models:  [ 'app/models/**/*.hpp' ],
  plugins: [ 'lib/db/*.cpp' ]
}

gulp.task('cmake', function() {
  var spawn = child.spawn;
  var cmake = spawn('cat', ['build/CMakeCache.txt']);

  cmake.stdout.on('data', function(data) {
    var debug_option = ('' + data).match(/SERVER_DEBUG:BOOL=(ON|OFF)/)[1];

    debug = debug_option == 'ON';
    console.log("- Using debug mode", debug);
  });
});

gulp.task('scripts', ['clean'], function() {
  for (output_file in assets.javascripts)
  {
    var scripts_paths = [];
    var scripts;

    for (var i = 0 ; i < assets.javascripts[output_file].length ; ++i)
      scripts_paths.push('app/assets/javascripts/' + assets.javascripts[output_file][i]);
    scripts   = gulp.src(scripts_paths).pipe(gulpif(/[.]coffee$/, coffee()));
    if (debug == false)
      scripts = scripts.pipe(uglify);
    scripts.pipe(concat(output_file + '.js')).pipe(gulp.dest('./public/assets'));
  }
});

gulp.task('css', ['clean'], function() {
  for (output_file in assets.stylesheets)
  {
    var scripts_paths = [];
    var scripts;

    for (var i = 0 ; i < assets.stylesheets[output_file].length ; ++i)
      scripts_paths.push('app/assets/stylesheets/' + assets.stylesheets[output_file][i]);
    scripts   = gulp.src(scripts_paths).pipe(gulpif(/[.]scss$/, sass({ includePaths: paths.css })));
    scripts.pipe(concat(output_file + '.css')).pipe(gulp.dest('./public/assets'));
  }
});

gulp.task('assets.json', ['clean'], function() {
  assets          = require_nocache('./config/assets.json', require);
});

gulp.task('views', ['clean'], function() {
  var compile_erb = gulp.src(paths.views).pipe(exec('crails compile_view <%= file.path %>'), { continueOnError: false, pipeStdout: false }).pipe(exec.reporter({ err: true, stderr: true, stdout: true }));
  var compile_cpp = compile_erb.pipe(exec(command_compile('-I./app <%= file.path %>.cpp -o <%= file.path %>.' + dynlib_extension)), { continueOnError: false, pipeStdout: false }).pipe(exec.reporter({ err: true, stderr: true, stdout: true }));
  return (compile_cpp);
});

gulp.task('plugins', ['clean'], function() {
  return (gulp.src(paths.plugins).pipe(exec(command_compile('<%= file.path %>.cpp -o <%= file.path %>.' + dynlib_extension)), { continueOnError: false, pipeStdout: false }).pipe(exec.reporter({ err: true, stderr: true, stdout: true })));
});

gulp.task('models', ['clean'], function() {
  var compile_model = gulp.src(paths.models).pipe(exec('crails compile_model <%= file.path %> ; echo <%= file.path %> >> bite'), { continueOnError: false, pipeStdout: false }).pipe(exec.reporter({ err: true, stderr: true, stdout: true }));
  return (compile_model);
});

gulp.task('clean', function() {
});

gulp.task('watch', function() {
  gulp.watch(['config/assets.json'], ['assets.json', 'scripts', 'css']);
  gulp.watch(paths.cmake,   ['cmake']);
  gulp.watch(paths.scripts, ['scripts']);
  gulp.watch(paths.css,     ['css']);
  gulp.watch(paths.views,   ['views']);
  gulp.watch(paths.models,  ['models']);
  gulp.watch(paths.plugins, ['plugins']);
});

gulp.task('default', ['watch', 'cmake', 'scripts', 'css', 'views', 'models', 'plugins']);
