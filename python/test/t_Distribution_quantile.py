#! /usr/bin/env python

import openturns as ot
import math as m

ot.TESTPREAMBLE()

coll = [f.build() for f in ot.DistributionFactory.GetUniVariateFactories()]
coll += [ot.Distribution(d) for d in [ot.NonCentralStudent(), ot.NonCentralChiSquare(),
    ot.InverseWishart(), ot.Wishart(), ot.ZipfMandelbrot()]]
# TODO: InverseGamma, InverseChiseSquare
for dist in coll:
    q0 = dist.computeScalarQuantile(0.0)
    qm1 = dist.computeScalarQuantile(-1.0)
    if q0 != qm1:
        print(dist.getImplementation().getClassName(), '<0', q0, qm1)
    q1 = dist.computeScalarQuantile(1.0)
    q0p = dist.computeScalarQuantile(ot.SpecFunc.MinScalar)
    if q0 != q0p:
        print(dist.getImplementation().getClassName(), '0+', q0, q0p)
    q1 = dist.computeScalarQuantile(1.0)
    q2 = dist.computeScalarQuantile(2.0)
    if q1 != q2:
        print(dist.getImplementation().getClassName(), '>1', q1, q2)
    q1m = dist.computeScalarQuantile(1.0-ot.SpecFunc.ScalarEpsilon)
    if q1m != q1:
        print(dist.getImplementation().getClassName(), '1-', q1m, q1)

coll += [f.build() for f in ot.DistributionFactory.GetDiscreteUniVariateFactories()]
for dist in coll:
    for p in [0.0, ot.SpecFunc.ScalarEpsilon, 1e-50, 1.0 - ot.SpecFunc.ScalarEpsilon, 1.0]:
        for tail in [False, True]:
            q = dist.computeQuantile(p, tail)[0]
            sq = dist.computeScalarQuantile(p, tail)
            if m.isinf(q):
                raise ValueError(f"{str(dist)} p={p} tail={tail}")
            if abs(q-sq)> 1e-15:
                print(dist.getImplementation().getClassName(), p, tail, q, sq, dist, dist.getRange())
