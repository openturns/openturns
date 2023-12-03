//                                               -*- C++ -*-
/**
 *  @brief The test file of class ExperimentIntegration for standard methods
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
  setRandomGenerator();

  // Create the Ishigami test case
  // Problem parameters
  const UnsignedInteger dimension = 3;
  const Scalar a = 7.0;
  const Scalar b = 0.1;
  // Reference analytical values
  const Scalar mean = a / 2;
  const Scalar cov = (pow(b, 2.0) * pow(M_PI, 8.0)) / 18.0 + (b * pow(M_PI, 4.0)) / 5.0 + (pow(a, 2.0)) / 8.0 + 1.0 / 2.0;
  const Scalar exactL2Norm = std::sqrt(cov);
  // Create the centered Ishigami function
  fullprint << "Create the centered Ishigami function" << std::endl;
  const Description inputVariables = {"xi1", "xi2", "xi3", "a", "b", "mean"};
  Description formula(1);
  formula[0] = (OSS() << "sin(xi1) + a * (sin(xi2)) ^ 2 + b * xi3^4 * sin(xi1) - mean");
  const SymbolicFunction centeredIshigamiParam(inputVariables, formula);
  const Point parameters({a, b, 0.0});
  const ParametricFunction ishigamiFunction(centeredIshigamiParam, {3, 4, 5}, parameters);
  const Point parameters2({a, b, mean});
  const ParametricFunction centeredIshigami(centeredIshigamiParam, {3, 4, 5}, parameters2);
  // Create the input distribution
  fullprint << "Create the input distribution" << std::endl;
  const Collection<Distribution> marginals(dimension, Uniform(-M_PI, M_PI));
  const ComposedDistribution distributionIshigami(marginals);

  const UnsignedInteger sampleSize = 1000000;
  const MonteCarloExperiment experiment2(distributionIshigami, sampleSize);
  const ExperimentIntegration integration(experiment2);
  Scalar atolIntegrate = 10.0 / std::sqrt(sampleSize);
  fullprint << "Absolute tolerance = " << atolIntegrate << std::endl;

  fullprint << "Test integrate()" << std::endl;
  const Point ishigamiMeanComputed(integration.integrate(ishigamiFunction));
  assert(ishigamiMeanComputed.getDimension() == 1);
  fullprint << "    ishigamiMeanComputed[0] = " << ishigamiMeanComputed[0] << std::endl;
  fullprint << "    mean = " << mean << std::endl;
  assert_almost_equal(ishigamiMeanComputed[0], mean, atolIntegrate);

  fullprint << "Test computeL2Norm()" << std::endl;
  const Point centeredIshigamiL2Norm(integration.computeL2Norm(centeredIshigami));
  assert(centeredIshigamiL2Norm.getDimension() == 1);
  fullprint << "    centeredIshigamiL2Norm[0] = " << centeredIshigamiL2Norm[0] << std::endl;
  fullprint << "    exactL2Norm = " << exactL2Norm << std::endl;
  assert_almost_equal(centeredIshigamiL2Norm[0], exactL2Norm, atolIntegrate);

  // This is Ishigami, where the first part, sin(xi1), has been removed
  // The difference between ishigamiFunction and ishigamiPartFunction is sin(xi1).
  Description formulaPart(1);
  formulaPart[0] = (OSS() << "a * (sin(xi2)) ^ 2 + b * xi3^4 * sin(xi1) - mean");
  const SymbolicFunction ishigamiPartParam(inputVariables, formulaPart);
  const ParametricFunction ishigamiPartFunction(ishigamiPartParam, {3, 4, 5}, parameters);

  fullprint << "Test computeL2Norm()" << std::endl;
  const Point ishigamiErrorComputed(integration.computeL2Norm(ishigamiFunction - ishigamiPartFunction));
  assert(ishigamiErrorComputed.getDimension() == 1);
  Scalar errorExact = std::sqrt(0.5);
  fullprint << "    ishigamiErrorComputed[0] = " << ishigamiErrorComputed[0] << std::endl;
  fullprint << "    errorExact = " << errorExact << std::endl;
  assert_almost_equal(ishigamiErrorComputed[0], errorExact, atolIntegrate);

  return ExitCode::Success;
}
