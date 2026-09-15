#! /usr/bin/env python

import pickle

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)

# Instantiate one distribution object
f = ot.SymbolicFunction("x", "x^2 + 2 * sin(x)")
distribution = ot.CompositeDistribution(f, ot.Normal())
print("Distribution ", repr(distribution))
print("Distribution ", distribution)

# Is this distribution elliptical ?
print("Elliptical = ", distribution.isElliptical())

# Is this distribution continuous ?
print("Continuous = ", distribution.isContinuous())

# Test for realization of distribution
oneRealization = distribution.getRealization()
print("oneRealization=", oneRealization)

# Test for sampling
size = 10000
oneSample = distribution.getSample(size)
print("oneSample first=", oneSample[0], " last=", oneSample[size - 1])
print("mean=", oneSample.computeMean())
print("covariance=", oneSample.computeCovariance())

# Define a point
point = ot.Point(distribution.getDimension(), 1.0)
print("Point= ", point)

# Show PDF and CDF of point
DDF = distribution.computeDDF(point)
print("ddf     =", DDF)
LPDF = distribution.computeLogPDF(point)
print("log pdf= %.12g" % LPDF)
PDF = distribution.computePDF(point)
print("pdf     = %.10g" % PDF)

CDF = distribution.computeCDF(point)
print("cdf= %.12g" % CDF)
CCDF = distribution.computeComplementaryCDF(point)
print("ccdf= %.12g" % CCDF)
Survival = distribution.computeSurvivalFunction(point)
print("survival= %.12g" % Survival)
quantile = distribution.computeQuantile(0.95)
print("quantile=", quantile)
print("cdf(quantile)= %.12g" % distribution.computeCDF(quantile))
quantileTail = distribution.computeQuantile(0.95, True)
print("quantile (tail)=", quantileTail)
CDFTail = distribution.computeComplementaryCDF(quantileTail)
print("cdf (tail)= %.12g" % CDFTail)
# Get 95% survival function
inverseSurvival = ot.Point(distribution.computeInverseSurvivalFunction(0.95))
print("InverseSurvival=", repr(inverseSurvival))
print(
    "Survival(inverseSurvival)=%.6f"
    % distribution.computeSurvivalFunction(inverseSurvival)
)

# Confidence regions
interval, threshold = distribution.computeMinimumVolumeIntervalWithMarginalProbability(
    0.95
)
print("Minimum volume interval=", interval)
print("threshold=", ot.Point(1, threshold))
levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
print("Minimum volume level set=", levelSet)
print("beta=", ot.Point(1, beta))
interval, beta = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(
    0.95
)
print("Bilateral confidence interval=", interval)
print("beta=", ot.Point(1, beta))
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, False)
print("Unilateral confidence interval (lower tail)=", interval)
print("beta=", ot.Point(1, beta))
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, True)
print("Unilateral confidence interval (upper tail)=", interval)
print("beta=", ot.Point(1, beta))

CF = distribution.computeCharacteristicFunction(point[0])
print("characteristic function=(%.6g+%.6gj)" % (CF.real, CF.imag))
LCF = distribution.computeLogCharacteristicFunction(point[0])
print("log characteristic function=(%.6g+%.6gj)" % (LCF.real, LCF.imag))
PDFgr = distribution.computePDFGradient(point)
print("pdf gradient     =", PDFgr)
CDFgr = distribution.computeCDFGradient(point)
print("cdf gradient     =", CDFgr)
mean = distribution.getMean()
print("mean=", mean)
standardDeviation = distribution.getStandardDeviation()
print("standard deviation=", standardDeviation)
skewness = distribution.getSkewness()
print("skewness=", skewness)
kurtosis = distribution.getKurtosis()
print("kurtosis=", kurtosis)
covariance = distribution.getCovariance()
print("covariance=", covariance)
correlation = distribution.getCorrelation()
print("correlation=", correlation)
spearman = distribution.getSpearmanCorrelation()
print("spearman=", spearman)
kendall = distribution.getKendallTau()
print("kendall=", kendall)
parameters = distribution.getParametersCollection()
print("parameters=", parameters)
print("Standard representative=", distribution.getStandardRepresentative())

# Specific to this distribution
antecedent = distribution.getAntecedent()
print("antecedent=", antecedent)
function = distribution.getFunction()
print("function=", function)
newDistribution = ot.CompositeDistribution(function, antecedent)
print("newDistribution=", newDistribution)

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.skipEntropy()  # slow, uses Monte-Carlo
validation.skipPDFAtLowerBound()  # Testing that the PDF is null at its lower bound if finite is a complex task
validation.run()

# Check that the PDF is zero where it is given by a fraction with null denominator
pdf = ot.Uniform(-5.0, 5.0).sqr().computePDF(0.0)
assert pdf == 0.0, f"Expected a zero value for the PDF, got PDF={pdf}"

# verify PDF at segment endpoints is correctly computed (not excluded by strict <)
# Increasing function f(x)=x, Uniform(0,1) -> PDF should be 1.0 at both range endpoints
antecedent = ot.Uniform(0.0, 1.0)
f_inc = ot.SymbolicFunction(['x'], ['x'])
d_inc = ot.CompositeDistribution(f_inc, antecedent)
eps = ot.ResourceMap.GetAsScalar('Distribution-DefaultQuantileEpsilon')
lb = antecedent.getRange().getLowerBound()[0] + eps
ub = antecedent.getRange().getUpperBound()[0] - eps
pdf_lb = d_inc.computePDF([f_inc([lb])[0]])
pdf_ub = d_inc.computePDF([f_inc([ub])[0]])
ott.assert_almost_equal(pdf_lb, 1.0)
ott.assert_almost_equal(pdf_ub, 1.0)

# Decreasing function f(x)=1-x, Uniform(0,1) -> PDF should be 1.0 at both range endpoints
f_dec = ot.SymbolicFunction(['x'], ['1-x'])
d_dec = ot.CompositeDistribution(f_dec, antecedent)
pdf_ub2 = d_dec.computePDF([f_dec([lb])[0]])
pdf_lb2 = d_dec.computePDF([f_dec([ub])[0]])
ott.assert_almost_equal(pdf_ub2, 1.0)
ott.assert_almost_equal(pdf_lb2, 1.0)

# Equality tests
d1 = ot.CompositeDistribution(f, ot.Normal())
assert d1 == distribution, "same function and antecedent should be equal"
assert not (d1 != distribution), "same should not be different"
f2 = ot.SymbolicFunction("x", "x^3")
d2 = ot.CompositeDistribution(f2, ot.Normal())
assert not (d1 == d2), "different function should not be equal"
assert d1 != d2, "different function should be different"
d3 = ot.CompositeDistribution(f, ot.Uniform())  # different antecedent
assert not (d1 == d3), "different antecedent should not be equal"

# Nesting composite distributions composes the functions and keeps a
# non-composite antecedent, see issue #1479
g = ot.SymbolicFunction(["x"], ["sin(x) + cos(x)"])
h = ot.SymbolicFunction(["x"], ["x^3"])
distY = ot.CompositeDistribution(g, ot.Normal(1.0, 0.5))
distZ = ot.CompositeDistribution(h, distY)
antecedent_class = distZ.getAntecedent().getImplementation().getClassName()
assert antecedent_class != "CompositeDistribution", "antecedent not composite"
# for increasing h, F_Z(z) = F_Y(h^-1(z))
z = 2.0
hinv = z ** (1.0 / 3.0)
ott.assert_almost_equal(distZ.computeCDF([z]), distY.computeCDF([hinv]))
# same law as the explicitly composed version
expected = ot.CompositeDistribution(
    ot.ComposedFunction(h, distY.getFunction()), distY.getAntecedent()
)
ott.assert_almost_equal(distZ.computeCDF([1.2]), expected.computeCDF([1.2]))
ott.assert_almost_equal(distZ.computePDF([1.2]), expected.computePDF([1.2]))

# solver precision is rescaled to the range of the flattened, non-composite
# antecedent, so that a large-scale inner transformation does not leave a
# solver precision scaled to the outer composite range, see issue #1479
smallAnt = ot.Uniform(0.0, 1e-3)
bigAnt = ot.CompositeDistribution(ot.SymbolicFunction(["x"], ["1e6 * x"]), smallAnt)
assert bigAnt.getRange().getUpperBound()[0] > 10.0, "a relatively large range was expected"
scaled = ot.CompositeDistribution(ot.SymbolicFunction(["x"], ["x"]), bigAnt)
scale = ot.ResourceMap.GetAsScalar("CompositeDistribution-SolverEpsilon")
anteRange = scaled.getAntecedent().getRange()
offset = scale * (anteRange.getUpperBound()[0] - anteRange.getLowerBound()[0])
ott.assert_almost_equal(scaled.getSolver().getAbsoluteError(), offset, 1e-12, 1e-12)
# the flattened law is Y = 1e6 * U with U uniform on [0, 1e-3]
for y in [250.0, 500.0, 750.0]:
    ott.assert_almost_equal(scaled.computeCDF([y]), y / 1000.0, 1e-8, 1e-4)
# Copy constructor and assignment
d_copy = ot.CompositeDistribution(d1)
assert d_copy == d1, "copy constructor should preserve equality"
d_assign = ot.CompositeDistribution(d_copy)
d_assign.setFunction(f2)
assert d_assign != d1, "setFunction should update the distribution"

# Constructor argument checks
with ott.assert_raises(Exception):
    ot.CompositeDistribution(ot.SymbolicFunction(["x", "y"], ["x+y"]), ot.Normal())  # wrong input dim
with ott.assert_raises(Exception):
    ot.CompositeDistribution(ot.SymbolicFunction("x", ["x", "x^2"]), ot.Normal())  # wrong output dim
with ott.assert_raises(Exception):
    ot.CompositeDistribution(ot.SymbolicFunction("x", "x^2"), ot.Normal(2))  # wrong antecedent dim

# Analytic reference: x^2 with X~N(0,1) is exactly a Chi2(1) distribution
# (up to numerical precision of the truncated antecedent range)
f_sq = ot.SymbolicFunction("x", "x^2")
d_sq = ot.CompositeDistribution(f_sq, ot.Normal())
chi2 = ot.ChiSquare(1.0)
for x_point in [0.1, 0.5, 1.0, 2.0, 4.0]:
    ott.assert_almost_equal(d_sq.computePDF(x_point), chi2.computePDF(x_point), 1e-5, 1e-5)
    ott.assert_almost_equal(d_sq.computeCDF(x_point), chi2.computeCDF(x_point), 1e-5, 1e-5)
    ott.assert_almost_equal(d_sq.computeSurvivalFunction(x_point), chi2.computeSurvivalFunction(x_point), 1e-5, 1e-5)
ott.assert_almost_equal(d_sq.getMean()[0], chi2.getMean()[0], 1e-4, 1e-4)
ott.assert_almost_equal(d_sq.getStandardDeviation()[0], chi2.getStandardDeviation()[0], 1e-4, 1e-4)
ott.assert_almost_equal(d_sq.getSkewness()[0], chi2.getSkewness()[0], 1e-4, 1e-4)
ott.assert_almost_equal(d_sq.getKurtosis()[0], chi2.getKurtosis()[0], 1e-4, 1e-4)

# Analytic reference: exp(x) with X~N(0,1) is exactly a LogNormal(0,1)
f_exp = ot.SymbolicFunction("x", "exp(x)")
d_exp = ot.CompositeDistribution(f_exp, ot.Normal())
logn = ot.LogNormal(0.0, 1.0)
ott.assert_almost_equal(d_exp.getMean()[0], logn.getMean()[0], 1e-4, 1e-4)
ott.assert_almost_equal(d_exp.getStandardDeviation()[0], logn.getStandardDeviation()[0], 1e-4, 1e-4)

# Analytic reference: identity with X~N(0,1) is exactly N(0,1)
f_id = ot.SymbolicFunction("x", "x")
d_id = ot.CompositeDistribution(f_id, ot.Normal())
norm = ot.Normal()
for x_point in [-2.0, -1.0, 0.0, 1.0, 2.0]:
    ott.assert_almost_equal(d_id.computePDF(x_point), norm.computePDF(x_point), 1e-5, 1e-5)
    ott.assert_almost_equal(d_id.computeCDF(x_point), norm.computeCDF(x_point), 1e-5, 1e-5)
    ott.assert_almost_equal(d_id.computeQuantile(0.75)[0], norm.computeQuantile(0.75)[0], 1e-5, 1e-5)

# getSingularities: must return images of critical points (where f'=0)
# x^2: f'(x)=2x=0 at x=0, singularity at y=f(0)=0
sing = d_sq.getSingularities()
ott.assert_almost_equal(sing.getSize(), 1)
ott.assert_almost_equal(sing[0], 0.0)

# x^3-3x: f'(x)=3x^2-3=0 at x=+/-1, singularities at y=f(-1)=2, y=f(1)=-2
f_pl = ot.SymbolicFunction("x", "x^3 - 3*x")
d_pl = ot.CompositeDistribution(f_pl, ot.Normal())
sing_pl = d_pl.getSingularities()
ott.assert_almost_equal(sing_pl.getSize(), 2)
ott.assert_almost_equal(sing_pl, [-2.0, 2.0])  # sorted ascending

# sin: f'(x)=cos(x)=0 at pi/2, 3pi/2, singularities at y=...=+/-1
f_sin = ot.SymbolicFunction("x", "sin(x)")
d_sin = ot.CompositeDistribution(f_sin, ot.Uniform(0, 6.283185307))
sing_sin = d_sin.getSingularities()
ott.assert_almost_equal(sing_sin.getSize(), 2)
ott.assert_almost_equal(sing_sin, [-1.0, 1.0])

# exp: f'(x)=exp(x)>0 everywhere, no critical points, empty singularities
sing_exp = d_exp.getSingularities()
ott.assert_almost_equal(sing_exp.getSize(), 0)

# identity: f'(x)=1, no critical points, empty singularities
sing_id = d_id.getSingularities()
ott.assert_almost_equal(sing_id.getSize(), 0)

# computePDF at a singularity must be finite: the singularities are points
# where the PDF is undefined/infinite, but the solver-based summation keeps
# the result finite in double precision
pdf_sing = d_sq.computePDF(0.0)
assert pdf_sing == 0.0, f"x^2 PDF at singularity should vanish, got {pdf_sing}"

# computeProbability: segment-wise summation avoids GK divergence at singular endpoints
# x^2 P[0,1] = P(-1<=X<=1) for X~N(0,1)
p_sq = d_sq.computeProbability(ot.Interval([0.0], [1.0]))
ott.assert_almost_equal(p_sq, norm.computeProbability(ot.Interval([-1.0], [1.0])), 1e-6, 1e-8)

# Full range probability should be 1.0
p_full = d_sin.computeProbability(ot.Interval([-2.0], [2.0]))
ott.assert_almost_equal(p_full, 1.0, 1e-3, 0.0)

# Inverted or degenerate intervals give probability 0
ott.assert_almost_equal(d_sq.computeProbability(ot.Interval([1.0], [0.0])), 0.0)
ott.assert_almost_equal(d_sq.computeProbability(ot.Interval([1.0], [1.0])), 0.0)

# Intervals lying outside the range give probability 0
ott.assert_almost_equal(d_sq.computeProbability(ot.Interval([100.0], [200.0])), 0.0)
ott.assert_almost_equal(d_sq.computeProbability(ot.Interval([-10.0], [-5.0])), 0.0)

# computeProbability must be consistent with the CDF: P(-inf, x] == CDF(x)
for x_point in [-1.0, 0.0, 1.0, 2.0]:
    p_interval = d_sq.computeProbability(ot.Interval([-1e6], [x_point]))
    ott.assert_almost_equal(p_interval, d_sq.computeCDF(x_point), 1e-6, 1e-8)

# Sub-additivity and total mass
p_mid = d_sq.computeProbability(ot.Interval([0.0], [2.0]))
p_tail = d_sq.computeProbability(ot.Interval([2.0], [1e6]))
ott.assert_almost_equal(p_mid + p_tail, 1.0, 1e-6, 1e-8)

# sin P[-1,0] = 0.5 (half the support maps to [-1,0])
p_sin1 = d_sin.computeProbability(ot.Interval([-1.0], [0.0]))
ott.assert_almost_equal(p_sin1, 0.5, 1e-3, 0.0)

# sin P[0.5,1] = 1/3 (arcsine-symmetric)
p_sin2 = d_sin.computeProbability(ot.Interval([0.5], [1.0]))
ott.assert_almost_equal(p_sin2, 1.0 / 3.0, 1e-3, 0.0)

# sin P[-0.99,-0.5] should be finite and strictly below 1 (the old GK bug gave 1.0)
p_sin3 = d_sin.computeProbability(ot.Interval([-0.99], [-0.5]))
assert p_sin3 > 0.0 and p_sin3 < 1.0, f"sin P[-0.99,-0.5]={p_sin3} should be in (0,1)"

# computePDF must sum to 1 when integrated over its full range:
# PDF at 0.5 for sin: 2 preimages, each with |cos(x)|=sqrt(3)/2, pdf=1/(2pi)*(2/|cos|)=4/(2pi sqrt(3))
ott.assert_almost_equal(
    d_sin.computePDF(0.5),
    4.0 / (2.0 * 6.283185307 * 0.8660254037844387),
    1e-4,
    1e-3,
)

# Entropy of exp(N(0,1)) should be finite and close to the LogNormal analytic value
entropy_analytic = logn.computeEntropy()
e_exp = d_exp.computeEntropy()
assert e_exp < 10.0, f"exp(N(0,1)) entropy={e_exp} should be finite"
ott.assert_almost_equal(e_exp, entropy_analytic, 1e-2, 1e-2)

# Realizations must lie within the range
sample_sq = d_sq.getSample(1000)
lower = sample_sq.getMin()[0]
upper = sample_sq.getMax()[0]
assert lower >= 0.0, f"x^2 realizations must be non-negative, got {lower}"
assert sample_sq.computeMean()[0] > 0.0, "x^2 sample mean should be positive"

# Minimum volume interval must have the requested marginal probability (exactly)
interval_mvi, marginal = d_sq.computeMinimumVolumeIntervalWithMarginalProbability(0.95)
ott.assert_almost_equal(d_sq.computeProbability(interval_mvi), 0.95, 1e-2, 1e-3)

# PDF must be zero outside the range
ott.assert_almost_equal(d_sq.computePDF(-1.0), 0.0)
ott.assert_almost_equal(d_id.computePDF(100.0), 0.0)

# CDF must be 0/1 outside the range
ott.assert_almost_equal(d_id.computeCDF(-100.0), 0.0)
ott.assert_almost_equal(d_id.computeCDF(100.0), 1.0)

# Check the antecedent and function accessors
antecedent = distribution.getAntecedent()
function = distribution.getFunction()
x_ant = antecedent.getMean()[0]
ott.assert_almost_equal(function(ot.Point([x_ant]))[0], f(ot.Point([x_ant]))[0])

# setAntecedent / setFunction must recompute singularities and range
d_mut = ot.CompositeDistribution(f_sq, ot.Normal())
d_mut.setAntecedent(ot.Normal(2.0, 1.0))
ott.assert_almost_equal(d_mut.getMean()[0], chi2.getMean()[0] + 4.0, 1e-2, 1e-2)
d_mut.setFunction(f_id)
ott.assert_almost_equal(d_mut.getMean()[0], 2.0, 1e-3, 1e-3)
# setFunction to a function with no critical points removes singularities
d_mut.setFunction(f_exp)
ott.assert_almost_equal(d_mut.getSingularities().getSize(), 0)

# setSolver changes the solver
d_solv = ot.CompositeDistribution(f_sin, ot.Uniform(0.0, 6.283185307))
brent = ot.Brent(1e-4, 1e-6, 1e-6)
d_solv.setSolver(brent)
ott.assert_almost_equal(d_solv.getSolver().getAbsoluteError(), brent.getAbsoluteError())
ott.assert_almost_equal(d_solv.getSolver().getRelativeError(), brent.getRelativeError())
ott.assert_almost_equal(d_solv.computeCDF(0.0), d_sin.computeCDF(0.0), 1e-6, 1e-8)
ott.assert_almost_equal(d_solv.computeCDF(0.0), d_sin.computeCDF(0.0), 1e-6, 1e-8)

# Serialization round trip
d_ser = pickle.loads(pickle.dumps(d_sq))
ott.assert_almost_equal(d_ser.getSingularities(), d_sq.getSingularities())
ott.assert_almost_equal(d_ser.computeCDF(1.0), d_sq.computeCDF(1.0), 1e-8, 1e-8)

# Composite chain: mean of g(U) with U~Uniform(0,1), g(x)=x^3 = integral x^3 = 1/4
d_uni = ot.CompositeDistribution(ot.SymbolicFunction("x", "x^3"), ot.Uniform(0.0, 1.0))
ott.assert_almost_equal(d_uni.getMean()[0], 0.25, 1e-4, 1e-4)

# Support on a negative range: g(x) = x^3 - 3x on x in [-2, 2] has range [-2, 2] and
# its PDF must vanish outside
d_neg = ot.CompositeDistribution(f_pl, ot.Uniform(-2.0, 2.0))
range_neg = d_neg.getRange()
ott.assert_almost_equal(range_neg.getLowerBound()[0], -2.0)
ott.assert_almost_equal(range_neg.getUpperBound()[0], 2.0)
ott.assert_almost_equal(d_neg.computePDF(-3.0), 0.0)
ott.assert_almost_equal(d_neg.computePDF(3.0), 0.0)
