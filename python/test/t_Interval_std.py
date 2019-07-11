#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    # The 1D interface
    interval1D = Interval(-3, 5)
    print("interval1D=", interval1D)
    # The default interface
    size = 2
    defaultInterval = Interval(2)
    print("defaultInterval=", repr(defaultInterval))
    # Construction from 2 Point
    p1 = Point(2, -1.0)
    p2 = Point(2, 2.0)
    print("interval from 2 points=", repr(Interval(p1, p2)))
    print("pretty print=")
    print(Interval(p1, p2))
    # Construction from 2 points and 2 flags
    flag1 = BoolCollection(2, False)
    flag2 = [True, True]  # try automatic conversion to BoolCollection
    interval = Interval(p1, p2, flag1, flag2)
    print("interval from 2 points and 2 flags=", repr(interval))
    print("pretty print=")
    print(interval)
    # Accessors
    print("lower bound=", repr(interval.getLowerBound()))
    print("upper bound=", repr(interval.getUpperBound()))
    print("lower bound flags=", repr(interval.getFiniteLowerBound()))
    print("upper bound flags=", repr(interval.getFiniteUpperBound()))
    # Check if a given interval is empty
    print("interval [p1, p2] empty? ", Interval(p1, p2).isEmpty())
    print("interval [p2, p1] empty? ", Interval(p2, p1).isEmpty())
    # Intersection
    interval1 = Interval(p1, p2)
    p3 = Point(2)
    p3[0] = 0.5
    p3[1] = -1.5
    p4 = Point(2)
    p4[0] = 1.5
    p4[1] = 2.5
    interval2 = Interval(p3, p4)
    print("intersection of ", repr(interval1), " and ", repr(
        interval2), " equals ", repr(interval1.intersect(interval2)))
    # Inside
    p5 = (p1 + p2) * 0.5
    print("is point", p5, "inside", repr(
        interval1), "? ", p5 in interval1)
    p6 = p2 * 2.0
    print("is point", p6, "inside", repr(
        interval1), "? ", p6 in interval1)
    print('marginal(1)=', interval2.getMarginal(1))
    print('marginal(1,0)=', interval2.getMarginal([1, 0]))
except:
    import sys
    print("t_Interval_std.py", sys.exc_info()[0], sys.exc_info()[1])
