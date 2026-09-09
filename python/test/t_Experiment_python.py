#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott


class RandomExp:
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
assert experiment == ot.Experiment(experiment)


class NoGenerate:
    pass


with ott.assert_raises(Exception):
    ot.Experiment(NoGenerate())


class BadGenerate:
    def generate(self):
        return 42


with ott.assert_raises(Exception):
    ot.Experiment(BadGenerate()).generate()
