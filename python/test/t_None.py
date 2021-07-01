#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

try:
    ot.Point(None)
    assert False
except Exception:
    assert True

assert ot.Point(3) is not None
assert ot.Point(3) != None
assert not ot.Point(3) == None

assert ot.Function() is not None
assert ot.Function() != None
assert not ot.Function() == None
