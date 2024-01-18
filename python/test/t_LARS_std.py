#! /usr/bin/env python

import openturns as ot
from math import pi

ot.TESTPREAMBLE()

# Log.Show( Log.Flags() | Log.INFO )

# Problem parameters
dimension = 3
a = 7.0
b = 0.1
# Reference analytical values
meanTh = a / 2
covTh = (b**2 * pi**8) / 18.0 + (b * pi**4) / 5.0 + (a**2) / 8.0 + 1.0 / 2.0
sob_1 = [
    (b * pi**4 / 5.0 + b**2 * pi**8 / 50.0 + 1.0 / 2.0) / covTh,
    (a**2 / 8.0) / covTh,
    0.0,
]
sob_2 = [0.0, (b**2 * pi**8 / 18.0 - b**2 * pi**8 / 50.0) / covTh, 0.0]
sob_3 = [0.0]
sob_T1 = [
    sob_1[0] + sob_2[0] + sob_2[1] + sob_3[0],
    sob_1[1] + sob_2[0] + sob_2[2] + sob_3[0],
    sob_1[2] + sob_2[1] + sob_2[2] + sob_3[0],
]
sob_T2 = [
    sob_2[0] + sob_2[1] + sob_3[0],
    sob_2[0] + sob_2[2] + sob_3[0],
    sob_2[1] + sob_2[2] + sob_3[0],
]
sob_T3 = [sob_3[0]]
# Create the Ishigami function
model = ot.SymbolicFunction(
    ["xi1", "xi2", "xi3"],
    [
        "sin(xi1) + ("
        + str(a)
        + ") * (sin(xi2)) ^ 2 + ("
        + str(b)
        + ") * xi3^4 * sin(xi1)"
    ],
)

# Create the input distribution
distribution = ot.JointDistribution([ot.Uniform(-pi, pi)] * dimension)

# Create the orthogonal basis
polynomialCollection = ot.PolynomialFamilyCollection(dimension)
for i in range(dimension):
    polynomialCollection[i] = ot.OrthogonalUniVariatePolynomialFamily(
        ot.LegendreFactory()
    )
enumerateFunction = ot.LinearEnumerateFunction(dimension)
productBasis = ot.OrthogonalBasis(
    ot.OrthogonalProductPolynomialFactory(polynomialCollection, enumerateFunction)
)

# design experiment
samplingSize = 75
experiment = ot.LowDiscrepancyExperiment(ot.SobolSequence(), distribution, samplingSize)

# generate samples
x = experiment.generate()
y = model(x)

# iso transfo
xToU = ot.DistributionTransformation(distribution, productBasis.getMeasure())

u = xToU(x)

# build basis
degree = 10
basisSize = enumerateFunction.getStrataCumulatedCardinal(degree)
basis = [productBasis.build(i) for i in range(basisSize)]

# run algorithm
factory = ot.BasisSequenceFactory(ot.LARS())
print("factory = ", factory)

seq = factory.build(u, y, basis, list(range(basisSize)))

first = 20
if seq.getSize() >= first:
    print("first ", first, " indices = ", seq.getIndices(first - 1))
