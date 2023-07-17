#! /usr/bin/env python

from openturns.testing import assert_almost_equal
from openturns.usecases import oscillator

"""
Test the Oscillator data class on a reference point.
"""
osc = oscillator.Oscillator()


assert_almost_equal(osc.model(osc.distribution.getMean()), [23.1897], 1e-2)


# Compute probability with FORM 

optimAlgo = ot.Cobyla()
optimAlgo.setMaximumEvaluationNumber(1000)
optimAlgo.setMaximumAbsoluteError(1.0e-3)
optimAlgo.setMaximumRelativeError(1.0e-3)
optimAlgo.setMaximumResidualError(1.0e-3)
optimAlgo.setMaximumConstraintError(1.0e-3)


algo = ot.FORM(optimAlgo, event, distribution.getMean())
algo.run()
result = algo.getResult()


assert_almost_equal(result.getEventProbability(), [3.8906e-06], 1e-3)
