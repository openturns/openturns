//                                               -*- C++ -*-
/**
 *  @brief The test file of class KarhunenLoeveQuadratureAlgorithm
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
#include <cmath>

using namespace OT;
using namespace OT::Test;

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    ResourceMap::SetAsScalar("LinearCombinationEvaluation-SmallCoefficient", 1.0e-10);
    UnsignedInteger dim = 1;
    Interval domain(Point(dim, -1.0), Point(dim, 1.0));
    OrthogonalProductPolynomialFactory basis(Collection<OrthogonalUniVariatePolynomialFamily>(dim, LegendreFactory()));
    UnsignedInteger basisSize = 5;
    LHSExperiment experiment(basis.getMeasure(), 100);
    Bool mustScale = false;
    Scalar threshold = 0.0001;
    AbsoluteExponential model(Point(dim, 1.0));
    KarhunenLoeveQuadratureAlgorithm algo(domain, domain, model, experiment, basis, basisSize, mustScale, threshold);
    algo.run();
    KarhunenLoeveResult result(algo.getResult());
    Point lambda(result.getEigenValues());
    FieldSample KLModes(result.getModesAsProcessSample());
    fullprint << "KL modes=" << KLModes << std::endl;
    fullprint << "KL eigenvalues=" << lambda << std::endl;
    GaussianProcess process(model, KLModes.getMesh());
    FieldSample sample(process.getSample(10));
    Sample coefficients(result.project(sample));
    fullprint << "KL coefficients=" << coefficients << std::endl;
    Basis KLFunctions(result.getModes());
    fullprint << "KL functions=" << KLFunctions.__str__() << std::endl;
    fullprint << "KL lift=" << result.lift(coefficients[0]).__str__() << std::endl;
    fullprint << "KL lift as field=" << result.liftAsField(coefficients[0]) << std::endl;
    // Now using Legendre/Gauss quadrature
    {
      UnsignedInteger marginalDegree = 5;
      KarhunenLoeveQuadratureAlgorithm algo2(domain, domain, model, marginalDegree, threshold);
      algo2.run();
      KarhunenLoeveResult result(algo2.getResult());
      Point lambda(result.getEigenValues());
      FieldSample KLModes(result.getModesAsProcessSample());
      // Due to symmetry many results can have a sign switch depending on the CPU/compiler/BLAS used
      // fullprint << "KL modes=" << KLModes << std::endl;
      fullprint << "KL eigenvalues=" << lambda << std::endl;
      Sample coefficients(result.project(sample));
      // fullprint << "KL coefficients=" << coefficients << std::endl;
      Basis KLFunctions(result.getModes());
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
