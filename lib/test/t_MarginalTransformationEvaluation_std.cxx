//                                               -*- C++ -*-
/**
 *  @brief The test file of class MarginalTransformationEvaluation for standard methods
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
    MarginalTransformationEvaluation::DistributionCollection coll1(0);
    coll1.add(Normal(1.0, 2.5));
    coll1.add(Gamma(1.5, 3.0, 0.0));
    Point pointLow(0);
    pointLow.add(coll1[0].computeQuantile(0.25)[0]);
    pointLow.add(coll1[1].computeQuantile(0.25)[0]);
    Point pointHigh(0);
    pointHigh.add(coll1[0].computeQuantile(0.75)[0]);
    pointHigh.add(coll1[1].computeQuantile(0.75)[0]);
    MarginalTransformationEvaluation::DistributionCollection coll2(0);
    coll2.add(Gamma(2.5, 2.0, 0.0));
    coll2.add(Normal(3.0, 1.5));
    // First, check the old constructor
    {
      MarginalTransformationEvaluation transformation(coll1);
      fullprint << "transformation=" << transformation << std::endl;
      fullprint << "transformation(" << pointLow << ")=" << transformation(pointLow) << std::endl;
      fullprint << "transformation(" << pointHigh << ")=" << transformation(pointHigh) << std::endl;
      // Validation using finite difference
      Scalar eps = 1e-5;
      Scalar factor = 1.0 / (2.0 * eps);
      Matrix gradientLow(5, 2);
      Matrix gradientHigh(5, 2);
      Point dTdp;
      {
        // dT/dp0
        MarginalTransformationEvaluation::DistributionCollection coll(coll1);
        coll[0] = Normal(1.0 + eps, 2.5);
        MarginalTransformationEvaluation left(coll);
        coll[0] = Normal(1.0 - eps, 2.5);
        MarginalTransformationEvaluation right(coll);
        dTdp = (left(pointLow) - right(pointLow)) * factor;
        gradientLow(0, 0) = dTdp[0];
        gradientLow(0, 1) = dTdp[1];
        dTdp = (left(pointHigh) - right(pointHigh)) * factor;
        gradientHigh(0, 0) = dTdp[0];
        gradientHigh(0, 1) = dTdp[1];
      }
      {
        // dT/dp1
        MarginalTransformationEvaluation::DistributionCollection coll(coll1);
        coll[0] = Normal(1.0, 2.5 + eps);
        MarginalTransformationEvaluation left(coll);
        coll[0] = Normal(1.0, 2.5 - eps);
        MarginalTransformationEvaluation right(coll);
        dTdp = (left(pointLow) - right(pointLow)) * factor;
        gradientLow(1, 0) = dTdp[0];
        gradientLow(1, 1) = dTdp[1];
        dTdp = (left(pointHigh) - right(pointHigh)) * factor;
        gradientHigh(1, 0) = dTdp[0];
        gradientHigh(1, 1) = dTdp[1];
      }
      {
        // dT/dp2
        MarginalTransformationEvaluation::DistributionCollection coll(coll1);
        coll[1] = Gamma(1.5 + eps, 3.0, 0.0);
        MarginalTransformationEvaluation left(coll);
        coll[1] = Gamma(1.5 - eps, 3.0, 0.0);
        MarginalTransformationEvaluation right(coll);
        dTdp = (left(pointLow) - right(pointLow)) * factor;
        gradientLow(2, 0) = dTdp[0];
        gradientLow(2, 1) = dTdp[1];
        dTdp = (left(pointHigh) - right(pointHigh)) * factor;
        gradientHigh(2, 0) = dTdp[0];
        gradientHigh(2, 1) = dTdp[1];
      }
      {
        // dT/dp3
        MarginalTransformationEvaluation::DistributionCollection coll(coll1);
        coll[1] = Gamma(1.5, 3.0 + eps, 0.0);
        MarginalTransformationEvaluation left(coll);
        coll[1] = Gamma(1.5, 3.0 - eps, 0.0);
        MarginalTransformationEvaluation right(coll);
        dTdp = (left(pointLow) - right(pointLow)) * factor;
        gradientLow(3, 0) = dTdp[0];
        gradientLow(3, 1) = dTdp[1];
        dTdp = (left(pointHigh) - right(pointHigh)) * factor;
        gradientHigh(3, 0) = dTdp[0];
        gradientHigh(3, 1) = dTdp[1];
      }
      {
        // dT/dp4
        MarginalTransformationEvaluation::DistributionCollection coll(coll1);
        coll[1] = Gamma(1.5, 3.0, 0.0 + eps);
        MarginalTransformationEvaluation left(coll);
        coll[1] = Gamma(1.5, 3.0, 0.0 - eps);
        MarginalTransformationEvaluation right(coll);
        dTdp = (left(pointLow) - right(pointLow)) * factor;
        gradientLow(4, 0) = dTdp[0];
        gradientLow(4, 1) = dTdp[1];
        dTdp = (left(pointHigh) - right(pointHigh)) * factor;
        gradientHigh(4, 0) = dTdp[0];
        gradientHigh(4, 1) = dTdp[1];
      }
      fullprint << "transformation    parameters gradient=" << transformation.parameterGradient(pointLow) << std::endl;
      fullprint << "finite difference parameters gradient=" << gradientLow << std::endl;
      fullprint << "transformation    parameters gradient=" << transformation.parameterGradient(pointHigh) << std::endl;
      fullprint << "finite difference parameters gradient=" << gradientHigh << std::endl;
      fullprint << "input dimension=" << transformation.getInputDimension() << std::endl;
      fullprint << "output dimension=" << transformation.getOutputDimension() << std::endl;
    }
    // Second, check the constructor for old inverse transformation
    {
      MarginalTransformationEvaluation transformation(coll1, MarginalTransformationEvaluation::TO);
      fullprint << "transformation=" << transformation << std::endl;
      Point uLow(coll1.getSize(), 0.25);
      Point uHigh(coll1.getSize(), 0.75);
      fullprint << "transformation(" << uLow << ")=" << transformation(uLow) << std::endl;
      fullprint << "transformation(" << uHigh << ")=" << transformation(uHigh) << std::endl;
      // Validation using finite difference
      Scalar eps = 1e-5;
      Scalar factor = 1.0 / (2.0 * eps);
      Matrix gradientLow(5, 2);
      Matrix gradientHigh(5, 2);
      Point dTdp;
      {
        // dT/dp0
        MarginalTransformationEvaluation::DistributionCollection coll(coll1);
        coll[0] = Normal(1.0 + eps, 2.5);
        MarginalTransformationEvaluation left(coll, MarginalTransformationEvaluation::TO);
        coll[0] = Normal(1.0 - eps, 2.5);
        MarginalTransformationEvaluation right(coll, MarginalTransformationEvaluation::TO);
        dTdp = (left(uLow) - right(uLow)) * factor;
        gradientLow(0, 0) = dTdp[0];
        gradientLow(0, 1) = dTdp[1];
        dTdp = (left(uHigh) - right(uHigh)) * factor;
        gradientHigh(0, 0) = dTdp[0];
        gradientHigh(0, 1) = dTdp[1];
      }
      {
        // dT/dp1
        MarginalTransformationEvaluation::DistributionCollection coll(coll1);
        coll[0] = Normal(1.0, 2.5 + eps);
        MarginalTransformationEvaluation left(coll, MarginalTransformationEvaluation::TO);
        coll[0] = Normal(1.0, 2.5 - eps);
        MarginalTransformationEvaluation right(coll, MarginalTransformationEvaluation::TO);
        dTdp = (left(uLow) - right(uLow)) * factor;
        gradientLow(1, 0) = dTdp[0];
        gradientLow(1, 1) = dTdp[1];
        dTdp = (left(uHigh) - right(uHigh)) * factor;
        gradientHigh(1, 0) = dTdp[0];
        gradientHigh(1, 1) = dTdp[1];
      }
      {
        // dT/dp2
        MarginalTransformationEvaluation::DistributionCollection coll(coll1);
        coll[1] = Gamma(1.5 + eps, 3.0, 0.0);
        MarginalTransformationEvaluation left(coll, MarginalTransformationEvaluation::TO);
        coll[1] = Gamma(1.5 - eps, 3.0, 0.0);
        MarginalTransformationEvaluation right(coll, MarginalTransformationEvaluation::TO);
        dTdp = (left(uLow) - right(uLow)) * factor;
        gradientLow(2, 0) = dTdp[0];
        gradientLow(2, 1) = dTdp[1];
        dTdp = (left(uHigh) - right(uHigh)) * factor;
        gradientHigh(2, 0) = dTdp[0];
        gradientHigh(2, 1) = dTdp[1];
      }
      {
        // dT/dp3
        MarginalTransformationEvaluation::DistributionCollection coll(coll1);
        coll[1] = Gamma(1.5, 3.0 + eps, 0.0);
        MarginalTransformationEvaluation left(coll, MarginalTransformationEvaluation::TO);
        coll[1] = Gamma(1.5, 3.0 - eps, 0.0);
        MarginalTransformationEvaluation right(coll, MarginalTransformationEvaluation::TO);
        dTdp = (left(uLow) - right(uLow)) * factor;
        gradientLow(3, 0) = dTdp[0];
        gradientLow(3, 1) = dTdp[1];
        dTdp = (left(uHigh) - right(uHigh)) * factor;
        gradientHigh(3, 0) = dTdp[0];
        gradientHigh(3, 1) = dTdp[1];
      }
      {
        // dT/dp4
        MarginalTransformationEvaluation::DistributionCollection coll(coll1);
        coll[1] = Gamma(1.5, 3.0, 0.0 + eps);
        MarginalTransformationEvaluation left(coll, MarginalTransformationEvaluation::TO);
        coll[1] = Gamma(1.5, 3.0, 0.0 - eps);
        MarginalTransformationEvaluation right(coll, MarginalTransformationEvaluation::TO);
        dTdp = (left(uLow) - right(uLow)) * factor;
        gradientLow(4, 0) = dTdp[0];
        gradientLow(4, 1) = dTdp[1];
        dTdp = (left(uHigh) - right(uHigh)) * factor;
        gradientHigh(4, 0) = dTdp[0];
        gradientHigh(4, 1) = dTdp[1];
      }
      fullprint << "transformation    parameters gradient=" << transformation.parameterGradient(uLow) << std::endl;
      fullprint << "finite difference parameters gradient=" << gradientLow << std::endl;
      fullprint << "transformation    parameters gradient=" << transformation.parameterGradient(uHigh) << std::endl;
      fullprint << "finite difference parameters gradient=" << gradientHigh << std::endl;
      fullprint << "input dimension=" << transformation.getInputDimension() << std::endl;
      fullprint << "output dimension=" << transformation.getOutputDimension() << std::endl;
    }
    // Third, check the constructor for the new transformation
    {
      MarginalTransformationEvaluation transformation(coll1, coll2);
      fullprint << "transformation=" << transformation << std::endl;
      fullprint << "transformation(" << pointLow << ")=" << transformation(pointLow) << std::endl;
      fullprint << "transformation(" << pointHigh << ")=" << transformation(pointHigh) << std::endl;
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
