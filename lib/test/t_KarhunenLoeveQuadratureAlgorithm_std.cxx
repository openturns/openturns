//                                               -*- C++ -*-
/**
 *  @brief The test file of class KarhunenLoeveQuadratureAlgorithm
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
#include <cmath>

using namespace OT;
using namespace OT::Test;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    ResourceMap::SetAsScalar("LinearCombinationEvaluation-SmallCoefficient", 1.0e-10);
    UnsignedInteger dim = 1;
    Interval domain(Point(dim, -1.0), Point(dim, 1.0));
    UnsignedInteger basisSize = 5;
    OrthogonalProductPolynomialFactory basis(Collection<OrthogonalUniVariatePolynomialFamily>(dim, LegendreFactory()));
    Collection<Function> functions(basisSize);
    for(UnsignedInteger index = 0; index < basisSize; ++index)
      functions[index] = basis.build(index);
    LHSExperiment experiment(basis.getMeasure(), 100);
    Bool mustScale = false;
    Scalar threshold = 0.0001;
    AbsoluteExponential model(Point(dim, 1.0));
    KarhunenLoeveQuadratureAlgorithm algo(domain, domain, model, experiment, functions, mustScale, threshold);
    algo.run();
    KarhunenLoeveResult result(algo.getResult());
    Point lambda(result.getEigenValues());
    ProcessSample KLModes(result.getModesAsProcessSample());
    fullprint << "KL modes=" << KLModes << std::endl;
    fullprint << "KL eigenvalues=" << lambda << std::endl;
    GaussianProcess process(model, KLModes.getMesh());
    ProcessSample sample(process.getSample(10));
    Sample coefficients(result.project(sample));
    fullprint << "KL coefficients=" << coefficients << std::endl;
    Collection<Function> KLFunctions(result.getModes());
    fullprint << "KL functions=" << KLFunctions.__str__() << std::endl;
    fullprint << "KL lift=" << result.lift(coefficients[0]).__str__() << std::endl;
    fullprint << "KL lift as field=" << result.liftAsField(coefficients[0]) << std::endl;
    // Now using Legendre/Gauss quadrature
    {
      UnsignedInteger marginalDegree = 5;
      algo = KarhunenLoeveQuadratureAlgorithm(domain, domain, model, marginalDegree, threshold);
      algo.run();
      result = algo.getResult();
      lambda = result.getEigenValues();
      KLModes = result.getModesAsProcessSample();
      // Due to symmetry many results can have a sign switch depending on the CPU/compiler/BLAS used
      // fullprint << "KL modes=" << KLModes << std::endl;
      fullprint << "KL eigenvalues=" << lambda << std::endl;
      coefficients = result.project(sample);
      // fullprint << "KL coefficients=" << coefficients << std::endl;
      KLFunctions = result.getModes();
      // fullprint << "KL functions=" << KLFunctions.__str__() << std::endl;
      Function lifted(result.lift(coefficients[0]));
      // fullprint << "KL lift=" << lifted.__str__() << std::endl;
      Field liftedAsField(result.liftAsField(coefficients[0]));
      // fullprint << "KL lift as field=" << liftedAsField << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
