//                                               -*- C++ -*-
/**
 *  @brief Implementation of the Halton' sequence
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
    // Create 6 points from a Halton sequence in dimension 1
    Sample expected1D(6, 1);
    expected1D[0] = Point({1.0 / 2.0});
    expected1D[1] = Point({1.0 / 4.0});
    expected1D[2] = Point({3.0 / 4.0});
    expected1D[3] = Point({1.0 / 8.0});
    expected1D[4] = Point({5.0 / 8.0});
    expected1D[5] = Point({3.0 / 8.0});
    HaltonSequence sequence1D(1);
    fullprint << sequence1D << std::endl;
    Sample haltonSample1D(sequence1D.generate(6));
    assert_almost_equal(haltonSample1D, expected1D);

    // Create 6 points from a Halton sequence in dimension 2
    Sample expected2D(6, 2);
    expected2D[0] = Point({1.0 / 2.0, 1.0 / 3.0});
    expected2D[1] = Point({1.0 / 4.0, 2.0 / 3.0});
    expected2D[2] = Point({3.0 / 4.0, 1.0 / 9.0});
    expected2D[3] = Point({1.0 / 8.0, 4.0 / 9.0});
    expected2D[4] = Point({5.0 / 8.0, 7.0 / 9.0});
    expected2D[5] = Point({3.0 / 8.0, 2.0 / 9.0});
    HaltonSequence sequence2D(2);
    fullprint << sequence2D << std::endl;
    Sample haltonSample2D(sequence2D.generate(6));
    assert_almost_equal(haltonSample2D, expected2D);

    // Create 12 points from a Halton sequence in dimension 4
    Sample expected4D(12, 4);
    expected4D[0] = Point({0.5, 0.333333, 0.2, 0.142857});
    expected4D[1] = Point({0.250000, 0.666667, 0.400000, 0.285714});
    expected4D[2] = Point({0.750000, 0.111111, 0.600000, 0.428571});
    expected4D[3] = Point({0.125000, 0.444444, 0.800000, 0.571429});
    expected4D[4] = Point({0.625000, 0.777778, 0.040000, 0.714286});
    expected4D[5] = Point({0.375000, 0.222222, 0.240000, 0.857143});
    expected4D[6] = Point({0.875000, 0.555556, 0.440000, 0.020408});
    expected4D[7] = Point({0.062500, 0.888889, 0.640000, 0.163265});
    expected4D[8] = Point({0.562500, 0.037037, 0.840000, 0.306122});
    expected4D[9] = Point({0.312500, 0.370370, 0.080000, 0.448980});
    expected4D[10] = Point({0.812500, 0.703704, 0.280000, 0.591837});
    expected4D[11] = Point({0.187500, 0.148148, 0.480000, 0.734694});
    HaltonSequence sequence4D(4);
    fullprint << sequence4D << std::endl;
    Sample haltonSample4D(sequence4D.generate(12));
    assert_almost_equal(haltonSample4D, expected4D);

    // Create a Halton sequence in dimension 15
    HaltonSequence sequence(15);
    fullprint << sequence << std::endl;
    sequence.generate(10);

    // Create another Halton' sequence of dimension 2 to estimate Pi in [0; 1)^2
    UnsignedInteger dimension = 2;
    sequence = HaltonSequence(dimension);
    UnsignedInteger pointInsideCircle = 0;
    UnsignedInteger sampleSize = std::pow((2 * 3), 4);  // This is significant!
    for(UnsignedInteger i = 0; i < sampleSize; ++i)
    {
      Point haltonPoint(sequence.generate());
      if(haltonPoint.norm() < 1.0)
        ++ pointInsideCircle;
    }
    Scalar probabilityEstimate = 1.0 * pointInsideCircle / sampleSize;
    Scalar probability = M_PI / 4.0;
    fullprint << "sample size=" << sampleSize << std::endl;
    fullprint << "computed probability =" << probabilityEstimate << std::endl;
    fullprint << "expected probability =" << probability << std::endl;
    Scalar rtol = 10.0 / sampleSize;
    assert_almost_equal(probabilityEstimate, probability, rtol);
  }

  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
