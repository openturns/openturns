#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()
pointNumber = 251


dist1D = ot.Normal(2.0, 4.0)
dist2D = ot.Normal(ot.Point(2, 2.0), ot.Point(2, 4.0), ot.CorrelationMatrix(2))
distND = ot.Normal(ot.Point(5, 2.0), ot.Point(5, 4.0), ot.CorrelationMatrix(5))
# Check drawing methods for 1D distributions
# PDF
graphPDF = dist1D.drawPDF()
graphPDF = dist1D.drawPDF(-4.0, 4.0, 101)
graphPDF = dist1D.drawPDF(-4.0, 4.0)
graphPDF = dist1D.drawPDF(101)
# log-PDF
graphLogPDF = dist1D.drawLogPDF()
graphLogPDF = dist1D.drawLogPDF(-4.0, 4.0, 101)
graphLogPDF = dist1D.drawLogPDF(-4.0, 4.0)
graphLogPDF = dist1D.drawLogPDF(101)
# CDF
graphCDF = dist1D.drawCDF()
graphCDF = dist1D.drawCDF(-4.0, 4.0, 101)
graphCDF = dist1D.drawCDF(-4.0, 4.0)
graphCDF = dist1D.drawCDF(101)
# Check drawing methods for 2D distributions
# PDF
graphPDF = dist2D.drawPDF()
graphPDF = dist2D.drawPDF([-4.0, -4.0], [4.0, 4.0], [101, 101])
graphPDF = dist2D.drawPDF([-4.0, -4.0], [4.0, 4.0])
graphPDF = dist2D.drawPDF([101, 101])
# log-PDF
graphLogPDF = dist2D.drawLogPDF()
graphLogPDF = dist2D.drawLogPDF([-4.0, -4.0], [4.0, 4.0], [101, 101])
graphLogPDF = dist2D.drawLogPDF([-4.0, -4.0], [4.0, 4.0])
graphLogPDF = dist2D.drawLogPDF([101, 101])
# CDF
graphCDF = dist2D.drawCDF()
graphCDF = dist2D.drawCDF([-4.0, -4.0], [4.0, 4.0], [101, 101])
graphCDF = dist2D.drawCDF([-4.0, -4.0], [4.0, 4.0])
graphCDF = dist2D.drawCDF([101, 101])
# Check drawing methods for ND distributions
# PDF
graphPDF = distND.drawMarginal1DPDF(2, -4.0, 4.0, 101)
graphPDF = distND.drawMarginal2DPDF(2, 3, [-4.0, -4.0], [4.0, 4.0], [101, 101])
# log-PDF
graphLogPDF = distND.drawMarginal1DLogPDF(2, -4.0, 4.0, 101)
graphLogPDF = distND.drawMarginal2DLogPDF(2, 3, [-4.0, -4.0], [4.0, 4.0], [101, 101])
# CDF
graphCDF = distND.drawMarginal1DCDF(2, -4.0, 4.0, 101)
graphCDF = distND.drawMarginal2DCDF(2, 3, [-4.0, -4.0], [4.0, 4.0], [101, 101])
# Quantile
graphQuantile = dist1D.drawQuantile()
graphQuantile = dist2D.drawQuantile()
