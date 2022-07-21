#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

factories = ot.DistributionFactory.GetUniVariateFactories()
for factory in factories:
    dist = factory.build()
    q0 = dist.computeQuantile(0.0)
    qm1 = dist.computeQuantile(-1.0)
    if q0 != qm1:
        print(dist.getName(), '<0', q0, qm1)
    q1 = dist.computeQuantile(1.0)
    q0p = dist.computeQuantile(ot.SpecFunc.MinScalar)
    if q0 != q0p:
        print(dist.getName(), '0+', q0, q0p)
    q1 = dist.computeQuantile(1.0)
    q2 = dist.computeQuantile(2.0)
    if q1 != q2:
        print(dist.getName(), '>1', q1, q2)
    q1m = dist.computeQuantile(1.0-ot.SpecFunc.ScalarEpsilon)
    if q1m != q1:
        print(dist.getName(), '1-', q1m, q1)
