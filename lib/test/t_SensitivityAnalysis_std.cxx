//                                               -*- C++ -*-
/**
 *  @brief The test file of class SensitivityAnalysis
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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

    UnsignedInteger inputDimension(3);
    UnsignedInteger outputDimension(1);

    Description inputName(inputDimension);
    inputName[0] = "X1";
    inputName[1] = "X2";
    inputName[2] = "X3";
    Description outputName(outputDimension);
    outputName[0] = "Y";
    Description formula(1);
    formula[0] = "sin(_pi*X1)+7*sin(_pi*X2)*sin(_pi*X2)+0.1*((_pi*X3)*(_pi*X3)*(_pi*X3)*(_pi*X3))*sin(_pi*X1)";

    NumericalMathFunction model(inputName, outputName, formula);

    ComposedDistribution::DistributionCollection marginals(inputDimension);
    marginals[0] = Uniform(-1.0, 1.0);
    //     marginals[0].setDescription("Marginal 1");
    marginals[1] = Uniform(-1.0, 1.0);
    //     marginals[1].setDescription("Marginal 2");
    marginals[2] = Uniform(-1.0, 1.0);
    //     marginals[2].setDescription("Marginal 3");
    ComposedDistribution maDistribution(ComposedDistribution(marginals, IndependentCopula(inputDimension)));


    UnsignedInteger size(10000);
    NumericalSample sample1(maDistribution.getSample(size));
    NumericalSample sample2(maDistribution.getSample(size));

    SensitivityAnalysis sensitivitySobol(sample1, sample2, model);
    sensitivitySobol.setBlockSize(17);
    SymmetricMatrix secondOrderIndices( sensitivitySobol.getSecondOrderIndices() );
    NumericalPoint firstOrderIndices(sensitivitySobol.getFirstOrderIndices());
    NumericalPoint totalOrderIndices(sensitivitySobol.getTotalOrderIndices());

    fullprint << "First order Sobol indice of Y|X1 = " << firstOrderIndices[0] << std::endl;
    fullprint << "Total order Sobol indice of Y|X3 = " << totalOrderIndices[2] << std::endl;
    fullprint << "Second order Sobol indice of Y|X1,X3 = " << secondOrderIndices(0, 2) << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
