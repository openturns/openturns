//                                               -*- C++ -*-
/**
 *  @brief The test file of class TaylorExpansionsMoments for the standard methods
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {


    /* We create a NumericalMathFunction */
    Description input_r(4);
    input_r[0] = "x1";
    input_r[1] = "x2";
    input_r[2] = "x3";
    input_r[3] = "x4";
    Description output_r(2);
    output_r[0] = "y1";
    output_r[1] = "y2";
    Description formula_r(2);
    formula_r[0] = "(x1*x1+x2^3*x1)/(2*x3*x3+x4^4+1)";
    formula_r[1] = "cos(x2*x2+x4)/(x1*x1+1+x3^4)";
    NumericalMathFunction myFunc(input_r, output_r, formula_r);

    /* We create a distribution */
    UnsignedInteger dim(myFunc.getInputDimension());
    NumericalPoint meanPoint(dim, 0.2);
    NumericalPoint sigma(dim);
    for (UnsignedInteger i = 0; i < dim; i++)
    {
      sigma[i] = 0.1 * (i + 1);
    }
    CorrelationMatrix R = IdentityMatrix(dim);
    for (UnsignedInteger i = 1; i < dim; i++)
    {
      R(i, i - 1) = 0.25;
    }
    Normal distribution(meanPoint, sigma, R);

    /* We create a distribution-based RandomVector */
    RandomVector X(distribution);

    /* We create a composite RandomVector Y from X and myFunction */
    RandomVector Y(myFunc, X);

    // We create a quadraticCumul algorithm
    TaylorExpansionsMoments myTaylorExpansionsMoments(Y);

    // We compute the several elements provided by the quadratic cumul algorithm
    fullprint << "First order mean=" << myTaylorExpansionsMoments.getMeanFirstOrder() << std::endl;
    fullprint << "Second order mean=" << myTaylorExpansionsMoments.getMeanSecondOrder() << std::endl;
    fullprint << "Covariance=" << myTaylorExpansionsMoments.getCovariance() << std::endl;
    fullprint << "Value at mean=" << myTaylorExpansionsMoments.getValueAtMean() << std::endl;
    fullprint << "Gradient at mean=" << myTaylorExpansionsMoments.getGradientAtMean() << std::endl;
    fullprint << "Hessian at mean=" << myTaylorExpansionsMoments.getHessianAtMean() << std::endl;

    TaylorExpansionsMoments myTaylorExpansionsMoments_1(Y.getMarginal(0));
    fullprint << "Importance factors=" << myTaylorExpansionsMoments_1.getImportanceFactors() << std::endl;
  }

  catch (OT::NotSymmetricDefinitePositiveException & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
