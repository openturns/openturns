#! /usr/bin/env python

from __future__ import print_function
import openturns as ot


# The 1D interface
interval1D = ot.Interval(-3, 5)
print('interval1D=', interval1D)
# The default interface
size = 2
defaultInterval = ot.Interval(2)
print('defaultInterval=', repr(defaultInterval))
# Construction from 2 NumericalPoint
p1 = ot.NumericalPoint(2, -1.0)
p2 = ot.NumericalPoint(2, 2.0)
print('interval from 2 points=', repr(ot.Interval(p1, p2)))
print('pretty print=')
print(ot.Interval(p1, p2))
# Construction from 2 points and 2 flags
flag1 = ot.BoolCollection(2, False)
flag2 = [True, True]  # try automatic conversion to BoolCollection
interval = ot.Interval(p1, p2, flag1, flag2)
print('interval from 2 points and 2 flags=', repr(interval))
print('pretty print=')
print(interval)
# Accessors
print('lower bound=', repr(interval.getLowerBound()))
print('upper bound=', repr(interval.getUpperBound()))
print('lower bound flags=', repr(interval.getFiniteLowerBound()))
print('upper bound flags=', repr(interval.getFiniteUpperBound()))
# Check if a given interval is empty
print('interval [p1, p2] empty? ', ot.Interval(p1, p2).isEmpty())
print('interval [p2, p1] empty? ', ot.Interval(p2, p1).isEmpty())
# Intersection
interval1 = ot.Interval(p1, p2)
p3 = ot.NumericalPoint([0.5, -1.5])
p4 = ot.NumericalPoint([1.5, 2.5])
interval2 = ot.Interval(p3, p4)
print('intersection of ', repr(interval1), ' and ', repr(
    interval2), ' equals ', repr(interval1.intersect(interval2)))
# Inside
p5 = (p1 + p2) * 0.5
print('is point', p5, 'inside', repr(interval1), '? ', interval1.contains(p5))
p6 = p2 * 2.0
print('is point', p6, 'inside', repr(interval1), '? ', interval1.contains(p6))
interval1 = ot.Interval([-1], [1], [True], [False]) # (-1, +inf)
interval2 = ot.Interval([-1], [2], [True], [True]) # (-1, 2)
print('intersection of ', repr(interval1), ' and ', repr(
    interval2), ' equals ', repr(interval1.intersect(interval2)))