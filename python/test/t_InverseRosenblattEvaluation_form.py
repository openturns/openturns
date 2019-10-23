#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

model = ot.SymbolicFunction(['x0', 'x1', 'x2', 'x3'], ['-(6+x0^2-x1+x2+3*x3)'])
dim = model.getInputDimension()
marginals = [ot.Normal(5.0, 3.0) for i in range(dim)]
distribution = ot.ComposedDistribution(
    marginals, ot.ComposedCopula([ot.ClaytonCopula(), ot.NormalCopula()]))
#distribution = ot.Normal([5]*dim, [3]*dim, ot.CorrelationMatrix(dim))
#distribution = ot.ComposedDistribution(marginals, ot.IndependentCopula(dim))
distribution.setDescription(['marginal'+str(i) for i in range(dim)])
vect = ot.RandomVector(distribution)
output = ot.CompositeRandomVector(model, vect)
event = ot.ThresholdEvent(output, ot.Greater(), 0.0)
solver = ot.Cobyla()
solver.setMaximumEvaluationNumber(200)
solver.setMaximumAbsoluteError(1.0e-10)
solver.setMaximumRelativeError(1.0e-10)
solver.setMaximumResidualError(1.0e-10)
solver.setMaximumConstraintError(1.0e-10)
algo = ot.FORM(solver, event, distribution.getMean())
algo.run()
result = algo.getResult()
hasoferReliabilityIndexSensitivity = result.getHasoferReliabilityIndexSensitivity()
print(hasoferReliabilityIndexSensitivity)
