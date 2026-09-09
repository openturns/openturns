#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import math as m


class UniformNdPy(ot.PythonDistribution):
    def __init__(self, a=[0.0], b=[1.0]):
        super().__init__(len(a))
        if len(a) != len(b):
            raise ValueError("Invalid bounds")
        for i in range(len(a)):
            if a[i] > b[i]:
                raise ValueError("Invalid bounds")
        self.a = a
        self.b = b
        self.factor = 1.0
        for i in range(len(a)):
            self.factor *= b[i] - a[i]

    def getRange(self):
        return ot.Interval(self.a, self.b, [True] * len(self.a), [True] * len(self.a))

    def getRealization(self):
        X = []
        for i in range(len(self.a)):
            X.append(
                self.a[i] + (self.b[i] - self.a[i]) * ot.RandomGenerator.Generate()
            )
        return X

    def getSample(self, size):
        X = []
        for i in range(size):
            X.append(self.getRealization())
        return X

    def computeCDF(self, X):
        prod = 1.0
        for i in range(len(self.a)):
            if X[i] <= self.a[i]:
                return 0.0
            prod *= min(self.b[i], X[i]) - self.a[i]
        return prod / self.factor

    def computePDF(self, X):
        for i in range(len(self.a)):
            if X[i] <= self.a[i]:
                return 0.0
            if X[i] > self.b[i]:
                return 0.0
        return 1.0 / self.factor

    def getRoughness(self):
        return 42.0

    def getMean(self):
        mu = []
        for i in range(len(self.a)):
            mu.append(0.5 * (self.a[i] + self.b[i]))
        return mu

    def getStandardDeviation(self):
        stdev = []
        for i in range(len(self.a)):
            stdev.append((self.b[i] - self.a[i]) / m.sqrt(12.0))
        return stdev

    def getSkewness(self):
        return [0.0] * len(self.a)

    def getKurtosis(self):
        return [1.8] * len(self.a)

    def getMoment(self, n):
        return [-0.1 * n] * len(self.a)

    # def getCentralMoment(self, n):
    #     return [0.0] * len(self.a)

    def computeCharacteristicFunction(self, x):
        if len(self.a) > 1:
            raise ValueError("dim>1")
        ax = self.a[0] * x
        bx = self.b[0] * x
        return (m.sin(bx) - m.sin(ax) + 1j * (m.cos(ax) - m.cos(bx))) / (bx - ax)

    def isElliptical(self):
        return (len(self.a) == 1) and (self.a[0] == -self.b[0])

    def isCopula(self):
        for i in range(len(self.a)):
            if self.a[i] != 0.0:
                return False
            if self.b[i] != 1.0:
                return False
        return True

    def getMarginal(self, indices):
        subA = []
        subB = []
        for i in indices:
            subA.append(self.a[i])
            subB.append(self.b[i])
        py_dist = UniformNdPy(subA, subB)
        return ot.Distribution(py_dist)

    def computeQuantile(self, prob, tail=False):
        p = 1.0 - prob if tail else prob
        quantile = list(self.a)
        for i in range(len(self.a)):
            quantile[i] += p * (self.b[i] - self.a[i])
        return quantile

    def getParameter(self):
        return list(self.a) + list(self.b)

    def getParameterDescription(self):
        paramDesc = ["a_" + str(i) for i in range(len(self.a))]
        paramDesc.extend(["b_" + str(i) for i in range(len(self.a))])
        return paramDesc

    def setParameter(self, parameter):
        dim = len(self.a)
        for i in range(dim):
            self.a[i] = parameter[i]
            self.b[i] = parameter[dim + i]


for pyDist in [UniformNdPy(), UniformNdPy([0.0] * 2, [1.0] * 2)]:
    print("pyDist=", pyDist)

    # Instance creation
    myDist = ot.Distribution(pyDist)
    print("myDist=", repr(myDist))

    # Copy constructor
    newRV = ot.Distribution(myDist)

    # Dimension
    dim = myDist.getDimension()
    print("dimension=", dim)

    # Realization
    X = myDist.getRealization()
    print("realization=", X)

    # Sample
    X = myDist.getSample(5)
    print("sample=", X)

    # PDF
    point = [0.2] * dim
    pdf = myDist.computePDF(point)
    print("pdf=", pdf)

    # CDF
    cdf = myDist.computeCDF(point)
    print("cdf= %.12g" % cdf)

    # roughness
    roughness = myDist.getRoughness()
    print("roughness=", roughness)

    # Mean
    mean = myDist.getMean()
    print("mean=", mean)

    # Standard deviation
    standardDeviation = myDist.getStandardDeviation()
    print("standard deviation=", standardDeviation)

    # Skewness
    skewness = myDist.getSkewness()
    print("skewness=", skewness)

    # Kurtosis
    kurtosis = myDist.getKurtosis()
    print("kurtosis=", kurtosis)

    # Moment
    moment = myDist.getMoment(3)
    print("moment=", moment)

    # Centered moment
    centeredMoment = myDist.getCentralMoment(3)
    print("centered moment=", centeredMoment)

    if dim == 1:
        CF = myDist.computeCharacteristicFunction(point[0])
        print("characteristic function= (%.12g%+.12gj)" % (CF.real, CF.imag))

    isElliptical = myDist.isElliptical()
    print("isElliptical=", isElliptical)

    isCopula = myDist.isCopula()
    print("isCopula=", isCopula)

    # Range
    range_ = myDist.getRange()
    print("range=", range_)

    # marginal
    marginal = myDist.getMarginal(0)
    print("marginal=", marginal)

    # quantile
    quantile = myDist.computeQuantile(0.5)
    print("quantile=", quantile)

    ot.Log.Show(ot.Log.TRACE)
    validation = ott.DistributionValidation(myDist)
    validation.skipEntropy()  # slow
    validation.skipMinimumVolumeLevelSet()  # slow
    validation.skipCharacteristicFunction()  # undefined
    validation.run()

    param = myDist.getParameter()
    print("parameter=", param)
    param[0] = 0.4
    myDist.setParameter(param)
    print("parameter=", myDist.getParameter())
    print("parameterDesc=", myDist.getParameterDescription())

    print("Cloning distribution")
    newDist = ot.Distribution(myDist)
    param[0] = 0.5
    newDist.setParameter(param)
    print("dist parameter=", myDist.getParameter())
    print("copy dist parameter=", newDist.getParameter())

# Use the distribution as a copula
myDist = ot.Distribution(UniformNdPy([0.0] * 2, [1.0] * 2))
print(ot.JointDistribution([ot.Normal(), ot.Normal()], myDist))
try:
    print("try with another Python distribution")
    myDist = ot.Distribution(UniformNdPy([0.0] * 2, [2.0] * 2))
    print(ot.JointDistribution([ot.Normal(), ot.Normal()], myDist))
except Exception:
    print("The construction failed on purpose as", myDist, "is not a copula")

# Extract the copula
myDist = ot.Distribution(UniformNdPy([0.0] * 2, [2.0] * 2))
copula = myDist.getCopula()

# Test computePDF over a sample (ticket #899)
res = copula.computePDF([[0.5] * 2] * 10)

# Test a discrete distribution


class PoissonPy(ot.PythonDistribution):
    def __init__(self, lamb):
        super().__init__(1)
        if lamb <= 0.0:
            raise ValueError("Expected a positive lambda")
        self.poisson_ = ot.Poisson(lamb)

    def getRange(self):
        return self.poisson_.getRange()

    def computeCDF(self, X):
        return self.poisson_.computeCDF(X)

    def computePDF(self, X):
        return self.poisson_.computeCDF(X)

    def isDiscrete(self):
        return True

    def getSupport(self, interval):
        return self.poisson_.getSupport(interval)

    def isIntegral(self):
        return True


dist = ot.Distribution(PoissonPy(2.5))
assert dist.isDiscrete()
assert dist.isIntegral()


class FullPy(ot.PythonDistribution):
    def __init__(self):
        super().__init__(1)
        self.a = [0.0]
        self.b = [1.0]

    def getRange(self):
        return ot.Interval(self.a, self.b, [True], [True])

    def getRealization(self):
        return [0.5]

    def getSample(self, size):
        return [[0.5]] * size

    def computeDDF(self, X):
        return [0.0]

    def computePDF(self, X):
        return 1.0

    def computeLogPDF(self, X):
        return 0.0

    def computeCDF(self, X):
        return float(X[0])

    def computeComplementaryCDF(self, X):
        return 1.0 - float(X[0])

    def computeQuantile(self, prob, tail=False):
        p = 1.0 - prob if tail else prob
        return [p]

    def computeCharacteristicFunction(self, x):
        return 1.0 + 0.0j

    def computePDFGradient(self, X):
        return [0.0]

    def computeCDFGradient(self, X):
        return [0.0]

    def computeScalarQuantile(self, prob, tail=False):
        return 1.0 - prob if tail else prob

    def getRoughness(self):
        return 1.0

    def getMean(self):
        return [0.5]

    def getStandardDeviation(self):
        return [0.28]

    def getSkewness(self):
        return [0.0]

    def getKurtosis(self):
        return [1.8]

    def getMoment(self, n):
        return [0.5]

    def getCentralMoment(self, n):
        return [0.08]

    def isElliptical(self):
        return False

    def isCopula(self):
        return True

    def isContinuous(self):
        return True

    def isDiscrete(self):
        return False

    def isIntegral(self):
        return False

    def hasEllipticalCopula(self):
        return False

    def hasIndependentCopula(self):
        return True

    def getMarginal(self, indices):
        return ot.Distribution(ot.Uniform())

    def getSupport(self, interval):
        return [[0.5]]

    def getParameter(self):
        return [0.0, 1.0]

    def getParameterDescription(self):
        return ["a0", "b0"]

    def setParameter(self, parameter):
        pass


full = ot.Distribution(FullPy())
ott.assert_almost_equal(full.computeDDF([0.5]), [0.0])
ott.assert_almost_equal([full.computeLogPDF([0.5])], [0.0])
ott.assert_almost_equal([full.computeComplementaryCDF([0.5])], [0.5])
ott.assert_almost_equal(full.computePDFGradient([0.5]), [0.0])
ott.assert_almost_equal(full.computeCDFGradient([0.5]), [0.0])
ott.assert_almost_equal([full.computeScalarQuantile(0.25)], [0.25])
ott.assert_almost_equal([full.computeScalarQuantile(0.25, True)], [0.75])
ott.assert_almost_equal(full.getCentralMoment(2), [0.08])
ott.assert_almost_equal(full.computeQuantile(0.25), [0.25])
ott.assert_almost_equal(full.computeQuantile(0.25, True), [0.75])
assert full.computeCharacteristicFunction(0.1) == 1.0 + 0.0j
assert full.isContinuous()
assert not full.isDiscrete()
assert not full.isIntegral()
assert not full.hasEllipticalCopula()
assert full.hasIndependentCopula()
assert full.getMarginal(0).getDimension() == 1
ott.assert_almost_equal(full.getSupport(full.getRange()), [[0.5]])
assert full == ot.Distribution(full)
assert not (full != ot.Distribution(full))


class MinimalPy(ot.PythonDistribution):
    def __init__(self):
        super().__init__(1)

    def getRange(self):
        return ot.Interval([0.0], [1.0], [True], [True])

    def computeCDF(self, X):
        return float(X[0])


minimal = ot.Distribution(MinimalPy())
minimal.getRealization()
minimal.getSample(2)
minimal.computeDDF([0.5])
minimal.computeLogPDF([0.5])
minimal.computeComplementaryCDF([0.5])
minimal.computePDFGradient([0.5])
minimal.computeCDFGradient([0.5])
minimal.computeScalarQuantile(0.5)
minimal.getCentralMoment(2)
minimal.getMoment(1)
minimal.getMean()
minimal.getStandardDeviation()
minimal.getSkewness()
minimal.getKurtosis()
minimal.getRoughness()
minimal.computeQuantile(0.5)
minimal.computeQuantile(0.5, True)
minimal.computeCharacteristicFunction(0.1)
minimal.isContinuous()
minimal.isDiscrete()
minimal.isIntegral()
minimal.isElliptical()
minimal.isCopula()
minimal.hasEllipticalCopula()
minimal.hasIndependentCopula()
minimal.getMarginal(0)
minimal.getMarginal([0])
assert minimal.getParameter() == []
assert minimal.getParameterDescription() == []
minimal.setParameter([])
with ott.assert_raises(Exception):
    minimal.getSupport(minimal.getRange())
with ott.assert_raises(Exception):
    minimal.computePDF([0.5, 0.6])
with ott.assert_raises(Exception):
    minimal.computeCDF([0.5, 0.6])
with ott.assert_raises(Exception):
    minimal.computeDDF([0.5, 0.6])
with ott.assert_raises(Exception):
    minimal.computeLogPDF([0.5, 0.6])


class NoCDFPy(ot.PythonDistribution):
    def __init__(self):
        super().__init__(1)

    def getRange(self):
        return ot.Interval([0.0], [1.0], [True], [True])


with ott.assert_raises(Exception):
    ot.Distribution(NoCDFPy())


class NoRangePy(ot.PythonDistribution):
    def __init__(self):
        super().__init__(2)

    def computeCDF(self, X):
        return 0.5


with ott.assert_raises(Exception):
    ot.Distribution(NoRangePy())


class BadRealizationPy(ot.PythonDistribution):
    def __init__(self):
        super().__init__(1)

    def getRange(self):
        return ot.Interval([0.0], [1.0], [True], [True])

    def computeCDF(self, X):
        return 0.5

    def getRealization(self):
        return [0.1, 0.2]


with ott.assert_raises(Exception):
    ot.Distribution(BadRealizationPy()).getRealization()


class BadSamplePy(ot.PythonDistribution):
    def __init__(self):
        super().__init__(1)

    def getRange(self):
        return ot.Interval([0.0], [1.0], [True], [True])

    def computeCDF(self, X):
        return 0.5

    def getSample(self, size):
        return [[0.1]] * (size + 1)


with ott.assert_raises(Exception):
    ot.Distribution(BadSamplePy()).getSample(3)


class BadDDFPy(ot.PythonDistribution):
    def __init__(self):
        super().__init__(1)

    def getRange(self):
        return ot.Interval([0.0], [1.0], [True], [True])

    def computeCDF(self, X):
        return 0.5

    def computeDDF(self, X):
        return [0.1, 0.2]


with ott.assert_raises(Exception):
    ot.Distribution(BadDDFPy()).computeDDF([0.5])


class BadMeanPy(ot.PythonDistribution):
    def __init__(self):
        super().__init__(1)

    def getRange(self):
        return ot.Interval([0.0], [1.0], [True], [True])

    def computeCDF(self, X):
        return 0.5

    def getMean(self):
        return [0.1, 0.2]


with ott.assert_raises(Exception):
    ot.Distribution(BadMeanPy()).getMean()


class BadMarginalPy(ot.PythonDistribution):
    def __init__(self):
        super().__init__(1)

    def getRange(self):
        return ot.Interval([0.0], [1.0], [True], [True])

    def computeCDF(self, X):
        return 0.5

    def getMarginal(self, indices):
        return 42


with ott.assert_raises(Exception):
    ot.Distribution(BadMarginalPy()).getMarginal(0)
