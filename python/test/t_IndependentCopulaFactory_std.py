#! /usr/bin/env python

from openturns import *

TESTPREAMBLE()

factory = IndependentCopulaFactory()
factory.build()
factory.build(Sample())
factory.build(Point())
