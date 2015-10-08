//                                               -*- C++ -*-
/**
 *  @brief The test file of class Spectral Normal Process
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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

    RandomGenerator::SetSeed(0);

    /* Default dimension parameter to evaluate the model */
    const UnsignedInteger defaultDimension(1);

    /* Amplitude values */
    NumericalPoint amplitude(defaultDimension, 1.0);
    /* Scale values */
    NumericalPoint scale(defaultDimension, 1.0);

    /* Second order model with parameters */
    ExponentialCauchy myModel(amplitude, scale);

    /* checking the copy-cast*/
    SecondOrderModel mySecondOrderModel(myModel);

    const UnsignedInteger points(8);
    const NumericalScalar tMin(0.0);
    const NumericalScalar tStep(1.0 / (points - 1));

    // RegularGrid --> Build list of frequencies using the RegularGrid
    RegularGrid myTimeGrid(tMin, tStep, points);

    SpectralNormalProcess mySpectralProcess0(myModel, myTimeGrid);

    fullprint << "mySpectralProcess0 = " << mySpectralProcess0.__str__() << std::endl;
    fullprint << "Realization = " << mySpectralProcess0.getRealization().__str__() << std::endl;

    // Constructor using maximalFrequency value and size of discretization
    const NumericalScalar maximalFrequency(10.0);
    SpectralNormalProcess mySpectralProcess1(myModel, maximalFrequency, points);
    RegularGrid tg(mySpectralProcess1.getTimeGrid());

    fullprint << "mySpectralProcess1 = " << mySpectralProcess1.__str__() << std::endl;
    fullprint << "Realization = " << mySpectralProcess1.getRealization().__str__() << std::endl;

    /* Second order model with parameters */
    const UnsignedInteger highDimension(3);
    amplitude.resize(highDimension);
    for (UnsignedInteger k = 0; k < highDimension; ++k) amplitude[k] = 1.0;
    CauchyModel mySpecModel(amplitude, scale);
    fullprint << "mySpecModel = " << mySpecModel << std::endl;

    SpectralNormalProcess mySpectralProcess2(mySpecModel, myTimeGrid);
    fullprint << "mySpectralProcess2 = " << mySpectralProcess2.__str__() << std::endl;
    fullprint << "Realization = " << mySpectralProcess2.getRealization().__str__() << std::endl;
    SpectralNormalProcess mySpectralProcess3(mySpecModel, maximalFrequency, points);
    fullprint << "mySpectralProcess3 = " << mySpectralProcess3.__str__() << std::endl;
    fullprint << "Realization = " << mySpectralProcess3.getRealization().__str__() << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
