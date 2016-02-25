//                                               -*- C++ -*-
/**
 *  @brief The test file of class GaussKronrod
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
    Mesh mesh(IntervalMesher(Indices(1, 9)).build(Interval(-1.0, 1.0)));
    KarhunenLoeveP1Factory factory(mesh, 0.0);
    NumericalPoint lambda;
    ProcessSample KLModes(factory.buildAsProcessSample(AbsoluteExponential(1, 1.0), lambda));
    fullprint << "KL modes=" << KLModes << std::endl;
    fullprint << "KL eigenvalues=" << lambda << std::endl;
    AbsoluteExponential cov1D(1, 1.0);
    Basis KLFunctions(factory.build(cov1D, lambda));
    fullprint << "KL functions=" << KLFunctions << std::endl;
    fullprint << "KL eigenvalues=" << lambda << std::endl;
    CorrelationMatrix R(2);
    R(0, 1) = 0.5;
    NumericalPoint scale(1, 1.0);
    NumericalPoint amplitude(2);
    amplitude[0] = 1.0;
    amplitude[1] = 2.0;
    ExponentialModel cov2D(1, amplitude, scale, R);
    KLFunctions = factory.build(cov2D, lambda);
    fullprint << "KL functions=" << KLFunctions << std::endl;
    fullprint << "KL eigenvalues=" << lambda << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
