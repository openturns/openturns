import openturns as ot
import openturns.testing as ott
import math as m

# comparison via implementation
assert ot.Triangular() == ot.Triangular(), "Triangular==Triangular"
assert not ot.Rice() != ot.Rice(), "Rice!=Rice"
assert ot.Arcsine() != ot.Gumbel(), "Arcsine!=Gumbel"
assert ot.Gumbel() != ot.Arcsine(), "Arcsine!=Gumbel"
assert not ot.Gumbel() == ot.Arcsine(), "Arcsine==Gumbel"
assert ot.Exponential(2.5) != ot.Exponential(3.0), "Exponential(2.5)!=Exponential(3.0)"
assert not ot.Exponential(2.5) == ot.Exponential(3.0), "Exponential(2.5)==Exponential(3.0)"
assert ot.ComposedDistribution([ot.Normal()] * 2) == ot.Normal(2), "Normal(2)==Composed(2N)"
assert not ot.ComposedDistribution([ot.Normal()] * 2) != ot.Normal(2), "Normal(2)!=Composed(2N)"
assert ot.ComposedDistribution([ot.Normal()] * 2) != ot.Normal(3), "Composed(2N)!=Normal(3)"
assert not ot.ComposedDistribution([ot.Normal()] * 2) == ot.Normal(3), "!Composed(2N)==Normal(3)"
assert ot.Normal(2) == ot.ComposedDistribution([ot.Normal()] * 2), "Normal(2)==Composed(2N)"
assert not ot.Normal(3) == ot.ComposedDistribution([ot.Normal()] * 2), "Normal(3)==Composed(2N)"
assert ot.Normal(3) != ot.ComposedDistribution([ot.Normal()] * 2), "Normal(3)!=Composed(2N)"
assert ot.Normal(2) != ot.Student(3.0, 2), "Normal(2)!=Student(2)"
assert not ot.Student(3.0, 2) == ot.Normal(2), "!Student(2)==Normal(2)"

for factory in ot.DistributionFactory.GetContinuousUniVariateFactories():
    distribution = factory.build()

    # comparison
    distribution2 = factory.build()
    print(distribution, distribution2)
    assert distribution == distribution2, "=="
    assert not distribution != distribution2, "!="
    assert distribution2 == distribution, "=="
    assert not distribution2 != distribution, "!="
    dirac = ot.Dirac(42.0)
    assert distribution != dirac, "==Dirac"
    assert not distribution == dirac, "!=Dirac"

    # avoid flat pdfs
    if distribution.getName() == "Dirichlet":
        distribution = ot.Dirichlet([2, 6])
        distribution.setName("Dirichlet")
    elif distribution.getName() == "Histogram":
        distribution = ot.Histogram([-1.0, 0.5, 1.0, 2.0], [0.45, 0.4, 0.15])
        distribution.setName("Histogram")

    sample = distribution.getSample(10)
    epsilon = ot.ResourceMap.GetAsScalar(
        "CenteredFiniteDifferenceGradient-DefaultEpsilon"
    )
    pdfepsilon = ot.ResourceMap.GetAsScalar("Distribution-DefaultPDFEpsilon") ** (
        1.0 / 3.0
    )

    # logpdf
    logPDF1 = [y[0] for y in distribution.computeLogPDF(sample)]
    logPDF2 = [m.log(y[0]) for y in distribution.computePDF(sample)]
    ott.assert_almost_equal(logPDF1, logPDF2, 1e-5, 1e-8, str(distribution))

    # cdf
    PDF1 = distribution.computePDF(sample)
    CDF1 = distribution.computeCDF(sample + epsilon)
    CDF2 = distribution.computeCDF(sample - epsilon)
    PDF2 = (CDF1 - CDF2) / (2.0 * epsilon)
    ott.assert_almost_equal(PDF1, PDF2, 1e-3, 1e-3, str(distribution))

    # ddf
    DDF1 = distribution.computeDDF(sample)
    PDF1 = distribution.computePDF(sample + pdfepsilon)
    PDF2 = distribution.computePDF(sample - pdfepsilon)
    DDF2 = (PDF1 - PDF2) / (2.0 * pdfepsilon)
    ott.assert_almost_equal(DDF1, DDF2, 1e-4, 1e-4, str(distribution))

    # ccdf
    CCDF1 = distribution.computeComplementaryCDF(sample)
    CCDF2 = distribution.computeCDF(sample)
    for i in range(len(CCDF2)):
        CCDF2[i, 0] = 1.0 - CCDF2[i, 0]
    ott.assert_almost_equal(CCDF1, CCDF2, 1e-5, 1e-5, str(distribution))

    # pdf grad
    point = sample[0]
    PDFgr = distribution.computePDFGradient(point)
    assert distribution.computePDFGradient([point]).getDimension() == len(PDFgr)

    # proba
    interval = distribution.getRange()
    ott.assert_almost_equal(
        distribution.computeCDF(interval.getLowerBound())
        - m.sqrt(ot.SpecFunc.ScalarEpsilon),
        0.0,
        1e-5,
        1e-5,
        str(distribution),
    )
    ott.assert_almost_equal(
        distribution.computeCDF(interval.getUpperBound())
        + m.sqrt(ot.SpecFunc.ScalarEpsilon),
        1.0,
        1e-5,
        1e-5,
        str(distribution),
    )
    ott.assert_almost_equal(
        distribution.computeProbability(interval), 1.0, 1e-5, 1e-5, str(distribution)
    )

    # MinimumVolumeInterval
    probability = 0.9
    interval = distribution.computeMinimumVolumeIntervalWithMarginalProbability(
        probability
    )[0]
    CDF_up = distribution.computeCDF(interval.getUpperBound())
    CDF_low = distribution.computeCDF(interval.getLowerBound())
    computed_probability = CDF_up - CDF_low
    ott.assert_almost_equal(
        probability, computed_probability, 1e-5, 1e-5, str(distribution)
    )

    # MinimumVolumeLevelSet
    probability = 0.9
    levelSet, threshold = distribution.computeMinimumVolumeLevelSetWithThreshold(
        probability
    )
    event = ot.DomainEvent(ot.RandomVector(distribution), levelSet)
    algo = ot.ProbabilitySimulationAlgorithm(event)
    algo.setBlockSize(int(1e6))
    algo.setMaximumOuterSampling(1)
    algo.run()
    p = algo.getResult().getProbabilityEstimate()
    if distribution.getName() != "Histogram":
        ott.assert_almost_equal(p, probability, 1e-3, 1e-3, str(distribution))

    # parameters
    p = distribution.getParameter()
    pd = distribution.getParameterDescription()
    pc = distribution.getParametersCollection()
    assert len(p) == len(pd), "len p/pd"
    assert len(pc) == 1, "len(pc)"
    assert len(p) == len(pc[0]), "len p/pc"

# dependence coefficients
for factory in ot.DistributionFactory.GetContinuousMultiVariateFactories():
    copula = factory.build()
    if copula.getDimension() < 2:
        # bernstein
        continue
    print(copula, copula.getDimension())
    chi = copula.computeUpperTailDependenceMatrix()[1, 0]
    chib = copula.computeUpperExtremalDependenceMatrix()[1, 0]
    chiL = copula.computeLowerTailDependenceMatrix()[1, 0]
    chiLb = copula.computeLowerExtremalDependenceMatrix()[1, 0]
    print("chi=", chi, "chib=", chib, "chiL=", chiL, "chiLb=", chiLb)

# negative proba bug
Torque = ot.LogNormal(0.0, 0.25)
Angle = ot.Normal(0.0, 2.0)
rho = 0.5
TorqueAngleCopula = ot.NormalCopula(ot.CorrelationMatrix(2, [1.0, rho, rho, 1.0]))
d = ot.ComposedDistribution([Torque, Angle], TorqueAngleCopula)
interval = ot.Interval(
    [6.30177467808195, 3.56435643564356], [6.36881483423176, 3.72277227722772]
)
p = d.computeProbability(interval)
assert p > 0.0, "!positive proba"
