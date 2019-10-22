#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import numpy as np
import multiprocessing
n_cpus = multiprocessing.cpu_count()

ot.TESTPREAMBLE()

nvar = 100
nsim = 10000


def myfunPython(X):
    XArray = np.array(X, copy=False)
    res = 1 - XArray[:, 0]
    return res.reshape(len(XArray), 1)


def createFunction():
    return ot.PythonFunction(nvar, 1, func_sample=myfunPython, n_cpus=n_cpus)


myfun = createFunction()
sample = np.linspace(1.0 / nsim, 1.0, num=nsim)
input = np.repeat(np.expand_dims(sample, axis=1),
                  myfun.getInputDimension(), axis=1)
out = myfun(input)
# sum = 4999
print("sum =", int(np.sum(out)))
