//                                               -*- C++ -*-
/**
 *  @brief TensorImplementation implements the Tensor classes
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
#include "TensorImplementation.hxx"
#include "PersistentObjectFactory.hxx"
#include "Os.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(TensorImplementation);

static const Factory<TensorImplementation> RegisteredFactory;

/* Default constructor */
TensorImplementation::TensorImplementation()
  : PersistentCollection<NumericalScalar>()
  , nbRows_(0)
  , nbColumns_(0)
  , nbSheets_(0)
{
  // Nothing to do
}

/* Constructor with size (rowDim, colDim and sheetDim) */
/* The TensorImplementation is made up of a collection of rowDim*colDim*sheetDim elements */
/* The TensorImplementation is viewed as a set of column vectors read one after another, one sheet after another */
TensorImplementation::TensorImplementation(const UnsignedInteger rowDim,
    const UnsignedInteger colDim,
    const UnsignedInteger sheetDim)
  : PersistentCollection<NumericalScalar>(rowDim * colDim * sheetDim, 0.0)
  , nbRows_(rowDim)
  , nbColumns_(colDim)
  , nbSheets_(sheetDim)
{
  // Nothing to do
}

/* Constructor from external collection */
TensorImplementation::TensorImplementation(const UnsignedInteger rowDim,
    const UnsignedInteger colDim,
    const UnsignedInteger sheetDim,
    const Collection<NumericalScalar> & elementsValues)
  : PersistentCollection<NumericalScalar>(rowDim * colDim * sheetDim, 0.0)
  , nbRows_(rowDim)
  , nbColumns_(colDim)
  , nbSheets_(sheetDim)
{
  const UnsignedInteger tensorSize = std::min(rowDim * colDim * sheetDim, elementsValues.getSize());
  std::copy(elementsValues.begin(), elementsValues.begin() + tensorSize, begin());
}


/* Virtual constructor */
TensorImplementation * TensorImplementation::clone() const
{
  return new TensorImplementation(*this);
}


/* Set small elements to zero */
TensorImplementation TensorImplementation::clean(const NumericalScalar threshold) const
{
  // Nothing to do for nonpositive threshold
  if (threshold <= 0.0) return *this;
  TensorImplementation result(nbRows_, nbColumns_, nbSheets_);
  for (UnsignedInteger k = 0; k < nbSheets_; ++k)
    result.setSheet(k, getSheet(k).clean(threshold));
  return result;
}

/* String converter */
String TensorImplementation::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << " name=" << getName()
         << " rows=" << getNbRows()
         << " columns=" << getNbColumns()
         << " sheets=" << getNbSheets()
         << " values=" << PersistentCollection<NumericalScalar>::__repr__();
}

String TensorImplementation::__str__(const String & offset) const
{
  OSS oss(false);
  for (UnsignedInteger k = 0; k < getNbSheets(); ++k)
  {
    oss << (k == 0 ? "" : Os::GetEndOfLine()) << offset << "sheet " << k << ":" << Os::GetEndOfLine();
    oss << offset << getSheet(k).__str__(offset) << Os::GetEndOfLine();
  }
  return oss;
}

/* Get the dimensions of the TensorImplementation : number of rows */
UnsignedInteger TensorImplementation::getNbRows() const
{
  return nbRows_;
}

/* Get the dimensions of the TensorImplementation : number of columns */
UnsignedInteger TensorImplementation::getNbColumns() const
{
  return nbColumns_;
}

/* Get the dimensions of the TensorImplementation : number of sheets */
UnsignedInteger TensorImplementation::getNbSheets() const
{
  return nbSheets_;
}

/* Operator () gives access to the elements of the TensorImplementation (to modify these elements) */
/* The element of the TensorImplementation is designated by its row number i, its column number j and its sheet number k */
/* the first element of the TensorImplementation is t(0,0,0) */
NumericalScalar & TensorImplementation::operator() (const UnsignedInteger i,
    const UnsignedInteger j,
    const UnsignedInteger k)
{
  if ((i >= nbRows_) || (j >= nbColumns_) || (k >= nbSheets_)) throw InvalidDimensionException(HERE);

  return (*this)[this->convertPosition(i, j, k)];
}

/* Operator () gives access to the elements of the TensorImplementation (read only) */
/* The element of the TensorImplementation is designated by its row number i, its column number j and its sheet number k */
const NumericalScalar & TensorImplementation::operator() (const UnsignedInteger i,
    const UnsignedInteger j,
    const UnsignedInteger k)  const
{
  if ((i >= nbRows_) || (j >= nbColumns_) || (k >= nbSheets_)) throw InvalidDimensionException(HERE);

  return (*this)[this->convertPosition(i, j, k)];
}

/* getSheet returns the sheet specified by its sheet number k */
Matrix TensorImplementation::getSheet(const UnsignedInteger k) const
{
  if (k >= nbSheets_) throw InvalidDimensionException(HERE);

  MatrixImplementation sheet(nbRows_, nbColumns_);
  const UnsignedInteger shift(convertPosition(0, 0, k));
  std::copy(begin() + shift, begin() + shift + nbRows_ * nbColumns_, sheet.begin());
  return sheet;
}

/* setSheet sets matrix m as the sheet specified by its sheet number k  */
void TensorImplementation::setSheet(const UnsignedInteger k,
                                    const Matrix & m)
{
  if (k >= nbSheets_) throw InvalidDimensionException(HERE);
  if (m.getNbRows() != nbRows_) throw InvalidDimensionException(HERE);
  if (m.getNbColumns() != nbColumns_) throw InvalidDimensionException(HERE);
  std::copy(m.getImplementation()->begin(), m.getImplementation()->end(), begin() + convertPosition(0, 0, k));
}

/* getSheetSym returns the symmetric sheet specified by its sheet number k */
SymmetricMatrix TensorImplementation::getSheetSym(const UnsignedInteger k) const
{
  Pointer<MatrixImplementation> sheet(getSheet(k).getImplementation());
  sheet->symmetrize();
  return sheet;
}

/* setSheetSym sets symmetric matrix m as the sheet specified by its sheet number k  */
void TensorImplementation::setSheetSym(const UnsignedInteger k,
                                       const SymmetricMatrix & m)
{
  Pointer<MatrixImplementation> sheet(m.getImplementation());
  sheet->symmetrize();
  setSheet(k, m);
}

/* Empty returns true if there is no element in the TensorImplementation */
Bool TensorImplementation::isEmpty() const
{
  return ((nbRows_ == 0)  || (nbColumns_ == 0) || (nbSheets_ == 0) || (PersistentCollection<NumericalScalar>::isEmpty())) ;
}

/* Check for symmetry */
Bool TensorImplementation::isSymmetric() const
{
  if (nbColumns_ != nbRows_) return false;
  for (UnsignedInteger k = 0; k < nbSheets_; ++k)
    for (UnsignedInteger j = 0; j < nbColumns_; ++j)
      for (UnsignedInteger i = 0; i < j; ++i)
        if((*this)[this->convertPosition(i, j, k)] != (*this)[this->convertPosition(j, i, k)]) return false;
  return true;
}

/* Symmetrize MatrixImplementation in case it is a symmetric matrix (stored as a triangular matrix) */
void TensorImplementation::symmetrize() const
{
  TensorImplementation & refThis = * const_cast<TensorImplementation *>(this);
  for (UnsignedInteger k = 0; k < nbSheets_; ++k)
    for (UnsignedInteger j = 0; j < nbColumns_; ++j)
      for (UnsignedInteger i = 0; i < j; ++i)
        refThis[this->convertPosition(i, j, k)] = (*this)[this->convertPosition(j, i, k)];
}

/* Comparison operator */
Bool TensorImplementation::operator == (const TensorImplementation & rhs) const
{
  const TensorImplementation &lhs(*this);
  Bool equality = true;

  if (&lhs != &rhs)   // Not the same object
  {
    const PersistentCollection<NumericalScalar> & refLhs = static_cast<const PersistentCollection<NumericalScalar> >(lhs);
    const PersistentCollection<NumericalScalar> & refRhs = static_cast<const PersistentCollection<NumericalScalar> >(rhs);

    equality = ( lhs.nbRows_ == rhs.nbRows_ && lhs.nbColumns_ == rhs.nbColumns_ && lhs.nbSheets_ == rhs.nbSheets_ && refLhs == refRhs);
  }

  return equality;
}

/* Method save() stores the object through the StorageManager */
void TensorImplementation::save(Advocate & adv) const
{
  PersistentCollection<NumericalScalar>::save(adv);
  adv.saveAttribute("nbRows_",    nbRows_);
  adv.saveAttribute("nbColumns_", nbColumns_);
  adv.saveAttribute("nbSheets_", nbSheets_);
}

/* Method load() reloads the object from the StorageManager */
void TensorImplementation::load(Advocate & adv)
{
  PersistentCollection<NumericalScalar>::load(adv);

  adv.loadAttribute("nbRows_",    nbRows_);
  adv.loadAttribute("nbColumns_", nbColumns_);
  adv.loadAttribute("nbSheets_", nbSheets_);
}


const NumericalScalar* TensorImplementation::__baseaddress__() const
{
  return &(*this)[0];
}


UnsignedInteger TensorImplementation::__elementsize__() const
{
  return sizeof(NumericalScalar);
}


UnsignedInteger TensorImplementation::__stride__(UnsignedInteger dim) const
{
  UnsignedInteger stride = __elementsize__();
  if (dim > 0)
    stride *= nbRows_;
  if (dim > 1)
    stride *= nbColumns_;
  return stride;
}


END_NAMESPACE_OPENTURNS
