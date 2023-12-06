import openturns as ot

assert ot.Interval() != ot.ExpectationSimulationAlgorithm()
assert not ot.Interval() == ot.ExpectationSimulationAlgorithm()

assert ot.Interval() != ot.Sample([[1]])
assert not ot.Interval() == ot.Sample([[1]])

assert ot.Laplace() != ot.UsualRandomVector(ot.Normal())
assert not ot.Laplace() == ot.UsualRandomVector(ot.Normal())
