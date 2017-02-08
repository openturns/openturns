#!/bin/sh
# upload doc to openturns.github.io

set -e

git clone https://${GH_TOKEN}@github.com/openturns/openturns.github.io.github
cp -r $1/* openturns.github.io/
cd openturns.github.io
touch .nojekyll
git config user.email "sayhi@circleci.com"
git config user.name "CircleCI"
git add -A .
git commit -a -m "CircleCI build ${CIRCLE_BUILD_NUM}"
git push --quiet origin master > /dev/null 2>&1
