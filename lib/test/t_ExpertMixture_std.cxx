//                                               -*- C++ -*-
/**
 *  @brief The test file of class ExpertMixture for standard methods
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

  try
  {

    // Create a collection of distribution
    Mixture::DistributionCollection aCollection;
    CorrelationMatrix R(2);
    R(0, 1) = -0.99;
    Point mean(2);
    mean[0] = -1.0;
    mean[1] = 1.0;
    aCollection.add( Normal(mean, Point(2, 1.0), R) );
    R(0, 1) = 0.99;
    mean[0] = 1.0;
    aCollection.add( Normal(mean, Point(2, 1.0), R) );

    // Instantiate one distribution object
    Mixture distribution(aCollection, Point(aCollection.getSize(), 1.0));

    // Create a mixture classifier
    MixtureClassifier classifier(distribution);
    fullprint << "classifier dimension=" << classifier.getDimension() << std::endl;

    // Create local experts
    Collection<Function> experts(0);
    experts.add(SymbolicFunction("x", "-x"));
    experts.add(SymbolicFunction("x", "x"));

    // Create a mixture of experts
    ExpertMixture moe(experts, classifier);

    fullprint << "Mixture of experts=" << moe << std::endl;

    // Evaluate the mixture of experts on some points
    for (UnsignedInteger i = 0; i < 5; ++i)
    {
      Point p(1, -0.3 + 0.8 * i / 4.0);
      fullprint << "moe(" << p[0] << ")=" << moe(p) << std::endl;
    }

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
