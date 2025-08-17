#!/bin/bash
set -e
git checkout -b tmp
git checkout master
git merge master
git pull

