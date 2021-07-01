#!/bin/sh

# indent some code, requires python, astyle and autopep8:
# sudo apt install python3-examples astyle python-autopep8

astyle --suffix=none --style=ansi --recursive --indent=spaces=2 --indent-switches --pad-oper --break-closing-brackets --max-instatement-indent=40 lib/*.?xx python/*.?xx

# python
/usr/share/doc/python3.8/examples/scripts/reindent.py --verbose --recurse python/
autopep8 --verbose --in-place --recursive python/
