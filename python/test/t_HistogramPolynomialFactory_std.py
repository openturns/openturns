#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    histogramPolynomial = HistogramPolynomialFactory(
        -2.0, [1.0, 2.0, 1.5], [2.0, 5.0, 3.5])
    print("histogramPolynomial=", histogramPolynomial)
    for i in range(10):
        print("histogramPolynomial(", i, ")=", histogramPolynomial.build(i))
    roots = histogramPolynomial.getRoots(10)
    print("histogramPolynomial(10) roots=", repr(roots))
    nodes, weights = histogramPolynomial.getNodesAndWeights(10)
    print("histogramPolynomial(10) nodes=", nodes, "and weights=", weights)
except:
    import sys
    print("t_HistogramPolynomialFactory_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
