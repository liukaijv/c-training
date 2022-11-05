#!/usr/bin/env bash

project_name=$1

if [ -z "$project_name" ];then
    echo "project_name required!"
    exit 0
fi

cd ..

if [ ! -d "$project_name" ];then
    echo "project: [$project_name] not exist!"
    exit 0
fi

cmake \
 -D CMAKE_MAKE_PROGRAM:PATH="D:/Program Files/cmake-3.24.0-rc1-windows-x86_64/bin/cmake" \
 -D CMAKE_BUILD_TYPE=Debug \
 -G "MinGW Makefiles" $project_name