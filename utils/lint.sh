#!/bin/sh

# pip install flake8 
# TODO: reduce max line length

PATH=~/.local/bin/:$PATH flake8 python/ --ignore=W503 --max-line-length=400
