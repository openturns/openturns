//                                               -*- C++ -*-
/**
 *  @brief The test file of class Normal for standard methods
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
    graphPDF = dist1D.drawPDF(-4.0, 4.0, 101);
    graphPDF = dist1D.drawPDF(-4.0, 4.0);
    graphPDF = dist1D.drawPDF(101);
    // log-PDF
    Graph graphLogPDF(dist1D.drawLogPDF());
    graphLogPDF = dist1D.drawLogPDF(-4.0, 4.0, 101);
    graphLogPDF = dist1D.drawLogPDF(-4.0, 4.0);
    graphLogPDF = dist1D.drawLogPDF(101);
    // CDF
    Graph graphCDF(dist1D.drawCDF());
    graphCDF = dist1D.drawCDF(-4.0, 4.0, 101);
    graphCDF = dist1D.drawCDF(-4.0, 4.0);
    graphCDF = dist1D.drawCDF(101);
    Graph graphQuantile(dist1D.drawQuantile());
    graphQuantile = dist1D.drawQuantile(101);
    graphQuantile = dist1D.drawQuantile(0.1, 0.9, 101);
    graphQuantile = dist1D.drawQuantile(0.1, 0.9);
    // Check drawing methods for 2D distributions
    // PDF
    graphPDF = dist2D.drawPDF();
    graphPDF = dist2D.drawPDF(Point(2, -4.0), Point(2, 4.0), Indices(2, 101));
    graphPDF = dist2D.drawPDF(Point(2, -4.0), Point(2, 4.0));
    graphPDF = dist2D.drawPDF(Indices(2, 101));
    // log-PDF
    graphLogPDF = dist2D.drawLogPDF();
    graphLogPDF = dist2D.drawLogPDF(Point(2, -4.0), Point(2, 4.0), Indices(2, 101));
    graphLogPDF = dist2D.drawPDF(Point(2, -4.0), Point(2, 4.0));
    graphLogPDF = dist2D.drawLogPDF(Indices(2, 101));
    // CDF
    graphCDF = dist2D.drawCDF();
    graphCDF = dist2D.drawCDF(Point(2, -4.0), Point(2, 4.0), Indices(2, 101));
    graphCDF = dist2D.drawCDF(Point(2, -4.0), Point(2, 4.0));
    graphCDF = dist2D.drawCDF(Indices(2, 101));
    // Check drawing methods for ND distributions
    // PDF
    graphPDF = distND.drawMarginal1DPDF(2, -4.0, 4.0, 101);
    graphPDF = distND.drawMarginal2DPDF(2, 3, Point(2, -4.0), Point(2, 4.0), Indices(2, 101));
    // log-PDF
    graphLogPDF = distND.drawMarginal1DLogPDF(2, -4.0, 4.0, 101);
    graphLogPDF = distND.drawMarginal2DLogPDF(2, 3, Point(2, -4.0), Point(2, 4.0), Indices(2, 101));
    // CDF
    graphCDF = distND.drawMarginal1DCDF(2, -4.0, 4.0, 101);
    graphCDF = distND.drawMarginal2DCDF(2, 3, Point(2, -4.0), Point(2, 4.0), Indices(2, 101));
    // Quantile
    graphQuantile = dist2D.drawQuantile();
    graphQuantile = dist2D.drawQuantile(101);
    graphQuantile = dist2D.drawQuantile(0.1, 0.9, 101);
    graphQuantile = dist2D.drawQuantile(0.1, 0.9);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
