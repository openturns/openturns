#! /usr/bin/env python

import openturns as ot
import math as m

ot.TESTPREAMBLE()

coll = ot.DistributionFactory.GetUniVariateFactories()
for factory in coll:
    dist = factory.build()
    q0 = dist.computeScalarQuantile(0.0)
    qm1 = dist.computeScalarQuantile(-1.0)
    if q0 != qm1:
        print(dist.getName(), '<0', q0, qm1)
    q1 = dist.computeScalarQuantile(1.0)
    q0p = dist.computeScalarQuantile(ot.SpecFunc.MinScalar)
    if q0 != q0p:
        print(dist.getName(), '0+', q0, q0p)
    q1 = dist.computeScalarQuantile(1.0)
    q2 = dist.computeScalarQuantile(2.0)
    if q1 != q2:
        print(dist.getName(), '>1', q1, q2)
    q1m = dist.computeScalarQuantile(1.0-ot.SpecFunc.ScalarEpsilon)
    if q1m != q1:
        print(dist.getName(), '1-', q1m, q1)

coll.add(ot.DistributionFactory.GetDiscreteUniVariateFactories())
for factory in coll:
    distribution = factory.build()
    for p in [0.0, ot.SpecFunc.ScalarEpsilon, 1e-50, 1.0 - ot.SpecFunc.ScalarEpsilon, 1.0]:
        for tail in [False, True]:
            q = distribution.computeQuantile(p, tail)[0]
            sq = distribution.computeScalarQuantile(p, tail)
            if m.isinf(q):
                raise ValueError(f"{str(distribution)} p={p} tail={tail}")
            if abs(q-sq)> 1e-15:
                print(distribution.getImplementation().getClassName(), p, tail, q, sq, distribution, distribution.getRange())
