#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

try:
    ot.Point(None)
    assert False
except Exception:
    assert True

assert ot.Point(3) is not None
assert not (ot.Point(3) is None)

assert ot.Function() is not None
assert not (ot.Function() is None)
