//                                               -*- C++ -*-
/**
 *  @brief The test file of class LinearModelAlgorithm
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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

int main(int argc, char *argv[])
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
    LinearModelAnalysis analysis(result);
    fullprint << analysis.__str__() << std::endl;
    // Compute confidence level (95%) for coefficients estimate (1-alpha = 0.95)
    const Scalar alpha = 0.05;
    const Scalar t = Student(analysis.getDegreesOfFreedom()).computeQuantile(alpha / 2.0, true)[0];
    // Transform errors as Point
    Point coefficientsErrors(analysis.getCoefficientsStandardErrors().getImplementation()->getData());
    // Define lower and upper bounds
    // First transform return type into NP
    Point lowerBounds(analysis.getCoefficientsEstimates().getImplementation()->getData());
    Point upperBounds(analysis.getCoefficientsEstimates().getImplementation()->getData());
    // update bounds
    lowerBounds -= coefficientsErrors * t;
    upperBounds += coefficientsErrors * t;
    // Finally define the interval
    const Interval interval(lowerBounds, upperBounds);
    fullprint << "Confidence intervals with level=" << 1 - alpha << " : "  << interval << std::endl;
  }

  {
    setRandomGenerator();
    fullprint << std::endl;
    fullprint << std::endl;
    fullprint << std::endl;
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
    LinearModelAnalysis analysis(result);
    fullprint << analysis.__str__() << std::endl;
    // Compute confidence level (95%) for coefficients estimate (1-alpha = 0.95)
    const Scalar alpha = 0.05;
    const Scalar t = Student(analysis.getDegreesOfFreedom()).computeQuantile(alpha / 2.0, true)[0];
    // Transform errors as Point
    Point coefficientsErrors(analysis.getCoefficientsStandardErrors().getImplementation()->getData());
    // Define lower and upper bounds
    // First transform return type into NP
    Point lowerBounds(analysis.getCoefficientsEstimates().getImplementation()->getData());
    Point upperBounds(analysis.getCoefficientsEstimates().getImplementation()->getData());
    // update bounds
    lowerBounds -= coefficientsErrors * t;
    upperBounds += coefficientsErrors * t;
    // Finally define the interval
    const Interval interval(lowerBounds, upperBounds);
    fullprint << "Confidence intervals with level=" << 1 - alpha << " : "  << interval << std::endl;
  }

  return ExitCode::Success;

}
