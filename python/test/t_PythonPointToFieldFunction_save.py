#! /usr/bin/env python

import openturns as ot
import math as m
import dill

# ensures python code is included
dill.settings['recurse'] = True

ot.TESTPREAMBLE()

mesh = ot.RegularGrid(0, 1, 100)


def g(X):
    a, b = X
    Y = [[a*m.sin(t)+b] for t in range(100)]
    return Y


f = ot.PythonPointToFieldFunction(2, mesh, 1, g)
x = [4, 5]
print(f(x))

# save
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager('pyp2ff.xml'))
study.add('f', f)
study.save()
