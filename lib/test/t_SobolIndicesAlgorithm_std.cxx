//                                               -*- C++ -*-
/**
 *  @brief The test file of class SobolIndicesAlgorithm
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  try
  {

    RandomGenerator::SetSeed(0);

    UnsignedInteger inputDimension = 3;
    UnsignedInteger outputDimension = 1;

    Description inputName(inputDimension);
    inputName[0] = "X1";
    inputName[1] = "X2";
    inputName[2] = "X3";
    Description formula(1);
    formula[0] = "sin(_pi*X1)+7*sin(_pi*X2)*sin(_pi*X2)+0.1*((_pi*X3)*(_pi*X3)*(_pi*X3)*(_pi*X3))*sin(_pi*X1)";

    SymbolicFunction model(inputName, formula);

    ComposedDistribution::DistributionCollection marginals(inputDimension);
    marginals[0] = Uniform(-1.0, 1.0);
    //     marginals[0].setDescription("Marginal 1");
    marginals[1] = Uniform(-1.0, 1.0);
    //     marginals[1].setDescription("Marginal 2");
    marginals[2] = Uniform(-1.0, 1.0);
    //     marginals[2].setDescription("Marginal 3");
    ComposedDistribution maDistribution(ComposedDistribution(marginals, IndependentCopula(inputDimension)));


    const UnsignedInteger size = 10000;
    const UnsignedInteger nr_bootstrap = 100;
    const NumericalScalar confidence_level = 0.95;

    const Sample inputDesign(SobolIndicesAlgorithmImplementation::Generate(maDistribution, size, true));
    const Sample outputDesign(model(inputDesign));

    {
      SaltelliSensitivityAlgorithm sensitivitySobol(inputDesign, outputDesign, size);

      const SymmetricMatrix secondOrderIndices( sensitivitySobol.getSecondOrderIndices() );
      const NumericalPoint firstOrderIndices(sensitivitySobol.getFirstOrderIndices());
      const NumericalPoint totalOrderIndices(sensitivitySobol.getTotalOrderIndices());

      fullprint << "Method = " << sensitivitySobol.getClassName() << std::endl;
      fullprint << "First order Sobol indice of Y|X1 = " << firstOrderIndices[0] << std::endl;
      fullprint << "Total order Sobol indice of Y|X3 = " << totalOrderIndices[2] << std::endl;
      fullprint << "Second order Sobol indice of Y|X1,X3 = " << secondOrderIndices(0, 2) << std::endl;
      // Confidence interval
      sensitivitySobol.setBootstrapSize(nr_bootstrap);
      sensitivitySobol.setBootstrapConfidenceLevel(confidence_level);

      const Interval confidenceIntervalFirstOrder(sensitivitySobol.getFirstOrderIndicesInterval());
      const Interval confidenceIntervalTotalOrder(sensitivitySobol.getTotalOrderIndicesInterval());
      fullprint << "Confidence interval of first order Y|X1 = [" << confidenceIntervalFirstOrder.getLowerBound()[0]
                << ", " << confidenceIntervalFirstOrder.getUpperBound()[0] << "]" << std::endl;
      fullprint << "Confidence interval of total order Y|X3 = [" << confidenceIntervalTotalOrder.getLowerBound()[2]
                << ", " << confidenceIntervalTotalOrder.getUpperBound()[2] << "]" << std::endl;
    }

    {
      JansenSensitivityAlgorithm sensitivitySobol(inputDesign, outputDesign, size);

      const SymmetricMatrix secondOrderIndices( sensitivitySobol.getSecondOrderIndices() );
      const NumericalPoint firstOrderIndices(sensitivitySobol.getFirstOrderIndices());
      const NumericalPoint totalOrderIndices(sensitivitySobol.getTotalOrderIndices());

      fullprint << "Method = " << sensitivitySobol.getClassName() << std::endl;
      fullprint << "First order Sobol indice of Y|X1 = " << firstOrderIndices[0] << std::endl;
      fullprint << "Total order Sobol indice of Y|X3 = " << totalOrderIndices[2] << std::endl;
      fullprint << "Second order Sobol indice of Y|X1,X3 = " << secondOrderIndices(0, 2) << std::endl;
      // Confidence interval
      sensitivitySobol.setBootstrapSize(nr_bootstrap);
      sensitivitySobol.setBootstrapConfidenceLevel(confidence_level);

      const Interval confidenceIntervalFirstOrder(sensitivitySobol.getFirstOrderIndicesInterval());
      const Interval confidenceIntervalTotalOrder(sensitivitySobol.getTotalOrderIndicesInterval());
      fullprint << "Confidence interval of first order Y|X1 = [" << confidenceIntervalFirstOrder.getLowerBound()[0]
                << ", " << confidenceIntervalFirstOrder.getUpperBound()[0] << "]" << std::endl;
      fullprint << "Confidence interval of total order Y|X3 = [" << confidenceIntervalTotalOrder.getLowerBound()[2]
                << ", " << confidenceIntervalTotalOrder.getUpperBound()[2] << "]" << std::endl;
    }
    {
      MauntzKucherenkoSensitivityAlgorithm sensitivitySobol(inputDesign, outputDesign, size);

      const SymmetricMatrix secondOrderIndices( sensitivitySobol.getSecondOrderIndices() );
      const NumericalPoint firstOrderIndices(sensitivitySobol.getFirstOrderIndices());
      const NumericalPoint totalOrderIndices(sensitivitySobol.getTotalOrderIndices());

      fullprint << "Method = " << sensitivitySobol.getClassName() << std::endl;
      fullprint << "First order Sobol indice of Y|X1 = " << firstOrderIndices[0] << std::endl;
      fullprint << "Total order Sobol indice of Y|X3 = " << totalOrderIndices[2] << std::endl;
      fullprint << "Second order Sobol indice of Y|X1,X3 = " << secondOrderIndices(0, 2) << std::endl;
      // Confidence interval
      sensitivitySobol.setBootstrapSize(nr_bootstrap);
      sensitivitySobol.setBootstrapConfidenceLevel(confidence_level);

      const Interval confidenceIntervalFirstOrder(sensitivitySobol.getFirstOrderIndicesInterval());
      const Interval confidenceIntervalTotalOrder(sensitivitySobol.getTotalOrderIndicesInterval());
      fullprint << "Confidence interval of first order Y|X1 = [" << confidenceIntervalFirstOrder.getLowerBound()[0]
                << ", " << confidenceIntervalFirstOrder.getUpperBound()[0] << "]" << std::endl;
      fullprint << "Confidence interval of total order Y|X3 = [" << confidenceIntervalTotalOrder.getLowerBound()[2]
                << ", " << confidenceIntervalTotalOrder.getUpperBound()[2] << "]" << std::endl;
    }
    {
      MartinezSensitivityAlgorithm sensitivitySobol(inputDesign, outputDesign, size);

      const SymmetricMatrix secondOrderIndices( sensitivitySobol.getSecondOrderIndices() );
      const NumericalPoint firstOrderIndices(sensitivitySobol.getFirstOrderIndices());
      const NumericalPoint totalOrderIndices(sensitivitySobol.getTotalOrderIndices());

      fullprint << "Method = " << sensitivitySobol.getClassName() << std::endl;
      fullprint << "First order Sobol indice of Y|X1 = " << firstOrderIndices[0] << std::endl;
      fullprint << "Total order Sobol indice of Y|X3 = " << totalOrderIndices[2] << std::endl;
      fullprint << "Second order Sobol indice of Y|X1,X3 = " << secondOrderIndices(0, 2) << std::endl;
      // Confidence interval
      sensitivitySobol.setBootstrapSize(nr_bootstrap);
      sensitivitySobol.setBootstrapConfidenceLevel(confidence_level);

      const Interval confidenceIntervalFirstOrder(sensitivitySobol.getFirstOrderIndicesInterval());
      const Interval confidenceIntervalTotalOrder(sensitivitySobol.getTotalOrderIndicesInterval());
      fullprint << "Confidence interval of first order Y|X1 = [" << confidenceIntervalFirstOrder.getLowerBound()[0]
                << ", " << confidenceIntervalFirstOrder.getUpperBound()[0] << "]" << std::endl;
      fullprint << "Confidence interval of total order Y|X3 = [" << confidenceIntervalTotalOrder.getLowerBound()[2]
                << ", " << confidenceIntervalTotalOrder.getUpperBound()[2] << "]" << std::endl;


      fullprint << "Asymptotic estimate" << std::endl;
      ResourceMap::SetAsBool("MartinezSensitivityAlgorithm-UseAsymptoticInterval", true);
      const Interval asymptoticConfidenceIntervalFirstOrder(sensitivitySobol.getFirstOrderIndicesInterval());
      const Interval asymptoticConfidenceIntervalTotalOrder(sensitivitySobol.getTotalOrderIndicesInterval());
      fullprint << "Confidence interval of first order Y|X1 = [" << asymptoticConfidenceIntervalFirstOrder.getLowerBound()[0]
                << ", " << asymptoticConfidenceIntervalFirstOrder.getUpperBound()[0] << "]" << std::endl;
      fullprint << "Confidence interval of total order Y|X3 = [" << asymptoticConfidenceIntervalTotalOrder.getLowerBound()[2]
                << ", " << asymptoticConfidenceIntervalTotalOrder.getUpperBound()[2] << "]" << std::endl;
    }

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
