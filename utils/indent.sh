#!/bin/sh

# indent some code, requires python, astyle and autopep8:
# cd /tmp && git clone https://github.com/hhatto/autopep8.git && cd autopep8 && ./setup.py install --user

astyle --style=ansi --recursive --indent=spaces=2 --indent-switches --pad-oper --break-closing-brackets lib/*.?xx
astyle --style=ansi             --indent=spaces=2 --indent-switches --pad-oper --break-closing-brackets python/src/*.?xx

# python
/usr/share/doc/python2.7/examples/Tools/scripts/reindent.py --verbose --recurse python/
~/.local/bin/autopep8 --verbose --in-place --recursive python/
