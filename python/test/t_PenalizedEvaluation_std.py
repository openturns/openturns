#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
from openturns.testing import assert_almost_equal


def f_py(x):
    if x[0] < 0.0:
        raise RuntimeError("negative input")
    return [x[0] ** 0.5]


f = ot.PythonFunction(1, 1, f_py)
penalty = 1.0e6
g = otexp.PenalizedEvaluation(f.getEvaluation(), penalty)

# point evaluation
assert_almost_equal(g([4.0]), [2.0], 1e-12, 0.0)
assert_almost_equal(g([-1.0]), [penalty], 0.0, 0.0)

# sample evaluation with mixed success/failure
inSample = ot.Sample([[-1.0], [4.0], [9.0], [-4.0]])
outSample = g(inSample)
assert_almost_equal(
    outSample, [[penalty], [2.0], [3.0], [penalty]], 1e-12, 0.0
)

# default penalized value is sqrt(MaxScalar) to stay finite if squared
g2 = otexp.PenalizedEvaluation(f.getEvaluation())
defaultPenalty = ot.SpecFunc.MaxScalar**0.5
assert_almost_equal(g2.getPenalizedValue(), [defaultPenalty])
assert_almost_equal(g2([-1.0]), [defaultPenalty])

# accessors
assert g.getEvaluation().getInputDimension() == 1
g.setPenalizedValue([123.0])
assert_almost_equal(g.getPenalizedValue(), [123.0], 0.0, 0.0)
assert_almost_equal(g([-1.0]), [123.0], 0.0, 0.0)
g.setPenalizedValue([penalty])
g.setEvaluation(f.getEvaluation())

# marginal keeps penalization
h = otexp.PenalizedEvaluation(
    ot.SymbolicEvaluation(["x1", "x2"], ["y1", "y2"], ["x1", "x2"]),
    [111.0, 222.0],
)
assert_almost_equal(h([1.0, 2.0]), [1.0, 2.0], 1e-12, 0.0)
h1 = h.getMarginal(1)
assert h1.getImplementation().getClassName() == "PenalizedEvaluation"
assert_almost_equal(h1([1.0, 2.0]), [2.0], 1e-12, 0.0)

# use through Function for optimization
gfun = ot.Function(g)
problem = ot.OptimizationProblem(gfun)
problem.setMinimization(True)
algo = ot.Cobyla(problem)
algo.setStartingPoint([4.0])
algo.run()
result = algo.getResult()
assert result.getOptimalValue()[0] < penalty
print("optimal point=", result.getOptimalPoint())
print("calls number=", g.getCallsNumber())
print("ok")
