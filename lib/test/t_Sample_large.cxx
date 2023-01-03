//                                               -*- C++ -*-
/**
 *  @brief The test file of class Sample of big size
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

  try
  {

#ifdef DEBUG_MEMORY
    std::cerr << "Memory usage std=" << GetMemoryUsage() << " full=" << GetFullMemoryUsage() << std::endl;
#endif

    {
      // We create an empty Sample
      Sample sample(0, 2);
      sample.setName("EmptySample");
      fullprint << "sample=" << sample << std::endl;

      try
      {
        // We access the element of the sample
        Point p(sample.at(0));
        fullprint << "p=" << p << std::endl;

        // We should NEVER go here
        throw TestFailed("Exception NOT thrown");
      }
      catch (OutOfBoundException &)
      {
        // Nothing to do
      }
    }

#ifdef DEBUG_MEMORY
    std::cerr << "Memory usage std=" << GetMemoryUsage() << " full=" << GetFullMemoryUsage() << std::endl;
#endif

    {
      // We create an small Sample
      Sample sample(1, 2);
      sample.setName("SmallSample");
      fullprint << "sample=" << sample << std::endl;

      // We access the element of the sample
      Point p(sample[0]);
      fullprint << "p=" << p << std::endl;
      try
      {
        // We try to access past the last element of the point
        Point err( sample.at(2) );

        // We should NEVER go here
        throw TestFailed("Exception NOT thrown");
      }
      catch (OutOfBoundException &)
      {
        // Nothing to do
      }
    }

#ifdef DEBUG_MEMORY
    std::cerr << "Memory usage std=" << GetMemoryUsage() << " full=" << GetFullMemoryUsage() << std::endl;
#endif

    {
      // We create a big Sample
      Sample sample(1000000, 2);
      sample.setName("BigSample");

#ifdef DEBUG_MEMORY
      std::cerr << "Memory usage std=" << GetMemoryUsage() << " full=" << GetFullMemoryUsage() << std::endl;
      printMemoryUsage();
#endif

      // We populate the sample
      UnsignedInteger size = sample.getSize();
      for(UnsignedInteger i = 0; i < size; i++)
      {
        sample(i, 0) = i;
        sample(i, 1) = i;
      }

      Point mean = sample.computeMean();

      fullprint << "sample first point=" << sample[0]      << std::endl;
      fullprint << "sample last  point=" << sample[size - 1] << std::endl;
      fullprint << "sample mean  value=" << mean           << std::endl;

#ifdef DEBUG_MEMORY
      std::cerr << "Memory usage std=" << GetMemoryUsage() << " full=" << GetFullMemoryUsage() << std::endl;
#endif
    }

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

