#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# Indicator function of an Interval of dimension 0
singleton = ot.Interval()
assert singleton.contains([])
assert ot.IndicatorFunction(singleton)([]) == ot.Point([1.0])

# Indicator function of an Interval

box = ot.Interval([1.2, 0.7], [2.0, 1.0])
indicator = ot.IndicatorFunction(box)

point1 = ot.Point([1.5, 0.8])
assert box.contains(point1)
indicator_at_point1 = indicator(point1)
assert indicator_at_point1 == ot.Point([1.0])

point2 = ot.Point([1.1, 0.8])
assert not box.contains(point2)
indicator_at_point2 = indicator(point2)
assert indicator_at_point2 == ot.Point([0.0])

sample = ot.Sample([point1, point2])
assert indicator(sample) == ot.Sample([[1.0], [0.0]])
