#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

N = 10
mesh = ot.RegularGrid(0, 1, N)
h = ot.SymbolicFunction(['t', 'a', 'b'], ['a * t + b + 1'])
f2p = ot.VertexValuePointToFieldFunction(h, mesh)
f = otexp.VertexFunction(f2p, [-1 % N])

print(f)
print(repr(f))

assert f.getInputDimension() == 2
assert f.getOutputDimension() == 1
assert f == f

x = [4, 5]
y = f(x)

print(y)
ott.assert_almost_equal(y, f2p(x)[-1])
