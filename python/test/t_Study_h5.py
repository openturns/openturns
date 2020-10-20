#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing
import os
import sys
import math as m

ot.TESTPREAMBLE()

try:
    fileName = 'myStudyH5.xml'
    # XMLH5StorageManager
    myStudy = ot.Study(fileName)
    myStudy.setStorageManager(ot.XMLH5StorageManager(fileName))

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

    myStudy.add('mySample', sample)
    myStudy.save()
    print(sample)

    myStudy2 = ot.Study(fileName)
    myStudy2.setStorageManager(ot.XMLH5StorageManager(fileName))
    myStudy2.load()
    sample2 = ot.Sample()
    myStudy2.fillObject('mySample', sample2)
    print(sample2)

    # cleanup
    os.remove(fileName)
    os.remove(fileName.split(".")[0]+".h5")


except:
    import os
    import traceback
    traceback.print_exc()
    os._exit(1)
