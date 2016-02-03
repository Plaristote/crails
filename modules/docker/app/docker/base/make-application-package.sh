#!/bin/sh -x

app="/home/crails/crails-app"
target="/home/crails/package"
install_path="/usr/local"
build_path="$app/heroku-build"

target_libs="${target}/lib"
target_builds="${target}/bin"
target_public="${target}/public"

rm -Rf "$target"/*
mkdir -p "$target_libs" "$target_builds" "$target_public"

# Builds the application
mkdir -p "$build_path"
cd "$build_path"
ruby /usr/local/share/crails compile assets before_compile
cmake .. -DDEVELOPER_MODE=OFF && make

# Copy the dependencies to the lib directory
dependencies=`ldd server | grep /usr/local | cut -d' ' -f3`
for dependency in $dependencies ; do
  cp "$dependency" "$target_libs"
done

# Copy the server and the tasks to the bin directory
cp "$app/heroku-build/server" "$app/heroku-build/libcrails-app.so" "$target_builds"

for task in `ls "$app/heroku-build/tasks"` ; do
  mkdir -p "$target_builds/tasks/$task"
  cp $app/heroku-build/tasks/$task/task $target_builds/tasks/$task/
done

# Copy the SQL files if there are any
mkdir -p $target/app/models
cp $app/app/models/*.sql $target/app/models

# Copy the config files
mkdir -p "$target/config"
cp -R "$app"/config/*.json "$target/config"

# Copy the assets
cp -R "$app/public" "$target"

# Copy the import folder
cp -R "$app/import" "${target}" # WARNING this is specific to eduplan

# Generate a Procfile
echo 'web: bin/server -p $PORT -h 0.0.0.0' > "$target/Procfile"

# Make sure the app assets are accessible to everyone
chmod -R ugo+rw "$target"
chmod ugo+x "$target/bin/server" "$target"/bin/tasks/**/task
