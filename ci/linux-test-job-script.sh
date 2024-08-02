#!/usr/bin/env bash

set -e
set -o pipefail
set -u
set -x

ci/install-packages.sh
export GV_VERSION=$( cat GRAPHVIZ_VERSION )
python3 -m pytest --strict-markers --verbose --junitxml=report.xml ci/tests.py tests
