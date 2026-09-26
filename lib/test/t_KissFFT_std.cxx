//                                               -*- C++ -*-
/**
 *  @brief The test file of class KissFFT
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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

  try
  {
    RandomGenerator::SetSeed(0);

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

    // 1D case
    const ComplexCollection transformedCollection(myFFT.transform(collection));
    const ComplexCollection inverseTransformedCollection(myFFT.inverseTransform(transformedCollection));
    for (UnsignedInteger i = 0; i < collection.getSize(); ++i)
      assert_almost_equal(inverseTransformedCollection[i], collection[i]);

    // 2D ND case
    const UnsignedInteger N = 8;
    Normal distribution(N);
    Sample sample(distribution.getSample(2 * N));

    // FFT transform via ND
    const UnsignedInteger rows = sample.getSize();
    const UnsignedInteger cols = sample.getDimension();
    ComplexCollection sampleFlat(rows * cols);
    for (UnsignedInteger j = 0; j < cols; ++j)
      for (UnsignedInteger i = 0; i < rows; ++i)
        sampleFlat[i + j * rows] = Complex(sample(i, j), 0.0);
    const ComplexCollection transformedSample(myFFT.transform(sampleFlat, {rows, cols}));

    // Inverse transformation
    const ComplexCollection inverseTransformedSample(myFFT.inverseTransform(transformedSample, {rows, cols}));
    for (UnsignedInteger i = 0; i < sampleFlat.getSize(); ++i)
      assert_almost_equal(inverseTransformedSample[i], sampleFlat[i]);

    // 3D ND case
    Tensor tensor(N, N, N);
    for(UnsignedInteger k = 0; k < N; ++k)
      for (UnsignedInteger j = 0; j < N; ++j)
        for (UnsignedInteger i = 0; i < N; ++i)
          tensor(i, j, k) = RandomGenerator::Generate();

    // FFT transform via ND
    ComplexCollection tensorFlat(N * N * N);
    for (UnsignedInteger k = 0; k < N; ++k)
      for (UnsignedInteger j = 0; j < N; ++j)
        for (UnsignedInteger i = 0; i < N; ++i)
          tensorFlat[i + j * N + k * N * N] = Complex(tensor(i, j, k), 0.0);
    const ComplexCollection transformedTensor(myFFT.transform(tensorFlat, {N, N, N}));

    // Inverse transformation
    const ComplexCollection inverseTransformedTensor(myFFT.inverseTransform(transformedTensor, {N, N, N}));
    for (UnsignedInteger i = 0; i < tensorFlat.getSize(); ++i)
      assert_almost_equal(inverseTransformedTensor[i], tensorFlat[i]);


  }
  catch (const TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
