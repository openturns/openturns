//                                               -*- C++ -*-
/**
 *  @brief The test file of namespace FastHermite for standard methods
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

// (k-1)!! for even k, 0 for odd k
static Scalar normalEvenMoment(const UnsignedInteger k)
{
  if (k % 2 == 1) return 0.0;
  Scalar value = 1.0;
  for (UnsignedInteger j = 1; j < k; j += 2)
    value *= j;
  return value;
}

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    // Invalid argument: n == 0
    {
      Point nodes(1);
      Point weights(1);
      Bool thrown = false;
      try
      {
        FastHermite::ComputeNodesAndWeights(0, &nodes[0], &weights[0]);
      }
      catch (const InvalidArgumentException & ex)
      {
        thrown = true;
        fullprint << "OK: n=0 throws " << ex.type() << std::endl;
      }
      if (!thrown) throw TestFailed("expected InvalidArgumentException for n==0");
    }
    // n=1: single node at 0, weight one
    {
      Point nodes(1);
      Point weights(1);
      FastHermite::ComputeNodesAndWeights(1, &nodes[0], &weights[0]);
      assert_almost_equal(nodes[0], 0.0, 0.0, 1.0e-14, ", n=1 node");
      assert_almost_equal(weights[0], 1.0, 0.0, 1.0e-14, ", n=1 weight");
    }
    // Cross-check against the reference Golub-Welsch implementation exposed
    // by HermiteFactory (weight = standard normal density, orthonormal basis)
    {
      const UnsignedInteger order[] = {4, 12, 40, 1024};
      const HermiteFactory refFactory;
      for (UnsignedInteger q = 0; q < 4; ++q)
      {
        const UnsignedInteger n = order[q];
        Point nodes(n);
        Point weights(n);
        FastHermite::ComputeNodesAndWeights(n, &nodes[0], &weights[0]);
        Point refWeights;
        const Point refNodes(refFactory.getNodesAndWeights(n, refWeights));
        assert_almost_equal(nodes, refNodes, 1.0e-10, 1.0e-12, OSS() << ", n=" << n << " nodes");
        assert_almost_equal(weights, refWeights, 1.0e-10, 1.0e-12, OSS() << ", n=" << n << " weights");
      }
    }
    // Exactness check: an n-point rule integrates polynomials of degree up to
    // 2n-1 exactly w.r.t. N(0, 1)
    {
      const UnsignedInteger order[] = {2, 4, 8};
      for (UnsignedInteger q = 0; q < 3; ++q)
      {
        const UnsignedInteger n = order[q];
        Point nodes(n);
        Point weights(n);
        FastHermite::ComputeNodesAndWeights(n, &nodes[0], &weights[0]);
        for (UnsignedInteger m = 0; m < 2 * n; ++m)
        {
          Scalar integral = 0.0;
          for (UnsignedInteger i = 0; i < n; ++i)
            integral += weights[i] * std::pow(nodes[i], static_cast<Scalar>(m));
          assert_almost_equal(integral, normalEvenMoment(m), 1.0e-7, 1.0e-9, OSS() << ", n=" << n << " degree " << m);
        }
      }
    }
    // Structural properties for a moderate n
    {
      const UnsignedInteger n = 16;
      Point nodes(n);
      Point weights(n);
      FastHermite::ComputeNodesAndWeights(n, &nodes[0], &weights[0]);
      for (UnsignedInteger i = 0; i < n; ++i)
      {
        if (!(weights[i] > 0.0)) throw TestFailed(OSS() << "non positive weight " << weights[i]);
        // the nodes are symmetric with respect to 0
        assert_almost_equal(nodes[i] + nodes[n - 1 - i], 0.0, 1.0e-10, 1.0e-12, OSS() << ", symmetry i=" << i);
        assert_almost_equal(weights[i], weights[n - 1 - i], 1.0e-10, 1.0e-12, OSS() << ", weight symmetry i=" << i);
      }
    }
    // Large-n sanity check: weights positive, nodes finite and ascending
    {
      const UnsignedInteger n = 1024;
      Point nodes(n);
      Point weights(n);
      FastHermite::ComputeNodesAndWeights(n, &nodes[0], &weights[0]);
      Scalar sum = 0.0;
      for (UnsignedInteger i = 0; i < n; ++i)
      {
        sum += weights[i];
        if ((weights[i] < 0.0) || !std::isfinite(weights[i]))
          throw TestFailed(OSS() << "Hermite weight " << weights[i] << " invalid for n=" << n);
        if (!std::isfinite(nodes[i]))
          throw TestFailed(OSS() << "Hermite node " << nodes[i] << " not finite for n=" << n);
        if (i > 0)
          if (nodes[i] <= nodes[i - 1])
            throw TestFailed(OSS() << "Hermite nodes not increasing: " << nodes[i - 1] << " >= " << nodes[i]);
      }
      assert_almost_equal(sum, 1.0, 1.0e-9, 1.0e-9, ", n=1024 weight sum");
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