#! /usr/bin/env python

import openturns as ot
import math as m
import dill

# ensures python code is included
dill.settings["recurse"] = True

ot.TESTPREAMBLE()


def g(X):
    a, b = X
    Y = [a * m.sin(b)]
    return Y


f = ot.PythonFunction(2, 1, g)
x = [4, 5]
print(f(x))

# save
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("pyf.xml"))
study.add("f", f)
study.save()
