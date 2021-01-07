//                                               -*- C++ -*-
/**
 *  @brief The test file of class CauchyModel
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

  try
  {

    /* Default dimension parameter to evaluate the model */
    const UnsignedInteger defaultDimension = 1;

    /* Default input dimension parameter  */
    const UnsignedInteger inputDimension = 1;

    /* Amplitude values */
    Point amplitude(defaultDimension, 1.0);
    /* Scale values */
    Point scale(inputDimension, 1.0);


    /* Default constructor */
    CauchyModel myDefautModel;
    fullprint << "myDefautModel = " << myDefautModel << std::endl;

    /* Second order model with parameters */
    CauchyModel myModel(scale, amplitude);
    fullprint << "myModel = " << myModel << std::endl;

    const Scalar frequencyValue = 1.0;
    fullprint << "spectral density matrix at f = " << frequencyValue << " : " << myModel(frequencyValue) << std::endl;

    /* Evaluation at time higher to check the decrease of the cauchy values */
    const Scalar frequencyValueHigh = 10.0;
    fullprint << "spectral density matrix at f = " << frequencyValueHigh << " : " << myModel(frequencyValueHigh) << std::endl;

    /* Default dimension parameter to evaluate the model */
    const UnsignedInteger highDimension = 3;

    /* Reallocation of adequate sizes*/
    amplitude.resize(highDimension);

    CorrelationMatrix spatialCorrelation(highDimension);
    for (UnsignedInteger index = 0 ; index < highDimension; ++index)
    {
      // amplitude
      amplitude[index] = (index + 1.0) / defaultDimension ;
      if (index > 0) spatialCorrelation(index, index - 1) = 1.0 / index;
    }
    fullprint << "spatialCorrelation=\n" << spatialCorrelation << std::endl;
    fullprint << "spatialCorrelation=\n" << spatialCorrelation.__str__() << std::endl;

    /* checking the cast */
    SpectralModel mySpectralModel(CauchyModel(scale, amplitude, spatialCorrelation));
    fullprint << "mySpectralModel = " << mySpectralModel << std::endl;

    /* Second order model - high dimension */
    CauchyModel myHighModel(scale, amplitude, spatialCorrelation);
    fullprint << "myHighModel = " << myHighModel << std::endl;

    fullprint << "spectral density matrix at f = " << frequencyValue << " : " << myHighModel(frequencyValue).clean(1.0e-6) << std::endl;
    fullprint << "spectral density matrix at f = " << frequencyValue << " :\n" << myHighModel(frequencyValue).clean(1.0e-6).__str__() << std::endl;
    fullprint << "spectral density matrix at f = "  << frequencyValueHigh << " : " << myHighModel(frequencyValueHigh).clean(1.0e-6) << std::endl;
    fullprint << "spectral density matrix at f = "  << frequencyValueHigh << " :\n" << myHighModel(frequencyValueHigh).clean(1.0e-6).__str__() << std::endl;

    CovarianceMatrix spatialCovariance(highDimension);
    for (UnsignedInteger index = 0 ; index < highDimension; ++index)
    {
      spatialCovariance(index, index) = 1.0;
      if (index > 0) spatialCovariance(index, index - 1) = 1.0 / index;
    }

    /* constructor based on spatial covariance */
    CauchyModel myHighModelCovariance(scale, spatialCovariance);
    fullprint << "myHighModelCovariance = " << myHighModelCovariance << std::endl;

    fullprint << "spectral density matrix at f = " << frequencyValue << " : " << myHighModelCovariance(frequencyValue).clean(1.0e-6) << std::endl;
    fullprint << "spectral density matrix at f = " << frequencyValue << " :\n" << myHighModelCovariance(frequencyValue).clean(1.0e-6).__str__() << std::endl;
    fullprint << "spectral density matrix at f = "  << frequencyValueHigh << " : " << myHighModelCovariance(frequencyValueHigh).clean(1.0e-6) << std::endl;
    fullprint << "spectral density matrix at f = "  << frequencyValueHigh << " :\n" << myHighModelCovariance(frequencyValueHigh).clean(1.0e-6).__str__() << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
