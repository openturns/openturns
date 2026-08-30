//                                               -*- C++ -*-
/**
 *  @brief The test file of namespace FastLegendre for standard methods
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
#include <algorithm>
#include <cmath>

using namespace OT;
using namespace OT::Test;

// Sort the nodes in increasing order, applying the same permutation to the
// weights so that both arrays stay matched pairwise
static void sortAscending(Point & nodes, Point & weights)
{
  const UnsignedInteger n = nodes.getDimension();
  for (UnsignedInteger i = 0; i < n; ++i)
    for (UnsignedInteger j = i + 1; j < n; ++j)
      if (nodes[j] < nodes[i])
      {
        std::swap(nodes[i], nodes[j]);
        std::swap(weights[i], weights[j]);
      }
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
        FastLegendre::ComputeNodesAndWeights(0, &nodes[0], &weights[0]);
      }
      catch (const InvalidArgumentException & ex)
      {
        thrown = true;
        fullprint << "OK: n=0 throws " << ex.type() << std::endl;
      }
      if (!thrown) throw TestFailed("expected InvalidArgumentException for n==0");
    }
    // n=1: single node at 0, weight two (weight function is dx on [-1,1])
    {
      Point nodes(1);
      Point weights(1);
      FastLegendre::ComputeNodesAndWeights(1, &nodes[0], &weights[0]);
      assert_almost_equal(nodes[0], 0.0, 0.0, 1.0e-14, ", n=1 node");
      assert_almost_equal(weights[0], 2.0, 0.0, 1.0e-14, ", n=1 weight");
    }
    // Cross-check against the reference Golub-Welsch implementation exposed
    // by LegendreFactory, whose weights sum to 1 w.r.t. the uniform
    // probability measure on [-1, 1]: FastLegendre weights are twice larger
    {
      const UnsignedInteger order[] = {4, 10, 64};
      const LegendreFactory refFactory;
      for (UnsignedInteger q = 0; q < 3; ++q)
      {
        const UnsignedInteger n = order[q];
        Point nodes(n);
        Point weights(n);
        FastLegendre::ComputeNodesAndWeights(n, &nodes[0], &weights[0]);
        sortAscending(nodes, weights);
        Point refWeights;
        const Point refNodes(refFactory.getNodesAndWeights(n, refWeights));
        for (UnsignedInteger i = 0; i < n; ++i)
          refWeights[i] *= 2.0;
        assert_almost_equal(nodes, refNodes, 1.0e-10, 1.0e-12, OSS() << ", n=" << n << " nodes");
        assert_almost_equal(weights, refWeights, 1.0e-10, 1.0e-12, OSS() << ", n=" << n << " weights");
      }
    }
    // Exactness check: an n-point rule integrates polynomials of degree up to
    // 2n-1 exactly for the Lebesgue measure on [-1, 1]
    {
      const UnsignedInteger order[] = {4, 8};
      for (UnsignedInteger q = 0; q < 2; ++q)
      {
        const UnsignedInteger n = order[q];
        Point nodes(n);
        Point weights(n);
        FastLegendre::ComputeNodesAndWeights(n, &nodes[0], &weights[0]);
        for (UnsignedInteger m = 0; m < 2 * n; ++m)
        {
          Scalar integral = 0.0;
          for (UnsignedInteger i = 0; i < n; ++i)
            integral += weights[i] * std::pow(nodes[i], static_cast<Scalar>(m));
          const Scalar ref = (m % 2 == 0) ? 2.0 / (m + 1.0) : 0.0;
          assert_almost_equal(integral, ref, 1.0e-10, 1.0e-12, OSS() << ", n=" << n << " degree " << m);
        }
      }
    }
    // Structural properties: symmetric nodes in [-1, 1], positive weights
    // summing to two
    {
      const UnsignedInteger n = 32;
      Point nodes(n);
      Point weights(n);
      FastLegendre::ComputeNodesAndWeights(n, &nodes[0], &weights[0]);
      Scalar sum = 0.0;
      for (UnsignedInteger i = 0; i < n; ++i)
      {
        if (!(weights[i] > 0.0)) throw TestFailed(OSS() << "non positive weight " << weights[i]);
        if ((nodes[i] < -1.0) || (nodes[i] > 1.0)) throw TestFailed(OSS() << "node outside [-1, 1] " << nodes[i]);
        assert_almost_equal(nodes[i] + nodes[n - 1 - i], 0.0, 1.0e-10, 1.0e-12, OSS() << ", symmetry i=" << i);
        assert_almost_equal(weights[i], weights[n - 1 - i], 1.0e-10, 1.0e-12, OSS() << ", weight symmetry i=" << i);
        sum += weights[i];
      }
      assert_almost_equal(sum, 2.0, 1.0e-10, 1.0e-10, ", weight sum");
    }
    // Large-n sanity check
    {
      const UnsignedInteger n = 4096;
      Point nodes(n);
      Point weights(n);
      FastLegendre::ComputeNodesAndWeights(n, &nodes[0], &weights[0]);
      Scalar sum = 0.0;
      for (UnsignedInteger i = 0; i < n; ++i)
      {
        sum += weights[i];
        if ((nodes[i] <= -1.0) || (nodes[i] >= 1.0) || !std::isfinite(nodes[i]))
          throw TestFailed(OSS() << "invalid Legendre node " << nodes[i]);
      }
      assert_almost_equal(sum, 2.0, 1.0e-8, 1.0e-8, ", n=4096 weight sum");
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