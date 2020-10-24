"""
Link Pandas and OpenTURNS
=========================
"""
# sphinx_gallery_thumbnail_path = '_static/pandas.png'

# %%
# In this example we are going to explore interaction with Pandas data analysis tool.

# %%
from __future__ import print_function
import openturns as ot
import pandas as pd
ot.Log.Show(ot.Log.NONE)

# %%
# create a sample from a 3-d gaussian distribution
sample = ot.Normal(3).getSample(10)

# %%
# Create a DataFrame from a Sample
df = pd.DataFrame.from_records(sample, columns=sample.getDescription())
df.describe()

# %%
# Create a Sample from a DataFrame
sample2 = ot.Sample(df.values)
sample2.setDescription(df.columns)
sample2
