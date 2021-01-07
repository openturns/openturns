//                                               -*- C++ -*-
/**
 *  @brief The test file of class BoxCoxFactory for standard methods using GLM
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
  setRandomGenerator();

  try
  {
    const UnsignedInteger size = 200;

    // input sample
    const Sample inputSample = Uniform(-1.0, 1.0).getSample(size);
    Sample outputSample(inputSample);

    // Evaluation of y = ax + b (a: scale, b: translate)

    // scale
    const Point scale(1, 3.0);
    outputSample *= scale;

    // translate sample
    const Point translate(1, 3.1);
    outputSample += translate;

    // Finally inverse transform using an arbitrary lambda
    const Point lambda(1, 1.8);
    const InverseBoxCoxEvaluation  boxCoxFunction(lambda);

    // transform y using BoxCox function
    outputSample = boxCoxFunction(outputSample);

    // Add small noise
    const Sample epsilon = Normal(0, 1e-2).getSample(size);
    outputSample +=  epsilon;
    // Now we build the factory
    BoxCoxFactory factory;

    // Creation of the BoxCoxTransform
    GeneralLinearModelResult result;
    const Basis basis = LinearBasisFactory(1).build();
    const DiracCovarianceModel covarianceModel;
    const Point shift(1, 1.0e-1);
    BoxCoxTransform myBoxCox = factory.build(inputSample, outputSample, covarianceModel, basis, shift, result);

    fullprint << "myBoxCox (GLM)=" << myBoxCox << std::endl;
    fullprint << "GLM result=" << result << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
