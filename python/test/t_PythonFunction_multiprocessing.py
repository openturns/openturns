#! /usr/bin/env python

import openturns as ot
import multiprocessing


ot.TESTPREAMBLE()


def myfunPython(x):
    n = x[0]
    y = [1.0 / (2.0**n)]
    return y


if __name__ == "__main__":

    n_cpus = multiprocessing.cpu_count()
    nsim = 1000
    myfun = ot.PythonFunction(1, 1, myfunPython, n_cpus=n_cpus)

    X = [[i] for i in range(nsim)]
    Y = myfun(X)
    sum_y = nsim * Y.computeMean()[0]
    assert sum_y == 2.0, "sum 2^-n = 2"
