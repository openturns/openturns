//                                               -*- C++ -*-
/**
 *  @brief The test file of class Normal for standard methods
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  // UnsignedInteger pointNumber(251);

  try
  {
    Normal dist1D(2.0, 4.0);
    Normal dist2D(Point(2, 2.0), Point(2, 4.0), CorrelationMatrix(2));
    Normal distND(Point(5, 2.0), Point(5, 4.0), CorrelationMatrix(5));
    // Check drawing methods for 1D distributions
    // PDF
    Graph graphPDF(dist1D.drawPDF());
    graphPDF.draw("DefaultDrawPDF_1D.png");
    graphPDF = dist1D.drawPDF(-4.0, 4.0, 101);
    graphPDF.draw("FullParametersDrawPDF_1D.png");
    graphPDF = dist1D.drawPDF(-4.0, 4.0);
    graphPDF.draw("RangeDrawPDF_1D.png");
    graphPDF = dist1D.drawPDF(101);
    graphPDF.draw("PointNumberDrawPDF_1D.png");
    // log-PDF
    Graph graphLogPDF(dist1D.drawLogPDF());
    graphLogPDF.draw("DefaultDrawLogPDF_1D.png");
    graphLogPDF = dist1D.drawLogPDF(-4.0, 4.0, 101);
    graphLogPDF.draw("FullParametersDrawLogPDF_1D.png");
    graphLogPDF = dist1D.drawLogPDF(-4.0, 4.0);
    graphLogPDF.draw("RangeDrawLogPDF_1D.png");
    graphLogPDF = dist1D.drawLogPDF(101);
    graphLogPDF.draw("PointNumberDrawLogPDF_1D.png");
    // CDF
    Graph graphCDF(dist1D.drawCDF());
    graphCDF.draw("DefaultDrawCDF_1D.png");
    graphCDF = dist1D.drawCDF(-4.0, 4.0, 101);
    graphCDF.draw("FullParametersDrawCDF_1D.png");
    graphCDF = dist1D.drawCDF(-4.0, 4.0);
    graphCDF.draw("RangeDrawCDF_1D.png");
    graphCDF = dist1D.drawCDF(101);
    graphCDF.draw("PointNumberDrawCDF_1D.png");
    Graph graphQuantile(dist1D.drawQuantile());
    graphQuantile.draw("DefaultDrawQuantile_1D.png");
    graphQuantile = dist1D.drawQuantile(101);
    graphQuantile.draw("PointNumberDrawQuantile_1D.png");
    graphQuantile = dist1D.drawQuantile(0.1, 0.9, 101);
    graphQuantile.draw("FullParameterDrawQuantile_1D.png");
    graphQuantile = dist1D.drawQuantile(0.1, 0.9);
    graphQuantile.draw("RangeDrawQuantile_1D.png");
    // Check drawing methods for 2D distributions
    // PDF
    graphPDF = dist2D.drawPDF();
    graphPDF.draw("DefaultDrawPDF_2D.png");
    graphPDF = dist2D.drawPDF(Point(2, -4.0), Point(2, 4.0), Indices(2, 101));
    graphPDF.draw("FullParametersDrawPDF_2D.png");
    graphPDF = dist2D.drawPDF(Point(2, -4.0), Point(2, 4.0));
    graphPDF.draw("RangeDrawPDF_2D.png");
    graphPDF = dist2D.drawPDF(Indices(2, 101));
    graphPDF.draw("PointNumberDrawPDF_2D.png");
    // log-PDF
    graphLogPDF = dist2D.drawLogPDF();
    graphLogPDF.draw("DefaultDrawLogPDF_2D.png");
    graphLogPDF = dist2D.drawLogPDF(Point(2, -4.0), Point(2, 4.0), Indices(2, 101));
    graphLogPDF.draw("FullParametersDrawLogPDF_2D.png");
    graphLogPDF = dist2D.drawPDF(Point(2, -4.0), Point(2, 4.0));
    graphLogPDF.draw("RangeDrawLogPDF_2D.png");
    graphLogPDF = dist2D.drawLogPDF(Indices(2, 101));
    graphLogPDF.draw("PointNumberDrawLogPDF_2D.png");
    // CDF
    graphCDF = dist2D.drawCDF();
    graphCDF.draw("DefaultDrawCDF_2D.png");
    graphCDF = dist2D.drawCDF(Point(2, -4.0), Point(2, 4.0), Indices(2, 101));
    graphCDF.draw("FullParametersDrawCDF_2D.png");
    graphCDF = dist2D.drawCDF(Point(2, -4.0), Point(2, 4.0));
    graphCDF.draw("RangeDrawCDF_2D.png");
    graphCDF = dist2D.drawCDF(Indices(2, 101));
    graphCDF.draw("PointNumberDrawCDF_2D.png");
    // Check drawing methods for ND distributions
    // PDF
    graphPDF = distND.drawMarginal1DPDF(2, -4.0, 4.0, 101);
    graphPDF.draw("FullParametersDrawMarginal1DPDF_ND.png");
    graphPDF = distND.drawMarginal2DPDF(2, 3, Point(2, -4.0), Point(2, 4.0), Indices(2, 101));
    graphPDF.draw("FullParametersDrawMarginal2DPDF_ND.png");
    // log-PDF
    graphLogPDF = distND.drawMarginal1DLogPDF(2, -4.0, 4.0, 101);
    graphLogPDF.draw("FullParametersDrawMarginal1DLogPDF_ND.png");
    graphLogPDF = distND.drawMarginal2DLogPDF(2, 3, Point(2, -4.0), Point(2, 4.0), Indices(2, 101));
    graphPDF.draw("FullParametersDrawMarginal2DLogPDF_ND.png");
    // CDF
    graphCDF = distND.drawMarginal1DCDF(2, -4.0, 4.0, 101);
    graphCDF.draw("FullParametersDrawMarginal1DCDF_ND.png");
    graphCDF = distND.drawMarginal2DCDF(2, 3, Point(2, -4.0), Point(2, 4.0), Indices(2, 101));
    graphCDF.draw("FullParametersDrawMarginal2DCDF_ND.png");
    // Quantile
    graphQuantile = dist2D.drawQuantile();
    graphQuantile.draw("DefaultDrawQuantile_2D.png");
    graphQuantile = dist2D.drawQuantile(101);
    graphQuantile.draw("PointNumberDrawQuantile_2D.png");
    graphQuantile = dist2D.drawQuantile(0.1, 0.9, 101);
    graphQuantile.draw("FullParameterDrawQuantile_2D.png");
    graphQuantile = dist2D.drawQuantile(0.1, 0.9);
    graphQuantile.draw("RangeDrawQuantile_2D.png");
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
