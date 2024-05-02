#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

factories = ot.DistributionFactory.GetUniVariateFactories()
for factory in factories:
    dist = factory.build()
    try:
        qm1 = dist.computeQuantile(-1.0)
        raise ValueError(
            "Expected a TypeError but with dist = {} got quantile(-1) = {}".format(
                dist, qm1
            )
        )
    except TypeError:
        pass
    try:
        q2 = dist.computeQuantile(2.0)
        raise ValueError(
            "Expected a TypeError but with dist = {} got quantile(2) = {}".format(
                dist, q2
            )
        )
    except TypeError:
        pass
    q0 = dist.computeQuantile(0.0)[0]
    q1 = dist.computeQuantile(1.0)[0]
    q0p = dist.computeQuantile(ot.SpecFunc.MinScalar)[0]
    if not q0 <= q0p:
        print(dist.getName(), "0+", q0, q0p)
    q1 = dist.computeQuantile(1.0)[0]
    q1m = dist.computeQuantile(1.0 - ot.SpecFunc.ScalarEpsilon)[0]
    if not q1m <= q1:
        print(dist.getName(), "1-", q1m, q1)
