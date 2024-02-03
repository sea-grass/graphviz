#!/usr/bin/env bash

# echo the release artifacts, for debugging failures in deploy.py

set -e
set -o pipefail
set -u
set -x

echo "Release artifacts:"
find Packages graphviz-*.tar.* -type f
echo -n "Total files: "
find Packages graphviz-*.tar.* -type f | wc -l
