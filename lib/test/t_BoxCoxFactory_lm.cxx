//                                               -*- C++ -*-
/**
 *  @brief The test file of class BoxCoxFactory for standard methods using GLM
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
  setRandomGenerator();

  try
  {
    const UnsignedInteger size = 200;

    // input sample
    const Sample inputSample = Uniform(-1.0, 1.0).getSample(size);
    Sample outputSample(inputSample);

    // Evaluation of y = ax + b (a: scale, b: translate)
    // scale
    const Point scale = {3.0};
    outputSample *= scale;

    // translate sample
    const Point translate = {10.0};
    outputSample += translate;

    // Finally inverse transform using an arbitrary lambda
    const Point lambda = {2.0};
    const InverseBoxCoxEvaluation boxCoxFunction(lambda);

    // transform y using BoxCox function
    outputSample = boxCoxFunction(outputSample);

    // Add small noise
    const Sample epsilon = Normal(0, 1e-3).getSample(size);
    outputSample +=  epsilon;
    // Now we build the factory
    BoxCoxFactory factory;

    // Creation of the BoxCoxTransform
    LinearModelResult result;
    const Point shift = {1e-10};
    BoxCoxTransform myBoxCox = factory.buildWithLM(inputSample, outputSample, shift, result);
    // estimated lambda =  1.99098;
    // beta = [9.90054,2.95995]
    assert_almost_equal(myBoxCox.getLambda(), lambda, 1e-2, 1e-2);
    const Point trendCoefficients = {9.90054, 2.95995};
    assert_almost_equal(result.getCoefficients(), trendCoefficients, 1e-3, 1e-3);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
