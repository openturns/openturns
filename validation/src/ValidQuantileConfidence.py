import openturns as ot
import openturns.experimental as otexp

alpha = 0.9
beta = 0.95
algo = otexp.QuantileConfidence(alpha, beta)

# validate confidence intervals by probability estimation
distribution = ot.Gumbel()
nreps = int(1e5)
for n in [60, 100]:
    print(f"n={n}")
    p1 = p2 = p3 = 0.0
    for r in range(nreps):
        sample = distribution.getSample(n)
        interval1 = algo.computeUnilateralConfidenceInterval(sample)
        interval2 = algo.computeUnilateralConfidenceInterval(sample, True)
        interval3 = algo.computeBilateralConfidenceInterval(sample)
        q = distribution.computeQuantile(alpha)
        if interval1.contains(q):
            p1 += 1.0 / nreps
        if interval2.contains(q):
            p2 += 1.0 / nreps
        if interval3.contains(q):
            p3 += 1.0 / nreps
        if r % 1000 == 0:
            print(f"{100.0 * r / nreps} %")

    print(f"p1={p1:.6f} p2={p2:.6f} p3={p3:.6f}")
    assert p1 >= beta
    assert p2 >= beta
    assert p3 >= beta
