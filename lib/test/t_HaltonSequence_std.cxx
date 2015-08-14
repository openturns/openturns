//                                               -*- C++ -*-
/**
 *  @brief Implementation of the Halton' sequence
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "OT.hxx"
#include "OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    // Create a Halton sequence
    HaltonSequence sequence(15);
    fullprint << sequence << std::endl;

    // Create a numerical sample of the sequence
    NumericalSample haltonSample(sequence.generate(10));
    fullprint << haltonSample << std::endl;

    // Create another Halton' sequence of dimension 2 to estimate Pi in [0; 1)^2
    UnsignedInteger dimension(2);
    sequence = HaltonSequence(dimension);
    UnsignedInteger pointInsideCircle(0);
    UnsignedInteger sampleSize(1000);
    for(UnsignedInteger i = 0; i < sampleSize; ++i)
    {
      NumericalPoint haltonPoint(sequence.generate());
      fullprint << haltonPoint << std::endl;
      if(haltonPoint.norm() < 1.0)
        ++ pointInsideCircle;
    }
    NumericalScalar probabilityEstimate(1.0 * pointInsideCircle / sampleSize);
    NumericalScalar probability(M_PI / 4.0);
    NumericalScalar relativeError(fabs(probability - probabilityEstimate) / probability);
    fullprint << "sample size=" << sampleSize << std::endl;
    fullprint << "relative error to Pi=" << relativeError << std::endl;
  }

  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
