#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott


ot.TESTPREAMBLE()

factories = ot.DistributionFactory.GetUniVariateFactories()
for factory in factories:
    dist = factory.build()
    with ott.assert_raises(TypeError):
        qm1 = dist.computeQuantile(-1.0)
    with ott.assert_raises(TypeError):
        q2 = dist.computeQuantile(2.0)
    q0 = dist.computeQuantile(0.0)[0]
    q1 = dist.computeQuantile(1.0)[0]
    q0p = dist.computeQuantile(ot.SpecFunc.MinScalar)[0]
    if not q0 <= q0p:
        print(dist.getName(), "0+", q0, q0p)
    q1 = dist.computeQuantile(1.0)[0]
    q1m = dist.computeQuantile(1.0 - ot.SpecFunc.ScalarEpsilon)[0]
    if not q1m <= q1:
        print(dist.getName(), "1-", q1m, q1)
