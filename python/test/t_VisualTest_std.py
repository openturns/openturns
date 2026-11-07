#!/usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# QQPlot tests
size = 100
normal = ot.Normal(1)
sample = normal.getSample(size)
sample2 = ot.Gamma(3.0, 4.0, 0.0).getSample(size)
twoSamplesQQPlot = ot.VisualTest.DrawQQplot(sample, sample2)
assert isinstance(twoSamplesQQPlot, ot.Graph)
assert "QQ-plot" in twoSamplesQQPlot.getTitle()
assert len(twoSamplesQQPlot.getDrawables()) >= 2
assert twoSamplesQQPlot.getLegends()[0] == "Test line"

sampleDistributionQQPlot = ot.VisualTest.DrawQQplot(sample, normal)
assert isinstance(sampleDistributionQQPlot, ot.Graph)
assert "QQ-plot" in sampleDistributionQQPlot.getTitle()
assert len(sampleDistributionQQPlot.getDrawables()) >= 2
assert sampleDistributionQQPlot.getLegends()[0] == "Test line"

dist = ot.Geometric()
qq_plot = ot.VisualTest.DrawQQplot(dist.getSample(size), dist)
assert isinstance(qq_plot, ot.Graph)
assert "QQ-plot" in qq_plot.getTitle()
assert len(qq_plot.getDrawables()) >= 2
assert qq_plot.getLegends()[0] == "Test line"

sample2 = normal.getSample(size)
twoSamplesPPPlot = ot.VisualTest.DrawPPplot(sample, sample2)
assert isinstance(twoSamplesPPPlot, ot.Graph)
assert "PP-plot" in twoSamplesPPPlot.getTitle()
assert len(twoSamplesPPPlot.getDrawables()) >= 2
assert twoSamplesPPPlot.getLegends()[0] == "Test line"

sampleDistributionPPplot = ot.VisualTest.DrawPPplot(sample, normal)
assert isinstance(sampleDistributionPPplot, ot.Graph)
assert "PP-plot" in sampleDistributionPPplot.getTitle()
assert len(sampleDistributionPPplot.getDrawables()) >= 2
assert sampleDistributionPPplot.getLegends()[0] == "Test line"

# HenryLine test
size = 100
normal = ot.Normal(1)
sample = normal.getSample(size)
henryPlot = ot.VisualTest.DrawHenryLine(sample)
assert isinstance(henryPlot, ot.Graph)
assert "Henry" in henryPlot.getTitle()
assert len(henryPlot.getDrawables()) >= 2
assert henryPlot.getLegends()[0] == "Henry line"

# LinearModel tests
dimension = 2
R = ot.CorrelationMatrix(dimension)
R[0, 1] = 0.8
distribution = ot.Normal(ot.Point(dimension, 3.0), ot.Point(dimension, 2.0), R)
size = 100
sample2D = distribution.getSample(size)
firstSample = ot.Sample(size, 1)
secondSample = ot.Sample(size, 1)
for i in range(size):
    firstSample[i] = ot.Point(1, sample2D[i, 0])
    secondSample[i] = ot.Point(1, sample2D[i, 1])

lmtest = ot.LinearModelAlgorithm(firstSample, secondSample).getResult()
drawLinearModelVTest = ot.VisualTest.DrawLinearModel(lmtest)
assert isinstance(drawLinearModelVTest, ot.Graph)
assert len(drawLinearModelVTest.getDrawables()) >= 1

drawLinearModelResidualTest = ot.VisualTest.DrawLinearModelResidual(lmtest)
assert isinstance(drawLinearModelResidualTest, ot.Graph)
assert len(drawLinearModelResidualTest.getDrawables()) >= 1

# Parallel coordinates tests
size = 100
inputDimension = 6
inputSample = ot.Normal(inputDimension).getSample(size)
inputVar = ["X" + str(i) for i in range(inputDimension)]
formula = ot.Description(1)
expression = ""
for i in range(inputDimension):
    if i > 0:
        expression += "+"
    expression += "cos(" + str(i + 1) + "*" + inputVar[i] + ")"
formula[0] = expression
model = ot.SymbolicFunction(inputVar, formula)
outputSample = model(inputSample)
parallelCoordinatesValue = ot.VisualTest.DrawParallelCoordinates(
    inputSample, outputSample, 2.5, 3.0, "red", False
)
assert isinstance(parallelCoordinatesValue, ot.Graph)
assert len(parallelCoordinatesValue.getDrawables()) >= 1

parallelCoordinatesQuantile = ot.VisualTest.DrawParallelCoordinates(
    inputSample, outputSample, 0.7, 0.9, "red", False
)
assert isinstance(parallelCoordinatesQuantile, ot.Graph)
assert len(parallelCoordinatesQuantile.getDrawables()) >= 1

# KendallPlot tests
size = 100
copula1 = ot.FrankCopula(1.5)
copula2 = ot.GumbelCopula(4.5)
sample1 = copula1.getSample(size)
sample1.setName("data 1")
sample2 = copula2.getSample(size)
sample2.setName("data 2")
kendallPlot1 = ot.VisualTest.DrawKendallPlot(sample1, copula2)
assert isinstance(kendallPlot1, ot.Graph)
assert len(kendallPlot1.getDrawables()) >= 1

kendallPlot2 = ot.VisualTest.DrawKendallPlot(sample2, sample1)
assert isinstance(kendallPlot2, ot.Graph)
assert len(kendallPlot2.getDrawables()) >= 1

# Clouds
sample = ot.Normal(4).getSample(200)
clouds = ot.VisualTest.DrawPairs(sample)
assert isinstance(clouds, ot.GridLayout)
assert clouds.getNbRows() == 3
assert clouds.getNbColumns() == 3
assert len(clouds.getGraph(0, 0).getDrawables()) >= 1
distribution = ot.JointDistribution(
    [ot.HistogramFactory().build(sample.getMarginal(i)) for i in range(4)]
)
cloudsMarginals = ot.VisualTest.DrawPairsMarginals(sample, distribution)
assert isinstance(cloudsMarginals, ot.GridLayout)
assert cloudsMarginals.getNbRows() == 4
assert cloudsMarginals.getNbColumns() == 4
assert len(cloudsMarginals.getGraph(0, 0).getDrawables()) >= 1

# dependence functions
copula = ot.GumbelCopula()
data = copula.getSample(100000)
graph1 = ot.VisualTest.DrawUpperTailDependenceFunction(data)
graph2 = ot.VisualTest.DrawUpperExtremalDependenceFunction(data)
graph3 = ot.VisualTest.DrawLowerTailDependenceFunction(data)
graph4 = ot.VisualTest.DrawLowerExtremalDependenceFunction(data)
for graph in [graph1, graph2, graph3, graph4]:
    assert isinstance(graph, ot.Graph)
    assert len(graph.getDrawables()) >= 1

# check vs theoretical value
theta = copula.getTheta()
ref = 2.0 - 2.0 ** (1.0 / theta)
value = graph1.getDrawable(0).getData()[-4, 1]
ott.assert_almost_equal(value, ref, 1e-2, 1e-3)

# Draw points inside and outside a domain
domain = ot.Domain(ot.Interval([-2.0, 0.0, -1], [2.0, 3.0, 1.0]))
U = ot.Uniform(-3, 3)
dist = ot.JointDistribution([U, U, U])
grid = ot.VisualTest.DrawInsideOutside(domain, dist.getSample(30))
assert isinstance(grid, ot.GridLayout)
assert grid.getNbRows() == 2
assert grid.getNbColumns() == 2

# CDF plot with unique values
dist = ot.Mixture([ot.Uniform(8, 15), ot.Dirac(10)], [0.1, 0.9])
ud = ot.FiniteDiscreteDistribution(dist.getSample(1000))
graph = ot.VisualTest.DrawCDFplot(ud.getSample(100), ud)
assert isinstance(graph, ot.Graph)
assert "CDF" in graph.getTitle()
assert len(graph.getDrawables()) >= 1

# large-sample QQ plot points must remain visible, see issue #1357
medium_size = ot.ResourceMap.GetAsUnsignedInteger("VisualTest-CloudMediumSize")
large_sample = normal.getSample(10 * medium_size)
large_qq_plot = ot.VisualTest.DrawQQplot(large_sample, normal)
assert large_qq_plot.getDrawable(1).getPointStyle() == "plus", "visible point style expected"

# DrawPairsWithGrid in 2D
distribution = ot.JointDistribution([ot.Uniform(0.0, 1.0)] * 2)
sample = distribution.getSample(10)
design_grid = ot.VisualTest.DrawPairsWithGrid(sample, distribution.getRange(), [10, 10])
assert design_grid.getNbRows() == 1, "wrong nb rows"
assert design_grid.getNbColumns() == 1, "wrong nb columns"
graph = design_grid.getGraph(0, 0)
assert len(graph.getDrawables()) == 2, "each graph must contain Grid+Cloud"
assert graph.getDrawable(0).getImplementation().getClassName() == "Grid", "first drawable must be Grid"
assert graph.getDrawable(1).getImplementation().getClassName() == "Cloud", "second drawable must be Cloud"
gridDrawable = graph.getDrawable(0)
assert list(gridDrawable.getSubdivisions()) == [10, 10], "wrong grid subdivisions"

# DrawPairsWithGrid in 3D, defaults (bounds from sample, 5 subdivisions)
sample3 = ot.JointDistribution([ot.Uniform(0.0, 1.0)] * 3).getSample(10)
design_grid3 = ot.VisualTest.DrawPairsWithGrid(sample3)
assert design_grid3.getNbRows() == 2, "wrong nb rows in 3D"
assert design_grid3.getNbColumns() == 2, "wrong nb columns in 3D"
# lower triangle filled, upper empty
assert len(design_grid3.getGraph(0, 0).getDrawables()) == 2, "missing (1,0) plane"
assert len(design_grid3.getGraph(1, 0).getDrawables()) == 2, "missing (2,0) plane"
assert len(design_grid3.getGraph(1, 1).getDrawables()) == 2, "missing (2,1) plane"
assert len(design_grid3.getGraph(0, 1).getDrawables()) == 0, "upper triangle must be empty"

# DrawPairsWithGrid with explicit bounds/subdivisions in 3D
bounds3 = ot.Interval([0.0] * 3, [1.0] * 3)
design_grid3b = ot.VisualTest.DrawPairsWithGrid(sample3, bounds3, [10, 10, 10])
for i in range(2):
    for j in range(2):
        if j > i:
            continue
        g = design_grid3b.getGraph(i, j)
        assert g.getDrawable(0).getImplementation().getClassName() == "Grid", "Grid missing"

# invalid dimension
with ott.assert_raises(ValueError):
    ot.VisualTest.DrawPairsWithGrid(ot.Normal(1).getSample(10))

# invalid subdivisions
with ott.assert_raises(ValueError):
    ot.VisualTest.DrawPairsWithGrid(sample3, bounds3, [10, 10])
with ott.assert_raises(TypeError):
    ot.VisualTest.DrawPairsWithGrid(sample3, bounds3, [10, 0, 10])
