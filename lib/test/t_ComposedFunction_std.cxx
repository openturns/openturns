//                                               -*- C++ -*-
/**
 *  @brief The test file of class Function for standard methods
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

    /** Left hand side of the composition */
    Description input(2);
    input[0] = "x1";
    input[1] = "x2";
    Description formula(3);
    formula[0] = "x1*sin(x2)";
    formula[1] = "cos(x1+x2)";
    formula[2] = "(x2+1)*exp(x1-2*x2)";
    SymbolicFunction left(input, formula);

    /** Right hand side of the composition */
    Description input_r(4);
    input_r[0] = "x1";
    input_r[1] = "x2";
    input_r[2] = "x3";
    input_r[3] = "x4";
    Description formula_r(2);
    formula_r[0] = "(x1*x1+x2^3*x1)/(2*x3*x3+x4^4+1)";
    formula_r[1] = "cos(x2*x2+x4)/(x1*x1+1+x3^4)";
    SymbolicFunction right(input_r, formula_r);

    /** Compositon of left and right */
    Function composed(ComposedFunction(left, right));

    fullprint << "right=" << right << std::endl;
    fullprint << "left=" << left << std::endl;
    fullprint << "composed=" << composed << std::endl;

    /** Does it worked? */
    Point x(right.getInputDimension(), 1.0);
    Point y(right(x));
    Point z(left(y));
    Matrix Dy(right.gradient(x));
    Matrix Dz(left.gradient(y));

    fullprint << "x=" << x << " y=right(x)=" << y << " z=left(y)=" << z << std::endl;
    fullprint << "left(right(x))=" << composed(x) << std::endl;
    fullprint << "D(right)(x)=" << Dy << " D(left)(y)=" << Dz;
    fullprint << " prod=" << Dy * Dz << std::endl;
    fullprint << "D(left(right(x)))=" << composed.gradient(x) << std::endl;
    SymmetricTensor result(composed.hessian(x));
    fullprint << "DD(left(right(x)))=" << std::endl;
    for(UnsignedInteger k = 0; k < result.getNbSheets(); k++)
    {
      for(UnsignedInteger j = 0; j < result.getNbColumns(); j++)
      {
        for(UnsignedInteger i = 0; i < result.getNbRows(); i++)
        {
          fullprint << std::setw(14) << std::scientific << std::right << result(i, j, k);
        }
        fullprint << std::endl;
      }
      fullprint << std::endl;
    }
    for (UnsignedInteger i = 0; i < composed.getOutputDimension(); ++i)
    {
      fullprint << "Marginal " << i << "=" << composed.getMarginal(i) << std::endl;
    }
    Indices indices(2);
    indices[0] = 0;
    indices[1] = 1;
    fullprint << "Marginal (0,1)=" << composed.getMarginal(indices) << std::endl;
    indices[0] = 0;
    indices[1] = 2;
    fullprint << "Marginal (0,2)=" << composed.getMarginal(indices) << std::endl;
    indices[0] = 1;
    indices[1] = 2;
    fullprint << "Marginal (1,2)=" << composed.getMarginal(indices) << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
