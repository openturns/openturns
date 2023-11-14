//                                               -*- C++ -*-
/**
 *  @brief The test file of class Student for standard methods
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
  PlatformInfo::SetNumericalPrecision(3);
  try
  {
    Collection<Student> distributions;
    distributions.add(Student(3.5, 2.5, 2.0));
    {
      CorrelationMatrix R(2);
      R(0, 1) = 0.5;
      distributions.add(Student(4.5, Point(2, 2.5), Point(2, 1.5), R));
    }
    {
      CorrelationMatrix R(10);
      Point mu(10);
      Point sigma(10);
      for (UnsignedInteger i = 0; i < 10; ++i)
      {
        mu[i] = 0.5 * i;
        sigma[i] = 0.5 + i;
        for (UnsignedInteger j = 0; j < i; ++j)
          R(i, j) = 1.0 / std::pow(i + j + 2.0, 1.0);
      }
      distributions.add(Student(4.5, mu, sigma, R));
    }
    UnsignedInteger size = 10000;
    StudentFactory factory;
    for (UnsignedInteger i = 0; i < distributions.getSize(); ++i)
    {
      Student distribution = distributions[i];
      Sample sample(distribution.getSample(size));
      Distribution estimatedDistribution(factory.build(sample));
      fullprint << "Distribution          =" << distribution << std::endl;
      fullprint << "Estimated distribution=" << estimatedDistribution << std::endl;
    }
    Distribution estimatedDistribution = factory.build();
    fullprint << "Default distribution=" << estimatedDistribution << std::endl;
    Student estimatedStudent = factory.buildAsStudent();
    fullprint << "Default student=" << estimatedStudent << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
