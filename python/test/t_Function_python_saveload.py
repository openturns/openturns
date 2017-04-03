#! /usr/bin/env python

from __future__ import print_function
import os
import openturns as ot

ot.TESTPREAMBLE()


class FUNC(ot.OpenTURNSPythonFunction):

    def __init__(self):
        ot.OpenTURNSPythonFunction.__init__(self, 2, 1)
        self.setInputDescription(['R', 'S'])
        self.setOutputDescription(['T'])

    def _exec(self, X):
        return [X[0] + X[1]]


F = FUNC()

# Instance creation
f1 = ot.Function(F)


def mul(X):
    return [X[0] * X[1] * X[2]]


f2 = ot.PythonFunction(3, 1, mul)


st = ot.Study()
fileName = 'PyNMF.xml'
st.setStorageManager(ot.XMLStorageManager(fileName))

st.add("f1", f1)
st.add("f2", f2)

st.save()

print('saved f1=', f1)
print('saved f2=', f2)

f1 = ot.Function()
f2 = ot.Function()

st = ot.Study()
st.setStorageManager(ot.XMLStorageManager(fileName))

st.load()

st.fillObject("f1", f1)
st.fillObject("f2", f2)
print('loaded f1=', f1)
print('loaded f2=', f2)


inPt = ot.Point(2, 2.)
outPt = f1(inPt)
print(repr(outPt))

outPt = f1((10., 11.))
print(repr(outPt))

inSample = ot.Sample(10, 2)
for i in range(10):
    inSample[i] = ot.Point((i, i))
print(repr(inSample))

outSample = f1(inSample)
print(repr(outSample))

outSample = f1(((100., 100.), (101., 101.), (102., 102.)))
print(repr(outSample))

os.remove(fileName)
