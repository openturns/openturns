#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


histogramPolynomial = ot.HistogramPolynomialFactory(
    -2.0, [1.0, 2.0, 1.5], [2.0, 5.0, 3.5])
print("histogramPolynomial=", histogramPolynomial)
for i in range(10):
    print("histogramPolynomial(", i, ")=", histogramPolynomial.build(i))
roots = histogramPolynomial.getRoots(10)
print("histogramPolynomial(10) roots=", repr(roots))
nodes, weights = histogramPolynomial.getNodesAndWeights(10)
print("histogramPolynomial(10) nodes=", nodes, "and weights=", weights)
