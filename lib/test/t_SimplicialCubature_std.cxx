//                                               -*- C++ -*-
/**
 *  @brief The test file of class GaussKronrod
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
#include <cmath>

using namespace OT;
using namespace OT::Test;

typedef Collection<Complex> ComplexCollection;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  PlatformInfo::SetNumericalPrecision(8);

  try
  {
    const Point ref = {0, 0, 1.0, 0.3591409, 0.09390606, 0.01935569};
    for (UnsignedInteger n = 2; n < 6; ++ n)
    {
      String sum = "x0";
      for (UnsignedInteger i = 1; i < n; ++ i)
        sum += (OSS() << "+x" << i);
      SymbolicFunction f(Description::BuildDefault(n, "x"), Description({"exp("+sum+")"}));
      fullprint << "f=" << f.__str__() << std::endl;
      Sample vertices(1, n);
      for (UnsignedInteger j = 0; j < n; ++ j)
      {
        Point e(n);
        e[j] = 1.0;
        vertices.add(e);
      }
      Indices indices(n + 1);
      indices.fill();
      IndicesCollection simplicies(Collection<Indices>(1, indices));
      Mesh canonicalSimplex(vertices, simplicies, true);
      for (UnsignedInteger k = 1; k < 5; ++ k)
      {
        SimplicialCubature algo;
        algo.setRule(k);
        // fullprint << "Algo=" << algo << std::endl;
        const Scalar value = algo.integrate(f, canonicalSimplex)[0];
        fullprint << "n=" << n << " k="<<k<<" value=" << std::setprecision(16)<<value << std::endl;
        assert_almost_equal(value, ref[n]);
      }
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }
  return ExitCode::Success;
}
