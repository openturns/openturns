#! /usr/bin/env python

from openturns import *

TESTPREAMBLE()
pointNumber = 251

try:
    dist1D = Normal(2.0, 4.0)
    dist2D = Normal(Point(2, 2.0),
                    Point(2, 4.0), CorrelationMatrix(2))
    distND = Normal(Point(5, 2.0),
                    Point(5, 4.0), CorrelationMatrix(5))
    # Check drawing methods for 1D distributions
    # PDF
    graphPDF = dist1D.drawPDF()
    graphPDF.draw("DefaultDrawPDF_1D.png")
    graphPDF = dist1D.drawPDF(-4.0, 4.0, 101)
    graphPDF.draw("FullParametersDrawPDF_1D.png")
    graphPDF = dist1D.drawPDF(-4.0, 4.0)
    graphPDF.draw("RangeDrawPDF_1D.png")
    graphPDF = dist1D.drawPDF(101)
    graphPDF.draw("PointNumberDrawPDF_1D.png")
    # log-PDF
    graphLogPDF = dist1D.drawLogPDF()
    graphLogPDF.draw("DefaultDrawLogPDF_1D.png")
    graphLogPDF = dist1D.drawLogPDF(-4.0, 4.0, 101)
    graphLogPDF.draw("FullParametersDrawLogPDF_1D.png")
    graphLogPDF = dist1D.drawLogPDF(-4.0, 4.0)
    graphLogPDF.draw("RangeDrawLogPDF_1D.png")
    graphLogPDF = dist1D.drawLogPDF(101)
    graphLogPDF.draw("PointNumberDrawLogPDF_1D.png")
    # CDF
    graphCDF = dist1D.drawCDF()
    graphCDF.draw("DefaultDrawCDF_1D.png")
    graphCDF = dist1D.drawCDF(-4.0, 4.0, 101)
    graphCDF.draw("FullParametersDrawCDF_1D.png")
    graphCDF = dist1D.drawCDF(-4.0, 4.0)
    graphCDF.draw("RangeDrawCDF_1D.png")
    graphCDF = dist1D.drawCDF(101)
    graphCDF.draw("PointNumberDrawCDF_1D.png")
    # Check drawing methods for 2D distributions
    # PDF
    graphPDF = dist2D.drawPDF()
    graphPDF.draw("DefaultDrawPDF_2D.png")
    graphPDF = dist2D.drawPDF([-4.0, -4.0], [4.0, 4.0], [101, 101])
    graphPDF.draw("FullParametersDrawPDF_2D.png")
    graphPDF = dist2D.drawPDF([-4.0, -4.0], [4.0, 4.0])
    graphPDF.draw("RangeDrawPDF_2D.png")
    graphPDF = dist2D.drawPDF([101, 101])
    graphPDF.draw("PointNumberDrawPDF_2D.png")
    # log-PDF
    graphLogPDF = dist2D.drawLogPDF()
    graphLogPDF.draw("DefaultDrawLogPDF_2D.png")
    graphLogPDF = dist2D.drawLogPDF([-4.0, -4.0], [4.0, 4.0], [101, 101])
    graphLogPDF.draw("FullParametersDrawLogPDF_2D.png")
    graphLogPDF = dist2D.drawLogPDF([-4.0, -4.0], [4.0, 4.0])
    graphLogPDF.draw("RangeDrawLogPDF_2D.png")
    graphLogPDF = dist2D.drawLogPDF([101, 101])
    graphLogPDF.draw("PointNumberDrawLogPDF_2D.png")
    # CDF
    graphCDF = dist2D.drawCDF()
    graphCDF.draw("DefaultDrawCDF_2D.png")
    graphCDF = dist2D.drawCDF([-4.0, -4.0], [4.0, 4.0], [101, 101])
    graphCDF.draw("FullParametersDrawCDF_2D.png")
    graphCDF = dist2D.drawCDF([-4.0, -4.0], [4.0, 4.0])
    graphCDF.draw("RangeDrawCDF_2D.png")
    graphCDF = dist2D.drawCDF([101, 101])
    graphCDF.draw("PointNumberDrawCDF_2D.png")
    # Check drawing methods for ND distributions
    # PDF
    graphPDF = distND.drawMarginal1DPDF(2, -4.0, 4.0, 101)
    graphPDF.draw("FullParametersDrawMarginal1DPDF_ND.png")
    graphPDF = distND.drawMarginal2DPDF(
        2, 3, [-4.0, -4.0], [4.0, 4.0], [101, 101])
    graphPDF.draw("FullParametersDrawMarginal2DPDF_ND.png")
    # log-PDF
    graphLogPDF = distND.drawMarginal1DLogPDF(2, -4.0, 4.0, 101)
    graphLogPDF.draw("FullParametersDrawMarginal1DLogPDF_ND.png")
    graphLogPDF = distND.drawMarginal2DLogPDF(
        2, 3, [-4.0, -4.0], [4.0, 4.0], [101, 101])
    graphLogPDF.draw("FullParametersDrawMarginal2DLogPDF_ND.png")
    # CDF
    graphCDF = distND.drawMarginal1DCDF(2, -4.0, 4.0, 101)
    graphCDF.draw("FullParametersDrawMarginal1DCDF_ND.png")
    graphCDF = distND.drawMarginal2DCDF(
        2, 3, [-4.0, -4.0], [4.0, 4.0], [101, 101])
    graphCDF.draw("FullParametersDrawMarginal2DCDF_ND.png")
    # Quantile
    graphQuantile = dist1D.drawQuantile()
    graphQuantile.draw("DrawQuantile_1D.png")
    graphQuantile = dist2D.drawQuantile()
    graphQuantile.draw("DrawQuantile_2D.png")
except:
    import sys
    print("t_Distributions_draw.py", sys.exc_info()[0], sys.exc_info()[1])
