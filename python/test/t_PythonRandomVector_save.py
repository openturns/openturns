#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

class RVEC(ot.PythonRandomVector):

    def __init__(self):
        # https://github.com/uqfoundation/dill/issues/300
        #super(RVEC, self).__init__(2)
        ot.PythonRandomVector.__init__(self, 2)
        self.setDescription(['R', 'S'])

    def getRealization(self):
        X = [ot.RandomGenerator.Generate(), 2 + ot.RandomGenerator.Generate()]
        return X

    def getSample(self, size):
        X = []
        for i in range(size):
            X.append(
                [ot.RandomGenerator.Generate(), 2 + ot.RandomGenerator.Generate()])
        return X

    def getMean(self):
        return [0.5, 2.5]


rv = ot.RandomVector(RVEC())
print(rv)
print(rv.getRealization())
print(rv.getMean())

# save
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager('pyrv.xml'))
study.add('rv', rv)
study.save()
