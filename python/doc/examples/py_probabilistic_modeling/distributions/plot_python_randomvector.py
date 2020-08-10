# ---
# jupyter:
#   jupytext:
#     text_representation:
#       extension: .py
#       format_name: percent
#       format_version: '1.3'
#       jupytext_version: 1.5.1
#   kernelspec:
#     display_name: Python 3
#     language: python
#     name: python3
# ---

# %%
"""
Creation of a custom random vector
==================================
"""
# %% 

# %%
# In this example we are going to create a distribution or copula.
#
# The way to go is inheriting the PythonRandomVector class and overload its methods:
#
# - getRealization
# - getSample
# - getMean
# - getCovariance

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
import math as m


# %%
# Inherit PythonRandomVector
class RVEC(ot.PythonRandomVector):

    def __init__(self):
        super(RVEC, self).__init__(2)
        self.setDescription(['R', 'S'])

    def getRealization(self):
        X = [ot.RandomGenerator.Generate(), 2.0 + ot.RandomGenerator.Generate()]
        return X

    def getSample(self, size):
        X = []
        for i in range(size):
            X.append(
                [ot.RandomGenerator.Generate(), 2.0 + ot.RandomGenerator.Generate()])
        return X

    def getMean(self):
        return [0.5, 2.5]

    def getCovariance(self):
        return [[1.0, 0.0], [0.0, 1.0]]


# %%
# Instanciate our distribution
randomVector = ot.RandomVector(RVEC())

# %%
# Get a sample
randomVector.getSample(5)

# %%
# Get mean
randomVector.getMean()

# %%
# Compute the probability contained in an interval
randomVector.getCovariance()
