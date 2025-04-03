#!/usr/bin/env python

import openturns as ot
import sys

ot.TESTPREAMBLE()

grid = ot.GridLayout(2, 3)
palette = ot.Drawable.BuildDefaultPalette(10)
for j in range(grid.getNbColumns()):
    alpha = 1.0 + j
    pdf_curve = ot.WeibullMin(1.0, alpha, 0.0).drawPDF()
    cdf_curve = ot.WeibullMin(1.0, alpha, 0.0).drawCDF()
    pdf_curve.setColors([palette[j]])
    cdf_curve.setColors([palette[j]])
    legends = [f"alpha={alpha}"]
    pdf_curve.setLegends(legends)
    cdf_curve.setLegends(legends)
    grid.setGraph(0, j, pdf_curve)
    grid.setGraph(1, j, cdf_curve)

assert grid.getNbColumns() == 3
assert grid.getNbRows() == 2
assert grid.getGraph(0, 0).getDrawable(0).getImplementation().getClassName() == "Curve"

grid2 = ot.GridLayout(2, 3)
for j in range(grid2.getNbColumns()):
    sample = ot.WeibullMin(1.0, alpha, 0.0).getSample(10)
    sample.stack(ot.Sample(10, 1))
    graph = ot.Graph()
    cloud = ot.Cloud(sample)
    cloud.setColor(palette[j])
    graph.add(cloud)
    grid2.setGraph(0, j, graph)
    grid2.setGraph(1, j, graph)

grid.add(grid2)

if len(sys.argv) > 1:
    from openturns.viewer import View

    View(grid).save("grid.png")
