//                                               -*- C++ -*-
/**
 *  @brief ComplexTensorImplementation implements the Tensor classes
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/ComplexTensorImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ComplexTensorImplementation)

static const Factory<ComplexTensorImplementation> Factory_ComplexTensorImplementation;

/* Default constructor */
ComplexTensorImplementation::ComplexTensorImplementation()
  : PersistentCollection<Complex>()
  , nbRows_(0)
  , nbColumns_(0)
  , nbSheets_(0)
{
  // Nothing to do
}

/* Constructor with size (rowDim, colDim and sheetDim) */
/* The ComplexTensorImplementation is made up of a collection of rowDim*colDim*sheetDim elements */
/* The ComplexTensorImplementation is viewed as a set of column vectors read one after another, one sheet after another */
ComplexTensorImplementation::ComplexTensorImplementation(const UnsignedInteger rowDim,
    const UnsignedInteger colDim,
    const UnsignedInteger sheetDim)
  : PersistentCollection<Complex>(rowDim * colDim * sheetDim, 0.0)
  , nbRows_(rowDim)
  , nbColumns_(colDim)
  , nbSheets_(sheetDim)
{
  // Nothing to do
}

/* Constructor from external collection */
ComplexTensorImplementation::ComplexTensorImplementation(const UnsignedInteger rowDim,
    const UnsignedInteger colDim,
    const UnsignedInteger sheetDim,
    const Collection<Complex> & elementsValues)
  : PersistentCollection<Complex>(rowDim * colDim * sheetDim, 0.0)
  , nbRows_(rowDim)
  , nbColumns_(colDim)
  , nbSheets_(sheetDim)
{
  const UnsignedInteger tensorSize = std::min(rowDim * colDim * sheetDim, elementsValues.getSize());
  std::copy(elementsValues.begin(), elementsValues.begin() + tensorSize, begin());
}


/* Virtual constructor */
ComplexTensorImplementation * ComplexTensorImplementation::clone() const
{
  return new ComplexTensorImplementation(*this);
}


/* Set small elements to zero */
ComplexTensorImplementation ComplexTensorImplementation::clean(const Scalar threshold) const
{
  // Nothing to do for nonpositive threshold
  if (threshold <= 0.0) return *this;
  ComplexTensorImplementation result(nbRows_, nbColumns_, nbSheets_);
  for (UnsignedInteger k = 0; k < nbSheets_; ++k)
    result.setSheet(k, getSheet(k).clean(threshold));
  return result;
}

/* String converter */
String ComplexTensorImplementation::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << " name=" << getName()
         << " rows=" << getNbRows()
         << " columns=" << getNbColumns()
         << " sheets=" << getNbSheets()
         << " values=" << PersistentCollection<Complex>::__repr__();
}

String ComplexTensorImplementation::__str__(const String & offset) const
{
  OSS oss(false);
  for (UnsignedInteger k = 0; k < getNbSheets(); ++k)
  {
    oss << (k == 0 ? "" : Os::GetEndOfLine()) << offset << "sheet " << k << ":" << Os::GetEndOfLine() << offset;
    oss << getSheet(k).__str__(offset) << Os::GetEndOfLine() << offset;
  }
  return oss;
}

/* Get the dimensions of the ComplexTensorImplementation : number of rows */
UnsignedInteger ComplexTensorImplementation::getNbRows() const
{
  return nbRows_;
}

/* Get the dimensions of the ComplexTensorImplementation : number of columns */
UnsignedInteger ComplexTensorImplementation::getNbColumns() const
{
  return nbColumns_;
}

/* Get the dimensions of the ComplexTensorImplementation : number of sheets */
UnsignedInteger ComplexTensorImplementation::getNbSheets() const
{
  return nbSheets_;
}

/* Operator () gives access to the elements of the ComplexTensorImplementation (to modify these elements) */
/* The element of the ComplexTensorImplementation is designated by its row number i, its column number j and its sheet number k */
/* the first element of the ComplexTensorImplementation is t(0,0,0) */
Complex & ComplexTensorImplementation::operator() (const UnsignedInteger i,
    const UnsignedInteger j,
    const UnsignedInteger k)
{
  if ((i >= nbRows_) || (j >= nbColumns_) || (k >= nbSheets_)) throw InvalidDimensionException(HERE);

  return (*this)[this->convertPosition(i, j, k)];
}

/* Operator () gives access to the elements of the ComplexTensorImplementation (read only) */
/* The element of the ComplexTensorImplementation is designated by its row number i, its column number j and its sheet number k */
const Complex & ComplexTensorImplementation::operator() (const UnsignedInteger i,
    const UnsignedInteger j,
    const UnsignedInteger k)  const
{
  if ((i >= nbRows_) || (j >= nbColumns_) || (k >= nbSheets_)) throw InvalidDimensionException(HERE);

  return (*this)[this->convertPosition(i, j, k)];
}

/* getSheet returns the sheet specified by its sheet number k */
ComplexMatrix ComplexTensorImplementation::getSheet(const UnsignedInteger k) const
{
  if (k >= nbSheets_) throw InvalidDimensionException(HERE);

  ComplexMatrixImplementation sheet(nbRows_, nbColumns_);
  const UnsignedInteger shift = convertPosition(0, 0, k);
  std::copy(begin() + shift, begin() + shift + nbRows_ * nbColumns_, sheet.begin());
  return sheet;
}

/* setSheet sets matrix m as the sheet specified by its sheet number k  */
void ComplexTensorImplementation::setSheet(const UnsignedInteger k,
    const ComplexMatrix & m)
{
  if (k >= nbSheets_) throw InvalidDimensionException(HERE);
  if (m.getNbRows() != nbRows_) throw InvalidDimensionException(HERE);
  if (m.getNbColumns() != nbColumns_) throw InvalidDimensionException(HERE);
  std::copy(m.getImplementation()->begin(), m.getImplementation()->end(), begin() + convertPosition(0, 0, k));
}

/* getSheetSym returns the hermitian sheet specified by its sheet number k */
HermitianMatrix ComplexTensorImplementation::getSheetSym(const UnsignedInteger k) const
{
  Pointer<ComplexMatrixImplementation> sheet(getSheet(k).getImplementation());
//   sheet->hermitianize();
  return sheet;
}

/* setSheetSym sets hermitian matrix m as the sheet specified by its sheet number k  */
void ComplexTensorImplementation::setSheetSym(const UnsignedInteger k,
    const HermitianMatrix & m)
{
  Pointer<ComplexMatrixImplementation> sheet(m.getImplementation());
//   sheet->hermitianize();
  setSheet(k, m);
}

/* Empty returns true if there is no element in the ComplexTensorImplementation */
Bool ComplexTensorImplementation::isEmpty() const
{
  return ((nbRows_ == 0)  || (nbColumns_ == 0) || (nbSheets_ == 0) || (PersistentCollection<Complex>::isEmpty())) ;
}

/* Check for symmetry */
Bool ComplexTensorImplementation::isSymmetric() const
{
  if (nbColumns_ != nbRows_) return false;
  for (UnsignedInteger k = 0; k < nbSheets_; ++k)
    for (UnsignedInteger j = 0; j < nbColumns_; ++j)
      for (UnsignedInteger i = 0; i < j; ++i)
        if((*this)[this->convertPosition(i, j, k)] != (*this)[this->convertPosition(j, i, k)]) return false;
  return true;
}

/* Symmetrize ComplexMatrixImplementation in case it is a hermitian matrix (stored as a triangular matrix) */
void ComplexTensorImplementation::hermitianize() const
{
  ComplexTensorImplementation & refThis = * const_cast<ComplexTensorImplementation *>(this);
  for (UnsignedInteger k = 0; k < nbSheets_; ++k)
    for (UnsignedInteger j = 0; j < nbColumns_; ++j)
      for (UnsignedInteger i = 0; i < j; ++i)
        refThis[this->convertPosition(i, j, k)] = std::conj( (*this)[this->convertPosition(j, i, k)] );
}

/* Comparison operator */
Bool ComplexTensorImplementation::operator == (const ComplexTensorImplementation & rhs) const
{
  const ComplexTensorImplementation &lhs(*this);
  Bool equality = true;

  if (&lhs != &rhs)   // Not the same object
  {
    const PersistentCollection<Complex> & refLhs = static_cast<const PersistentCollection<Complex> >(lhs);
    const PersistentCollection<Complex> & refRhs = static_cast<const PersistentCollection<Complex> >(rhs);

    equality = ( lhs.nbRows_ == rhs.nbRows_ && lhs.nbColumns_ == rhs.nbColumns_ && lhs.nbSheets_ == rhs.nbSheets_ && refLhs == refRhs);
  }

  return equality;
}

/* Method save() stores the object through the StorageManager */
void ComplexTensorImplementation::save(Advocate & adv) const
{
  PersistentCollection<Complex>::save(adv);
  adv.saveAttribute("nbRows_",    nbRows_);
  adv.saveAttribute("nbColumns_", nbColumns_);
  adv.saveAttribute("nbSheets_", nbSheets_);
}

/* Method load() reloads the object from the StorageManager */
void ComplexTensorImplementation::load(Advocate & adv)
{
  PersistentCollection<Complex>::load(adv);

  adv.loadAttribute("nbRows_",    nbRows_);
  adv.loadAttribute("nbColumns_", nbColumns_);
  adv.loadAttribute("nbSheets_", nbSheets_);
}


const Complex* ComplexTensorImplementation::__baseaddress__() const
{
  return &(*this)[0];
}


UnsignedInteger ComplexTensorImplementation::__elementsize__() const
{
  return sizeof(Complex);
}


UnsignedInteger ComplexTensorImplementation::__stride__(UnsignedInteger dim) const
{
  UnsignedInteger stride = __elementsize__();
  if (dim > 0)
    stride *= nbRows_;
  if (dim > 1)
    stride *= nbColumns_;
  if (dim > 2)
    stride *= nbSheets_;
  return stride;
}


END_NAMESPACE_OPENTURNS
