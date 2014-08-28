var gulp   = require('gulp');
var coffee = require('gulp-coffee');
var concat = require('gulp-concat');
var uglify = require('gulp-uglify');
var sass   = require('gulp-sass');
var gulpif = require('gulp-if');
var exec   = require('gulp-exec');
var child  = require('child_process');

var debug  = true;

var paths = {
  cmake:   [ 'build/CMakeCache.txt' ],
  scripts: [ 'app/assets/javascripts/**/*.js', 'app/assets/javascripts/**/*.coffee' ],
  css:     [ 'app/assets/stylesheets/**/*.scss', 'app/assets/stylesheets/**/*.css' ],
  ecpp:    [ 'app/views/**/*.ecpp', 'lib/*.ecpp' ]
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
  var scripts = gulp.src(paths.scripts).pipe(gulpif(/[.]coffee$/, coffee()));
  if (debug != true)
    scripts = scripts.pipe(uglify());
  return (scripts.pipe(concat('application.js')).pipe(gulp.dest('./public/assets')));
});

gulp.task('css', ['clean'], function() {
  return (gulp.src(paths.css).pipe(gulpif(/[.].scss$/, sass({ includePaths: paths.css }))).pipe(concat('application.css')).pipe(gulp.dest('./public/assets')));
});

gulp.task('ecpp', ['clean'], function() {
  var compiler    = 'c++';
  var opts        = [ '-Wall', '-fPIC', '--std=c++11', '-shared' ]
  if (debug) { opts.push('-g'); }
  var compile_erb = gulp.src(paths.ecpp).pipe(exec('crails compile_view <%= file.path %>'), { continueOnError: false, pipeStdout: false }).pipe(exec.reporter({ err: true, stderr: true, stdout: true }));
  var compile_cpp = compile_erb.pipe(exec(compiler + ' ' + opts.join(' ') + ' -I./app <%= file.path %>.cpp -o <%= file.path %>.so'), { continueOnError: false, pipeStdout: false }).pipe(exec.reporter({ err: true, stderr: true, stdout: true }));
  return (compile_cpp);
});

gulp.task('clean', function() {
});

gulp.task('watch', function() {
  gulp.watch(paths.cmake,   ['cmake']);
  gulp.watch(paths.scripts, ['scripts']);
  gulp.watch(paths.css,     ['css']);
  gulp.watch(paths.ecpp,    ['ecpp']);
});

gulp.task('default', ['watch', 'cmake', 'scripts', 'css', 'ecpp']);
