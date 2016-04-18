//                                               -*- C++ -*-
/**
 *  @brief The test file of class SecondOrderModel
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

using namespace OT;
using namespace OT::Test;

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {

    /* Default dimension parameter to evaluate the model */
    const UnsignedInteger dimension = 1;

    /* Spatial dimension of the the model */
    const UnsignedInteger spatialDimension = 1;

    /* Amplitude values */
    NumericalPoint amplitude(dimension, 1.0);
    /* Scale values */
    NumericalPoint scale(dimension, 1.0);

    /* Covariance model */
    ExponentialModel myCovarianceModel(spatialDimension, amplitude, scale);
    fullprint << "myCovarianceModel = " << myCovarianceModel << std::endl;

    /* Spectral model  */
    CauchyModel mySpectralModel(amplitude, scale);
    fullprint << "mySpectralModel = " << mySpectralModel << std::endl;

    /* We build the second order model using covariance and spectral models */
    SecondOrderModel myModel(myCovarianceModel, mySpectralModel);
    fullprint << "myModel = " << myModel << std::endl;

    /* Some computations ==> call the sub models methods */
    const NumericalScalar instant = 1.0;
    const NumericalScalar frequency = 0.5;
    fullprint << "covariance matrix at t = " << instant << " : " << myModel.computeCovariance(instant) << std::endl;
    fullprint << "covariance matrix at t = " << -instant << " : " << myModel.computeCovariance(-instant) << std::endl;
    fullprint << "spectral density at f = " << frequency << " : " << myModel.computeSpectralDensity(frequency) << std::endl;

    /* Discretize the process on a small time grid */
    RegularGrid timeGrid(0.0, 1.0, 11);
    fullprint << "discretized covariance over the time grid = " << timeGrid << " is " <<  myModel.discretize(timeGrid) << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
