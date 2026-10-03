#! /usr/bin/env python

import math
import pickle
from io import BytesIO

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)

# default constructor is a valid 1D state (needed by doc plots)
defaultDistribution = otexp.ChristoffelDistribution()
assert defaultDistribution.getDimension() == 1
assert defaultDistribution.getSize() == 1
ott.assert_almost_equal(defaultDistribution.computePDF([0.0]), 0.5)
assert isinstance(defaultDistribution.drawPDF(), ot.Graph)
assert isinstance(defaultDistribution.drawCDF(), ot.Graph)

# orthonormal Legendre basis on Uniform(-1, 1)
factory = ot.OrthogonalProductPolynomialFactory([ot.Uniform(-1.0, 1.0)])
basis = ot.OrthogonalBasis(factory)
distribution = otexp.ChristoffelDistribution(basis, 3)

# accessors
assert distribution.getDimension() == 1
assert distribution.getSize() == 3
assert distribution.getMeasure().getDimension() == 1
assert distribution.getBasis().getSize() == 3
assert distribution.isContinuous()
assert not distribution.isDiscrete()
assert "ChristoffelDistribution" in repr(distribution)
assert "ChristoffelDistribution" in str(distribution)
assert distribution.getOrthogonalBasis() == basis

# Christoffel function: k_3(0.5) = 1 + 3*0.5**2 + 5*P_2(0.5)**2
# with P_2(0.5) = -0.125 (orthonormal Legendre: L_2 = sqrt(5)*P_2),
# hence k_3(0.5) = 1.828125
ott.assert_almost_equal(distribution.computeChristoffel([0.5]), 1.828125)
ott.assert_almost_equal(distribution.computeChristoffel(ot.Sample([[0.5]]))[0, 0], 1.828125)
# the stable logarithm agrees with the logarithm of the direct value on tame points
ott.assert_almost_equal(distribution.computeLogChristoffel([0.5]), math.log(1.828125))
ott.assert_almost_equal(distribution.computeLogChristoffel(ot.Sample([[0.5]]))[0, 0], math.log(1.828125))

# PDF: 0.5 * 1.828125 / 3
ott.assert_almost_equal(distribution.computePDF([0.5]), 0.3046875)
ott.assert_almost_equal(distribution.computeLogPDF([0.5]), math.log(0.3046875))
assert distribution.computePDF([2.0]) == 0.0

# stability factor: 1 + 3 + 5 = 9.0 at the endpoints; the exact mode
# (candidate pool + multi-start maximization) covers it up to the margin
kn = distribution.computeKn()
assert 9.0 <= kn <= 9.0 * 1.1 * (1.0 + 1e-06)

# the PDF integrates to 1 over the range
probability = ot.GaussLegendre([41]).integrate(distribution.getPDF(), ot.Interval([-1.0], [1.0]))[0]
ott.assert_almost_equal(probability, 1.0)

# sampling stays inside the range
sample = distribution.getSample(10)
assert sample.getSize() == 10
assert sample.getDimension() == 1
for point in sample:
    assert -1.0 <= point[0] <= 1.0

# sequential draws target a symmetric law of mean 0
bigSample = distribution.getSample(200)
assert abs(bigSample.computeMean()[0]) < 0.2

# clone, comparison and persistence
assert distribution == otexp.ChristoffelDistribution(basis, 3)
assert distribution != otexp.ChristoffelDistribution(basis, 2)
buf = BytesIO()
pickle.dump(distribution, buf)
buf.seek(0)
ott.assert_almost_equal(pickle.load(buf).computePDF([0.5]), 0.3046875)

# errors: null size, discrete reference, wrong point dimension
with ott.assert_raises(TypeError):
    otexp.ChristoffelDistribution(basis, 0)
poissonBasis = ot.OrthogonalBasis(ot.OrthogonalProductPolynomialFactory([ot.Poisson(2.0)]))
with ott.assert_raises(TypeError):
    otexp.ChristoffelDistribution(poissonBasis, 2)
with ott.assert_raises(TypeError):
    distribution.computePDF([0.5, 0.5])
with ott.assert_raises(RuntimeError):
    distribution.getParametersCollection()

# documented ResourceMap defaults
assert ot.ResourceMap.GetAsString("ChristoffelDistribution-OptimizationAlgorithm") == "TNC"
assert ot.ResourceMap.GetAsUnsignedInteger("ChristoffelDistribution-RatioUniformCandidateNumber") == 10000
assert ot.ResourceMap.GetAsUnsignedInteger("ChristoffelDistribution-RatioUniformMaxDimension") == 5
assert ot.ResourceMap.GetAsUnsignedInteger("ChristoffelDistribution-KnSamplingSize") == 100000
assert ot.ResourceMap.GetAsBool("ChristoffelDistribution-ExactKn")
assert ot.ResourceMap.GetAsUnsignedInteger("ChristoffelDistribution-KnMaximumMultiStart") == 16
ott.assert_almost_equal(ot.ResourceMap.GetAsScalar("ChristoffelDistribution-KnSafetyFactor"), 1.1)
assert ot.ResourceMap.GetAsUnsignedInteger("ChristoffelDistribution-SliceGridSize") == 1000

# the Monte-Carlo-only mode cannot exceed the exact estimate
ot.ResourceMap.SetAsBool("ChristoffelDistribution-ExactKn", False)
try:
    knMonteCarlo = otexp.ChristoffelDistribution(basis, 3).computeKn()
finally:
    ot.ResourceMap.SetAsBool("ChristoffelDistribution-ExactKn", True)
assert 8.0 < knMonteCarlo <= kn

# a high-degree normal basis: the numerical range of an unbounded support is stored
# with infinite flags, so the solver used to escape it and overflow the raw function,
# which stalled its progress tests; the search box is now enforced, the objective stays
# in the log domain and the gradient (TNC) comes from the three-term recurrence of the
# factors instead of their monomial coefficients, which lose all their digits in the
# tails of high-degree factors. The pool is kept small here: the search climbs from any
# in-range seed to the range edge, so the assertion does not depend on it
bigBasis = ot.OrthogonalBasis(ot.OrthogonalProductPolynomialFactory([ot.Normal()], ot.LinearEnumerateFunction(1)))
ot.ResourceMap.SetAsUnsignedInteger("ChristoffelDistribution-KnSamplingSize", 10000)
try:
    for solver in ("Cobyla", "TNC"):
        ot.ResourceMap.SetAsString("ChristoffelDistribution-OptimizationAlgorithm", solver)
        try:
            knNormal = otexp.ChristoffelDistribution(bigBasis, 401).computeKn()
        finally:
            ot.ResourceMap.SetAsString("ChristoffelDistribution-OptimizationAlgorithm", "TNC")
        # both solvers reach 1.1 times the supremum of the numerical range, ~8.84e+13
        assert math.isfinite(knNormal)
        assert 8.8e+13 <= knNormal <= 8.9e+13
finally:
    ot.ResourceMap.SetAsUnsignedInteger("ChristoffelDistribution-KnSamplingSize", 100000)

# 2D tensor case: k_2(x) = 1 + 3*x0**2 with the linear enumerate
basis2 = ot.OrthogonalBasis(ot.OrthogonalProductPolynomialFactory([ot.Uniform(-1.0, 1.0), ot.Uniform(-1.0, 1.0)]))
distribution2 = otexp.ChristoffelDistribution(basis2, 2)
ott.assert_almost_equal(distribution2.computePDF([0.5, -0.3]), 0.21875)
# X1 | X0 is uniform here since k_2 ignores x1
ott.assert_almost_equal(distribution2.computeConditionalPDF(0.3, [0.2]), 0.5)
# sequential versions fan out to the scalar ones
point2 = [0.5, -0.3]
seqPDF = distribution2.computeSequentialConditionalPDF(point2)
ott.assert_almost_equal(seqPDF[0], 0.4375)
ott.assert_almost_equal(seqPDF[1], distribution2.computeConditionalPDF(point2[1], [point2[0]]))
seqCDF = distribution2.computeSequentialConditionalCDF(point2)
ott.assert_almost_equal(seqCDF[1], distribution2.computeConditionalCDF(point2[1], [point2[0]]))
# quantile inverts the conditional CDF
level = distribution2.computeConditionalCDF(0.3, [0.2])
ott.assert_almost_equal(distribution2.computeConditionalQuantile(level, [0.2]), 0.3)
# the Christoffel law itself is never independent
assert not distribution2.hasIndependentCopula()
sample2 = distribution2.getSample(5)
assert sample2.getSize() == 5
for point in sample2:
    assert -1.0 <= point[0] <= 1.0 and -1.0 <= point[1] <= 1.0

# correlated reference: ratio-of-uniforms branch (d=2 <= 5)
corr = ot.CorrelationMatrix(2)
corr[0, 1] = 0.5
measureC = ot.Normal([0.0, 0.0], [1.0, 1.0], corr)
hermite = ot.OrthogonalBasis(ot.OrthogonalProductPolynomialFactory([ot.Normal(), ot.Normal()]))
# Orthonormality under a correlated measure is the caller's responsibility:
# FiniteOrthogonalFunctionFactory stores the functions as given (dimension checks
# only), so compose the orthonormal Hermite functions -- orthonormal under the
# standard normal -- with the whitening map w(x) = (x0, (x1 - 0.5*x0) / sqrt(1 - 0.5**2)),
# which pushes measureC (rho = 0.5) back to the standard normal: phi_j(w(x)) is
# then orthonormal under measureC. The trace identity E[k_3] = 3 checked below
# is the cheap numerical check of that construction.
whiten = ot.SymbolicFunction(['x0', 'x1'], ['x0', '(x1 - 0.5 * x0) / 0.8660254037844386'])
corrBasis = ot.OrthogonalBasis(otexp.FiniteOrthogonalFunctionFactory([ot.ComposedFunction(hermite.build(j), whiten) for j in range(3)], measureC))
assert not corrBasis.getMeasure().hasIndependentCopula()
distributionC = otexp.ChristoffelDistribution(corrBasis, 3)
assert distributionC.computePDF([0.1, -0.2]) >= 0.0
sampleC = distributionC.getSample(5)
assert sampleC.getSize() == 5
# trace identity: mean of k_3 under mu is 3 by orthonormality
knVals = distributionC.computeChristoffel(measureC.getSample(2000))
assert 2.5 < knVals.computeMean()[0] < 3.5

# sampling stays available when RoU is disabled by the dimension key
# (the tensor sequential path has priority and remains selected here)
ot.ResourceMap.SetAsUnsignedInteger("ChristoffelDistribution-RatioUniformMaxDimension", 0)
try:
    keySample = distribution.getSample(5)
    assert keySample.getSize() == 5
    for point in keySample:
        assert -1.0 <= point[0] <= 1.0
finally:
    ot.ResourceMap.SetAsUnsignedInteger("ChristoffelDistribution-RatioUniformMaxDimension", 5)

# the crude rejection branch: non-tensor basis with RoU out of range
# forces getRealization() through drawByRejection
ot.ResourceMap.SetAsUnsignedInteger("ChristoffelDistribution-RatioUniformMaxDimension", 0)
try:
    rejSample = distributionC.getSample(5)
    assert rejSample.getSize() == 5
    assert rejSample.getDimension() == 2
    for point in rejSample:
        assert distributionC.computePDF(point) >= 0.0
finally:
    ot.ResourceMap.SetAsUnsignedInteger("ChristoffelDistribution-RatioUniformMaxDimension", 5)

# dispatch by measured acceptance, rejection wins: with size=1 the Christoffel
# function is identically 1, so rejection accepts at 1/kn = 1/1.1, above the
# ratio-of-uniforms acceptance of the reference itself, and the comparison must
# select rejection even though the dimension guard allows ratio-of-uniforms.
# The sample is pinned to the rejection loop (proposal, envelope test) replayed
# under the same seed, which no ratio-of-uniforms draw can match.
basis1 = ot.OrthogonalBasis(otexp.FiniteOrthogonalFunctionFactory([ot.ComposedFunction(hermite.build(0), whiten)], measureC))
ot.ResourceMap.SetAsUnsignedInteger("ChristoffelDistribution-KnSamplingSize", 10000)
try:
    dist1 = otexp.ChristoffelDistribution(basis1, 1)
    envelope1 = dist1.computeKn()
    assert abs(envelope1 - 1.1) < 1e-12
    ot.RandomGenerator.SetSeed(0)
    pinned = dist1.getSample(5)
    ot.RandomGenerator.SetSeed(0)
    replay = []
    while len(replay) < 5:
        proposal = measureC.getRealization()
        if ot.RandomGenerator.Generate() * envelope1 <= float(dist1.computeChristoffel(proposal)):
            replay.append(proposal)
    assert pinned.getSize() == 5
    assert [list(point) for point in pinned] == [list(point) for point in replay]

    # dispatch by measured acceptance, ratio-of-uniforms wins: the correlated
    # case above has acceptance 0.407 against 3/258.64 = 0.0116 for rejection, so
    # its sample must match the one an externally built sampler draws under the
    # same seed -- computing the stability factor consumes no random number
    externalRoU = ot.RatioOfUniforms()
    externalRoU.setCandidateNumber(ot.ResourceMap.GetAsUnsignedInteger("ChristoffelDistribution-RatioUniformCandidateNumber"))
    externalRoU.setOptimizationAlgorithm(ot.OptimizationAlgorithm.GetByName(ot.ResourceMap.GetAsString("ChristoffelDistribution-OptimizationAlgorithm")))
    externalRoU.setLogUnscaledPDFAndRange(distributionC.getLogPDF(), distributionC.getRange(), True)
    distributionC.computeKn()
    ot.RandomGenerator.SetSeed(1)
    expectedRoU = externalRoU.getSample(5)
    ot.RandomGenerator.SetSeed(1)
    actualRoU = distributionC.getSample(5)
    assert [list(point) for point in actualRoU] == [list(point) for point in expectedRoU]

    # a failing ratio-of-uniforms setup must not break the constructor: the
    # sampler stays unset and sampling falls back to rejection (here the key
    # asks for zero candidates, which the sampler rejects)
    ot.ResourceMap.SetAsUnsignedInteger("ChristoffelDistribution-RatioUniformCandidateNumber", 0)
    try:
        fallback = otexp.ChristoffelDistribution(corrBasis, 3)
        fbSample = fallback.getSample(5)
        assert fbSample.getSize() == 5
        for point in fbSample:
            assert fallback.computePDF(point) >= 0.0
    finally:
        ot.ResourceMap.SetAsUnsignedInteger("ChristoffelDistribution-RatioUniformCandidateNumber", 10000)
finally:
    ot.ResourceMap.SetAsUnsignedInteger("ChristoffelDistribution-KnSamplingSize", 100000)

# --- generic paths of the reference workflows ---

# setOrthogonalBasis rebuilds every derived member: tensor -> non-tensor -> tensor
rebuilt = otexp.ChristoffelDistribution()
rebuilt.setOrthogonalBasis(basis, 3)
ott.assert_almost_equal(rebuilt.computePDF([0.5]), 0.3046875)
rebuilt.setOrthogonalBasis(corrBasis, 3)
assert rebuilt.computePDF([0.1, -0.2]) >= 0.0
rebuilt.setOrthogonalBasis(basis2, 2)
ott.assert_almost_equal(rebuilt.computePDF([0.5, -0.3]), 0.21875)
with ott.assert_raises(TypeError):
    rebuilt.setOrthogonalBasis(basis, 0)

# no parametric representation in either direction
with ott.assert_raises(RuntimeError):
    distribution.setParametersCollection([[1.0]])

# the base conditional machinery handles a non-tensor reference
condPDFC = distributionC.computeConditionalPDF(0.3, [0.1])
assert condPDFC >= 0.0
condCDFC = distributionC.computeConditionalCDF(0.3, [0.1])
assert 0.0 <= condCDFC <= 1.0
seqPDFC = distributionC.computeSequentialConditionalPDF([0.1, -0.2])
assert all(value >= 0.0 for value in seqPDFC)
seqCDFC = distributionC.computeSequentialConditionalCDF([0.1, -0.2])
assert all(0.0 <= value <= 1.0 for value in seqCDFC)

# an unknown solver name falls back to the Monte-Carlo candidate estimate
ot.ResourceMap.SetAsString("ChristoffelDistribution-OptimizationAlgorithm", "NoSuchSolver")
try:
    knFallback = otexp.ChristoffelDistribution(basis, 3).computeKn()
finally:
    ot.ResourceMap.SetAsString("ChristoffelDistribution-OptimizationAlgorithm", "TNC")
assert 8.0 < knFallback <= kn

# a tensor function factory builds the cached factors from the function families
fourierBasis = ot.OrthogonalBasis(ot.OrthogonalProductFunctionFactory([ot.FourierSeriesFactory()] * 2))
fourierDistribution = otexp.ChristoffelDistribution(fourierBasis, 2)
assert fourierDistribution.getDimension() == 2
assert fourierDistribution.computePDF([0.5, -0.3]) >= 0.0
seqFourier = fourierDistribution.computeSequentialConditionalCDF([0.5, -0.3])
assert all(0.0 <= value <= 1.0 for value in seqFourier)
fourierSample = fourierDistribution.getSample(5)
assert fourierSample.getSize() == 5
for point in fourierSample:
    assert -math.pi <= point[0] <= math.pi and -math.pi <= point[1] <= math.pi

# the adaptive slice restart: a low safety leaves the grid envelope below the
# density of the proposals near the mode, which restarts the loop
ot.ResourceMap.SetAsScalar("ChristoffelDistribution-KnSafetyFactor", 0.5)
try:
    tightSlice = distribution.getSample(50)
    assert tightSlice.getSize() == 50
finally:
    ot.ResourceMap.SetAsScalar("ChristoffelDistribution-KnSafetyFactor", 1.1)

# the rejection restart: a single-point estimate plus the dimension cap leave
# the envelope far below k, so the first proposals above it restart the loop
ot.ResourceMap.SetAsBool("ChristoffelDistribution-ExactKn", False)
ot.ResourceMap.SetAsUnsignedInteger("ChristoffelDistribution-KnSamplingSize", 1)
ot.ResourceMap.SetAsUnsignedInteger("ChristoffelDistribution-RatioUniformMaxDimension", 0)
try:
    tightRejection = otexp.ChristoffelDistribution(corrBasis, 3)
    tightSample = tightRejection.getSample(20)
    assert tightSample.getSize() == 20
    for point in tightSample:
        assert tightRejection.computePDF(point) >= 0.0
finally:
    ot.ResourceMap.SetAsBool("ChristoffelDistribution-ExactKn", True)
    ot.ResourceMap.SetAsUnsignedInteger("ChristoffelDistribution-KnSamplingSize", 100000)
    ot.ResourceMap.SetAsUnsignedInteger("ChristoffelDistribution-RatioUniformMaxDimension", 5)

# the standard validation battery: checks of the overloaded methods plus the
# cheap generic ones; the law has no parameters and no analytic gradients, and
# the moments blocks need a 10^6-point sample (~3 minutes)
ot.RandomGenerator.SetSeed(0)
validation = ott.DistributionValidation(distribution2)
validation.skipParameters()
validation.skipGradient()
validation.skipMoments()
validation.skipCorrelation()
validation.run()
