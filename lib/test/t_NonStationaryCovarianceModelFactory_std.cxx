//                                               -*- C++ -*-
/**
 *  @brief The test file of class NonStationaryCovarianceModelFactory
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <iostream>
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"
#include "openturns/UserDefinedCovarianceModel.hxx"

using namespace OT;
using namespace OT::Test;


int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    // Dimension of the input model
    // Size of the TimeGrid
    UnsignedInteger size = 10;
    UnsignedInteger dimension = 1;
    UnsignedInteger inputDimension = 1;
    RegularGrid timeGrid(0.0, 0.1, size);
    Point amplitude(dimension, 1.0);
    Point scale(inputDimension, 1.0);
    ExponentialModel model(scale, amplitude);
    GaussianProcess myProcess(model, timeGrid);

    // Create a Process sample of size N
    UnsignedInteger N = 10000;
    ProcessSample sample(myProcess.getSample(N));

    // Factory initiate - No arguments
    NonStationaryCovarianceModelFactory myFactory;

    // Build a UserDefinedCovarianceModel using the Welch method
    UserDefinedCovarianceModel myCovarianceModel(myFactory.buildAsUserDefinedCovarianceModel(sample));

    // Get the frequency grid of the model
    RegularGrid myTimeGrid(myCovarianceModel.getTimeGrid());
    for (UnsignedInteger i = 0 ; i < size ; ++i)
    {
      const Scalar t = timeGrid.getValue(i);
      for (UnsignedInteger j = 0 ; j < size ; ++j)
      {
        const Scalar s = timeGrid.getValue(j);
        const Scalar estimatedValue = myCovarianceModel(t, s)(0, 0);
        const Scalar modelValue = model(t, s)(0, 0);
        fullprint << "Covariance C(" << t << ", " << s << ") : ";
        fullprint << " evaluation = " << estimatedValue << " model = " << modelValue << std::endl;
      }
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
