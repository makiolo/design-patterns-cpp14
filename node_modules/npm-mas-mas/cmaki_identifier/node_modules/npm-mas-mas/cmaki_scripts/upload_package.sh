#!/bin/bash

set -e

# upload package
conan upload '*' -r npm-mas-mas --all -c

