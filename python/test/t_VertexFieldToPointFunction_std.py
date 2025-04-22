#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

N = 10
mesh = ot.RegularGrid(0, 1, N)
f = otexp.VertexFieldToPointFunction(mesh, 2, [-1 % N])

print(f)
print(repr(f))

assert f.getInputDimension() == 2
assert f.getOutputDimension() == 2
assert f == f

x = [[4 + i * 0.1, 3] for i in range(N)]
y = f(x)

print(y)
ott.assert_almost_equal(y, [4.9, 3])

# try composition via connection
g = ot.SymbolicFunction(['t', 'x'], ['x + 0.1 * t', '2'])
p2f = ot.VertexValuePointToFieldFunction(g, mesh)
p2pc = ot.Function(ot.PointToPointConnection(f, p2f))
x = [4.0]
y = p2pc(x)
print(y)
ott.assert_almost_equal(y, [4.9, 2])
