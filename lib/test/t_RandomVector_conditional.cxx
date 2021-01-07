//                                               -*- C++ -*-
/**
 *  @brief The test file of distribution-based conditional RandomVector class
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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

  try
  {

    /* We create a distribution */
    Normal distribution(0.0, 1.0);

    fullprint << "distribution = " << distribution << std::endl;

    // Create a collection of distribution
    ComposedDistribution::DistributionCollection aCollection;
    aCollection.add(Normal(0.0, 1.0));
    aCollection.add(Uniform(1.0, 1.5));
    ComposedDistribution distributionParameters(aCollection);
    RandomVector randomParameters(distributionParameters);

    fullprint << "random parameters=" << randomParameters << std::endl;

    /* We create a distribution-based RandomVector */
    ConditionalRandomVector vect(distribution, randomParameters);
    fullprint << "vect=" << vect << std::endl;

    /* Check standard methods of class RandomVector */
    fullprint << "vect dimension=" << vect.getDimension() << std::endl;
    Point p;
    Point r(vect.getRealization(p));
    fullprint << "vect realization=" << r << std::endl;
    fullprint << "parameters value=" << p << std::endl;
    distribution.setParameter(p);
    RandomGenerator::SetSeed(0);
    /* Generate a parameter set to put the random generator into the proper state */
    randomParameters.getRealization();
    /* The realization of the distribution should be equal to the realization of the conditional vector */
    fullprint << "dist realization=" << distribution.getRealization() << std::endl;
    fullprint << "vect sample =" << vect.getSample(5) << std::endl;


  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
