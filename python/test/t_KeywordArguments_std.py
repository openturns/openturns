#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# Copulas, experimental distributions and adjacent modules
clayton = ot.ClaytonCopula(theta=2.0)
assert clayton == ot.ClaytonCopula(2.0)

gumbelCopula = ot.GumbelCopula(theta=2.5)
assert gumbelCopula == ot.GumbelCopula(2.5)

frank = ot.FrankCopula(theta=1.5)
assert frank == ot.FrankCopula(1.5)

normalCopula = ot.NormalCopula(dim=3)
assert normalCopula == ot.NormalCopula(3)

independentCopula = ot.IndependentCopula(dimension=2)
assert independentCopula == ot.IndependentCopula(2)

wrappedNormal = otexp.WrappedNormal(mu=[0.0], sigma=ot.CovarianceMatrix([[1.0]]))
assert wrappedNormal == otexp.WrappedNormal([0.0], ot.CovarianceMatrix([[1.0]]))

piecewise = otexp.PiecewiseLinearDistribution(
    x=ot.Point([0.0, 1.0]), y=ot.Point([0.5, 0.5])
)
assert piecewise == otexp.PiecewiseLinearDistribution(
    ot.Point([0.0, 1.0]), ot.Point([0.5, 0.5])
)

usual = ot.UsualRandomVector(distribution=ot.Uniform())
assert usual.getDistribution() == ot.Uniform()

atoms = [ot.Normal(), ot.Normal(1.0, 1.0)]
weights = [0.25, 0.75]
mixtureClassifier = ot.MixtureClassifier(mixture=ot.Mixture(atoms, weights))
assert mixtureClassifier.getMixture() == ot.Mixture(atoms, weights)

point = ot.Point([0.5, 0.5])
assert clayton.computePDF(point=point) == clayton.computePDF(point)

with ott.assert_raises(TypeError):
    ot.ClaytonCopula(unknown=2.0)

# Metamodel, bayesian, transformation
inputSample = ot.Sample([[0.0], [1.0], [2.0]])
outputSample = ot.Sample([[0.0], [1.0], [4.0]])
covarianceModel = ot.SquaredExponential()
basis = ot.ConstantBasisFactory().build()
kriging = ot.KrigingAlgorithm(
    inputSample=inputSample,
    outputSample=outputSample,
    covarianceModel=covarianceModel,
    basis=basis,
)
assert kriging.getInputSample() == inputSample
assert kriging.getOutputSample() == outputSample

baseFunction = ot.SymbolicFunction(["a", "x"], ["a*x"])
# ParametricFunction lives in the func module (not covered yet): positional call
model = ot.ParametricFunction(baseFunction, [0], [0.0])
calibration = ot.NonLinearLeastSquaresCalibration(
    model=model,
    inputObservations=inputSample,
    outputObservations=outputSample,
    startingPoint=[1.0],
)
assert calibration.getModel() == model

# BoxCoxTransform itself lives in the func module (not covered yet);
# exercise the transformation group through BoxCoxFactory.build kwargs
boxCoxSample = ot.Sample([[0.5], [1.5], [2.5]])
boxCoxTransform = ot.BoxCoxFactory().build(sample=boxCoxSample, shift=[0.0])
assert boxCoxTransform == ot.BoxCoxFactory().build(boxCoxSample, [0.0])

with ott.assert_raises(TypeError):
    ot.KrigingAlgorithm(unknown=inputSample)

# Typ, graph
point3 = ot.Point(size=3, value=1.0)
assert point3 == ot.Point(3, 1.0)

sample = ot.Sample(size=2, dim=2)
assert sample == ot.Sample(2, 2)

graph = ot.Graph(title="title", xTitle="x", yTitle="y")
assert graph.getTitle() == "title"
assert graph.getXTitle() == "x"
assert graph.getYTitle() == "y"

with ott.assert_raises(TypeError):
    ot.Point(unknown=3)

# Representatives from across the library
symbolic = ot.SymbolicFunction(inputVariableName="x", formula="x^2")
assert symbolic == ot.SymbolicFunction("x", "x^2")
assert symbolic.getFormulas() == ["x^2"]

interval = ot.Interval(lowerBound=[0.0], upperBound=[1.0])
assert interval == ot.Interval([0.0], [1.0])

problem = ot.OptimizationProblem(objective=symbolic)
assert problem.getObjective() == symbolic

cobyla = ot.Cobyla(problem=problem)
assert cobyla.getProblem().getObjective() == symbolic

axial = ot.Axial(center=[0.0], levels=[0.5])
assert axial.generate() == ot.Axial([0.0], [0.5]).generate()

brent = ot.Brent(
    absoluteError=1e-5,
    relativeError=1e-5,
    residualError=1e-5,
    maximumFunctionEvaluation=100,
)
assert brent.getMaximumCallsNumber() == 100
# Skipping an optional parameter cannot be forwarded positionally:
with ott.assert_raises(TypeError):
    ot.Brent(maximumFunctionEvaluation=100)
with ott.assert_raises(TypeError):
    ot.Graph(title="title", yTitle="y")

lhs = ot.LHSExperiment(distribution=ot.Uniform(), size=10, alwaysShuffle=True)
assert lhs.getDistribution() == ot.Uniform()
assert lhs.getSize() == 10

whiteNoise = ot.WhiteNoise(distribution=ot.Normal())
assert whiteNoise.getDistribution() == ot.Normal()

halton = ot.HaltonSequence(dimension=2)
assert halton.getDimension() == 2

step = ot.ConstantStep(eps=[1e-4])
assert step == ot.ConstantStep([1e-4])

with ott.assert_raises(TypeError):
    ot.SymbolicFunction(unknown="x")
with ott.assert_raises(TypeError):
    ot.Interval(unknown=[0.0])

# Module-level free functions (e.g. VisualTest namespace)
linInput = ot.Sample([[0.0], [1.0], [2.0]])
linOutput = ot.Sample([[0.5], [1.5], [2.5]])
linres = ot.LinearModelAlgorithm(linInput, linOutput).getResult()
graph1 = ot.VisualTest.DrawLinearModel(linearModelResult=linres)
assert isinstance(graph1, ot.Graph)
graph2 = ot.VisualTest.DrawLinearModel(
    inputSample=linInput, outputSample=linOutput, linearModelResult=linres
)
assert isinstance(graph2, ot.Graph)

with ott.assert_raises(TypeError):
    ot.VisualTest.DrawLinearModel(unknown=linres)
