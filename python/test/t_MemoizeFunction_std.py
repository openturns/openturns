#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    f = SymbolicFunction("x", "x^2")
    f = MemoizeFunction(f)
    f.disableHistory()
    print(f)
    size = 4
    inputSample = Sample(size, 1)
    for i in range(size):
        inputSample[i, 0] = i
    outputSample = f(inputSample)
    print("Is history enabled for f? ", f.isHistoryEnabled())
    print("input history=", f.getInputHistory())
    print("output history=", f.getOutputHistory())
    f.enableHistory()
    outputSample = f(inputSample)
    print("Is history enabled for f? ", f.isHistoryEnabled())
    print("input history=", f.getInputHistory())
    print("output history=", f.getOutputHistory())
    f.clearHistory()
    print("Is history enabled for f? ", f.isHistoryEnabled())
    print("input history=", f.getInputHistory())
    print("output history=", f.getOutputHistory())
    # Perform the computation twice
    outputSample = f(inputSample)
    outputSample = f(inputSample)
    print("input history=", f.getInputHistory())
    print("output history=", f.getOutputHistory())

except:
    import sys
    print("t_Function_history.py",
          sys.exc_info()[0], sys.exc_info()[1])
