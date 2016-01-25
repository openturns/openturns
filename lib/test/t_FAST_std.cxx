//                                               -*- C++ -*-
/**
 *  @brief The test file of class FAST
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
#include "OT.hxx"
#include "OTtestcode.hxx"

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

    // // Test with Ishigami function
    Description formulaIshigami(1);
    formulaIshigami[0] = "sin(_pi*X1)+7*sin(_pi*X2)*sin(_pi*X2)+0.1*((_pi*X3)*(_pi*X3)*(_pi*X3)*(_pi*X3))*sin(_pi*X1)";

    NumericalMathFunction modelIshigami(inputName, outputName, formulaIshigami);

    ComposedDistribution::DistributionCollection marginals(inputDimension);
    marginals[0] = Uniform(-1.0, 1.0);
    marginals[1] = Uniform(-1.0, 1.0);
    marginals[2] = Uniform(-1.0, 1.0);
    ComposedDistribution distributions(marginals);

    FAST sensitivityFast(modelIshigami, distributions, 400);

    // Comparaison with reference analytical values
    NumericalScalar a(7.0);
    NumericalScalar b(0.1);
    NumericalScalar covTh((pow(b, 2.0) * pow(M_PI, 8.0)) / 18.0 + (b * pow(M_PI, 4.0)) / 5.0 + (pow(a, 2.0)) / 8.0 + 1.0 / 2.0);
    NumericalPoint sob_1(3);
    sob_1[0] = (b * pow(M_PI, 4.0) / 5.0 + pow(b, 2.0) * pow(M_PI, 8.0) / 50.0 + 1.0 / 2.0) / covTh;
    sob_1[1] = (pow(a, 2.0) / 8.0) / covTh;
    sob_1[2] = 0.0;
    NumericalPoint sob_2(3);
    sob_2[0] = 0.0;
    sob_2[1] = (pow(b, 2.0) * pow(M_PI, 8.0) / 18.0 - pow(b, 2.0) * pow(M_PI, 8.0) / 50.0) / covTh;
    sob_2[2] = 0.0;
    NumericalPoint sob_3(1, 0.0);
    NumericalPoint sob_T1(3);
    sob_T1[0] = sob_1[0] + sob_2[0] + sob_2[1] + sob_3[0];
    sob_T1[1] = sob_1[1] + sob_2[0] + sob_2[2] + sob_3[0];
    sob_T1[2] = sob_1[2] + sob_2[1] + sob_2[2] + sob_3[0];

    NumericalPoint firstOrderFastIndices(sensitivityFast.getFirstOrderIndices());
    NumericalPoint totalOrderFastIndices(sensitivityFast.getTotalOrderIndices());
    for(UnsignedInteger i = 0; i < inputDimension; ++i)
    {
      NumericalScalar value(firstOrderFastIndices[i]);
      fullprint << "Ishigami first order FAST indice " << i << " = " << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << fabs(value - sob_1[i]) << std::endl;
    }
    fullprint << std::endl;
    for(UnsignedInteger i = 0; i < inputDimension; ++i)
    {
      NumericalScalar value(totalOrderFastIndices[i]);
      fullprint << "Ishigami total order FAST indice " << i << " = " << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << fabs(value - sob_T1[i]) << std::endl;
    }
    fullprint << std::endl;
    // // Test with G-Sobol function
    covTh = 1.0;
    NumericalPoint a_i(inputDimension);
    Description formulaGSobol(1);
    formulaGSobol[0] = "1.0";
    for (UnsignedInteger i = 0; i < inputDimension; ++i)
    {
      a_i[i] = 0.5 * i;
      covTh *= 1.0 + 1.0 / (3.0 * pow(1.0 + a_i[i], 2.0));
      formulaGSobol[0] = (OSS() << formulaGSobol[0] << " * ((abs(4.0 * X" << i + 1 << " - 2.0) + " << a_i[i] << ") / (1.0 + " << a_i[i] << "))");
    }
    covTh = covTh - 1.0;
    NumericalMathFunction modelGSobol(inputName, outputName, formulaGSobol);

    marginals[0] = Uniform(0., 1.0);
    marginals[1] = Uniform(0., 1.0);
    marginals[2] = Uniform(0., 1.0);
    distributions = ComposedDistribution(marginals);

    sensitivityFast = FAST(modelGSobol, distributions, 400);
    sensitivityFast.setBlockSize(7);

    // Comparaison with reference analytical values
    firstOrderFastIndices = sensitivityFast.getFirstOrderIndices();
    totalOrderFastIndices = sensitivityFast.getTotalOrderIndices();
    // First-order indices
    NumericalPoint V_i(inputDimension);
    NumericalPoint Vtot_i(inputDimension);
    NumericalScalar prod_V_i(1.0);
    for(UnsignedInteger i = 0; i < inputDimension; ++i)
    {
      V_i[i] = 1.0 / (3.0 * pow(1.0 + a_i[i], 2.0));
      prod_V_i *= V_i[i];
    }
    // Total indices
    Vtot_i[0] = V_i[0] + V_i[0] * V_i[1] + V_i[0] * V_i[2] + prod_V_i;
    Vtot_i[1] = V_i[1] + V_i[0] * V_i[1] + V_i[1] * V_i[2] + prod_V_i;
    Vtot_i[2] = V_i[2] + V_i[0] * V_i[2] + V_i[1] * V_i[2] + prod_V_i;
    // Results
    for(UnsignedInteger i = 0; i < inputDimension; ++i)
    {
      NumericalScalar value(firstOrderFastIndices[i]);
      fullprint << "G-Sobol first order FAST indice " << i << " = " << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << fabs(value - V_i[i]) << std::endl;
    }
    fullprint << std::endl;
    for(UnsignedInteger i = 0; i < inputDimension; ++i)
    {
      NumericalScalar value(totalOrderFastIndices[i]);
      fullprint << "G-Sobol total order FAST indice " << i << " = " << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << fabs(value - Vtot_i[i]) << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
