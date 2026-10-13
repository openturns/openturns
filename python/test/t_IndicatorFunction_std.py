#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import os

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

# extra coverage: default ctor, comparison, string converters

default = ot.IndicatorFunction()
assert default == default
assert indicator == indicator
assert not (indicator != indicator)
assert indicator == ot.IndicatorFunction(box)
assert indicator != ot.IndicatorFunction(ot.Interval([0.0, 0.0], [1.0, 1.0]))
assert len(repr(indicator)) > 0
assert len(str(indicator)) > 0
assert len(repr(default)) > 0
# gradient/hessian are not implemented
with ott.assert_raises(Exception):
    indicator.gradient(point1)
with ott.assert_raises(Exception):
    indicator.hessian(point1)
# save/load
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("indicator.xml"))
study.add("f", indicator)
study.save()
loaded_study = ot.Study()
loaded_study.setStorageManager(ot.XMLStorageManager("indicator.xml"))
loaded_study.load()
loaded = ot.Function()
loaded_study.fillObject("f", loaded)
assert loaded == indicator

os.remove("indicator.xml")
