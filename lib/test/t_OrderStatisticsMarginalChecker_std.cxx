//                                               -*- C++ -*-
/**
 *  @brief The test file of class OrderStatisticsMarginalChecker for standard methods
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


inline void checkMarginals(const ComposedDistribution::DistributionCollection& coll)
{
  OStream fullprint(std::cout);
  OrderStatisticsMarginalChecker osmc(coll);
  fullprint << "marginals=" << coll << std::endl;
  fullprint << "isCompatible=" << osmc.isCompatible() << std::endl;
  fullprint << "partition=" << osmc.buildPartition() << std::endl;
}

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    ComposedDistribution::DistributionCollection coll;
    coll.add(Uniform(-1.0, 1.0));
    coll.add(LogUniform(1.0, 1.2));
    coll.add(Triangular(3.0, 4.0, 5.));
    coll.add(Uniform(5.0, 6.0));
    coll.add(Uniform(5.5, 6.5));
    checkMarginals(coll);
    coll.add(Uniform(0.0, 1.0));
    checkMarginals(coll);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
