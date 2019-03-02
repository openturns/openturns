#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    # We create an empty Sample
    sample1 = Sample(0, 2)
    sample1.setName("Sample1")
    sample1.setDescription(('a0', 'a1'))

    # We populate the empty sample
    point1 = Point(2)
    point1[0] = 10.
    point1[1] = 20.
    point2 = Point(2)
    point2[0] = 11.
    point2[1] = 21.
    point3 = Point(2)
    point3[0] = 12.
    point3[1] = 22.
    sample1.add(point1)
    sample1.add(point2)
    sample1.add(point3)
    print("sample1=", repr(sample1))

    # We get the second element of the sample
    secondElement = sample1[1]
    print("second element=", repr(secondElement))

    # We set the third element to a valid new element
    newPoint = Point(2)
    newPoint[0] = 1000
    newPoint[1] = 2000
    sample1[2] = newPoint
    print("sample1=", repr(sample1))

    sample2 = Sample(10, point1)
    print("sample2=", repr(sample2))

    sample2[5] = point2
    print("sample2=", repr(sample2))

    try:
        # We get the tenth element of the sample
        # THIS SHOULD NORMALLY FAIL
        tenthElement = sample1[9]

        # Normally, we should never go here
        raise
    except:
        pass

    # We try to create a sample with 5 times the same point
    samePoint = Point(3)
    samePoint[0] = 1000.
    samePoint[1] = 2000.
    samePoint[2] = 3000.
    sample3 = Sample(5, samePoint)
    print("sample3=", repr(sample3))

    # We append a new point to the sample...
    anotherNewPoint = Point(3)
    anotherNewPoint[0] = -1000.
    anotherNewPoint[1] = -2000.
    anotherNewPoint[2] = -3000.
    sample3.add(anotherNewPoint)
    print("sample3=", repr(sample3))

    #... then the first point again
    sample3.add(samePoint)
    print("sample3=", repr(sample3))

    # We find the new point into the sample
    pos = sample3.find(anotherNewPoint)
    if (pos != 5):
        print("Error in finding point into sample. Expected 5. Got ", pos)

    # We erase the new point from the sample
    sample3.erase(pos)
    print("sample3=", repr(sample3))

    # Partial copy constructor containing sample2[4:7]
    sample4 = Sample(sample2, 4, 8)
    print("sample4=", repr(sample4))

    sample5 = Sample([[1.0], [3.0], [2.0]])
    print("sample5=", repr(sample5))
    print("sample5 (linear)=", sample5.asPoint())

    # Selection
    indices = [1, 0, 2, 1, 2]
    print("selection=", sample5.select(indices))

    # html conversion
    print(sample1._repr_html_())

    # check conversion
    v1 = [Point([12.]), Point([1.])]
    v2 = [Point([13.]), Point([2.])]
    try:
        s = Sample([v1, v2])
    except:
        print('ok')

    try:
        # uneven points
        s = Sample([[1.0, 2.0], [5.0]])
    except:
        print('ok')
except:
    import sys
    print("t_Sample_std.py", sys.exc_info()[0], sys.exc_info()[1])
