#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


f = ot.LeastSquaresMetaModelSelectionFactory(ot.LARS(), ot.CorrectedLeaveOneOut())
print("factory =", f)
