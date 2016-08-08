//                                               -*- C++ -*-
/**
 *  @brief The test file of class ExponentialCauchy
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
    const UnsignedInteger defaultDimension = 1;

    /* Default spaatial dimension parameter */
    const UnsignedInteger spatialDimension = 1;
    /* Amplitude values */
    NumericalPoint amplitude(defaultDimension, 1.0);
    /* Scale values */
    NumericalPoint scale(spatialDimension, 1.0);


    /* Default constructor */
    ExponentialCauchy myDefautModel;
    fullprint << "myDefautModel = " << myDefautModel << std::endl;

    /* Second order model with parameters */
    ExponentialCauchy myModel(amplitude, scale);
    fullprint << "myModel = " << myModel << std::endl;

    const NumericalScalar frequencyValueOne = 1.0;
    fullprint << "spectral density matrix at f = " << frequencyValueOne << " : " << myModel.computeSpectralDensity(frequencyValueOne) << std::endl;
    fullprint << "spectral density matrix at f = " << -frequencyValueOne << " : " << myModel.computeSpectralDensity(-frequencyValueOne) << std::endl;

    const NumericalScalar frequencyValueHigh = 4.0;
    fullprint << "spectral density matrix at f = " << frequencyValueHigh << " : " << myModel.computeSpectralDensity(frequencyValueHigh) << std::endl;
    fullprint << "spectral density matrix at f = " << -frequencyValueHigh << " : " << myModel.computeSpectralDensity(-frequencyValueHigh) << std::endl;

    const NumericalScalar timeValueOne = 1.0;
    fullprint << "covariance matrix at t = " << timeValueOne << " : " << myModel.computeCovariance(timeValueOne) << std::endl;
    fullprint << "covariance matrix at t = " << -timeValueOne << " : " << myModel.computeCovariance(-timeValueOne) << std::endl;

    /* Evaluation at time higher to check the decrease of the exponential values */
    const NumericalScalar timeValueHigh = 4.0;
    fullprint << "covariance matrix at t = "  << timeValueHigh << " : " << myModel.computeCovariance(timeValueHigh) << std::endl;

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
