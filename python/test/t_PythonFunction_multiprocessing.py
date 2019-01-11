#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import numpy as np
from time import time, sleep
import multiprocessing
n_cpus = multiprocessing.cpu_count()
 
ot.TESTPREAMBLE()

nvar = 100
ng   = 1 # Gb

nsim = int(ng*1e9/nvar/8) # double precision
rho  = 0.00005 # rho s/ point evaluation
T    = rho*nsim


def myfunPython(X):
    from time import time
    XArray = np.array(X, copy=False)
    res = 1 - XArray[:,0]
    sleep(int(rho * len(X)))
    return  res.reshape(len(XArray), 1)  

def createFunction():
    return ot.PythonFunction(nvar, 1, func_sample=myfunPython, n_cpus=n_cpus)    

try:
    myfun = createFunction()
    sample = np.linspace(1.0 / nsim, 1.0, num=nsim)
    input = np.repeat(np.expand_dims(sample, axis=1), myfun.getInputDimension(), axis=1)
    t0 = time()
    out = myfun(input)
    t1 = time()
    
    print("T sequential = ", int(T), "(s)")
    print("Speed factor/n_cpus ~ 1 : ",round(T /(t1 - t0)/n_cpus)  )
# except TestFailed, ex :
except:
    import sys
    print("t_PythonFunction_multiprocessing.py",
          sys.exc_info()[0], sys.exc_info()[1])
    
