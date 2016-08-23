#!/bin/bash

cur_dir=`dirname "$0"`; cur_dir=`eval "cd \"$cur_dir\" && pwd"`
build_path=$cur_dir"/Build"
install_path=$cur_dir"/bin"

# build
mkdir $build_path
cd $build_path
cmake -G "Unix Makefiles" ../ -DCMAKE_INSTALL_PREFIX=$install_path -DCMAKE_BUILD_TYPE=Release
make
make install

echo ""
echo "done"
read x
