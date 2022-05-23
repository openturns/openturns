#! /usr/bin/env python

from openturns import *

TESTPREAMBLE()

factory = IndependentCopulaFactory()
factory.build()
factory.build(Sample(3, 2))
factory.build(Point(3))
