//                                               -*- C++ -*-
/**
 *  @brief The test file of class OrthogonalBasis for standard methods
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
    UnsignedInteger dim = 3;
    OrthogonalProductPolynomialFactory::PolynomialFamilyCollection polynomCollection(dim);
    polynomCollection[0] = LaguerreFactory(2.5);
    polynomCollection[1] = LegendreFactory();
    polynomCollection[2] = HermiteFactory();
    OrthogonalProductPolynomialFactory basisFactory(polynomCollection);
    OrthogonalBasis basis(basisFactory);
    fullprint << "Basis=" << basis << std::endl;
    NumericalPoint point(dim, 0.5);
    for (UnsignedInteger i = 0; i < 10; ++i)
    {
      NumericalMathFunction f(basis.build(i));
      fullprint << "i=" << i << " f(point)=" << f(point) << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
