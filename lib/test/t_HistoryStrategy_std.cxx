//                                               -*- C++ -*-
/**
 *  @brief The test file of class HistoryStrategy for standard methods
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
    UnsignedInteger size = 10;
    UnsignedInteger dimension = 2;
    Sample sample(size, dimension);
    // Fill-in the sample
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      for (UnsignedInteger j = 0; j < dimension; ++j)
      {
        sample[i][j] = i + Scalar(j) / dimension;
      }
    }
    fullprint << "sample=" << sample << std::endl;
    // History using the Null strategy
    Null nullStrategy;
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      nullStrategy.store(sample[i]);
    }
    fullprint << "Null strategy sample=" << nullStrategy.getSample() << std::endl;
    // History using the Full strategy
    Full fullStrategy;
    fullStrategy.setDimension(dimension);
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      fullStrategy.store(sample[i]);
    }
    fullprint << "Full strategy sample=" << fullStrategy.getSample() << std::endl;
    // History using the Last strategy, large storage
    Last lastStrategy(3 * size);
    lastStrategy.setDimension(dimension);
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      lastStrategy.store(sample[i]);
    }
    fullprint << "Last strategy sample (large storage)=" << lastStrategy.getSample() << std::endl;
    lastStrategy = Last(size / 3);
    lastStrategy.setDimension(dimension);
    // History using the Last strategy, small storage
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      lastStrategy.store(sample[i]);
    }
    fullprint << "Last strategy sample (small storage)=" << lastStrategy.getSample() << std::endl;
    // History using the Compact strategy, large storage
    Compact compactStrategy(3 * size);
    compactStrategy.setDimension(dimension);
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      compactStrategy.store(sample[i]);
    }
    fullprint << "Compact strategy sample (large storage)=" << compactStrategy.getSample() << std::endl;
    compactStrategy = Compact(size / 3);
    compactStrategy.setDimension(dimension);
    // History using the Compact strategy, small storage
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      compactStrategy.store(sample[i]);
    }
    fullprint << "Compact strategy sample (small storage)=" << compactStrategy.getSample() << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
