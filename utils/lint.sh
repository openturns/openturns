#!/bin/sh

# pip install flake8 
# TODO: reduce max line length

PATH=~/.local/bin/:$PATH flake8 python/ --ignore=W503 --max-line-length=350

# check for windows line endings
! find python lib -type f | xargs file | grep CRLF

# check for typos using the linter used to check debian packages
! find lib python/ -type f | grep -E "xx$|\.py$|\.in$|\.i$|\.rst$" | grep -v bibliography.rst | xargs spellintian | grep -v duplicate | grep -v "ot -> to"
