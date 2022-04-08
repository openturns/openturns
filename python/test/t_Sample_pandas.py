#!/usr/bin/env python

from __future__ import print_function
import openturns as ot

sample = ot.Normal(3).getSample(10)

# Sample => Dataframe
df = sample.asDataFrame()
print(df)
assert df.shape == (sample.getSize(), sample.getDimension()), "wrong df shape"

# Dataframe => Sample
sample2 = ot.Sample.BuildFromDataFrame(df)
print(sample2)
assert (sample2.getSize(), sample2.getDimension()
        ) == df.shape, "wrong sample shape"
