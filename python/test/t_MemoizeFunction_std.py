#! /usr/bin/env python

from __future__ import print_function
import openturns as ot


f = ot.SymbolicFunction("x", "x^2")
f = ot.MemoizeFunction(f)
f.disableHistory()
print(f)
size = 4
inputSample = ot.Sample(size, 1)
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

mem = ot.MemoizeFunction(ot.SymbolicFunction(
    ['x1', 'x2'], ['x1+x2', '3*(x1+x2)']))
mem([1, 2])
mem2 = ot.MemoizeFunction(mem.getMarginal(1))
mem2([1, 2])
print('mem2.in', mem2.getCacheInput())
print('mem2.out', mem2.getCacheOutput())
print('mem2.hits', mem2.getCacheHits())

# check getCacheInput/getCacheOutput order
for i in range(5):
    mem([i+2, i+3])
assert mem(mem.getCacheInput()) == mem.getCacheOutput(), "wrong order"
print('ok')
