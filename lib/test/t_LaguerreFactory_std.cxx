//                                               -*- C++ -*-
/**
 *  @brief The test file of class LaguerreFactory for standard methods
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
    // Standard 1-arg factory
    LaguerreFactory laguerre(2.5);
    fullprint << "laguerre=" << laguerre << std::endl;

    // Build first 10 polynomials and check coefficient evaluation consistency
    for (UnsignedInteger i = 0; i < 10; ++i)
    {
      auto p = laguerre.build(i);
      fullprint << "laguerre(" << i << ")=" << p.__str__() << std::endl;
      Point coeffs(p.getCoefficients());
      for (Scalar x = 0.0; x <= 2.0; x += 1.0)
      {
        Scalar yCoeff = 0.0;
        for (UnsignedInteger j = 0; j < coeffs.getDimension(); ++j)
          yCoeff += coeffs[j] * std::pow(x, j);
        assert_almost_equal(yCoeff, p(x));
      }
    }

    // Check orthonormality using Gauss quadrature
    {
      const UnsignedInteger degreeMax = 6;
      const UnsignedInteger m = degreeMax + 1;
      Point weights;
      Point nodes = laguerre.getNodesAndWeights(m, weights);
      for (UnsignedInteger i = 0; i <= degreeMax; ++i)
      {
        auto pI = laguerre.build(i);
        for (UnsignedInteger j = 0; j <= i; ++j)
        {
          auto pJ = laguerre.build(j);
          Scalar val = 0.0;
          for (UnsignedInteger k = 0; k < m; ++k)
            val += weights[k] * pI(nodes[k]) * pJ(nodes[k]);
          assert_almost_equal(val, (i == j) ? 1.0 : 0.0);
        }
      }
    }

    // Check roots
    Point roots(laguerre.getRoots(10));
    fullprint << "laguerre(10) roots=" << roots << std::endl;
    assert_equal(static_cast<UnsignedInteger>(roots.getDimension()), static_cast<UnsignedInteger>(10));
    auto p10 = laguerre.build(10);
    for (UnsignedInteger i = 0; i < 10; ++i)
      assert_almost_equal(p10(roots[i]), 0.0);

    // Check nodes and weights
    Point weights2;
    roots = laguerre.getNodesAndWeights(10, weights2);
    fullprint << "laguerre(10) nodes=" << roots << " and weights=" << weights2 << std::endl;
    assert_equal(static_cast<UnsignedInteger>(weights2.getDimension()), static_cast<UnsignedInteger>(10));
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
