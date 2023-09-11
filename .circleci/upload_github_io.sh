#!/bin/sh
# upload doc to openturns.github.io

set -xe

# early exit on PRs
if test -n "${CIRCLE_PULL_REQUEST}" -o "${CIRCLE_PROJECT_USERNAME}" != "openturns"
then
  exit 0
fi

git clone --depth 1 https://${GH_TOKEN}@github.com/openturns/openturns.github.io.git
if test -n "${CIRCLE_TAG}"
then
  CIRCLE_BRANCH="${CIRCLE_TAG:1}"
fi
mkdir -p openturns.github.io/${CIRCLE_PROJECT_REPONAME}/${CIRCLE_BRANCH}
cp -r  ~/.local/share/doc/${CIRCLE_PROJECT_REPONAME}/html/* openturns.github.io/${CIRCLE_PROJECT_REPONAME}/${CIRCLE_BRANCH}
cd openturns.github.io
touch .nojekyll
git config user.email "sayhi@circleci.com"
git config user.name "CircleCI"
git add -A .
git commit -a -m "CircleCI build ${CIRCLE_PROJECT_REPONAME} ${CIRCLE_BUILD_NUM}" || exit 0
git push --quiet origin master > /dev/null 2>&1
