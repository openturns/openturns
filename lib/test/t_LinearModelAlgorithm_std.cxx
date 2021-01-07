//                                               -*- C++ -*-
/**
 *  @brief The test file of class LinearModelAlgorithm
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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

  {
    setRandomGenerator();
    fullprint << "Fit y ~ 3 - 2 x + 0.05 * sin(x) model using 20 points (sin(x) ~ noise)" << std::endl;
    UnsignedInteger size = 20;
    Sample oneSample(size, 1);
    Sample twoSample(size, 1);
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      oneSample[i][0] = 7.0 * sin(-3.5 + (6.5 * i) / (size - 1.0)) + 2.0;
      twoSample[i][0] = -2.0 * oneSample[i][0] + 3.0 + 0.05 * sin(oneSample[i][0]);
    }
    LinearModelAlgorithm test(oneSample, twoSample);
    LinearModelResult result(test.getResult());
    fullprint << "trend coefficients = " << result.getCoefficients() << std::endl;
  }

  {
    setRandomGenerator();
    fullprint << "Fit y ~ 1 + 0.1 x + 10 x^2 model using 100 points" << std::endl;
    UnsignedInteger size = 100;
    // Define a linespace from 0 to 10 with size points
    // We use a Box expermient ==> remove 0 & 1 points
    const Box experiment(Indices(1, size - 2));
    Sample X(experiment.generate());
    // X is defined in [0,1]
    const Point scale(1, 10.0);
    X *= scale;
    // Stack X2
    Sample X2(X);
    for (UnsignedInteger i = 0; i < size; ++i)
      X2(i, 0) = X(i, 0) * X(i, 0);
    // Stack
    X.stack(X2);
    // Define y = 1 + 0.1 * x + 10  x^2 + e with e a gaussian noise
    Sample Y(size, 1);
    for (UnsignedInteger i = 0; i < size; ++i)
      Y(i, 0) = 1.0 +  0.1 * X(i, 0) + 10.0 * X(i, 0) * X(i, 0) + 0.1 * DistFunc::rNormal() ;
    LinearModelAlgorithm test(X, Y);
    LinearModelResult result(test.getResult());
    fullprint << "trend coefficients = " << result.getCoefficients() << std::endl;
  }

  return ExitCode::Success;

}
