#! /usr/bin/env python

from matplotlib import pyplot as plt

from openturns.viewer import PlotDesign
import openturns as ot

samplesize = 12

# Plot 2D - basic
distribution = ot.JointDistribution([ot.Uniform(0.0, 1.0)] * 2)
bounds = distribution.getRange()
design = distribution.getSample(samplesize)
fig = PlotDesign(design)

# Plot 3D - basic
distribution = ot.JointDistribution([ot.Uniform(0.0, 1.0)] * 3)
bounds = distribution.getRange()
design = distribution.getSample(samplesize)
fig = PlotDesign(design)

# Plot 2D - set bounds and subdivisions
distribution = ot.JointDistribution([ot.Uniform(0.0, 1.0)] * 2)
bounds = distribution.getRange()
design = distribution.getSample(samplesize)
fig = PlotDesign(design, bounds, [10, 10])

# Plot 3D - set bounds and subdivisions
distribution = ot.JointDistribution([ot.Uniform(0.0, 1.0)] * 3)
bounds = distribution.getRange()
design = distribution.getSample(samplesize)
fig = PlotDesign(design, bounds, [10, 10, 10])

# Plot 2D - advanced - enable ticks
distribution = ot.JointDistribution([ot.Uniform(0.0, 1.0)] * 2)
bounds = distribution.getRange()
design = distribution.getSample(samplesize)
fig = PlotDesign(design, bounds, [10, 10], enableTicks=True)
fig.set_size_inches(12, 12)

# Plot 3D - advanced - enable ticks
distribution = ot.JointDistribution([ot.Uniform(0.0, 1.0)] * 3)
bounds = distribution.getRange()
design = distribution.getSample(samplesize)
fig = PlotDesign(design, bounds, [10, 10, 10], enableTicks=True)
fig.set_size_inches(12, 12)

# Plot 2D - advanced - unequal number of divisions
distribution = ot.JointDistribution([ot.Uniform(0.0, 1.0)] * 2)
bounds = distribution.getRange()
design = distribution.getSample(samplesize)
fig = PlotDesign(design, bounds, [10, 5])

# Plot 3D - advanced - unequal number of divisions
distribution = ot.JointDistribution([ot.Uniform(0.0, 1.0)] * 3)
bounds = distribution.getRange()
design = distribution.getSample(samplesize)
fig = PlotDesign(design, bounds, [10, 5, 3])

# Plot 2D - set figure prior to calling the plot
distribution = ot.JointDistribution([ot.Uniform(0.0, 1.0)] * 2)
bounds = distribution.getRange()
design = distribution.getSample(samplesize)
fig = plt.figure()
fig = PlotDesign(design, figure=fig)
