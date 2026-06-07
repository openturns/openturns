//                                               -*- C++ -*-
/**
 *  @brief The test file of class KentFactory for standard methods
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
    KentFactory factory;

    // Default build
    Distribution defaultDist(factory.build());
    fullprint << "Default distribution=" << defaultDist << std::endl;
    Kent defaultKent(factory.buildAsKent());
    fullprint << "Default Kent=" << defaultKent << std::endl;

    // Build from parameters
    Point parameters(11);
    parameters[0] = 10.0;
    parameters[1] = 0.25;
    parameters[2] = 1.0;
    parameters[3] = 0.0;
    parameters[4] = 0.0;
    parameters[5] = 0.0;
    parameters[6] = 1.0;
    parameters[7] = 0.0;
    parameters[8] = 0.0;
    parameters[9] = 0.0;
    parameters[10] = 1.0;
    Distribution paramDist(factory.build(parameters));
    fullprint << "Distribution from parameters=" << paramDist << std::endl;
    Kent paramKent(factory.buildAsKent(parameters));
    fullprint << "Kent from parameters=" << paramKent << std::endl;

    // Build from sample
    SquareMatrix gamma(3);
    gamma(0, 0) = 1.0; gamma(0, 1) = 0.0; gamma(0, 2) = 0.0;
    gamma(1, 0) = 0.0; gamma(1, 1) = 1.0; gamma(1, 2) = 0.0;
    gamma(2, 0) = 0.0; gamma(2, 1) = 0.0; gamma(2, 2) = 1.0;
    Kent trueDist(25.0, 0.5, gamma);
    UnsignedInteger size = 50000;
    Sample sample(trueDist.getSample(size));
    Kent estimatedKent(factory.buildAsKent(sample));
    fullprint << "True distribution =" << trueDist << std::endl;
    fullprint << "Estimated Kent    =" << estimatedKent << std::endl;
    assert_almost_equal(estimatedKent.getKappa(), 25.0, 0.5, 0.0);
    assert_almost_equal(estimatedKent.getBeta(), 0.5, 0.5, 0.0);
    SquareMatrix gammaEst(estimatedKent.getGamma());
    assert_almost_equal(gammaEst(0, 0), 1.0, 0.1, 0.0);
    assert_almost_equal(gammaEst(1, 1), 1.0, 0.1, 0.0);
    assert_almost_equal(gammaEst(2, 2), 1.0, 0.1, 0.0);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
