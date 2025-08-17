#!/bin/sh
PWD="`dirname \"$0\"`"

cp -v $PWD/init/.travis.yml .
git add .travis.yml

cp -v $PWD/init/appveyor.yml .
git add appveyor.yml

cp -v $PWD/init/.clang-format .
git add .clang-format

cp -v $PWD/init/.gitignore .
git add .gitignore

cp -v $PWD/init/cmaki.yml .
git add cmaki.yml

