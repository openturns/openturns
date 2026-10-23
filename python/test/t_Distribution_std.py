import cmath
import math

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

# comparison via implementation
assert ot.Triangular() == ot.Triangular(), "Triangular==Triangular"
assert not ot.Rice() != ot.Rice(), "Rice!=Rice"
assert ot.Arcsine() != ot.Gumbel(), "Arcsine!=Gumbel"
assert ot.Gumbel() != ot.Arcsine(), "Arcsine!=Gumbel"
assert not ot.Gumbel() == ot.Arcsine(), "Arcsine==Gumbel"
assert ot.Exponential(2.5) != ot.Exponential(3.0), "Exponential(2.5)!=Exponential(3.0)"
assert not ot.Exponential(2.5) == ot.Exponential(
    3.0
), "Exponential(2.5)==Exponential(3.0)"
assert ot.JointDistribution([ot.Normal()] * 2) == ot.Normal(
    2
), "Normal(2)==Composed(2N)"
assert not ot.JointDistribution([ot.Normal()] * 2) != ot.Normal(
    2
), "Normal(2)!=Composed(2N)"
assert ot.JointDistribution([ot.Normal()] * 2) != ot.Normal(
    3
), "Composed(2N)!=Normal(3)"
assert not ot.JointDistribution([ot.Normal()] * 2) == ot.Normal(
    3
), "!Composed(2N)==Normal(3)"
assert ot.Normal(2) == ot.JointDistribution(
    [ot.Normal()] * 2
), "Normal(2)==Composed(2N)"
assert not ot.Normal(3) == ot.JointDistribution(
    [ot.Normal()] * 2
), "Normal(3)==Composed(2N)"
assert ot.Normal(3) != ot.JointDistribution(
    [ot.Normal()] * 2
), "Normal(3)!=Composed(2N)"
assert ot.Normal(2) != ot.Student(3.0, 2), "Normal(2)!=Student(2)"
assert not ot.Student(3.0, 2) == ot.Normal(2), "!Student(2)==Normal(2)"

factories = ot.DistributionFactory.GetUniVariateFactories()
factories.add(ot.DistributionFactory.GetMultiVariateFactories())

for factory in factories:
    distribution = factory.build()

    # avoid flat pdfs
    if distribution.getName() == "Dirichlet":
        distribution = ot.Dirichlet([2, 6])
        distribution.setName("Dirichlet")
    elif distribution.getName() == "Histogram":
        distribution = ot.Histogram([-1.0, 0.5, 1.0, 2.0], [0.45, 0.4, 0.15])
        distribution.setName("Histogram")

    if distribution.isContinuous():
        if distribution.getDimension() > 1:
            # dependence coefficients
            chi = distribution.computeUpperTailDependenceMatrix()[1, 0]
            chib = distribution.computeUpperExtremalDependenceMatrix()[1, 0]
            chiL = distribution.computeLowerTailDependenceMatrix()[1, 0]
            chiLb = distribution.computeLowerExtremalDependenceMatrix()[1, 0]
            print("chi=", chi, "chib=", chib, "chiL=", chiL, "chiLb=", chiLb)

    elif distribution.isDiscrete():
        if distribution.getDimension() == 1:
            # check getSupport not overflowing with negative upper bound
            interval = ot.Interval(-1e3, -10)
            support = distribution.getSupport(interval)

# negative proba bug
Torque = ot.LogNormal(0.0, 0.25)
Angle = ot.Normal(0.0, 2.0)
rho = 0.5
R = ot.CorrelationMatrix(2, [1.0, rho, rho, 1.0])
listTorqueAngleCopula = [
    ot.NormalCopula(R),
    ot.StudentCopula(3.0, R),
    ot.StudentCopula(10.5, R),
]
interval = ot.Interval(
    [6.30177467808195, 3.56435643564356], [6.36881483423176, 3.72277227722772]
)
for TorqueAngleCopula in listTorqueAngleCopula:
    d = ot.JointDistribution([Torque, Angle], TorqueAngleCopula)
    p = d.computeProbability(interval)
    assert p > 0.0, "!positive proba"

# getSampleByQMC: static SobolSequence must match each distribution's dimension
dist1 = ot.Normal()
sample1 = dist1.getSampleByQMC(10)
assert sample1.getDimension() == 1
dist2 = ot.Normal(2)
sample2 = dist2.getSampleByQMC(10)
assert sample2.getDimension() == 2

# check generic computeCharacteristicFunction has no global cache
x = 500000.0
dist1 = ot.Student(3.0)
cf = dist1.computeCharacteristicFunction(x)
dist2 = ot.Rice()
cf = dist2.computeCharacteristicFunction(x)
ott.assert_almost_equal(cf, 2.655225e-12 + 1.321260e-18j)

# DistributionCollection slicing
dc = ot.DistributionCollection([ot.Normal(), ot.Uniform(), ot.Exponential(), ot.Gamma(2.0, 3.0)])
sliced_dc = dc[1:3]
assert isinstance(sliced_dc, ot.DistributionCollection)
assert sliced_dc.getSize() == 2
assert sliced_dc[0].getMean()[0] == ot.Uniform().getMean()[0]
seq_dc = dc[[3, 0]]
assert seq_dc.getSize() == 2
dc[0:2] = ot.DistributionCollection([ot.Triangular(), ot.Beta(2.0, 2.0, 0.0, 1.0)])
assert dc.getSize() == 4

# minimum volume level sets for several levels, see issue #1734
distribution = ot.Normal(2.0, 3.0)
probs = [0.5, 0.8, 0.95]
levelSets, thresholds = distribution.computeMinimumVolumeLevelSetCollectionWithThreshold(probs)
assert levelSets.getSize() == 3, "collection size"
assert thresholds.getSize() == 3, "threshold size"
# consistent with the scalar version
for i, p in enumerate(probs):
    _, refThreshold = distribution.computeMinimumVolumeLevelSetWithThreshold(p)
    ott.assert_almost_equal(thresholds[i], refThreshold, 1e-6, 0.0)
# shared-sampling path agrees with the analytic path
ot.ResourceMap.SetAsBool("Distribution-MinimumVolumeLevelSetBySampling", True)
levelSetsQMC, thresholdsQMC = distribution.computeMinimumVolumeLevelSetCollectionWithThreshold(
    probs
)
ot.ResourceMap.SetAsBool("Distribution-MinimumVolumeLevelSetBySampling", False)
ott.assert_almost_equal(thresholdsQMC, thresholds, 0.0, 1e-4)
# invalid probability raises
with ott.assert_raises(TypeError):
    distribution.computeMinimumVolumeLevelSetCollectionWithThreshold([0.5, 1.5])

# characteristic function, see issue #3205
# The triangular-mimic PiecewiseLinearDistribution goes through the generic
# DistributionImplementation path (Uniform and Triangular override it
# analytically). Both integration branches are covered explicitly:
# - Filon branch (default keys): powers of 2 from 1 to 2**23 plus 1e7
# - Gauss branch (large NMax): smooth truncated exponential at low x
triangular = ot.Triangular(-1.0, 0.0, 2.0)
mimic = otexp.PiecewiseLinearDistribution([-1.0, 0.0, 2.0], [0.0, 1.0, 0.0])
texp = ot.TruncatedDistribution(ot.Exponential(1.0), ot.Interval([0.0], [5.0]))
ot.ResourceMap.SetAsUnsignedInteger("Distribution-CharacteristicFunctionNMax", 255)
try:
    for x in [2.0**k for k in range(24)] + [1.0e7]:
        cf = mimic.computeCharacteristicFunction(x)
        cf_ref = triangular.computeCharacteristicFunction(x)
        ott.assert_almost_equal(cf, cf_ref, 1e-4, 1e-4)
    ot.ResourceMap.SetAsUnsignedInteger(
        "Distribution-CharacteristicFunctionNMax", 1000000
    )
    for x in [1.0, 2.0, 4.0, 8.0, 16.0, 32.0]:
        cf = texp.computeCharacteristicFunction(x)
        # closed form (lam / (lam - i*x)) * (1 - exp(-(lam - i*x) * b)) / norm
        lam, b = 1.0, 5.0
        norm = 1.0 - math.exp(-lam * b)
        cf_ref = (lam / complex(lam, -x)) * (1.0 - cmath.exp(complex(-lam * b, x * b))) / norm
        ott.assert_almost_equal(cf, cf_ref, 1e-6, 1e-6)
finally:
    ot.ResourceMap.SetAsUnsignedInteger(
        "Distribution-CharacteristicFunctionNMax", 1000000
    )

# Negative arguments must select the same integration panels as positive
# arguments.  With NMax=1020 and |x|>=1000, a signed panel count would send
# negative arguments to the single-panel fixed branch while positive arguments
# use Filon.
ot.ResourceMap.SetAsUnsignedInteger("Distribution-CharacteristicFunctionNMax", 1020)
try:
    for x in [1000.0, 10000.0]:
        for sign in [-1.0, 1.0]:
            cf = mimic.computeCharacteristicFunction(sign * x)
            cf_ref = triangular.computeCharacteristicFunction(sign * x)
            ott.assert_almost_equal(cf, cf_ref, 1e-4, 1e-4)
finally:
    ot.ResourceMap.SetAsUnsignedInteger(
        "Distribution-CharacteristicFunctionNMax", 1000000
    )

# zero integration nodes number is rejected, as the GaussLegendre
# construction used to do before the FastLegendre switch, see PR #3352 review
defaultNodesNumber = mimic.getIntegrationNodesNumber()
mimic.setIntegrationNodesNumber(0)
try:
    with ott.assert_raises(TypeError):
        mimic.computeCharacteristicFunction(1.0)
finally:
    mimic.setIntegrationNodesNumber(defaultNodesNumber)
