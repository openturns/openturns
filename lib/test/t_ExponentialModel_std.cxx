//                                               -*- C++ -*-
/**
 *  @brief The test file of class ExponentialModel
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
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {

    /* Default dimension parameter to evaluate the model */
    const UnsignedInteger defaultDimension = 1;

    /* Spatial dimension of the model */
    const UnsignedInteger inputDimension = 1;

    /* Amplitude values */
    Point amplitude(defaultDimension, 2.0);
    /* Scale values */
    Point scale(inputDimension, 1.0);


    /* Default constructor */
    ExponentialModel myDefaultModel;
    fullprint << "myDefaultModel = " << myDefaultModel << std::endl;

    /* Second order model with parameters */
    ExponentialModel myModel(scale, amplitude);
    fullprint << "myModel = " << myModel << std::endl;

    const Scalar timeValueOne = 1.0;
    fullprint << "covariance matrix at t = " << timeValueOne << " : " << myModel(timeValueOne) << std::endl;
    fullprint << "covariance matrix at t = " << -1.0 * timeValueOne << " : " << myModel(-1.0 * timeValueOne) << std::endl;

    /* Evaluation at time higher to check the decrease of the exponential values */
    const Scalar timeValueHigh = 4.0;
    fullprint << "covariance matrix at t = "  << timeValueHigh << " : " << myModel(timeValueHigh) << std::endl;

    /* Discretize the process on a small time grid */
    RegularGrid timeGrid(0.0, 1.0 / 3.0, 4);
    fullprint << "discretized covariance over the time grid=" << timeGrid << " is" << std::endl;
    fullprint << myModel.discretize(timeGrid) << std::endl;
    /* Default dimension parameter to evaluate the model */
    const UnsignedInteger highDimension = 3;

    /* Reallocation of adequate sizes*/
    amplitude.resize(highDimension);

    CorrelationMatrix spatialCorrelation(highDimension);
    for (UnsignedInteger index = 0 ; index < highDimension; ++index)
    {
      // constant amplitude
      amplitude[index] = 1.0 ;
      if (index > 0) spatialCorrelation(index, index - 1) = 1.0 / index;
    }
    fullprint << "spatialCorrelation=" << spatialCorrelation << std::endl;

    /* Second order model - high dimension */
    ExponentialModel myHighModel(scale, amplitude, spatialCorrelation);
    fullprint << "myHighModel = " << myHighModel << std::endl;

    fullprint << "covariance matrix at t = " << timeValueOne << " : " << myHighModel(timeValueOne) << std::endl;
    fullprint << "covariance matrix at t = " << -timeValueOne << " : " << myHighModel(-timeValueOne) << std::endl;
    fullprint << "covariance matrix at t = "  << timeValueHigh << " : " << myHighModel(timeValueHigh) << std::endl;

    fullprint << "discretized covariance over the time grid=" << timeGrid << " is" << std::endl;
    fullprint << myHighModel.discretize(timeGrid) << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
