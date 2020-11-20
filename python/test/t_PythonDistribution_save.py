#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import math as m
import dill

# ensures python code is included
dill.settings['recurse'] = True

ot.TESTPREAMBLE()

class UniformNdPy(ot.PythonDistribution):

    def __init__(self, a=[-1.0], b=[1.0]):
        # https://github.com/uqfoundation/dill/issues/300
        #super(UniformNdPy, self).__init__(len(a))
        ot.PythonDistribution.__init__(self, len(a))
        if len(a) != len(b):
            raise ValueError('Invalid bounds')
        for i in range(len(a)):
            if a[i] > b[i]:
                raise ValueError('Invalid bounds')
        self.a = a
        self.b = b
        self.factor = 1.0
        for i in range(len(a)):
            self.factor *= (b[i] - a[i])

    def getRange(self):
        return ot.Interval(self.a, self.b, [True] * len(self.a), [True] * len(self.a))

    def getRealization(self):
        X = []
        for i in range(len(self.a)):
            X.append(
                self.a[i] + (self.b[i] - self.a[i]) * ot.RandomGenerator.Generate())
        return X

    def getSample(self, size):
        X = []
        for i in range(size):
            X.append(self.getRealization())
        return X

    def computeCDF(self, X):
        prod = 1.0
        for i in range(len(self.a)):
            if X[i] < self.a[i]:
                return 0.0
            prod *= (min(self.b[i], X[i]) - self.a[i])
        return prod / self.factor

    def computePDF(self, X):
        for i in range(len(self.a)):
            if X[i] < self.a[i]:
                return 0.0
            if X[i] > self.b[i]:
                return 0.0
        return 1.0 / self.factor

    def getMean(self):
        mu = []
        for i in range(len(self.a)):
            mu.append(0.5 * (self.a[i] + self.b[i]))
        return mu

d = ot.Distribution(UniformNdPy())
print(d.getKurtosis())
print(d.computePDF([0.5]))
print(d.computeCDF([0.5]))

# save
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager('pyd.xml'))
study.add('d', d)
study.save()
