#!/bin/sh

# pip install flake8 
# TODO: reduce max line length

PATH=~/.local/bin/:$PATH flake8 python/ --ignore=W503 --max-line-length=350

# check for windows line endings
find python lib -type f | xargs file | grep CRLF && exit 1 || exit 0
