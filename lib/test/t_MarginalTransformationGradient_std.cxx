//                                               -*- C++ -*-
/**
 *  @brief The test file of class MarginalTransformationGradient for standard methods
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
    MarginalTransformationEvaluation::DistributionCollection coll1(0);
    coll1.add(Normal(1.0, 2.5));
    coll1.add(Gamma(1.5, 3.0));
    Point pointLow(0);
    pointLow.add(coll1[0].computeQuantile(0.25)[0]);
    pointLow.add(coll1[1].computeQuantile(0.25)[0]);
    Point pointHigh(0);
    pointHigh.add(coll1[0].computeQuantile(0.75)[0]);
    pointHigh.add(coll1[1].computeQuantile(0.75)[0]);
    MarginalTransformationEvaluation::DistributionCollection coll2(0);
    coll2.add(Gamma(2.5, 2.0));
    coll2.add(Normal(3.0, 1.5));
    // First, check the old constructor
    {
      MarginalTransformationEvaluation evaluation(coll1);
      MarginalTransformationGradient transformation(evaluation);

      fullprint << "transformation=" << transformation << std::endl;
      fullprint << "transformation.gradient(" << pointLow << ")=" << transformation.gradient(pointLow) << std::endl;
      fullprint << "finite difference gradient(" << pointLow << ")=" << CenteredFiniteDifferenceGradient(1.0e-5, evaluation.clone()).gradient(pointLow) << std::endl;
      fullprint << "transformation.gradient(" << pointHigh << ")=" << transformation.gradient(pointHigh) << std::endl;
      fullprint << "finite difference gradient(" << pointHigh << ")=" << CenteredFiniteDifferenceGradient(1.0e-5, evaluation.clone()).gradient(pointHigh) << std::endl;
      fullprint << "input dimension=" << transformation.getInputDimension() << std::endl;
      fullprint << "output dimension=" << transformation.getOutputDimension() << std::endl;
    }
    // Second, check the constructor for old inverse transformation
    {
      MarginalTransformationEvaluation evaluation(coll1, MarginalTransformationEvaluation::TO);
      MarginalTransformationGradient transformation(evaluation);
      fullprint << "transformation=" << transformation << std::endl;
      Point uLow(coll1.getSize(), 0.25);
      Point uHigh(coll1.getSize(), 0.75);
      fullprint << "transformation.gradient(" << uLow << ")=" << transformation.gradient(uLow) << std::endl;
      fullprint << "finite difference gradient(" << uLow << ")=" << CenteredFiniteDifferenceGradient(1.0e-5, evaluation.clone()).gradient(uLow) << std::endl;
      fullprint << "transformation.gradient(" << uHigh << ")=" << transformation.gradient(uHigh) << std::endl;
      fullprint << "finite difference gradient(" << uHigh << ")=" << CenteredFiniteDifferenceGradient(1.0e-5, evaluation.clone()).gradient(uHigh) << std::endl;
      fullprint << "input dimension=" << transformation.getInputDimension() << std::endl;
      fullprint << "output dimension=" << transformation.getOutputDimension() << std::endl;
    }
    // Third, check the constructor for the new transformation
    {
      MarginalTransformationEvaluation evaluation(coll1, coll2);
      MarginalTransformationGradient transformation(evaluation);
      fullprint << "transformation=" << transformation << std::endl;
      fullprint << "transformation.gradient(" << pointLow << ")=" << transformation.gradient(pointLow) << std::endl;
      fullprint << "finite difference gradient(" << pointLow << ")=" << CenteredFiniteDifferenceGradient(1.0e-5, evaluation.clone()).gradient(pointLow) << std::endl;
      fullprint << "transformation.gradient(" << pointHigh << ")=" << transformation.gradient(pointHigh) << std::endl;
      fullprint << "finite difference gradient(" << pointHigh << ")=" << CenteredFiniteDifferenceGradient(1.0e-5, evaluation.clone()).gradient(pointHigh) << std::endl;
      fullprint << "input dimension=" << transformation.getInputDimension() << std::endl;
      fullprint << "output dimension=" << transformation.getOutputDimension() << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
