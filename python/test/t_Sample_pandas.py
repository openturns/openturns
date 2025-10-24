#!/usr/bin/env python

import openturns as ot
import pandas as pd

# check indexation compatibility
columns = ot.Description(["x1", "x2", "x3"])
df1 = pd.DataFrame(index=["R1"], columns=columns)
columns = ot.Point([1.0, 2.0, 3.0])
df2 = pd.DataFrame(index=["R2"], columns=columns)
columns = ot.Indices([1, 2, 3])
df3 = pd.DataFrame(index=["R3"], columns=columns)

sample = ot.Normal(3).getSample(10)

# Sample => Dataframe
df = sample.asDataFrame()
print(df)
assert df.shape == (sample.getSize(), sample.getDimension()), "wrong df shape"

# Dataframe => Sample
sample2 = ot.Sample.BuildFromDataFrame(df)
print(sample2)
assert (sample2.getSize(), sample2.getDimension()) == df.shape, "wrong sample shape"
