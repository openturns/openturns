//                                               -*- C++ -*-
/**
 *  @brief The test file of class DistFunc for orthant methods
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
    // pNormalOrthantND (Ridgway SMC algorithm)
    {
      fullprint << "MVN orthant d=1 exact" << std::endl;
      TriangularMatrix L(1);
      L(0, 0) = 1.0;
      const Scalar p = DistFunc::pNormalOrthantND({-1.0}, {1.0}, L, 100);
      const Scalar expected = DistFunc::pNormal(1.0) - DistFunc::pNormal(-1.0);
      assert_almost_equal(p, expected);
    }
    {
      fullprint << "MVN orthant d=2 independent" << std::endl;
      const UnsignedInteger dim = 2;
      TriangularMatrix L(dim);
      L(0, 0) = 1.0;
      L(1, 0) = 0.0;
      L(1, 1) = 1.0;
      const Point a = {-1.0, -1.0};
      const Point b = {1.0, 1.0};
      const Scalar p = DistFunc::pNormalOrthantND(a, b, L, 1000);
      const Scalar p1 = DistFunc::pNormal(1.0) - DistFunc::pNormal(-1.0);
      const Scalar expected = p1 * p1;
      assert_almost_equal(p, expected, 1e-2);
    }
    {
      fullprint << "MVN orthant d=2 correlated" << std::endl;
      const UnsignedInteger dim = 2;
      CovarianceMatrix sigma(dim);
      sigma(0, 0) = 1.0;
      sigma(1, 0) = 0.5;
      sigma(1, 1) = 2.0;
      const Point a = {-1.0, -2.0};
      const Point b = {1.0, 2.0};
      const Scalar p = DistFunc::pNormalOrthantND(a, b, sigma, 1000);
      assert_almost_equal(p, 0.5, 2e-1);
    }
    {
      fullprint << "MVN orthant d=2 with mu" << std::endl;
      const UnsignedInteger dim = 2;
      CovarianceMatrix sigma(dim);
      sigma(0, 0) = 1.0;
      sigma(1, 0) = 0.3;
      sigma(1, 1) = 1.0;
      const Point a = {-1.0, -1.0};
      const Point b = {1.0, 1.0};
      const Point mu = {0.5, -0.5};
      const Scalar p = DistFunc::pNormalOrthantND(a, b, mu, sigma, 1000);
      assert_almost_equal(p, 0.3808, 2e-1);
    }
    {
      fullprint << "Error: dimension mismatch" << std::endl;
      TriangularMatrix L(2);
      L(0, 0) = 1.0; L(1, 0) = 0.0; L(1, 1) = 1.0;
      try
      {
        DistFunc::pNormalOrthantND({-1.0}, {1.0}, L);
        throw TestFailed("Exception has NOT been thrown or caught!");
      }
      catch (const InvalidDimensionException &)
      {
        // expected
      }
    }
    {
      fullprint << "Error: a > b" << std::endl;
      TriangularMatrix L(1);
      L(0, 0) = 1.0;
      try
      {
        DistFunc::pNormalOrthantND({1.0}, {-1.0}, L);
        throw TestFailed("Exception has NOT been thrown or caught!");
      }
      catch (const InvalidArgumentException &)
      {
        // expected
      }
    }
    {
      fullprint << "Error: M = 0" << std::endl;
      TriangularMatrix L(1);
      L(0, 0) = 1.0;
      try
      {
        DistFunc::pNormalOrthantND({-1.0}, {1.0}, L, 0);
        throw TestFailed("Exception has NOT been thrown or caught!");
      }
      catch (const InvalidArgumentException &)
      {
        // expected
      }
    }
    {
      fullprint << "Error: L(i,i) <= 0" << std::endl;
      TriangularMatrix L(1);
      L(0, 0) = 0.0;
      try
      {
        DistFunc::pNormalOrthantND({-1.0}, {1.0}, L);
        throw TestFailed("Exception has NOT been thrown or caught!");
      }
      catch (const InvalidArgumentException &)
      {
        // expected
      }
    }
    {
      fullprint << "Edge: a == b" << std::endl;
      TriangularMatrix L(1);
      L(0, 0) = 1.0;
      const Scalar p = DistFunc::pNormalOrthantND({0.0}, {0.0}, L);
      assert_almost_equal(p, 0.0, 0.0, 1e-15);
    }
    {
      fullprint << "Edge: wide interval" << std::endl;
      const UnsignedInteger dim = 2;
      TriangularMatrix L(dim);
      L(0, 0) = 1.0; L(1, 0) = 0.0; L(1, 1) = 1.0;
      const Scalar p = DistFunc::pNormalOrthantND({-10.0, -10.0}, {10.0, 10.0}, L);
      assert_almost_equal(p, 1.0, 1.0, 1e-10);
    }
    {
      fullprint << "MVN orthant d=3 independent" << std::endl;
      const UnsignedInteger dim = 3;
      TriangularMatrix L(dim);
      L(0, 0) = 1.0; L(1, 0) = 0.0; L(2, 0) = 0.0;
      L(1, 1) = 1.0; L(2, 1) = 0.0; L(2, 2) = 1.0;
      const Point a = {-1.0, -1.0, -1.0};
      const Point b = {1.0, 1.0, 1.0};
      const Scalar p = DistFunc::pNormalOrthantND(a, b, L, 1000);
      const Scalar p1 = DistFunc::pNormal(1.0) - DistFunc::pNormal(-1.0);
      const Scalar expected = p1 * p1 * p1;
      assert_almost_equal(p, expected, 1e-2);
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
