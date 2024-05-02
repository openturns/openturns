#!/bin/sh

set -e

# pip install flake8 doc8 
# TODO: reduce max line length

PATH=~/.local/bin/:$PATH flake8 python/ --ignore=W503,E203 --max-line-length=185

PATH=~/.local/bin/:$PATH doc8 python/doc/ --ignore D000 --ignore-path python/doc/theory/meta_modeling/orthogonal_polynomials.rst --ignore-path python/doc/theory/reliability_sensitivity/sensitivity_sobol.rst --max-line-length 185

# check for windows line endings
! find python lib -type f | xargs file | grep CRLF

# check for typos using the linter used to check debian packages
! find lib python/ -type f | grep -E "xx$|\.py$|\.in$|\.i$|\.rst$" | grep -v bibliography.rst | xargs spellintian | grep -v duplicate | grep -v "ot -> to"
