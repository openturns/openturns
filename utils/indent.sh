#!/bin/sh

# indent code, requires astyle and black:
# sudo apt install astyle
# pip install black

astyle --suffix=none --style=ansi --recursive --indent=spaces=2 --indent-switches --pad-oper --break-closing-brackets --max-instatement-indent=40 lib/*.?xx python/*.?xx

PATH=~/.local/bin/:$PATH black python/
