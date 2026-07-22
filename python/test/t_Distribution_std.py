import openturns as ot
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
