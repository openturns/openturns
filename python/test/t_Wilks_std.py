#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

alpha = [0.05 * i for i in range(1, 20)]
beta = [0.05 * i for i in range(1, 20)]
for a in alpha:
    for b in beta:
        print("alpha=%.2f" % a, "beta=%.2f" % b)
        for i in range(5):
            print("n(%.2f" % a, ",%.2f" %b, ",", i, ")=",
                  Wilks.ComputeSampleSize(a, b, i))
