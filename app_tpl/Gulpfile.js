var gulp            = require('gulp');
var coffee          = require('gulp-coffee');
var concat          = require('gulp-concat');
var uglify          = require('gulp-uglify');
var sass            = require('gulp-sass');
var gulpif          = require('gulp-if');
var require_nocache = require('require-without-cache');
var assets          = require_nocache('./config/assets.json', require);

var paths = {
  scripts: [ 'app/assets/javascripts/**/*.js', 'app/assets/javascripts/**/*.coffee' ],
  css:     [ 'app/assets/stylesheets/**/*.scss', 'app/assets/stylesheets/**/*.css' ]
}

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

gulp.task('watch', function() {
  gulp.watch(['config/assets.json'], ['assets.json', 'scripts', 'css']);
  gulp.watch(paths.scripts, ['scripts']);
  gulp.watch(paths.css,     ['css']);
});

gulp.task('build',   ['scripts', 'css']);
gulp.task('default', ['watch', 'build']);

