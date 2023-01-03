//                                               -*- C++ -*-
/**
 *  @brief The test file of class DistFunc for standard methods
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

using namespace OT;
using namespace OT::Test;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    // Kolmogorov related functions
    Indices nValues(0);
    nValues.add(2);
    nValues.add(5);
    nValues.add(10);
    nValues.add(20);
    nValues.add(50);
    nValues.add(100);
    nValues.add(200);
    nValues.add(500);
    nValues.add(1000);
    nValues.add(2000);
    nValues.add(5000);
    nValues.add(10000);
    nValues.add(20000);
    nValues.add(100000);
    nValues.add(1000000);
    Point xValues(0);
    xValues.add(0.0001);
    xValues.add(0.0002);
    xValues.add(0.0005);
    xValues.add(0.001);
    xValues.add(0.002);
    xValues.add(0.005);
    xValues.add(0.01);
    xValues.add(0.02);
    xValues.add(0.05);
    xValues.add(0.1);
    xValues.add(0.2);
    xValues.add(0.5);
    xValues.add(1.0);
    xValues.add(1.2);
    xValues.add(1.5);
    xValues.add(2.0);
    xValues.add(5.0);
    xValues.add(10.0);
    xValues.add(20.0);
    xValues.add(50.0);
    for (UnsignedInteger i = 0; i < nValues.getSize(); ++i)
    {
      const UnsignedInteger n = nValues[i];
      for (UnsignedInteger j = 0; j < xValues.getSize(); ++j)
      {
        const Scalar x = xValues[j];
        const Scalar cdf = DistFunc::pKolmogorov(n, x);
        const Scalar ccdf = DistFunc::pKolmogorov(n, x, true);
        fullprint << "pKolmogorov(" << n << ", " << x << ")=" << std::fixed << (cdf < 1e-16 ? 0.0 : cdf) << ", complementary=" << (ccdf < 1e-16 ? 0.0 : ccdf) << std::endl;
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
