#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

mesh = ot.RegularGrid(0, 1, 100)


def g(X):
    a, b = X
    Y = [[a * t + b] for t in range(100)]
    return Y


f = ot.PythonPointToFieldFunction(2, mesh, 1, g)
x = [4, 5]
y = f(x)
print(y[0])
assert y[0] == [5.0], "wrong y value"

# save
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("pyp2ff.xml"))
study.add("f", f)
study.save()
