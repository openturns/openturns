//                                               -*- C++ -*-
/**
 *  @brief The test file of class Sample for split method
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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

class TestObject : public Sample
{
public:
  TestObject() : Sample(1, 1) {}
  virtual ~TestObject() {}
};


int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    // Test basic functionnalities
    checkClassWithClassName<TestObject>();

    // We create an empty Sample
    const int N = 100;
    Sample sample(N, 2);
    fullprint << "Sample size=" << sample.getSize() << std::endl;

    // We populate the empty sample with numerous data
    for (int i = 0; i < N; i++)
    {
      Point point(2);
      point[0] = 1000 + i;
      point[1] = 2000 + i;
      sample[i] = point;
    }
    fullprint << "sample=" << sample << std::endl;


    // We try to split the sample
    Sample sample90 = sample.split(90);
    Sample sample10 = sample.split(10);
    fullprint << "sample  =" << sample   << std::endl;
    fullprint << "sample10=" << sample10 << std::endl;
    fullprint << "sample90=" << sample90 << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
