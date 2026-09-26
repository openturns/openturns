#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
from concurrent.futures import ThreadPoolExecutor

# Regression test for the concurrent evaluation of SymbolicEvaluation: the
# point evaluations are serialized with a mutex and the batch evaluations use
# one expression per thread, so the same function must be safe to evaluate
# concurrently from several threads for both point and sample inputs.
ot.TESTPREAMBLE()

for backend in ot.SymbolicFunction.GetValidParsers():
    ot.ResourceMap.Set("SymbolicParser-Backend", backend)
    f = ot.SymbolicFunction(
        ["x0", "x1", "x2"],
        ["x0^2 + 2*x1 + sin(x2)", "cos(x0) * exp(-x1)"],
    )

    # Reference values computed serially
    point = [1.0, 2.0, 3.0]
    refPoint = f(point)
    sample = ot.Normal(3).getSample(10000)
    refSample = f(sample)

    def worker():
        return f(point), f(sample)

    with ThreadPoolExecutor(max_workers=8) as pool:
        futures = [pool.submit(worker) for _ in range(20)]
        results = [future.result() for future in futures]

    for resPoint, resSample in results:
        ott.assert_almost_equal(resPoint, refPoint)
        ott.assert_almost_equal(resSample, refSample)

print("OK")
