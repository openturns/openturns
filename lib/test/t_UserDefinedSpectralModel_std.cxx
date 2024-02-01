//                                               -*- C++ -*-
/**
 *  @brief The test file of class UserDefinedSpectralModel
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

Scalar clean(Scalar in)
{
  // For -0.0 trouble
  if (std::abs(in) < 1.e-6) return 0.0;
  return in;
  //return 1.e-6*round(1.e6*in);
}

HermitianMatrix clean(HermitianMatrix in)
{
  UnsignedInteger dim = in.getDimension();
  for(UnsignedInteger i = 0; i < dim; i++)
    for(UnsignedInteger j = 0; j <= i; j++)
    {
      Scalar realIJ = clean(real(in(i, j)));
      Scalar imagIJ = clean(imag(in(i, j)));
      in(i, j) = Complex(realIJ, imagIJ);
    }
  return in;
}

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {

    /* Default constructor */
    UserDefinedSpectralModel myDefaultModel;
    fullprint << "myDefaultModel = " << myDefaultModel << std::endl;

    /* Default dimension parameter to evaluate the model */
    const UnsignedInteger dimension = 2;

    /* Amplitude values */
    Point amplitude(dimension);
    /* Scale values */
    Point scale(dimension);
    /* Spatial correclation */
    CorrelationMatrix spatialCorrelation(dimension);
    for (UnsignedInteger index = 0 ; index < dimension; ++index)
    {
      // constant amplitude
      amplitude[index] = 1.0 ;
      scale[index] = (index + 1.0) / dimension ;
      if (index > 0) spatialCorrelation(index, index - 1) = 1.0 / index;
    }

    /* Sample a CauchyModel */
    CauchyModel referenceModel(scale, amplitude, spatialCorrelation);

    UnsignedInteger size = 5;
    UserDefinedSpectralModel::HermitianMatrixCollection dspCollection(size);
    RegularGrid frequencyGrid(0.0, 2.0 / size, size);
    for (UnsignedInteger i = 0; i < size; ++i)
      dspCollection[i] = referenceModel(frequencyGrid.getValue(i));

    /* Create a UserDefinedSpectralModel */
    UserDefinedSpectralModel myModel(frequencyGrid, dspCollection);
    fullprint << "myModel=" << myModel << std::endl;

    /* Sample the UserDefinedSpectralModel */
    RegularGrid samplingGrid(-0.4, 1.0 / 16, 5 * size);
    for (UnsignedInteger i = 0; i < samplingGrid.getN(); ++i)
    {
      Scalar frequency = samplingGrid.getValue(i);
      fullprint << "frequency=" << clean(frequency) << ", myModel=\n" << clean(myModel(frequency)) << ", referenceModel=\n" << clean(referenceModel(frequency)) << std::endl;
    }

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
