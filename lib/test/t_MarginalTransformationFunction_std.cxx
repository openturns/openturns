//                                               -*- C++ -*-
/**
 *  @brief The test file of class MarginalTransformationFunction for standard methods
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
    MarginalTransformationFunction::DistributionCollection coll1(0);
    coll1.add(Normal(1.0, 2.5));
    coll1.add(Gamma(1.5, 3.0, 0.0));
    MarginalTransformationFunction::DistributionCollection coll2(0);
    coll2.add(Gamma(2.5, 2.0, 0.0));
    coll2.add(Normal(3.0, 1.5));
    MarginalTransformationFunction transformation(coll1, coll2);
    fullprint << "transformation=" << transformation << std::endl;
    Point point(coll1.getSize());
    point[0] = coll1[0].computeQuantile(0.25)[0];
    point[1] = coll1[1].computeQuantile(0.25)[0];
    fullprint << "transformation(" << point << ")=" << transformation(point) << std::endl;
    fullprint << "input dimension=" << transformation.getInputDimension() << std::endl;
    fullprint << "output dimension=" << transformation.getOutputDimension() << std::endl;
    fullprint << "input distribution collection=" << transformation.getInputDistributionCollection() << std::endl;
    fullprint << "output distribution collection=" << transformation.getOutputDistributionCollection() << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
