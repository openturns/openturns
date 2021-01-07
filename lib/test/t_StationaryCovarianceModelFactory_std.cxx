//                                               -*- C++ -*-
/**
 *  @brief The test file of class StationaryCovarianceModelFactory
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
#include <iostream>
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"
#include "openturns/CovarianceModel.hxx"

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
    const UnsignedInteger size = 128;
    const UnsignedInteger dimension = 1;
    const RegularGrid timeGrid(0.0, 0.1, size);
    const Point amplitude(dimension, 1.0);
    const Point scale(dimension, 1.0);
    CauchyModel model(scale, amplitude);
    AbsoluteExponential covModel(scale, amplitude);
    SpectralGaussianProcess myProcess(model, timeGrid);

    // Create a sample of time series
    const UnsignedInteger N = 1000;
    ProcessSample sample(myProcess.getSample(N));

    // Create the spectral density factory
    WelchFactory spectralFactory;

    // Covariance Factory creation
    StationaryCovarianceModelFactory myFactory(spectralFactory);

    // Build a CovarianceModel using the Welch method
    UserDefinedStationaryCovarianceModel myCovarianceModel(myFactory.buildAsUserDefinedStationaryCovarianceModel(sample));

    // Get the time grid of the model
    RegularGrid myTimeGrid(myCovarianceModel.getTimeGrid());
    for (UnsignedInteger i = 0 ; i < myTimeGrid.getN() ; ++i)
    {
      const Scalar t = myTimeGrid.getValue(i);
      const Scalar estimatedValue = myCovarianceModel(t)(0, 0);
      const Scalar modelValue = covModel(t)(0, 0);
      fullprint << "Covariance C(" << t << ") : ";
      fullprint << " evaluation = " << estimatedValue << " model = " << modelValue << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
