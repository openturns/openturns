//                                               -*- C++ -*-
/**
 *  @brief The test file of class OrthogonalBasis for standard methods
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
    UnsignedInteger dim = 3;
    OrthogonalProductPolynomialFactory::PolynomialFamilyCollection polynomCollection(dim);
    polynomCollection[0] = LaguerreFactory(3.5);
    polynomCollection[1] = LegendreFactory();
    polynomCollection[2] = HermiteFactory();
    OrthogonalProductPolynomialFactory basisFactory(polynomCollection);
    OrthogonalBasis basis(basisFactory);
    Point point(dim, 0.5);

    // Test build by index and build by multi-index produce the same result
    EnumerateFunction enumFunction(basis.getEnumerateFunction());
    for (UnsignedInteger i = 0; i < 10; ++i)
    {
      Function fByIndex(basis.build(i));
      Indices indices(enumFunction(i));
      Function fByMultiIndex(basis.build(indices));
      Point valByIndex(fByIndex(point));
      Point valByMultiIndex(fByMultiIndex(point));
      fullprint << "i=" << i << " f(point)=" << valByIndex << std::endl;
      assert_almost_equal(valByIndex, valByMultiIndex, 1e-14, 0.0);
    }

    // Using multi-indices explicitly
    for (UnsignedInteger i = 0; i < 10; ++i)
    {
      Indices indices(enumFunction(i));
      Function f(basis.build(indices));
      fullprint << "indices=" << indices << " f(point)=" << f(point) << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
