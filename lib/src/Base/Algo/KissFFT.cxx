//                                               -*- C++ -*-
/**
 *  @brief
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
#include "openturns/KissFFT.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "kissfft.hh"
#include "openturns/TBB.hxx"

BEGIN_NAMESPACE_OPENTURNS

typedef kissfft<Scalar> KISSFFTScalar;

struct FFTPolicy
{
  const KissFFT::ComplexCollection & input_;
  KissFFT::ComplexCollection  & output_;
  KISSFFTScalar & fft_;
  const UnsignedInteger fftSize_;

  FFTPolicy(const KissFFT::ComplexCollection & input,
            KissFFT::ComplexCollection & output,
            KISSFFTScalar & fft,
            UnsignedInteger fftSize)
    : input_(input)
    , output_(output)
    , fft_(fft)
    , fftSize_(fftSize)
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      const UnsignedInteger first = fftSize_ * i;
      fft_.transform( &input_[first], &output_[first] );
    }
  }
}; /* end struct FFTPolicy */

/**
 * @class KissFFT
 */

CLASSNAMEINIT(KissFFT)

static const Factory<KissFFT> Factory_KissFFT;

/* Constructor with parameters */
KissFFT::KissFFT()
  : FFTImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
KissFFT * KissFFT::clone() const
{
  return new KissFFT(*this);
}

/* String converter */
String KissFFT::__repr__() const
{
  OSS oss(true);
  oss << "class=" << KissFFT::GetClassName();
  return oss;
}

/* String converter */
String KissFFT::__str__(const String & ) const
{
  OSS oss(false);
  oss << "class=" << KissFFT::GetClassName();
  return oss;
}


/* FFT transformation on complex */
KissFFT::ComplexCollection KissFFT::transform(const ComplexCollection & collection) const
{
  // Get the size of the collection
  const UnsignedInteger fftSize = collection.getSize();

  // Init of kissfft  - Boolean argument is to tell that we perform a direct transformation
  KISSFFTScalar fft(fftSize, false);

  // Allocation of memory for final result
  ComplexCollection transformationResult(fftSize);
  fft.transform( &collection[0], &transformationResult[0] );
  return transformationResult;
}

/* FFT transformation on a regular complex sequence of the collection (between first and last, with elements separated by step)*/
KissFFT::ComplexCollection KissFFT::transform(const ComplexCollection & collection,
    const UnsignedInteger first,
    const UnsignedInteger size) const
{
  // Init of kissfft  - Boolean argument is to tell that we perform a direct transformation
  KISSFFTScalar fft(size, false);

  // Allocation of memory for final result
  ComplexCollection transformationResult(size);
  fft.transform( &collection[first], &transformationResult[0] );
  return transformationResult;
}

/* FFT T transformation */
KissFFT::ComplexCollection KissFFT::inverseTransform(const ComplexCollection & collection) const
{
  // Get the size of the collection
  const UnsignedInteger size = collection.getSize();

  // Init of kissfft  - Boolean argument is to tell that we perform an inverse transformation
  KISSFFTScalar fft(size, true);

  // Allocation of memory for final result
  ComplexCollection transformationResult(size);

  // Inverse transform = transform with the opposite sign in the exponent and a 1/N factor
  // Call inverse transformation
  fft.transform( &collection[0], &transformationResult[0] );
  const Scalar alpha = 1.0 / size;
  for (UnsignedInteger index = 0; index < size; ++index) transformationResult[index] *= alpha;
  return transformationResult;
}

/* FFT transformation on a regular complex sequence of the collection (between first and last, with elements separated by step)*/
KissFFT::ComplexCollection KissFFT::inverseTransform(const ComplexCollection & collection,
    const UnsignedInteger first,
    const UnsignedInteger size) const
{
  // Init of kissfft  - Boolean argument is true
  KISSFFTScalar fft(size, true);

  // Allocation of memory for final result
  ComplexCollection transformationResult(size);
  fft.transform( &collection[first], &transformationResult[0] );
  const Scalar alpha = 1.0 / size;
  for (UnsignedInteger index = 0; index < size; ++index) transformationResult[index] *= alpha;
  return transformationResult;
}

ComplexMatrix KissFFT::fft2D(const ComplexMatrix & complexMatrix, const Bool isIFFT) const
{
  const UnsignedInteger columns = complexMatrix.getNbColumns();
  const UnsignedInteger rows = complexMatrix.getNbRows();
  ComplexCollection output(rows * columns);
  KISSFFTScalar fftRows(rows, isIFFT);
  const  FFTPolicy policyRows(*(complexMatrix.getImplementation().get()), output, fftRows, rows);
  TBB::ParallelFor( 0, columns, policyRows);
  ComplexCollection transposedData(rows * columns);
  for (UnsignedInteger rowIndex = 0; rowIndex < rows; ++rowIndex)
    for (UnsignedInteger columnIndex = 0; columnIndex < columns; ++columnIndex)
      transposedData[columnIndex + rowIndex * columns] = output[rowIndex + rows * columnIndex];
  KISSFFTScalar fftColumns(columns, isIFFT);
  const  FFTPolicy policyColumns(transposedData, output, fftColumns, columns);
  TBB::ParallelFor( 0, rows, policyColumns );
  ComplexMatrix result(rows, columns);
  Complex factor(1.0);
  if (isIFFT) factor /= (rows * columns);
  for (UnsignedInteger rowIndex = 0; rowIndex < rows; ++rowIndex)
    for (UnsignedInteger columnIndex = 0; columnIndex < columns; ++columnIndex)
      result(rowIndex, columnIndex) = output[columnIndex + rowIndex * columns] * factor;
  return result;
}

/** FFT 2D transformation on complex */
ComplexMatrix KissFFT::transform2D(const ComplexMatrix & complexMatrix) const
{
  return fft2D(complexMatrix, false);
}


/** FFT 2D transformation on real matrix */
ComplexMatrix KissFFT::transform2D(const Matrix & matrix) const
{
  const UnsignedInteger rows = matrix.getNbRows();
  const UnsignedInteger columns = matrix.getNbColumns();
  ComplexMatrix complexMatrix(rows, columns);

  for (UnsignedInteger columnIndex = 0; columnIndex < columns; ++columnIndex)
    for (UnsignedInteger rowIndex = 0; rowIndex < rows; ++rowIndex)
      complexMatrix(rowIndex, columnIndex) = matrix(rowIndex, columnIndex);
  return transform2D(complexMatrix);
}

/** FFT 2D transformation on sample */
ComplexMatrix KissFFT::transform2D(const Sample & sample) const
{
  const UnsignedInteger rows = sample.getSize();
  const UnsignedInteger columns = sample.getDimension();
  ComplexMatrix complexMatrix(rows, columns);

  for (UnsignedInteger columnIndex = 0; columnIndex < columns; ++columnIndex)
    for (UnsignedInteger rowIndex = 0; rowIndex < rows; ++rowIndex)
      complexMatrix(rowIndex, columnIndex) = sample(rowIndex, columnIndex);
  return transform2D(complexMatrix);
}

/** IFFT 2D transformation on complex */
ComplexMatrix KissFFT::inverseTransform2D(const ComplexMatrix & complexMatrix) const
{
  return fft2D(complexMatrix, true);
}

/** IFFT 2D transformation on real matrix */
ComplexMatrix KissFFT::inverseTransform2D(const Matrix & matrix) const
{
  const UnsignedInteger rows = matrix.getNbRows();
  const UnsignedInteger columns = matrix.getNbColumns();
  ComplexMatrix complexMatrix(rows, columns);

  for (UnsignedInteger columnIndex = 0; columnIndex < columns; ++columnIndex)
    for (UnsignedInteger rowIndex = 0; rowIndex < rows; ++rowIndex)
      complexMatrix(rowIndex, columnIndex) = matrix(rowIndex, columnIndex);
  return inverseTransform2D(complexMatrix);
}

/** IFFT 2D transformation on sample */
ComplexMatrix KissFFT::inverseTransform2D(const Sample & sample) const
{
  const UnsignedInteger rows = sample.getSize();
  const UnsignedInteger columns = sample.getDimension();
  ComplexMatrix complexMatrix(rows, columns);

  for (UnsignedInteger columnIndex = 0; columnIndex < columns; ++columnIndex)
    for (UnsignedInteger rowIndex = 0; rowIndex < rows; ++rowIndex)
      complexMatrix(rowIndex, columnIndex) = sample(rowIndex, columnIndex);
  return inverseTransform2D(complexMatrix);
}

ComplexTensor KissFFT::fft3D(const ComplexTensor & tensor, const Bool isIFFT) const
{
  // Get the dimensions of the tensor
  const UnsignedInteger rows = tensor.getNbRows();
  const UnsignedInteger columns = tensor.getNbColumns();
  const UnsignedInteger sheets = tensor.getNbSheets();
  // tensor result
  ComplexTensor result(rows, columns, sheets);
  for(UnsignedInteger sheetIndex = 0; sheetIndex < sheets; ++sheetIndex)
  {
    // 2D transform on the current sheet
    ComplexMatrix sheet(tensor.getSheet(sheetIndex));
    result.setSheet(sheetIndex, fft2D(sheet, isIFFT));
  }

  ComplexCollection input(rows * columns * sheets);
  ComplexCollection output(rows * columns * sheets);
  UnsignedInteger index = 0;
  for (UnsignedInteger columnIndex = 0; columnIndex < columns; ++columnIndex)
  {
    for (UnsignedInteger rowIndex = 0; rowIndex < rows; ++rowIndex)
    {
      for(UnsignedInteger sheetIndex = 0; sheetIndex < sheets; ++sheetIndex)
      {
        input[index] = result(rowIndex, columnIndex, sheetIndex);
        index += 1;
      }
    }
  }
  KISSFFTScalar fft(sheets, isIFFT);
  const FFTPolicy policy(input, output, fft, sheets);
  TBB::ParallelFor( 0, rows * columns, policy );
  index = 0;
  Complex factor(1.0);
  if (isIFFT) factor /= sheets;
  for (UnsignedInteger columnIndex = 0; columnIndex < columns; ++columnIndex)
  {
    for (UnsignedInteger rowIndex = 0; rowIndex < rows; ++rowIndex)
    {
      for(UnsignedInteger sheetIndex = 0; sheetIndex < sheets; ++sheetIndex)
      {
        result(rowIndex, columnIndex, sheetIndex) = output[index] * factor;
        index += 1;
      }
    }
  }
  return result;
}

/** FFT 3D transformation on complex */
ComplexTensor KissFFT::transform3D(const ComplexTensor & tensor) const
{
  return fft3D(tensor, false);
}

/* FFT 3D transformation on real data */
ComplexTensor KissFFT::transform3D(const Tensor & tensor) const
{
  // Get the dimensions of the tensor
  const UnsignedInteger rows = tensor.getNbRows();
  const UnsignedInteger columns = tensor.getNbColumns();
  const UnsignedInteger sheets = tensor.getNbSheets();
  // tensor result
  ComplexTensor complexTensor(rows, columns, sheets);
  for(UnsignedInteger sheetIndex = 0; sheetIndex < sheets; ++sheetIndex)
    for (UnsignedInteger columnIndex = 0; columnIndex < columns; ++columnIndex)
      for (UnsignedInteger rowIndex = 0; rowIndex < rows; ++rowIndex)
        complexTensor(rowIndex, columnIndex, sheetIndex) = tensor(rowIndex, columnIndex, sheetIndex);
  return transform3D(complexTensor);
}

/** IFFT 3D transformation on complex */
ComplexTensor KissFFT::inverseTransform3D(const ComplexTensor & tensor) const
{
  return fft3D(tensor, true);
}

/** IFFT 3D transformation on real */
ComplexTensor KissFFT::inverseTransform3D(const Tensor & tensor) const
{
  // Get the dimensions of the tensor
  const UnsignedInteger rows = tensor.getNbRows();
  const UnsignedInteger columns = tensor.getNbColumns();
  const UnsignedInteger sheets = tensor.getNbSheets();
  // tensor result
  ComplexTensor complexTensor(rows, columns, sheets);
  for(UnsignedInteger sheetIndex = 0; sheetIndex < sheets; ++sheetIndex)
    for (UnsignedInteger columnIndex = 0; columnIndex < columns; ++columnIndex)
      for (UnsignedInteger rowIndex = 0; rowIndex < rows; ++rowIndex)
        complexTensor(rowIndex, columnIndex, sheetIndex) = tensor(rowIndex, columnIndex, sheetIndex);
  return inverseTransform3D(complexTensor);
}

END_NAMESPACE_OPENTURNS
