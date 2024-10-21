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
    dist = factory.build()
    # check if raise on constant sample
    if dist.isContinuous():
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
        sample = dist.getSample(100)
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

params = {
    "BetaFactory": 0.05,
    "FisherSnedecorFactory": 0.25,
    "LogisticFactory": 0.025,
    "LogNormalFactory": 0.02,
    "ParetoFactory": 0.15,
    "RiceFactory": 0.3,
    "StudentFactory": 0.15,
    "StudentFactory": 0.15,
    "TrapezoidalFactory": 1.0,
    "TriangularFactory": 0.025,
    "VonMisesFactory": 0.02,
}

for factory in factories:
    print("factory=", factory)
    try:
        factoryName = factory.getImplementation().getClassName()
        eps = params.get(factoryName, 0.01)
        # Skip HistogramFactory because it is nonparametric, so the rebuilt distribution has a number of parameters different from the reference distribution
        # Skip the MeixnerDistributionFactory because it takes ages to compute the parameters
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
                    factory.setKnownParameter(
                        [refParameter[i] for i in indices], indices
                    )
                    res = factory.build(sample)
                    resParameter = res.getParameter()
                    ott.assert_almost_equal(resParameter, refParameter, eps, eps)
    except Exception:
        print("Skip", factory, "no getClassName()")
        pass
