#!/usr/bin/env bash

project_name=$1

if [ -z "$project_name" ];then
    echo "project_name required!"
    exit 0
fi

cd ..

if [ -d "$project_name" ];then
    echo "project: [$project_name] exist!"
    exit 0
fi

mkdir "$project_name"

cp scripts/CMakeLists.txt $project_name
cp scripts/main.c $project_name
cp scripts/README.md $project_name

sed -i "s/_project_/${project_name}/" ${project_name}/CMakeLists.txt
sed -i "s/_project_/${project_name}/" ${project_name}/README.md

