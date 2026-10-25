#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import os


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


class FailingGenerate:
    def generate(self):
        raise RuntimeError("cannot generate")


with ott.assert_raises(Exception):
    ot.Experiment(FailingGenerate()).generate()

assert "PythonExperiment" in repr(experiment)

# save/load
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("pyexp.xml"))
study.add("experiment", experiment)
study.save()
loaded = ot.Experiment()
reloader = ot.Study()
reloader.setStorageManager(ot.XMLStorageManager("pyexp.xml"))
reloader.load()
reloader.fillObject("experiment", loaded)
assert loaded.generate().getSize() == 10
os.remove("pyexp.xml")


# Minimal duck-typed interface: generate() alone suffices, see issue #3193
class MinimalExp:
    def generate(self):
        return ot.Sample(5, 2)


minimal = ot.Experiment(MinimalExp())
assert minimal.generate().getSize() == 5
# weights fall back to uniform without generateWithWeights
minimalWeights = ot.Point()
minimal.generateWithWeights(minimalWeights)
assert minimalWeights.getSize() == 5
ott.assert_almost_equal(minimalWeights, [0.2] * 5)
