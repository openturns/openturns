"""
Estimate a GPD on the Wooster temperature data
==============================================
"""
# %%
# In this example, we illustrate various techniques of extreme value modeling applied
# to the 5-year series of daily minimum temperatures recorded in Wooster, Ohio between 1983 and 1988.
# Readers should refer to [coles2001]_ example 1.7 to get more details.
import openturns as ot
import openturns.experimental as otexp
import openturns.viewer as otv
from openturns.usecases import coles
import pandas as pd

# %%
# First, we load the Wooster dataset (negate the values to transform minimum to maximum) and look at it through time.
full = pd.read_csv(coles.Coles().wooster, index_col=0, parse_dates=True)
full["Temperature"] = full["Temperature"].apply(lambda x: x * -1.0)
print(full[:10])
title = "Negated Wooster daily minimum temperatures"
graph = ot.Graph(title, "Day index", "Temperature (°F)", True, "")
days = ot.Sample([[i] for i in range(len(full))])
sample = ot.Sample.BuildFromDataFrame(full)
cloud = ot.Cloud(days, sample)
cloud.setColor("red")
cloud.setPointStyle(",")
graph.add(cloud)
graph.setIntegerXTick(True)
view = otv.View(graph)

# %%
# Stratification of values by season
full_season = {}
full_season["winter"] = full[(full.index.month >= 12) | (full.index.month < 3)]
full_season["spring"] = full[(full.index.month >= 3) & (full.index.month < 6)]
full_season["summer"] = full[(full.index.month >= 6) & (full.index.month < 9)]
full_season["autumn"] = full[(full.index.month >= 9) & (full.index.month < 12)]
for season in full_season:
    df = full_season[season]
    days = ot.Sample([[i] for i in range(len(df))])
    sample = ot.Sample.BuildFromDataFrame(df)
    cloud = ot.Cloud(days, sample)
    cloud.setColor("red")
    cloud.setPointStyle(",")
    graph.setDrawable(cloud, 0)
    graph.setTitle(f"{title} ({season})")
    view = otv.View(graph)

# %%
# Draw clusters on the first 40 days
first40 = ot.Sample.BuildFromDataFrame(full[350:450])
part = otexp.SamplePartition(first40)
u = 0.0
r = 4
peaks, clusters = part.getPeakOverThreshold(u, r)
graph = clusters.draw(u)
view = otv.View(graph)

# %%
# Find consecutive exceedances clusters and the associated peaks for several thresholds/minimum gap
winter = full_season["winter"]
winter_indices = [i for i, x in enumerate(full.index.isin(winter.index)) if x]
winter_sample = ot.Sample.BuildFromDataFrame(full)
part = otexp.SamplePartition(winter_sample, winter_indices)
factory = ot.GeneralizedParetoFactory()
for u in [-10.0, -20.0]:
    for r in [1, 3]:
        peaks, clusters = part.getPeakOverThreshold(u, r)
        nc = len(peaks)
        nu = (winter > u).values.sum()

        # fit a stationary gpd on the clusters and estimate the return level
        theta = nc / nu
        result_LL = factory.buildMethodOfLikelihoodMaximizationEstimator(peaks, u)
        xm_100 = factory.buildReturnLevelEstimator(result_LL, 100.0 * 365 * theta, peaks)
        sigma, xi, _ = result_LL.getParameterDistribution().getMean()
        sigma_stddev, xi_stddev, _ = result_LL.getParameterDistribution().getStandardDeviation()
        print(f"u={u} r={r} nc={nc} sigma={sigma:.2f} ({sigma_stddev:.2f}) xi={xi:.2f} ({xi_stddev:.2f})", end=" ")
        print(f"x100={xm_100.getMean()} ({xm_100.getStandardDeviation()}) theta={theta:.2f}")

        # plot the return level
        validation = otexp.GeneralizedParetoValidation(result_LL, peaks)
        grid = validation.drawDiagnosticPlot()
        rlPlot = grid.getGraph(1, 0)
        rlPlot.setTitle(rlPlot.getTitle() + f" (u={u} r={r})")
        view = otv.View(rlPlot)

# %%
otv.View.ShowAll()
