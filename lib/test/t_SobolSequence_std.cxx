//                                               -*- C++ -*-
/**
 *  @brief Implementation of the Sobol' sequence
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
    // Create 6 points from a Sobol' sequence in dimension 1
    Sample expected1D(6, 1);
    expected1D[0] = Point({1.0 / 2.0});
    expected1D[1] = Point({3.0 / 4.0});
    expected1D[2] = Point({1.0 / 4.0});
    expected1D[3] = Point({3.0 / 8.0});
    expected1D[4] = Point({7.0 / 8.0});
    expected1D[5] = Point({5.0 / 8.0});
    SobolSequence sequence1D(1);
    fullprint << sequence1D << std::endl;
    Sample sobolSample1D(sequence1D.generate(6));
    assert_almost_equal(sobolSample1D, expected1D);

    // Create 6 points from a Sobol' sequence in dimension 2
    Sample expected2D(6, 2);
    expected2D[0] = Point({1.0 / 2.0, 1.0 / 2.0});
    expected2D[1] = Point({3.0 / 4.0, 1.0 / 4.0});
    expected2D[2] = Point({1.0 / 4.0, 3.0 / 4.0});
    expected2D[3] = Point({3.0 / 8.0, 3.0 / 8.0});
    expected2D[4] = Point({7.0 / 8.0, 7.0 / 8.0});
    expected2D[5] = Point({5.0 / 8.0, 1.0 / 8.0});
    SobolSequence sequence2D(2);
    fullprint << sequence2D << std::endl;
    Sample sobolSample2D(sequence2D.generate(6));
    assert_almost_equal(sobolSample2D, expected2D);

    // Create a Sobol' sequence of maximum dimension
    SobolSequence sequence(SobolSequence::MaximumDimension);
    fullprint << sequence << std::endl;
    sequence.generate(10);

    // Create another Sobol' sequence of dimension 2 to estimate Pi in [0; 1)^2
    UnsignedInteger dimension = 2;
    sequence = SobolSequence(dimension);
    UnsignedInteger pointInsideCircle = 0;
    UnsignedInteger sampleSize = std::pow(2, 11);  // This is significant!
    for(UnsignedInteger i = 0; i < sampleSize; ++i)
    {
      Point sobolPoint(sequence.generate());
      if(sobolPoint.norm() < 1.0)
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
