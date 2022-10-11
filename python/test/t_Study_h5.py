#! /usr/bin/env python

import openturns as ot
import openturns.testing
import os
import sys
import math as m

ot.TESTPREAMBLE()

fileName = "pyxmlh5.xml.gz"
study = ot.Study()
study.setStorageManager(ot.XMLH5StorageManager(fileName, 5))

point = ot.Point([123.456, 125.43, 3975.4567])
point2 = ot.Point(3, 789.123)
point3 = ot.Point(3, 1673.456)
point4 = ot.Point(3, 789.654123)

sample = ot.Sample(1, point)
sample.add(point2)
sample.add(point3)
sample.add(point4)
sample.add(point2)
sample.add(point4)
sample.add(point3)
print(sample)
study.add("sample", sample)


mesh = ot.IntervalMesher([50] * 3).build(ot.Interval(3))
study.add("mesh", mesh)

study.save()

study2 = ot.Study()
study2.setStorageManager(ot.XMLH5StorageManager(fileName))

study2.load()
sample2 = ot.Sample()
study2.fillObject("sample", sample2)
print(sample2)
assert sample == sample2, "wrong sample"

mesh2 = ot.Mesh()
study2.fillObject("mesh", mesh2)
assert mesh == mesh2, "wrong mesh"

# cleanup
os.remove(fileName)
os.remove(fileName.replace(".xml.gz", ".h5"))
