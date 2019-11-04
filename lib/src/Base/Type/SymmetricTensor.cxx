//                                               -*- C++ -*-
/**
 *  @brief SymmetricTensor implements the classical mathematical symmetric tensor
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/SymmetricTensor.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(SymmetricTensor)

/* Default constructor */
SymmetricTensor::SymmetricTensor()
  : Tensor(),
    hasBeenSymmetrized_(false)
{
  // Nothing to do
}

/* Constructor with size (squareDim and sheetDim) */
/* The SymmetricTensor is made up of a collection of squareDim*squareDim*sheetDim elements */
/* The SymmetricTensor is viewed as a set of column vectors read one after another, one sheet after another */
SymmetricTensor::SymmetricTensor(const UnsignedInteger squareDim,
                                 const UnsignedInteger sheetDim)
  : Tensor(squareDim, squareDim, sheetDim),
    hasBeenSymmetrized_(false)
{
  // Nothing to do
}

/* Constructor from external collection */
SymmetricTensor::SymmetricTensor(const UnsignedInteger squareDim,
                                 const UnsignedInteger sheetDim,
                                 const Collection<Scalar> & elementsValues)
  : Tensor(squareDim, squareDim, sheetDim, elementsValues),
    hasBeenSymmetrized_(false)
{
  // Nothing to do
}

/* Constructor with implementation */
SymmetricTensor::SymmetricTensor(const Implementation & i)
  : Tensor(i),
    hasBeenSymmetrized_(false)
{
  // Nothing to do
}


/* String converter */
String SymmetricTensor::__repr__() const
{
  return Tensor::__repr__();
}

/* String converter */
String SymmetricTensor::__str__(const String & offset) const
{
  checkSymmetry();
  return Tensor::__str__( offset );
}

/* Check if the internal representation is really symmetric */
void SymmetricTensor::checkSymmetry() const
{
  if (!hasBeenSymmetrized_)
  {
    getImplementation()->symmetrize();
    hasBeenSymmetrized_ = true;
  }
}

/* Operator () gives access to the elements of the symmetric tensor (to modify these elements and their symmetric element) */
/* The element of the symmetric tensor is designated by its row number i, its column number j and its sheet number k*/
Scalar & SymmetricTensor::operator () (const UnsignedInteger i,
                                       const UnsignedInteger j,
                                       const UnsignedInteger k)
{
  copyOnWrite();
  hasBeenSymmetrized_ = false;
  return ( (i > j) ? (*getImplementation())(i, j, k) : (*getImplementation())(j, i, k) );
}

/* Operator () gives access to the elements of the tensor (read only) */
/* The element of the tensor is designated by its row number i and its column number j */
const Scalar & SymmetricTensor::operator () (const UnsignedInteger i,
    const UnsignedInteger j,
    const UnsignedInteger k) const
{
  return ( (i > j) ? (*getImplementation())(i, j, k) : (*getImplementation())(j, i, k) );
}

/* getSheet returns the sheet specified by its sheet number k */
SymmetricMatrix SymmetricTensor::getSheet(const UnsignedInteger k) const
{
  return getImplementation()->getSheetSym(k);
}

/* setSheet sets matrix m as the sheet specified by its sheet number k  */
void SymmetricTensor::setSheet(const UnsignedInteger k,
                               const SymmetricMatrix & m)
{
  hasBeenSymmetrized_ = false;
  getImplementation()->setSheetSym(k, m);
}

END_NAMESPACE_OPENTURNS
