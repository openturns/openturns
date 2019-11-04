//                                               -*- C++ -*-
/**
 *  @brief The test file of class KarhunenLoeveP1Algorithm
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
    Mesh mesh(IntervalMesher(Indices(1, 9)).build(Interval(-1.0, 1.0)));
    {
      AbsoluteExponential cov1D(Point(1, 1.0));
      KarhunenLoeveP1Algorithm algo(mesh, cov1D, 0.0);
      algo.run();
      KarhunenLoeveResult result(algo.getResult());
      Point lambda(result.getEigenValues());
      ProcessSample KLModes(result.getModesAsProcessSample());
      fullprint << "KL modes=" << KLModes << std::endl;
      fullprint << "KL eigenvalues=" << lambda << std::endl;
      GaussianProcess process(cov1D, KLModes.getMesh());
      Sample coefficients(result.project(process.getSample(10)));
      fullprint << "KL coefficients=" << coefficients << std::endl;
      Basis KLFunctions(result.getModes());
      fullprint << "KL functions=" << KLFunctions.__str__() << std::endl;
      fullprint << "KL lift=" << result.lift(coefficients[0]).__str__() << std::endl;
      fullprint << "KL lift as field=" << result.liftAsField(coefficients[0]) << std::endl;
    }
    {
      CorrelationMatrix R(2);
      R(0, 1) = 0.5;
      Point scale(1, 1.0);
      Point amplitude(2);
      amplitude[0] = 1.0;
      amplitude[1] = 2.0;
      ExponentialModel cov2D(scale, amplitude, R);
      KarhunenLoeveP1Algorithm algo(mesh, cov2D, 0.0);
      algo.run();
      KarhunenLoeveResult result(algo.getResult());
      Point lambda(result.getEigenValues());
      ProcessSample KLModes(result.getModesAsProcessSample());
      fullprint << "KL modes=" << KLModes << std::endl;
      fullprint << "KL eigenvalues=" << lambda << std::endl;
      GaussianProcess process(cov2D, KLModes.getMesh());
      Sample coefficients(result.project(process.getSample(10)));
      fullprint << "KL coefficients=" << coefficients << std::endl;
      Basis KLFunctions(result.getModes());
      fullprint << "KL functions=" << KLFunctions.__str__() << std::endl;
      fullprint << "KL lift=" << result.lift(coefficients[0]).__str__() << std::endl;
      fullprint << "KL lift as field=" << result.liftAsField(coefficients[0]) << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

