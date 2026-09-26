//                                               -*- C++ -*-
/**
 *  @brief The test file of namespace FastLaguerre for standard methods
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

// m-th raw moment of the Gamma(k, 1) distribution = Gamma(k+m)/Gamma(k)
static Scalar gammaMoment(const Scalar k, const UnsignedInteger m)
{
  Scalar value = 1.0;
  for (UnsignedInteger j = 0; j < m; ++j)
    value *= k + j;
  return value;
}

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    // Invalid arguments
    {
      Point nodes(1);
      Point weights(1);
      Bool thrown = false;
      try
      {
        FastLaguerre::ComputeNodesAndWeights(0, 1.0, &nodes[0], &weights[0]);
      }
      catch (const InvalidArgumentException & ex)
      {
        thrown = true;
        fullprint << "OK: n=0 throws " << ex.type() << std::endl;
      }
      if (!thrown) throw TestFailed("expected InvalidArgumentException for n==0");
    }
    {
      Point nodes(1);
      Point weights(1);
      Bool thrown = false;
      try
      {
        FastLaguerre::ComputeNodesAndWeights(4, 0.0, &nodes[0], &weights[0]);
      }
      catch (const InvalidArgumentException & ex)
      {
        thrown = true;
        fullprint << "OK: k=0 throws " << ex.type() << std::endl;
      }
      if (!thrown) throw TestFailed("expected InvalidArgumentException for k==0");
    }
    // n=1: single node at k, weight one
    {
      const Scalar k = 2.5;
      Point nodes(1);
      Point weights(1);
      FastLaguerre::ComputeNodesAndWeights(1, k, &nodes[0], &weights[0]);
      assert_almost_equal(nodes[0], k, 1.0e-12, 1.0e-12, ", n=1 node");
      assert_almost_equal(weights[0], 1.0, 1.0e-12, 1.0e-12, ", n=1 weight");
    }
    // Cross-check against the reference Golub-Welsch implementation exposed
    // by LaguerreFactory: FastLaguerre(k) integrates the Gamma(k, 1) density
    // x^{k-1} exp(-x) / Gamma(k), which is the ANALYSIS measure of
    // LaguerreFactory(k-1)
    {
      const UnsignedInteger order[] = {4, 10, 32};
      const Scalar kValues[] = {1.0, 2.0, 5.5};
      for (UnsignedInteger r = 0; r < 3; ++r)
      {
        const Scalar k = kValues[r];
        const LaguerreFactory refFactory(k - 1.0);
        for (UnsignedInteger q = 0; q < 3; ++q)
        {
          const UnsignedInteger n = order[q];
          Point nodes(n);
          Point weights(n);
          FastLaguerre::ComputeNodesAndWeights(n, k, &nodes[0], &weights[0]);
          Point refWeights;
          const Point refNodes(refFactory.getNodesAndWeights(n, refWeights));
          assert_almost_equal(nodes, refNodes, 1.0e-10, 1.0e-12, OSS() << ", k=" << k << " n=" << n << " nodes");
          assert_almost_equal(weights, refWeights, 1.0e-10, 1.0e-12, OSS() << ", k=" << k << " n=" << n << " weights");
        }
      }
    }
    // Exactness check: an n-point rule integrates polynomials of degree up to
    // 2n-1 exactly w.r.t. Gamma(k, 1)
    {
      const UnsignedInteger order[] = {2, 4, 8};
      const Scalar kValues[] = {0.5, 2.5};
      for (UnsignedInteger r = 0; r < 2; ++r)
      {
        const Scalar k = kValues[r];
        for (UnsignedInteger q = 0; q < 3; ++q)
        {
          const UnsignedInteger n = order[q];
          Point nodes(n);
          Point weights(n);
          FastLaguerre::ComputeNodesAndWeights(n, k, &nodes[0], &weights[0]);
          for (UnsignedInteger m = 0; m < 2 * n; ++m)
          {
            Scalar integral = 0.0;
            for (UnsignedInteger i = 0; i < n; ++i)
              integral += weights[i] * std::pow(nodes[i], static_cast<Scalar>(m));
            assert_almost_equal(integral, gammaMoment(k, m), 1.0e-9, 1.0e-12, OSS() << ", k=" << k << " n=" << n << " degree " << m);
          }
        }
      }
    }
    // Structural properties: strictly increasing non-negative nodes,
    // positive weights summing to one
    {
      const Scalar k = 0.5;
      const UnsignedInteger n = 16;
      Point nodes(n);
      Point weights(n);
      FastLaguerre::ComputeNodesAndWeights(n, k, &nodes[0], &weights[0]);
      Scalar sum = 0.0;
      for (UnsignedInteger i = 0; i < n; ++i)
      {
        if (!(weights[i] > 0.0)) throw TestFailed(OSS() << "non positive weight " << weights[i]);
        if (i > 0)
          if (nodes[i] <= nodes[i - 1]) throw TestFailed(OSS() << "nodes not strictly increasing: " << nodes[i - 1] << " >= " << nodes[i]);
        if (nodes[i] < 0.0) throw TestFailed(OSS() << "negative Laguerre node " << nodes[i]);
        sum += weights[i];
      }
      assert_almost_equal(sum, 1.0, 1.0e-10, 1.0e-10, ", weight sum");
    }
    // Large-n sanity check: previously overflowing in double precision
    // (NaN/Inf around n = 512), now kept finite by the log-domain rescaling
    {
      const UnsignedInteger n = 1024;
      Point nodes(n);
      Point weights(n);
      FastLaguerre::ComputeNodesAndWeights(n, 1.0, &nodes[0], &weights[0]);
      Scalar sum = 0.0;
      for (UnsignedInteger i = 0; i < n; ++i)
      {
        sum += weights[i];
        if ((nodes[i] < 0.0) || !std::isfinite(nodes[i]))
          throw TestFailed(OSS() << "invalid Laguerre node " << nodes[i]);
        if ((weights[i] < 0.0) || !std::isfinite(weights[i]))
          throw TestFailed(OSS() << "invalid Laguerre weight " << weights[i]);
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