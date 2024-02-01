//                                               -*- C++ -*-
/**
 *  @brief The test file of class OrthogonalUniVariatePolynomial for standard methods
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
    // Default constructor: null polynomial
    OrthogonalUniVariatePolynomial P;
    Scalar point = 1.2;
    fullprint << "P=" << P << std::endl;
    fullprint << "P(" << point << ")=" << P(point) << std::endl;

    // Construction from a collection of recurrence coefficients
    Sample recurrenceCoefficients(3, 3);
    recurrenceCoefficients(0, 0) = 1.1;
    recurrenceCoefficients(0, 1) = 2.2;
    recurrenceCoefficients(0, 2) = 3.3;
    recurrenceCoefficients(1, 0) = 0.111;
    recurrenceCoefficients(1, 1) = 0.222;
    recurrenceCoefficients(1, 2) = 0.333;
    recurrenceCoefficients(2, 0) = 0.01111;
    recurrenceCoefficients(2, 1) = 0.02222;
    recurrenceCoefficients(2, 2) = 0.03333;
    fullprint << "Recurrence coefficients=" << recurrenceCoefficients << std::endl;
    OrthogonalUniVariatePolynomial Q(recurrenceCoefficients);
    fullprint << "Q=" << Q << std::endl;
    fullprint << "Q(" << point << ")=" << Q(point) << std::endl;
    fullprint << "as an UniVariatePolynomial, Q(" << point << ")=" << UniVariatePolynomial(Q)(point) << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}

