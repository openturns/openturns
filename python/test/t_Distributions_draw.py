#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
pointNumber = 251

try:
    dist1D = Normal(2.0, 4.0)
    dist2D = Normal(NumericalPoint(2, 2.0),
                    NumericalPoint(2, 4.0), CorrelationMatrix(2))
    distND = Normal(NumericalPoint(5, 2.0),
                    NumericalPoint(5, 4.0), CorrelationMatrix(5))
    # Check drawing methods for 1D distributions
    # PDF
    graphPDF = dist1D.drawPDF()
    graphPDF.draw("DefaultDrawPDF_1D")
    graphPDF = dist1D.drawPDF(-4.0, 4.0, 101)
    graphPDF.draw("FullParametersDrawPDF_1D")
    graphPDF = dist1D.drawPDF(-4.0, 4.0)
    graphPDF.draw("RangeDrawPDF_1D")
    graphPDF = dist1D.drawPDF(101)
    graphPDF.draw("PointNumberDrawPDF_1D")
    # log-PDF
    graphLogPDF = dist1D.drawLogPDF()
    graphLogPDF.draw("DefaultDrawLogPDF_1D")
    graphLogPDF = dist1D.drawLogPDF(-4.0, 4.0, 101)
    graphLogPDF.draw("FullParametersDrawLogPDF_1D")
    graphLogPDF = dist1D.drawLogPDF(-4.0, 4.0)
    graphLogPDF.draw("RangeDrawLogPDF_1D")
    graphLogPDF = dist1D.drawLogPDF(101)
    graphLogPDF.draw("PointNumberDrawLogPDF_1D")
    # CDF
    graphCDF = dist1D.drawCDF()
    graphCDF.draw("DefaultDrawCDF_1D")
    graphCDF = dist1D.drawCDF(-4.0, 4.0, 101)
    graphCDF.draw("FullParametersDrawCDF_1D")
    graphCDF = dist1D.drawCDF(-4.0, 4.0)
    graphCDF.draw("RangeDrawCDF_1D")
    graphCDF = dist1D.drawCDF(101)
    graphCDF.draw("PointNumberDrawCDF_1D")
    # Check drawing methods for 2D distributions
    # PDF
    graphPDF = dist2D.drawPDF()
    graphPDF.draw("DefaultDrawPDF_2D")
    graphPDF = dist2D.drawPDF([-4.0, -4.0], [4.0, 4.0], [101, 101])
    graphPDF.draw("FullParametersDrawPDF_2D")
    graphPDF = dist2D.drawPDF([-4.0, -4.0], [4.0, 4.0])
    graphPDF.draw("RangeDrawPDF_2D")
    graphPDF = dist2D.drawPDF([101, 101])
    graphPDF.draw("PointNumberDrawPDF_2D")
    # log-PDF
    graphLogPDF = dist2D.drawLogPDF()
    graphLogPDF.draw("DefaultDrawLogPDF_2D")
    graphLogPDF = dist2D.drawLogPDF([-4.0, -4.0], [4.0, 4.0], [101, 101])
    graphLogPDF.draw("FullParametersDrawLogPDF_2D")
    graphLogPDF = dist2D.drawLogPDF([-4.0, -4.0], [4.0, 4.0])
    graphLogPDF.draw("RangeDrawLogPDF_2D")
    graphLogPDF = dist2D.drawLogPDF([101, 101])
    graphLogPDF.draw("PointNumberDrawLogPDF_2D")
    # CDF
    graphCDF = dist2D.drawCDF()
    graphCDF.draw("DefaultDrawCDF_2D")
    graphCDF = dist2D.drawCDF([-4.0, -4.0], [4.0, 4.0], [101, 101])
    graphCDF.draw("FullParametersDrawCDF_2D")
    graphCDF = dist2D.drawCDF([-4.0, -4.0], [4.0, 4.0])
    graphCDF.draw("RangeDrawCDF_2D")
    graphCDF = dist2D.drawCDF([101, 101])
    graphCDF.draw("PointNumberDrawCDF_2D")
    # Check drawing methods for ND distributions
    # PDF
    graphPDF = distND.drawMarginal1DPDF(2, -4.0, 4.0, 101)
    graphPDF.draw("FullParametersDrawMarginal1DPDF_ND")
    graphPDF = distND.drawMarginal2DPDF(
        2, 3, [-4.0, -4.0], [4.0, 4.0], [101, 101])
    graphPDF.draw("FullParametersDrawMarginal2DPDF_ND")
    # log-PDF
    graphLogPDF = distND.drawMarginal1DLogPDF(2, -4.0, 4.0, 101)
    graphLogPDF.draw("FullParametersDrawMarginal1DLogPDF_ND")
    graphLogPDF = distND.drawMarginal2DLogPDF(
        2, 3, [-4.0, -4.0], [4.0, 4.0], [101, 101])
    graphLogPDF.draw("FullParametersDrawMarginal2DLogPDF_ND")
    # CDF
    graphCDF = distND.drawMarginal1DCDF(2, -4.0, 4.0, 101)
    graphCDF.draw("FullParametersDrawMarginal1DCDF_ND")
    graphCDF = distND.drawMarginal2DCDF(
        2, 3, [-4.0, -4.0], [4.0, 4.0], [101, 101])
    graphCDF.draw("FullParametersDrawMarginal2DCDF_ND")
    # Quantile
    graphQuantile = dist1D.drawQuantile()
    graphQuantile.draw("DrawQuantile_1D")
    graphQuantile = dist2D.drawQuantile()
    graphQuantile.draw("DrawQuantile_2D")
except:
    import sys
    print("t_Distributions_draw.py", sys.exc_info()[0], sys.exc_info()[1])
