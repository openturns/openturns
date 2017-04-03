#!/bin/sh

# indent some code, requires python, astyle and autopep8:
# cd /tmp && git clone https://github.com/hhatto/autopep8.git && cd autopep8 && ./setup.py install --user

astyle --suffix=none --style=ansi --recursive --indent=spaces=2 --indent-switches --pad-oper --break-closing-brackets --max-instatement-indent=40 lib/*.?xx
astyle --suffix=none --style=ansi             --indent=spaces=2 --indent-switches --pad-oper --break-closing-brackets --max-instatement-indent=40 python/src/*.?xx

# python
/usr/share/doc/python2.7/examples/Tools/scripts/reindent.py --verbose --recurse python/
autopep8 --verbose --in-place --recursive python/
