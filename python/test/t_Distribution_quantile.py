#! /usr/bin/env python

import openturns as ot
import math as m

ot.TESTPREAMBLE()

coll = [f.build() for f in ot.DistributionFactory.GetUniVariateFactories()]
coll += [ot.NonCentralStudent(7.2, 4.8, -3.7),
         ot.NonCentralChiSquare(1.5, 2.5),
         ot.Epanechnikov(),
         ot.InverseWishart(ot.CovarianceMatrix(1), 5.0),
         ot.Wishart(ot.CovarianceMatrix(1), 3.0),
         ot.ZipfMandelbrot(15, 1.2, 2.),
         ot.InverseGamma(2.5, 5.5),
         ot.InverseChiSquare(10.5),
         ot.TruncatedDistribution(ot.Normal(), 1.0, 4.0)]
coll = [ot.Distribution(d) for d in coll]
tol = 1e-6
for dist in coll:
    # <0
    q0 = dist.computeScalarQuantile(0.0)
    qm1 = dist.computeScalarQuantile(-1.0)
    if q0 != qm1:
        print(dist.getImplementation().getClassName(), '<0', q0, qm1)

    # 0+
    q0p = dist.computeScalarQuantile(ot.SpecFunc.MinScalar)
    if abs(q0 - q0p) > tol:
        print(dist.getImplementation().getClassName(), '0+', q0, q0p)

    # 1-
    q1 = dist.computeScalarQuantile(1.0)
    q1m = dist.computeScalarQuantile(1.0 - ot.SpecFunc.ScalarEpsilon)
    if abs(q1m - q1) / max(q1, tol) > tol:
        print(dist.getImplementation().getClassName(), '1-', q1, q1m)

    # >1
    q2 = dist.computeScalarQuantile(2.0)
    if q1 != q2:
        print(dist.getImplementation().getClassName(), '>1', q1, q2)

coll += [f.build() for f in ot.DistributionFactory.GetDiscreteUniVariateFactories()]
for dist in coll:
    for p in [0.0, ot.SpecFunc.ScalarEpsilon, 1e-50, 1.0 - ot.SpecFunc.ScalarEpsilon, 1.0]:
        for tail in [False, True]:
            q = dist.computeQuantile(p, tail)[0]
            sq = dist.computeScalarQuantile(p, tail)
            if not m.isfinite(q):
                raise ValueError(f"{str(dist)} p={p} tail={tail} q={q}")
            if abs(q-sq)> 1e-15:
                print(dist.getImplementation().getClassName(), p, tail, q, sq, dist, dist.getRange())
