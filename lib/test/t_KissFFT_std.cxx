//                                               -*- C++ -*-
/**
 *  @brief The test file of class KissFFT
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

typedef Collection<Complex> ComplexCollection;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {

    // for fft, the best implementation is given for N = 2^p
    const UnsignedInteger size = 16;

    // collection for test
    ComplexCollection collection(size);

    // Fill the data with artificial values

    // Create a complex gaussian sample
    for (UnsignedInteger index = 0; index < size; ++index)
    {
      const Scalar realPart = 0.1 * (1.0 + index) / size;
      const Scalar imagPart = 0.3 * (1.0 + index) / size;
      collection[index] = Complex(realPart, imagPart);
    }

    /* Instantiation of FFT class */
    KissFFT myFFT;
    fullprint << "myFFT = " << myFFT << std::endl;

    // Initial transformation
    fullprint << "collection = " << collection << std::endl;

    // FFT transform
    const ComplexCollection transformedCollection(myFFT.transform(collection));
    fullprint << "FFT result = " << transformedCollection << std::endl;

    // Inverse transformation
    const ComplexCollection inverseTransformedCollection(myFFT.inverseTransform(transformedCollection));
    fullprint << "FFT back=" << inverseTransformedCollection << std::endl;

    const Scalar threshold = 1e-14;

    // 2D case now
    const UnsignedInteger N = 8;
    Normal distribution(N);
    Sample sample(distribution.getSample(2 * N));

    // FFT transform
    const ComplexMatrix transformedSample(myFFT.transform2D(sample));
    fullprint << "2D FFT result = " << transformedSample.clean(threshold) << std::endl;

    // Inverse transformation
    const ComplexMatrix inverseTransformedSample(myFFT.inverseTransform2D(transformedSample));
    fullprint << "2D FFT back=" << inverseTransformedSample.clean(threshold) << std::endl;

    // 3D case
    Tensor tensor(N, N, N);
    for(UnsignedInteger k = 0; k < N; ++k)
      for (UnsignedInteger j = 0; j < N; ++j)
        for (UnsignedInteger i = 0; i < N; ++i)
          tensor(i, j, k) = RandomGenerator::Generate();

    // FFT transform
    const ComplexTensor transformedTensor(myFFT.transform3D(tensor));
    fullprint << "3D FFT result = " << transformedTensor.clean(threshold) << std::endl;

    // Inverse transformation
    const ComplexTensor inverseTransformedTensor(myFFT.inverseTransform3D(transformedTensor));
    fullprint << "3D FFT back=" << inverseTransformedTensor.clean(threshold) << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
