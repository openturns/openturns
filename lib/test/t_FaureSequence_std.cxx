//                                               -*- C++ -*-
/**
 *  @brief Implementation of the Faure' sequence
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
    // Create 8 points from a Faure sequence in dimension 1
    Sample expected1D(8, 1);
    expected1D[0] = Point({1.0 / 2.0});
    expected1D[1] = Point({1.0 / 4.0});
    expected1D[2] = Point({3.0 / 4.0});
    expected1D[3] = Point({1.0 / 8.0});
    expected1D[4] = Point({5.0 / 8.0});
    expected1D[5] = Point({3.0 / 8.0});
    expected1D[6] = Point({7.0 / 8.0});
    expected1D[7] = Point({1.0 / 16.0});
    FaureSequence sequence1D(1);
    fullprint << sequence1D << std::endl;
    Sample faureSample1D(sequence1D.generate(8));
    assert_almost_equal(faureSample1D, expected1D);

    // Create 8 points from a Faure sequence in dimension 2
    Sample expected2D(8, 2);
    expected2D[0] = Point({0.5, 0.5});
    expected2D[1] = Point({0.25, 0.75});
    expected2D[2] = Point({0.75, 0.25});
    expected2D[3] = Point({0.125, 0.625});
    expected2D[4] = Point({0.625, 0.125});
    expected2D[5] = Point({0.375, 0.375});
    expected2D[6] = Point({0.875, 0.875});
    expected2D[7] = Point({0.0625, 0.9375});
    FaureSequence sequence2D(2);
    fullprint << sequence2D << std::endl;
    Sample faureSample2D(sequence2D.generate(8));
    assert_almost_equal(faureSample2D, expected2D);

    // Create 8 points from a Faure sequence in dimension 3
    Sample expected3D(8, 3);
    expected3D[0] = Point({1.0 / 3.0, 1.0 / 3.0, 1.0 / 3.0});
    expected3D[1] = Point({2.0 / 3.0, 2.0 / 3.0, 2.0 / 3.0});
    expected3D[2] = Point({1.0 / 9.0, 4.0 / 9.0, 7.0 / 9.0});
    expected3D[3] = Point({4.0 / 9.0, 7.0 / 9.0, 1.0 / 9.0});
    expected3D[4] = Point({7.0 / 9.0, 1.0 / 9.0, 4.0 / 9.0});
    expected3D[5] = Point({2.0 / 9.0, 8.0 / 9.0, 5.0 / 9.0});
    expected3D[6] = Point({5.0 / 9.0, 2.0 / 9.0, 8.0 / 9.0});
    expected3D[7] = Point({8.0 / 9.0, 5.0 / 9.0, 2.0 / 9.0});
    FaureSequence sequence3D(3);
    fullprint << sequence3D << std::endl;
    Sample faureSample3D(sequence3D.generate(8));
    assert_almost_equal(faureSample3D, expected3D);

    // Create a Faure sequence in dimension 15
    FaureSequence sequence(15);
    fullprint << sequence << std::endl;
    sequence.generate(10);

    // Create another Faure' sequence of dimension 2 to estimate Pi in [0; 1)^2
    UnsignedInteger dimension = 2;
    sequence = FaureSequence(dimension);
    UnsignedInteger pointInsideCircle = 0;
    UnsignedInteger sampleSize = std::pow(3, 7);  // This is significant!
    for(UnsignedInteger i = 0; i < sampleSize; ++i)
    {
      Point faurePoint(sequence.generate());
      if(faurePoint.norm() < 1.0)
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
