//                                               -*- C++ -*-
/**
 *  @brief The test file of composite RandomVector class
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
  setRandomGenerator();

  try
  {

    /* We create a Function */
    Description input_r(4);
    input_r[0] = "x1";
    input_r[1] = "x2";
    input_r[2] = "x3";
    input_r[3] = "x4";
    Description formula_r(2);
    formula_r[0] = "(x1*x1+x2^3*x1)/(2*x3*x3+x4^4+1)";
    formula_r[1] = "cos(x2*x2+x4)/(x1*x1+1+x3^4)";
    SymbolicFunction myFunction(input_r, formula_r);

    /* We create a distribution */
    UnsignedInteger dim = myFunction.getInputDimension();
    CorrelationMatrix R(dim);
    for (UnsignedInteger i = 0; i < dim; i++)
    {
      R(i, i) = 1.0;
      for (UnsignedInteger j = 0; j < i; j++)
      {
        R(i, j) = Scalar(j + 1) / dim;
      }
    }
    Point m(dim, 1.0);
    Point s(dim, 2.0);
    Normal distribution(m, s, R);
    const Normal & ref_distribution(distribution);
    fullprint << "distribution = " << ref_distribution << std::endl;


    /* We create a distribution-based RandomVector */
    RandomVector X(distribution);
    fullprint << "X=" << X << std::endl;
    fullprint << "is composite? " << X.isComposite() << std::endl;

    /* Check standard methods of class RandomVector */
    fullprint << "X dimension=" << X.getDimension() << std::endl;
    fullprint << "X realization (first )=" << X.getRealization() << std::endl;
    fullprint << "X realization (second)=" << X.getRealization() << std::endl;
    fullprint << "X realization (third )=" << X.getRealization() << std::endl;
    fullprint << "X sample =" << X.getSample(5) << std::endl;





    /* We create a composite RandomVector Y from X and myFunction */
    RandomVector Y(CompositeRandomVector(myFunction, X));
    fullprint << "Y=" << Y << std::endl;
    fullprint << "is composite? " << Y.isComposite() << std::endl;

    /* Check standard methods of class RandomVector */
    fullprint << "Y dimension=" << Y.getDimension() << std::endl;
    fullprint << "Y realization (first )=" << Y.getRealization() << std::endl;
    fullprint << "Y realization (second)=" << Y.getRealization() << std::endl;
    fullprint << "Y realization (third )=" << Y.getRealization() << std::endl;
    fullprint << "Y sample =" << Y.getSample(5) << std::endl;



  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
