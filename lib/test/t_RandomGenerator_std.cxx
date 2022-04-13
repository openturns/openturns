//                                               -*- C++ -*-
/**
 *  @brief The test file of class RandomGenerator for standard methods
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
    // Test the initialization method
    RandomGenerator::SetSeed(0);
    // Test the state accessor for reading
    RandomGeneratorState initialState(RandomGenerator::GetState());
    fullprint << "state index=" << initialState.getIndex() << " state array=" << initialState.getBuffer() << std::endl;
    // Test the generation of one realization
    Scalar realization = RandomGenerator::Generate();
    fullprint << "one realization=" << realization << std::endl;
    RandomGeneratorState state(RandomGenerator::GetState());
    fullprint << "new state index=" << state.getIndex() << std::endl;
    // Test the generation of several realizations
    UnsignedInteger size = 10000;
    Point several(RandomGenerator::Generate(size));
    fullprint << size << " realizations, first=" << several[0] << ", last=" << several[size  - 1] << std::endl;
    // Test the state accessor for writing
    RandomGenerator::SetState(initialState);
    fullprint << "restoring initial state, one realization=" << RandomGenerator::Generate() << std::endl;
    // Test the uniformity of the random generator
    size = 100000;
    Point manyPoints(RandomGenerator::Generate(size));
    UnsignedInteger slice = 10;
    Point frequencies(slice, 0.0);
    Scalar mean = 0.0;
    for (UnsignedInteger i = 0; i < size; i++)
    {
      frequencies[(UnsignedInteger)(slice * manyPoints[i])]++;
      mean += manyPoints[i];
    }
    mean /= size;
    fullprint << "mean of " << size << " realizations=" << mean << std::endl;
    for (UnsignedInteger i = 0; i < slice; i++)
    {
      frequencies[i] /= size;
      fullprint << "frequency in slice [" << Scalar(i) / slice << ", " << Scalar(i + 1) / slice << "]=" << frequencies[i] << std::endl;
    }
    fullprint << "One integer generation in [[0, " << slice - 1 << "]]=" << RandomGenerator::IntegerGenerate(slice) << std::endl;
    fullprint << "Another integer generation in [[0, " << slice - 1 << "]]=" << RandomGenerator::IntegerGenerate(slice) << std::endl;
    Collection<UnsignedInteger> manyUnsignedInteger(RandomGenerator::IntegerGenerate(size, slice));
    frequencies = Point(slice, 0.0);
    mean = 0.0;
    for (UnsignedInteger i = 0; i < size; i++)
    {
      frequencies[manyUnsignedInteger[i]]++;
      mean += manyUnsignedInteger[i];
    }
    mean /= size;
    fullprint << "mean of " << size << " integer realizations=" << mean << std::endl;
    for (UnsignedInteger i = 0; i < slice; i++)
    {
      frequencies[i] /= size;
      fullprint << "frequency for value " << i << "=" << frequencies[i] << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
