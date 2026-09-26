//                                               -*- C++ -*-
/**
 *  @brief The test file of namespace FastGaussQuadrature for standard methods
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
#include <cmath>

using namespace OT;
using namespace OT::Test;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    // A checker for invalid arguments
    {
      Point nodes(1);
      Point weights(1);
      Bool thrown = false;
      try
      {
        FastGaussQuadrature::PolishedSolve(&nodes[0], &weights[0], 0, &nodes[0], &weights[0]);
      }
      catch (const InvalidArgumentException & ex)
      {
        thrown = true;
        fullprint << "OK: n=0 throws " << ex.type() << std::endl;
      }
      if (!thrown) throw TestFailed("expected InvalidArgumentException for n==0");
    }
    // n=1: single node equals the diagonal entry, weight is one
    {
      Point gamma(1, 3.0);
      Point b(1, 0.0);
      Point nodes(1);
      Point weights(1);
      FastGaussQuadrature::PolishedSolve(&gamma[0], &b[0], 1, &nodes[0], &weights[0]);
      assert_almost_equal(nodes[0], 3.0, 0.0, 1.0e-14, ", n=1 node");
      assert_almost_equal(weights[0], 1.0, 0.0, 1.0e-14, ", n=1 weight");
    }
    // n=2 Hermite matrix: nodes at +/-1, weights 1/2
    {
      Point gamma(2, 0.0);
      Point b(2, 0.0);
      b[1] = 1.0;
      Point nodes(2);
      Point weights(2);
      FastGaussQuadrature::PolishedSolve(&gamma[0], &b[0], 2, &nodes[0], &weights[0]);
      assert_almost_equal(nodes, Point({-1.0, 1.0}), 1.0e-12, 1.0e-12, ", n=2 Hermite nodes");
      assert_almost_equal(weights, Point({0.5, 0.5}), 1.0e-12, 1.0e-12, ", n=2 Hermite weights");
    }
    // n=3 Hermite matrix: nodes at -sqrt(3), 0, sqrt(3), weights 1/6, 2/3, 1/6
    {
      Point gamma(3, 0.0);
      Point b(3, 0.0);
      b[1] = 1.0;
      b[2] = std::sqrt(2.0);
      Point nodes(3);
      Point weights(3);
      FastGaussQuadrature::PolishedSolve(&gamma[0], &b[0], 3, &nodes[0], &weights[0]);
      assert_almost_equal(nodes, Point({-std::sqrt(3.0), 0.0, std::sqrt(3.0)}), 1.0e-12, 1.0e-12, ", n=3 Hermite nodes");
      assert_almost_equal(weights, Point({1.0 / 6.0, 2.0 / 3.0, 1.0 / 6.0}), 1.0e-12, 1.0e-12, ", n=3 Hermite weights");
    }
    // n=4 Legendre matrix: the rule integrates polynomials of degree up to 7
    // w.r.t. the uniform probability measure on [-1, 1]
    {
      const UnsignedInteger n = 4;
      Point gamma(n, 0.0);
      Point b(n, 0.0);
      for (UnsignedInteger j = 1; j < n; ++j)
        b[j] = static_cast<Scalar>(j) / std::sqrt(4.0 * j * j - 1.0);
      Point nodes(n);
      Point weights(n);
      FastGaussQuadrature::PolishedSolve(&gamma[0], &b[0], n, &nodes[0], &weights[0]);
      // structural properties
      Scalar sum = 0.0;
      for (UnsignedInteger i = 0; i < n; ++i) sum += weights[i];
      assert_almost_equal(sum, 1.0, 1.0e-12, 1.0e-12, ", weight sum");
      for (UnsignedInteger i = 0; i < n; ++i)
      {
        if (!(weights[i] > 0.0)) throw TestFailed(OSS() << "non positive weight " << weights[i]);
        if (i > 0)
          if (nodes[i] <= nodes[i - 1]) throw TestFailed(OSS() << "nodes not strictly increasing: " << nodes[i - 1] << " >= " << nodes[i]);
      }
      if (!(nodes[0] >= -1.0) || !(nodes[n - 1] <= 1.0)) throw TestFailed("node outside [-1, 1]");
      // exactness up to degree 2n-1
      for (UnsignedInteger m = 0; m < 2 * n; ++m)
      {
        Scalar integral = 0.0;
        for (UnsignedInteger i = 0; i < n; ++i)
          integral += weights[i] * std::pow(nodes[i], static_cast<Scalar>(m));
        const Scalar ref = (m % 2 == 0) ? 1.0 / (m + 1.0) : 0.0;
        assert_almost_equal(integral, ref, 1.0e-12, 1.0e-12, OSS() << ", degree " << m);
      }
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }
  catch (const InvalidArgumentException & ex)
  {
    fullprint << "Unexpected InvalidArgumentException: " << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}