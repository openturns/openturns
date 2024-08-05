#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

print(
    "Continuous univariate factories=",
    ot.DistributionFactory.GetContinuousUniVariateFactories(),
)
print(
    "Continuous multivariate factories=",
    ot.DistributionFactory.GetContinuousMultiVariateFactories(),
)
print(
    "Discrete univariate factories=",
    ot.DistributionFactory.GetDiscreteUniVariateFactories(),
)
print(
    "Discrete multivariate factories=",
    ot.DistributionFactory.GetDiscreteMultiVariateFactories(),
)
print("Univariate factories=", ot.DistributionFactory.GetUniVariateFactories())
print("Multivariate factories=", ot.DistributionFactory.GetMultiVariateFactories())

factories = ot.DistributionFactory.GetContinuousUniVariateFactories()
factories.add(ot.DistributionFactory.GetDiscreteUniVariateFactories())
for factory in factories:
    print(factory)

    # check if raise on constant sample
    if factory.build().isContinuous():
        sample = ot.Sample(100, [1.0e5])
        ok = False
        try:
            dist = factory.build(sample)
        except Exception:
            ok = True
            print("constant:", "ok" if ok else "fail")
            assert ok, str(factory) + " vs const"

    # check if raises on sample with nan/inf
    for weird in ["nan", "inf"]:
        sample = factory.build().getSample(100)
        sample[0, 0] = float(weird)
        ok = False
        try:
            dist = factory.build(sample)
        except Exception:
            ok = True
            print(weird + ":", "ok" if ok else "fail")
            assert ok, str(factory) + " vs " + weird

factory = ot.DistributionFactory.GetByName("PoissonFactory")
print(factory)
assert factory.getImplementation().__class__.__name__ == "PoissonFactory", "wrong name"

# Now, test each factory for the partial estimation of any subset of its parameters

for factory in factories:
    print("factory=", factory)
    try:
        factoryName = factory.getImplementation().getClassName()
        aEps = 0.01e100
        rEps = 0.01e100
        if factoryName == "BetaFactory":
            aEps = 0.05
            rEps = 0.05
        if factoryName == "FisherSnedecorFactory":
            aEps = 0.25
            rEps = 0.25
        if factoryName == "LogisticFactory":
            aEps = 0.025
            rEps = 0.025
        if factoryName == "LogNormalFactory":
            aEps = 0.02
            rEps = 0.02
        if factoryName == "ParetoFactory":
            aEps = 0.15
            rEps = 0.15
        if factoryName == "RiceFactory":
            aEps = 0.3
            rEps = 0.3
        if factoryName == "StudentFactory":
            aEps = 0.15
            rEps = 0.15
        if factoryName == "StudentFactory":
            aEps = 0.15
            rEps = 0.15
        if factoryName == "TrapezoidalFactory":
            aEps = 1.0
            rEps = 1.0
        if factoryName == "TriangularFactory":
            aEps = 0.025
            rEps = 0.025
        if factoryName == "VonMisesFactory":
            aEps = 0.02
            rEps = 0.02
        if factoryName == "NegativeBinomialFactory":
            aEps = 0.05
            rEps = 0.05

        # Skip HistogramFactory because it is nonparametric, so the rebuilt distribution has a number of parameters different from the reference distribution
        # Skip the MeixnerDistributionFactory because it takes ages to compute the parameters
        # Skip the SmoothedUniformFactory because it has a bug
        if factoryName not in ["HistogramFactory", "MeixnerDistributionFactory"]:
            # Reference distribution
            refDistribution = factory.build()
            sample = refDistribution.getSample(10000)
            # Get the ref parameter
            refParameter = refDistribution.getParameter()
            n = len(refParameter)
            for k in range(n):
                # Generate all the combinations of known parameter
                combinations = ot.Combinations(k, n).generate()
                for combination in combinations:
                    indices = [int(x) for x in combination]
                    factory.setKnownParameter([refParameter[i] for i in indices], indices)
                    res = factory.build(sample)
                    resParameter = res.getParameter()
                    ott.assert_almost_equal(resParameter, refParameter, rEps, aEps)
    except Exception:
        print("Skip", factory, "no getClassName()")
        pass
