"""
Estimate a GPD on the Wooster temperature data
==============================================
"""
# %%
# In this example, we illustrate various techniques of extreme value modeling applied
# to the 5-year series of daily minimum temperatures recorded in Wooster, Ohio between 1983 and 1988.
# Readers should refer to [coles2001]_ example 1.7 to get more details.
#
# We illustrate techniques to:
#
# - estimate a stationary and a non stationary GPD,
# - estimate a return level,
#
# using:
#
# - the log-likelihood function,
# - the profile log-likelihood function.
#
import openturns as ot
# import openturns.experimental as otexp
import openturns.viewer as otv
from openturns.usecases import coles
import pandas as pd

# %%
# First, we load the Wooster dataset (negate the values to transform minimum to maximum) and look at it through time.
dataframe = pd.read_csv(coles.Coles().wooster, index_col=0, parse_dates=True)
dataframe["Temperature"] = dataframe["Temperature"].apply(lambda x: x * -1.0)
print(dataframe[:10])
title = "Negated Wooster daily minimum temperatures"
graph = ot.Graph(title, "Day index", "Temperature (°F)", True, "")
days = ot.Sample([[i] for i in range(len(dataframe))])
sample = ot.Sample.BuildFromDataFrame(dataframe)
cloud = ot.Cloud(days, sample)
cloud.setColor("red")
cloud.setPointStyle(",")
graph.add(cloud)
graph.setIntegerXTick(True)
view = otv.View(graph)

# %%
# Stratification of values by season
dataframe_season = {}
dataframe_season["winter"] = dataframe[(dataframe.index.month >= 12) | (dataframe.index.month < 3)]
dataframe_season["spring"] = dataframe[(dataframe.index.month >= 3) & (dataframe.index.month < 6)]
dataframe_season["summer"] = dataframe[(dataframe.index.month >= 6) & (dataframe.index.month < 9)]
dataframe_season["autumn"] = dataframe[(dataframe.index.month >= 9) & (dataframe.index.month < 12)]
for season in dataframe_season:
    df = dataframe_season[season]
    days = ot.Sample([[i] for i in range(len(df))])
    sample = ot.Sample.BuildFromDataFrame(df)
    cloud = ot.Cloud(days, sample)
    cloud.setColor("red")
    cloud.setPointStyle(",")
    graph.setDrawable(cloud, 0)
    graph.setTitle(f"{title} ({season})")
    view = otv.View(graph)

# %%
# Find consecutive exceedances clusters and the associated peaks for several thresholds/minimum gap
factory = ot.GeneralizedParetoFactory()
df = dataframe_season["winter"]
print(df)
sample = ot.Sample.BuildFromDataFrame(df)
for u in [-10.0, -20.0]:
    for r in [2, 4]:
        peaks, clusters = factory.getPeakOverThresholdWithClusters(sample, u, r)
        nc = len(peaks)
        nu = (df > u).values.sum()
        print(f"u={u} r={r} nu={nu} nc={nc} theta={nc/nu:.2f}")

# %%
otv.View.ShowAll()
