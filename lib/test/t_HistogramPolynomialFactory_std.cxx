//                                               -*- C++ -*-
/**
 *  @brief The test file of class HistogramPolynomialFactory for standard methods
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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

  try
  {
    Scalar first = -2.0;
    Point width;
    width.add(1.0);
    width.add(2.0);
    width.add(1.5);
    Point height;
    height.add(2.0);
    height.add(5.0);
    height.add(3.5);
    HistogramPolynomialFactory histogramPolynomial(first, width, height);
    fullprint << "histogramPolynomial=" << histogramPolynomial << std::endl;
    for (UnsignedInteger i = 0; i < 10; ++i)
    {
      fullprint << "histogramPolynomial(" << i << ")=" << histogramPolynomial.build(i).__str__() << std::endl;
    }
    Point roots(histogramPolynomial.getRoots(10));
    fullprint << "histogramPolynomial(10) roots=" << roots << std::endl;
    Point weights;
    Point nodes(histogramPolynomial.getNodesAndWeights(10, weights));
    fullprint << "histogramPolynomial(10) nodes=" << nodes << " and weights=" << weights <<  std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
