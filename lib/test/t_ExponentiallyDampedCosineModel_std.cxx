//                                               -*- C++ -*-
/**
 *  @brief The test file of class ExponentiallyDampedCosineModel
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
#include "OT.hxx"
#include "OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    /* Spatial dimension of the model */
    UnsignedInteger spatialDimension(1);
    /* Amplitude values */
    NumericalScalar amplitude(1.0);
    /* Scale values */
    NumericalScalar scale(1.0);
    /* Frequency values */
    NumericalScalar frequency(0.1);

    /* Default constructor */
    ExponentiallyDampedCosineModel myDefautModel;
    fullprint << "myDefautModel = " << myDefautModel << std::endl;

    /* Second order model with parameters */
    ExponentiallyDampedCosineModel myModel(spatialDimension, amplitude, scale, frequency);
    fullprint << "myModel = " << myModel << std::endl;

    const NumericalScalar timeValueOne(1.0);
    fullprint << "covariance matrix at t = " << timeValueOne << " : " << myModel(timeValueOne) << std::endl;
    fullprint << "covariance matrix at t = " << -1.0 * timeValueOne << " : " << myModel(-1.0 * timeValueOne) << std::endl;

    /* Evaluation at time higher to check the decrease of the exponentiallyDampedCosine values */
    const NumericalScalar timeValueHigh(4.0);
    fullprint << "covariance matrix at t = "  << timeValueHigh << " : " << myModel(timeValueHigh) << std::endl;

    /* Discretize the process on a small time grid */
    RegularGrid timeGrid(0.0, 1.0 / 3.0, 4);
    fullprint << "discretized covariance over the time grid=" << timeGrid << " is" << std::endl;
    fullprint << myModel.discretize(timeGrid) << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
