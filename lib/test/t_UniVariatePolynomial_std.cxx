//                                               -*- C++ -*-
/**
 *  @brief The test file of class UniVariatePolynomial for standard methods
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
    UniVariatePolynomial P;
    Scalar point = 1.2;
    fullprint << "P=" << P << std::endl;
    fullprint << "P(" << point << ")=" << P(point) << std::endl;

    // Constructor: construct the 1D polynomial with a vector of Coefficients e.g. UniVariatePolynomial P1 ( coefficients )
    UniVariatePolynomial::Coefficients coefficients(6);
    coefficients[0] = 1.3;
    coefficients[1] = 2.4;
    coefficients[2] = 2.0;
    coefficients[3] = -0.5;
    coefficients[4] = 4.0;
    coefficients[5] = 0.0;

    fullprint << "Initial coefficients=" << coefficients << std::endl;
    UniVariatePolynomial P1(coefficients);
    fullprint << "P1=" << P1 << std::endl;
    fullprint << "P1=" << P1.__str__() << std::endl;
    fullprint << "P1=" << P1.__str__("Y", "") << std::endl;
    fullprint << "P1 degree=" << P1.getDegree() << std::endl;
    fullprint << "P1 roots=" << P1.getRoots() << std::endl;
    fullprint << "P1(" << point << ")=" << P1(point) << std::endl;

    // Multiplication by a scalar (postfix only)
    fullprint << "P1*2.0 =" << P1 * 2.0 << std::endl;

    // P1.incrementDegree(incDeg) : multiply P1 by X^incDeg
    fullprint << "P1.incrementDegree(3)=" << P1.incrementDegree(3) << std::endl;
    fullprint << "P1.incrementDegree(0)=" << P1.incrementDegree(0) << std::endl;
    fullprint << "P1.incrementDegree(1)=" << P1.incrementDegree(1) << std::endl;
    fullprint << "P1.incrementDegree()=" << P1.incrementDegree() << std::endl;

    // Sum of two polynomials
    coefficients = UniVariatePolynomial::Coefficients(3);
    coefficients[0] = -2.5;
    coefficients[1] = 3.5;
    coefficients[2] = -1.2;
    UniVariatePolynomial P2(coefficients);
    fullprint << "P2=" << P2 << std::endl;
    fullprint << "P1+P2=" << P1 + P2 << std::endl;

    // Subtraction of two polynomials
    fullprint << "P1-P2=" << P1 - P2 << std::endl;

    // Multiplication of two polynomials
    fullprint << "P1*P2=" << P1 * P2 << std::endl;

    // Multiplication of two polynomials using FFT
    fullprint << "P1*P2=" << P1 * P2 << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}

