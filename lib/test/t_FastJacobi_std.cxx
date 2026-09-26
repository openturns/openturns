//                                               -*- C++ -*-
/**
 *  @brief The test file of namespace FastJacobi for standard methods
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

// m-th raw moment of the normalized Jacobi weight, i.e. of Y = 2X-1 with
// X ~ Beta(beta+1, alpha+1) on [0, 1]
// (the map x -> (1-x) = 2(1-t), (1+x) = 2t gives t ~ Beta(beta+1, alpha+1))
static Scalar jacobiMoment(const Scalar alpha,
                           const Scalar beta,
                           const UnsignedInteger m)
{
  const Scalar a = beta + 1.0;
  const Scalar b = alpha + 1.0;
  // E[X^j] = (a)_j / (a+b)_j
  Point ex(m + 1, 1.0);
  Scalar risingA = 1.0;
  Scalar risingAB = 1.0;
  for (UnsignedInteger j = 1; j <= m; ++j)
  {
    risingA *= a + (j - 1.0);
    risingAB *= a + b + (j - 1.0);
    ex[j] = risingA / risingAB;
  }
  // E[(2X-1)^m] = sum_j C(m,j) 2^j (-1)^(m-j) E[X^j]
  Scalar value = 0.0;
  Scalar binom = 1.0;
  Scalar powerTwo = 1.0;
  for (UnsignedInteger j = 0; j <= m; ++j)
  {
    const Scalar sign = ((m - j) % 2 == 0) ? 1.0 : -1.0;
    value += binom * powerTwo * sign * ex[j];
    // update C(m,j) and 2^j for the next step
    binom *= static_cast<Scalar>(m - j) / static_cast<Scalar>(j + 1);
    powerTwo *= 2.0;
  }
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
        FastJacobi::ComputeNodesAndWeights(0, 0.0, 0.0, &nodes[0], &weights[0]);
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
        FastJacobi::ComputeNodesAndWeights(4, -1.0, 0.0, &nodes[0], &weights[0]);
      }
      catch (const InvalidArgumentException & ex)
      {
        thrown = true;
        fullprint << "OK: alpha=-1 throws " << ex.type() << std::endl;
      }
      if (!thrown) throw TestFailed("expected InvalidArgumentException for alpha == -1");
    }
    {
      Point nodes(1);
      Point weights(1);
      Bool thrown = false;
      try
      {
        FastJacobi::ComputeNodesAndWeights(4, 0.0, -1.5, &nodes[0], &weights[0]);
      }
      catch (const InvalidArgumentException & ex)
      {
        thrown = true;
        fullprint << "OK: beta=-1.5 throws " << ex.type() << std::endl;
      }
      if (!thrown) throw TestFailed("expected InvalidArgumentException for beta <= -1");
    }
    // n=1: single node at (beta-alpha)/(alpha+beta+2), weight one
    {
      const Scalar alpha = 0.5;
      const Scalar beta = 1.5;
      Point nodes(1);
      Point weights(1);
      FastJacobi::ComputeNodesAndWeights(1, alpha, beta, &nodes[0], &weights[0]);
      assert_almost_equal(nodes[0], 1.0 / 4.0, 1.0e-12, 1.0e-12, ", n=1 node");
      assert_almost_equal(weights[0], 1.0, 1.0e-12, 1.0e-12, ", n=1 weight");
    }
    // Cross-check against the reference Golub-Welsch implementation exposed
    // by JacobiFactory: both use the weight (1-x)^alpha (1+x)^beta
    {
      const UnsignedInteger order[] = {4, 10, 30};
      // (alpha, beta) = (-0.5, -0.5) is the alpha+beta == -1 edge case
      const Scalar abValues[][2] = {{0.0, 0.0}, {0.5, 1.5}, {2.0, 0.5}, {-0.5, -0.5}};
      for (UnsignedInteger r = 0; r < 4; ++r)
      {
        const Scalar alpha = abValues[r][0];
        const Scalar beta = abValues[r][1];
        const JacobiFactory refFactory(alpha, beta);
        for (UnsignedInteger q = 0; q < 3; ++q)
        {
          const UnsignedInteger n = order[q];
          Point nodes(n);
          Point weights(n);
          FastJacobi::ComputeNodesAndWeights(n, alpha, beta, &nodes[0], &weights[0]);
          Point refWeights;
          const Point refNodes(refFactory.getNodesAndWeights(n, refWeights));
          assert_almost_equal(nodes, refNodes, 1.0e-9, 1.0e-12, OSS() << ", a=" << alpha << " b=" << beta << " n=" << n << " nodes");
          assert_almost_equal(weights, refWeights, 1.0e-9, 1.0e-12, OSS() << ", a=" << alpha << " b=" << beta << " n=" << n << " weights");
        }
      }
    }
    // Exactness check: an n-point rule integrates polynomials of degree up to
    // 2n-1 exactly w.r.t. the normalized (1-x)^alpha (1+x)^beta weight
    {
      const UnsignedInteger order[] = {3, 6};
      const Scalar abValues[][2] = {{0.0, 0.0}, {0.5, 1.5}, {2.0, 3.0}};
      for (UnsignedInteger r = 0; r < 3; ++r)
      {
        const Scalar alpha = abValues[r][0];
        const Scalar beta = abValues[r][1];
        for (UnsignedInteger q = 0; q < 2; ++q)
        {
          const UnsignedInteger n = order[q];
          Point nodes(n);
          Point weights(n);
          FastJacobi::ComputeNodesAndWeights(n, alpha, beta, &nodes[0], &weights[0]);
          for (UnsignedInteger m = 0; m < 2 * n; ++m)
          {
            Scalar integral = 0.0;
            for (UnsignedInteger i = 0; i < n; ++i)
              integral += weights[i] * std::pow(nodes[i], static_cast<Scalar>(m));
            assert_almost_equal(integral, jacobiMoment(alpha, beta, m), 1.0e-8, 1.0e-10, OSS() << ", a=" << alpha << " b=" << beta << " n=" << n << " degree " << m);
          }
        }
      }
    }
    // Structural properties: nodes in [-1, 1], strictly increasing,
    // positive weights summing to one
    {
      const UnsignedInteger n = 16;
      const Scalar alpha = 2.0;
      const Scalar beta = 0.5;
      Point nodes(n);
      Point weights(n);
      FastJacobi::ComputeNodesAndWeights(n, alpha, beta, &nodes[0], &weights[0]);
      Scalar sum = 0.0;
      for (UnsignedInteger i = 0; i < n; ++i)
      {
        if (!(weights[i] > 0.0)) throw TestFailed(OSS() << "non positive weight " << weights[i]);
        if (i > 0)
          if (nodes[i] <= nodes[i - 1]) throw TestFailed(OSS() << "nodes not strictly increasing: " << nodes[i - 1] << " >= " << nodes[i]);
        if ((nodes[i] < -1.0) || (nodes[i] > 1.0)) throw TestFailed(OSS() << "node outside [-1, 1] " << nodes[i]);
        sum += weights[i];
      }
      assert_almost_equal(sum, 1.0, 1.0e-10, 1.0e-10, ", weight sum");
    }
    // Large-n sanity check
    {
      const UnsignedInteger n = 1024;
      const Scalar alpha = 0.5;
      const Scalar beta = 1.5;
      Point nodes(n);
      Point weights(n);
      FastJacobi::ComputeNodesAndWeights(n, alpha, beta, &nodes[0], &weights[0]);
      Scalar sum = 0.0;
      for (UnsignedInteger i = 0; i < n; ++i)
      {
        sum += weights[i];
        if ((nodes[i] <= -1.0) || (nodes[i] >= 1.0) || !std::isfinite(nodes[i]))
          throw TestFailed(OSS() << "invalid Jacobi node " << nodes[i]);
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