#! /usr/bin/env python

import math

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
dimension = 3
meanPoint = ot.Point(dimension, 1.0)
meanPoint[0] = 0.5
meanPoint[1] = -0.5
sigma = ot.Point(dimension, 1.0)
sigma[0] = 2.0
sigma[1] = 3.0
R = ot.CorrelationMatrix(dimension)
for i in range(1, dimension):
    R[i, i - 1] = 0.5

# Create a collection of distribution
aCollection = ot.DistributionCollection()

aCollection.add(ot.Normal(meanPoint, sigma, R))
meanPoint += ot.Point(meanPoint.getDimension(), 1.0)
aCollection.add(ot.Normal(meanPoint, sigma, R))
meanPoint += ot.Point(meanPoint.getDimension(), 1.0)
aCollection.add(ot.Normal(meanPoint, sigma, R))

# Instantiate one distribution object
distribution = ot.Mixture(aCollection, ot.Point(aCollection.getSize(), 2.0))
assert distribution.getClassName() == "Mixture"
ott.assert_almost_equal(distribution.getWeights(), [1.0 / 3.0] * 3, 1e-12, 0.0)
weights = distribution.getWeights()
weights[0] = 2.0 * weights[0]
distribution.setWeights(weights)
ott.assert_almost_equal(distribution.getWeights(), [0.5, 0.25, 0.25], 1e-12, 0.0)
distribution = ot.Mixture(aCollection)
assert distribution.getClassName() == "Mixture"

# Is this distribution elliptical ?
assert not distribution.isElliptical()

# Is this distribution continuous ?
assert distribution.isContinuous()

# Test for realization of distribution
oneRealization = distribution.getRealization()
ott.assert_almost_equal(oneRealization, [2.07328, -1.03125, 2.18976], 1e-5, 0.0)

# Test for sampling
size = 1000
oneSample = distribution.getSample(size)
ott.assert_almost_equal(oneSample[0], [-1.86277, -0.862642, 2.91223], 1e-5, 0.0)
ott.assert_almost_equal(oneSample[size - 1], [1.56533, -1.10765, 2.76013], 1e-5, 0.0)
ott.assert_almost_equal(oneSample.computeMean(), [1.64038, 0.699193, 2.03941], 1e-5, 0.0)
ott.assert_almost_equal(
    oneSample.computeCovariance(),
    ot.CovarianceMatrix(
        3, [4.88439, 3.99293, 0.644831, 3.99293, 9.41608, 2.09091, 0.644831, 2.09091, 1.58061]
    ),
    1e-5,
    0.0,
)

# Define a point
point = ot.Point(distribution.getDimension(), 1.0)
ott.assert_almost_equal(point, [1.0, 1.0, 1.0], 0.0, 0.0)

# derivative of PDF with regards its arguments
DDF = distribution.computeDDF(point)
ott.assert_almost_equal(DDF, [0.00119456, -0.00197254, 0.00476249], 1e-5, 0.0)

# PDF value
LPDF = distribution.computeLogPDF(point)
ott.assert_almost_equal(LPDF, -5.135163, 1e-6, 1e-6)
PDF = distribution.computePDF(point)
ott.assert_almost_equal(PDF, 0.005886, 1e-6, 1e-6)

x = 0.6
y = [0.2] * (dimension - 1)
ott.assert_almost_equal(distribution.computeConditionalPDF(x, y), 0.184614, 1e-6, 1e-6)
ott.assert_almost_equal(distribution.computeConditionalCDF(x, y), 0.106212, 1e-6, 1e-6)
ott.assert_almost_equal(distribution.computeConditionalQuantile(x, y), 2.237650, 1e-6, 1e-6)
pt = ot.Point([i + 1.5 for i in range(dimension)])
ott.assert_almost_equal(
    distribution.computeSequentialConditionalPDF(point), [0.179079, 0.144334, 0.227727], 1e-5, 0.0
)
resCDF = distribution.computeSequentialConditionalCDF(pt)
ott.assert_almost_equal(resCDF, [0.5, 0.778617, 0.809658], 1e-5, 0.0)
ott.assert_almost_equal(
    distribution.computeSequentialConditionalQuantile(resCDF), [1.5, 2.5, 3.5], 1e-5, 0.0
)
# derivative of the PDF with regards the parameters of the distribution
CDF = distribution.computeCDF(point)
ott.assert_almost_equal(CDF, 0.115110, 1e-6, 1e-6)
CCDF = distribution.computeComplementaryCDF(point)
ott.assert_almost_equal(CCDF, 0.884890, 1e-6, 1e-6)
PDFgr = distribution.computePDFGradient(point)
assert distribution.computePDFGradient([point]).getDimension() == len(PDFgr)
ott.assert_almost_equal(
    PDFgr,
    [
        -0.000262548,
        -0.00216602,
        0.00105019,
        -0.000875159,
        -0.00157529,
        -0.00420076,
        0.00380694,
        -0.00190347,
        0.0030193,
        -0.00081614,
        -0.000579459,
        0.000827022,
        -0.000384492,
        -0.00280752,
        0.00124053,
        -0.00101745,
        0.00214101,
        -0.00287825,
        -0.000115876,
        2.77364e-05,
        9.53309e-05,
        -5.53358e-05,
        -0.00037968,
        0.000641018,
        -0.000441727,
        0.000684369,
        -0.000799218,
    ],
    1e-5,
    0.0,
)

# derivative of the PDF with regards the parameters of the distribution
CDFgr = distribution.computeCDFGradient(point)
ott.assert_almost_equal(
    CDFgr,
    [
        -0.027177,
        -0.00679426,
        -0.00553193,
        -0.00276597,
        -0.0681155,
        0.0,
        0.0205285,
        0.0360994,
        0.0238195,
        -0.00944187,
        0.00236047,
        -0.000721539,
        -0.000120256,
        -0.0302999,
        0.0302999,
        0.00402514,
        0.027088,
        0.00545694,
        -0.00111099,
        0.000833241,
        -2.55027e-05,
        4.25045e-06,
        -0.00399125,
        0.00798249,
        0.000197734,
        0.00518251,
        0.000286546,
    ],
    1e-5,
    0.0,
)

# quantile
quantile = distribution.computeQuantile(0.95)
ott.assert_almost_equal(quantile, [5.96551, 6.93727, 4.61725], 1e-5, 0.0)
ott.assert_almost_equal(distribution.computeCDF(quantile), 0.95, 1e-6, 1e-6)
# Get 95% survival function
inverseSurvival = ot.Point(distribution.computeInverseSurvivalFunction(0.95))
ott.assert_almost_equal(inverseSurvival, [-2.96551, -5.93727, -0.617253], 1e-5, 0.0)
ott.assert_almost_equal(
    distribution.computeSurvivalFunction(inverseSurvival), 0.95, 1e-6, 1e-6
)
# Confidence regions
if distribution.getDimension() <= 2:
    (
        interval,
        threshold,
    ) = distribution.computeMinimumVolumeIntervalWithMarginalProbability(0.95)
    print("Minimum volume interval=", interval)
    print("threshold=", ot.Point(1, threshold))

mean = distribution.getMean()
ott.assert_almost_equal(mean, [1.5, 0.5, 2.0], 1e-5, 0.0)
covariance = distribution.getCovariance()
ott.assert_almost_equal(
    covariance,
    ot.CovarianceMatrix(
        3, [4.66667, 3.66667, 0.666667, 3.66667, 9.66667, 2.16667, 0.666667, 2.16667, 1.66667]
    ),
    1e-5,
    0.0,
)
parameters = distribution.getParametersCollection()
assert len(parameters) == 4
parameter = distribution.getParameter()
assert len(parameter) == 27
ott.assert_almost_equal(
    parameter,
    [
        0.5, 2.0, -0.5, 3.0, 1.0, 1.0, 0.5, 0.0, 0.5,
        1.5, 2.0, 0.5, 3.0, 2.0, 1.0, 0.5, 0.0, 0.5,
        2.5, 2.0, 1.5, 3.0, 3.0, 1.0, 0.5, 0.0, 0.5,
    ],
    0.0,
    0.0,
)
parameterDescription = distribution.getParameterDescription()
assert len(parameterDescription) == 27
assert list(parameterDescription) == [
    "mu_0", "sigma_0", "mu_1", "sigma_1", "mu_2", "sigma_2", "R_1_0", "R_2_0", "R_2_1",
    "mu_0", "sigma_0", "mu_1", "sigma_1", "mu_2", "sigma_2", "R_1_0", "R_2_0", "R_2_1",
    "mu_0", "sigma_0", "mu_1", "sigma_1", "mu_2", "sigma_2", "R_1_0", "R_2_0", "R_2_1",
]
distribution.setParameter(parameter)
ott.assert_almost_equal(distribution.getParameter(), parameter, 0.0, 0.0)
assert distribution.getStandardRepresentative().getImplementation().getClassName() == "Mixture"

# Constructor with separate weights. Also check small weights removal
weights = [1.0e-20, 2.5, 32.0]
atoms = ot.DistributionCollection(
    [ot.Normal(1.0, 1.0), ot.Normal(2.0, 2.0), ot.Normal(3.0, 3.0)]
)
newMixture = ot.Mixture(atoms, weights)
ott.assert_almost_equal(newMixture.computePDF(2.5), 0.135652911626, 1e-9, 0.0)
assert len(newMixture.getDistributionCollection()) == 2
assert newMixture.getClassName() == "Mixture"
for atom in newMixture.getDistributionCollection():
    assert atom.getImplementation().getClassName() == "Normal"

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.skipMoments()  # slow
validation.skipCorrelation()  # slow
validation.run()

# check for non stricly increasing CDF: must return the inf of [1, 2]
distribution = ot.Mixture([ot.Uniform(0.0, 1.0), ot.Uniform(2.0, 3.0)])
q = distribution.computeQuantile(0.5)[0]
ott.assert_almost_equal(q, 1.0, 1e-6, 1e-6)

# getParametersCollection out-of-bounds when atoms return dimension entries (no copula)
# Dirac atoms return exactly dimension entries from getParametersCollection().
# The bug accesses index 'dimension' which is out of bounds for dimension=2.
atoms = ot.DistributionCollection([ot.Dirac([0.0, 1.0]), ot.Dirac([2.0, 3.0])])
mixture = ot.Mixture(atoms)
coll = mixture.getParametersCollection()
assert len(coll) == 3
ott.assert_almost_equal(coll[0], [0.0], 0.0, 0.0)
ott.assert_almost_equal(coll[1], [2.0], 0.0, 0.0)
assert coll[2].getSize() == 0

# computeConditionalPDF used cumulative conditioningPDF instead of per-atom check
# Verify conditional PDF produces a valid result
atoms = ot.DistributionCollection(
    [ot.Normal([0.0, 0.0], [1.0, 1.0], ot.CorrelationMatrix(2)),
     ot.Normal([3.0, 3.0], [1.0, 1.0], ot.CorrelationMatrix(2))]
)
mixture = ot.Mixture(atoms)
condPDF = mixture.computeConditionalPDF(1.0, [2.0])
ott.assert_almost_equal(condPDF, 0.088283, 1e-6, 1e-6)
seqCondPDF = mixture.computeSequentialConditionalPDF([2.0, 1.0])
ott.assert_almost_equal(seqCondPDF, [0.147981, 0.0882833], 1e-5, 0.0)

# Mixed continuous/discrete drawing, see issue #1489
mixed = ot.Mixture([ot.Dirac(-3.0), ot.Normal()], [0.5, 0.5])
graph = mixed.drawPDF(-6.0, 6.0)
drawables = graph.getDrawables()
# one curve, one shaft, one arrow head and one text label
assert drawables.getSize() == 4, "expected curve plus shaft, head and text"
shaft = drawables[1].getData()
ott.assert_almost_equal(shaft[0], [-3.0, 0.0])
assert shaft[1][0] == -3.0, "atom abscissa"
assert shaft[1][1] > 0.0, "atom shaft height"
head = drawables[2]
head_class = head.getImplementation().getClassName()
assert head_class == "Polygon", "arrow head expected"
headData = head.getData()
assert headData.getSize() == 3, "triangle head"
assert headData[0][0] == -3.0, "apex abscissa"
assert headData[0][1] > shaft[1][1], "head points up"
# a text label gives the probability of the atom above its arrow
label = drawables[3]
assert label.getImplementation().getClassName() == "Text", "text label expected"
annotations = label.getTextAnnotations()
assert len(annotations) == 1, "one annotation expected"
ott.assert_almost_equal(float(annotations[0]), 0.5, 1e-8, 1e-8)
# the arrow height matches the peak of the weighted continuous density
peak = ot.Normal().computePDF([0.0])
half = ot.Mixture([ot.Dirac(-3.0), ot.Normal()], [0.25, 0.75])
h1 = mixed.drawPDF(-6.0, 6.0).getDrawable(1).getData()[1, 1]
h2 = half.drawPDF(-6.0, 6.0).getDrawable(1).getData()[1, 1]
ott.assert_almost_equal(h1, 0.5 * peak, 1e-6, 1e-6)
ott.assert_almost_equal(h2, 0.75 * peak, 1e-6, 1e-6)
# log scale is applied to the mixed drawing as well, see #1489
assert mixed.drawPDF(-6.0, 6.0, 129, True).getLogScale() == ot.GraphImplementation.LOGX
# several atoms
mixed3 = ot.Mixture(
    [ot.Dirac(-3.0), ot.Dirac(3.0), ot.Normal()], [0.25, 0.25, 0.5]
)
n_drawables = mixed3.drawPDF(-6.0, 6.0).getDrawables().getSize()
assert n_drawables == 6, "two arrows and their text label expected"
# purely continuous mixtures keep the generic drawing
continuous = ot.Mixture(
    [ot.Normal(-1.0, 1.0), ot.Normal(1.0, 1.0)], [0.5, 0.5]
)
n_drawables = continuous.drawPDF(-6.0, 6.0).getDrawables().getSize()
assert n_drawables == 1, "no atom expected"

# generic discrete components also get arrows, see issue #1596
geometric = ot.Geometric(0.1)
mixedGeo = ot.Mixture([geometric, ot.Normal(3.0, 1.0)], [0.7, 0.3])
ggeo = mixedGeo.drawPDF(-2.0, 10.0)
supportInRange = geometric.getSupport(ot.Interval([-2.0], [10.0]))
expected = 2 + 2 * supportInRange.getSize()
assert ggeo.getDrawables().getSize() == expected, "one arrow per support point"

# Junction value of adjacent components, see #2928: the density at the
# shared endpoint sums both sides, with the uniform edge at full height
beta = math.log(10.0)
mc = 5.5
delta = 0.9
junction = ot.Mixture(
    [
        ot.TruncatedDistribution(
            ot.Exponential(beta), mc, ot.TruncatedDistribution.UPPER
        ),
        ot.Uniform(mc, 7.5),
    ],
    [delta, 1.0 - delta],
)
ott.assert_almost_equal(junction.computePDF(mc), 0.05, 0.0, 1e-4)

# --- Mixture: equality sees weights, singularities of atoms are safe ---
assert ot.Mixture([ot.Normal(), ot.Normal()], [0.3, 0.7]) != ot.Mixture(
    [ot.Normal(), ot.Normal()], [0.6, 0.4]
)
assert ot.Mixture([ot.Normal(), ot.Normal()], [0.3, 0.7]) == ot.Mixture(
    [ot.Normal(), ot.Normal()], [0.3, 0.7]
)
assert list(ot.Mixture([ot.Dirac(1.0), ot.Dirac(1.0)], [0.5, 0.5]).getSingularities()) == []
assert list(
    ot.Mixture([ot.Dirac(1.0), ot.Uniform(0.0, 3.0)], [0.5, 0.5]).getSingularities()
) == [1.0]
