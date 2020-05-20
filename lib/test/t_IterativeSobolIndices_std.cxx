//                                               -*- C++ -*-
/**
 *  @brief The test file of class SobolIndicesAlgorithm
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
  try
  {

    RandomGenerator::SetSeed(0);

    UnsignedInteger inputDimension = 3;
    //UnsignedInteger outputDimension = 1;

    Description inputName(inputDimension);
    inputName[0] = "X1";
    inputName[1] = "X2";
    inputName[2] = "X3";
    Description formula(1);
    formula[0] = "sin(pi_*X1)+7*sin(pi_*X2)*sin(pi_*X2)+0.1*((pi_*X3)*(pi_*X3)*(pi_*X3)*(pi_*X3))*sin(pi_*X1)";

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
    const Scalar confidence_level = 0.95;

    Description methods;
    methods.add("MonteCarlo");
    methods.add("LHS");
    methods.add("QMC");
    for (UnsignedInteger i = 0; i < methods.getSize(); ++i)
    {
      ResourceMap::SetAsString("SobolIndicesExperiment-SamplingMethod", methods[i]);
      fullprint << "Sampling method=" << methods[i] << std::endl;
      const SobolIndicesExperiment sobolExperiment(maDistribution, size, true);
      const Sample inputDesign(sobolExperiment.generate());
      const Sample fullOutputDesign(model(inputDesign));
      const Sample outputDesign(fullOutputDesign, 0, size * (inputDesign.getDimension() + 2));

      {
        SaltelliSobolIndices sensitivitySobol(inputDesign.getDimension(), outputDesign.getDimension());

        sensitivitySobol.computeIndices(outputDesign);

        const Point firstOrderIndicesFullSample(sensitivitySobol.getFirstOrderIndices());
        const Point totalOrderIndicesFullSample(sensitivitySobol.getTotalOrderIndices());

        fullprint << "Method = " << sensitivitySobol.getClassName() << std::endl;
        fullprint << "First order Sobol indice of Y|X1 = " << firstOrderIndicesFullSample[0] << std::endl;
        fullprint << "Total order Sobol indice of Y|X3 = " << totalOrderIndicesFullSample[2] << std::endl;

        sensitivitySobol.incrementIndices(outputDesign);

        const Point firstOrderIndicesIterativeSample(sensitivitySobol.getFirstOrderIndices());
        const Point totalOrderIndicesIterativeSample(sensitivitySobol.getTotalOrderIndices());

        fullprint << "First order Sobol indice of Y|X1 = " << firstOrderIndicesIterativeSample[0] << std::endl;
        fullprint << "Total order Sobol indice of Y|X3 = " << totalOrderIndicesIterativeSample[2] << std::endl;
      }

      {
        JansenSobolIndices sensitivitySobol(inputDesign.getDimension(), outputDesign.getDimension());

        sensitivitySobol.computeIndices(outputDesign);

        const Point firstOrderIndicesFullSample(sensitivitySobol.getFirstOrderIndices());
        const Point totalOrderIndicesFullSample(sensitivitySobol.getTotalOrderIndices());

        fullprint << "Method = " << sensitivitySobol.getClassName() << std::endl;
        fullprint << "First order Sobol indice of Y|X1 = " << firstOrderIndicesFullSample[0] << std::endl;
        fullprint << "Total order Sobol indice of Y|X3 = " << totalOrderIndicesFullSample[2] << std::endl;

        sensitivitySobol.incrementIndices(outputDesign);

        const Point firstOrderIndicesIterativeSample(sensitivitySobol.getFirstOrderIndices());
        const Point totalOrderIndicesIterativeSample(sensitivitySobol.getTotalOrderIndices());

        fullprint << "First order Sobol indice of Y|X1 = " << firstOrderIndicesIterativeSample[0] << std::endl;
        fullprint << "Total order Sobol indice of Y|X3 = " << totalOrderIndicesIterativeSample[2] << std::endl;
      }
      {
        MauntzKucherenkoSobolIndices sensitivitySobol(inputDesign.getDimension(), outputDesign.getDimension());

        sensitivitySobol.computeIndices(outputDesign);

        const Point firstOrderIndicesFullSample(sensitivitySobol.getFirstOrderIndices());
        const Point totalOrderIndicesFullSample(sensitivitySobol.getTotalOrderIndices());

        fullprint << "Method = " << sensitivitySobol.getClassName() << std::endl;
        fullprint << "First order Sobol indice of Y|X1 = " << firstOrderIndicesFullSample[0] << std::endl;
        fullprint << "Total order Sobol indice of Y|X3 = " << totalOrderIndicesFullSample[2] << std::endl;

        sensitivitySobol.incrementIndices(outputDesign);

        const Point firstOrderIndicesIterativeSample(sensitivitySobol.getFirstOrderIndices());
        const Point totalOrderIndicesIterativeSample(sensitivitySobol.getTotalOrderIndices());

        fullprint << "First order Sobol indice of Y|X1 = " << firstOrderIndicesIterativeSample[0] << std::endl;
        fullprint << "Total order Sobol indice of Y|X3 = " << totalOrderIndicesIterativeSample[2] << std::endl;
      }
      {
        MartinezSobolIndices sensitivitySobol(inputDesign.getDimension(), outputDesign.getDimension());

        sensitivitySobol.computeIndices(outputDesign);

        const Point firstOrderIndicesFullSample(sensitivitySobol.getFirstOrderIndices());
        const Point totalOrderIndicesFullSample(sensitivitySobol.getTotalOrderIndices());

        fullprint << "Method = " << sensitivitySobol.getClassName() << std::endl;
        fullprint << "First order Sobol indice of Y|X1 = " << firstOrderIndicesFullSample[0] << std::endl;
        fullprint << "Total order Sobol indice of Y|X3 = " << totalOrderIndicesFullSample[2] << std::endl;

        sensitivitySobol.incrementIndices(outputDesign);

        const Point firstOrderIndicesIterativeSample(sensitivitySobol.getFirstOrderIndices());
        const Point totalOrderIndicesIterativeSample(sensitivitySobol.getTotalOrderIndices());

        fullprint << "First order Sobol indice of Y|X1 = " << firstOrderIndicesIterativeSample[0] << std::endl;
        fullprint << "Total order Sobol indice of Y|X3 = " << totalOrderIndicesIterativeSample[2] << std::endl;
      }
    } // Sampling method
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
