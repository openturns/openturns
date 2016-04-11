#!/usr/bin/env python

import openturns as ot
import scipy.stats as st
from numpy import asarray

n1 = 200
n2 = 300

def compare(sample1, sample2):
    print(st.ks_2samp(asarray(sample1).flatten(), asarray(sample2).flatten()))
    print(ot.FittingTest.TwoSamplesKolmogorov(sample1, sample2))

sample1 = ot.Normal().getSample(n1)
sample2 = ot.Normal(0.5, 1.5).getSample(n2)
compare(sample1, sample2)

sample3 = ot.Normal(0.01, 1.0).getSample(n2)
compare(sample1, sample3)

sample4 = ot.Normal().getSample(n2)
compare(sample1, sample4)

