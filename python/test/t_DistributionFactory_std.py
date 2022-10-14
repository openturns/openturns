#! /usr/bin/env python

import openturns as ot

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
