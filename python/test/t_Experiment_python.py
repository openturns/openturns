#! /usr/bin/env python

from __future__ import print_function
import openturns as ot


class RandomExp(object):

    def __init__(self, size, dim):
        self.size = size
        self.dim = dim

    def generate(self):
        res = ot.Sample(self.size, self.dim)
        for i in range(self.size):
            for j in range(self.dim):
                res[i, j] = ot.RandomGenerator.Generate()
        return res


pyexp = RandomExp(10, 2)
experiment = ot.Experiment(pyexp)
print(experiment)
sample = experiment.generate()
print(sample)
