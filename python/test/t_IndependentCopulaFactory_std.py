#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

factory = ot.IndependentCopulaFactory()
factory.build()
factory.build(ot.Sample(3, 2))
factory.build(ot.Point())
