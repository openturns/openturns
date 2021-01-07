//                                               -*- C++ -*-
/**
 *  @brief ComplexTensorImplementation implements the Tensor classes
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
#ifndef OPENTURNS_COMPLEXTENSORIMPLEMENTATION_HXX
#define OPENTURNS_COMPLEXTENSORIMPLEMENTATION_HXX

#include "openturns/PersistentCollection.hxx"
#include "openturns/Point.hxx"
#include "openturns/ComplexMatrix.hxx"
#include "openturns/HermitianMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class ComplexTensorImplementation
 *
 * ComplexTensorImplementation implements the Tensor classes
 */

class OT_API ComplexTensorImplementation
  : public PersistentCollection<Complex>

{
  CLASSNAME

public:


  /** Default constructor */
  ComplexTensorImplementation();

  /** Constructor with size (rowDim, colDim, sheetDim) */
  ComplexTensorImplementation(const UnsignedInteger rowDim,
                              const UnsignedInteger colDim,
                              const UnsignedInteger sheetDim);

  /** Constructor from range of external collection */
  template <class InputIterator>
  ComplexTensorImplementation(const UnsignedInteger rowDim,
                              const UnsignedInteger colDim,
                              const UnsignedInteger sheetDim,
                              InputIterator first,
                              InputIterator last);

  /** Constructor from external collection */
  /** If the dimensions don't correspond with the size of the collection, */
  /** the collection is either truncated or completed with zeros*/
  ComplexTensorImplementation(const UnsignedInteger rowDim,
                              const UnsignedInteger colDim,
                              const UnsignedInteger sheetDim,
                              const Collection<Complex> & elementsValues);


  /** Virtual constructor */
  ComplexTensorImplementation * clone() const override;

  /** Set small elements to zero */
  ComplexTensorImplementation clean(const Scalar threshold) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Operator () gives access to the elements of the tensor (to modify these elements) */
  /** The element of the tensor is designated by its row number i, its column number j and its sheet number k */
  Complex & operator () (const UnsignedInteger i,
                         const UnsignedInteger j,
                         const UnsignedInteger k);

  /** Operator () gives access to the elements of the tensor (read only) */
  /** The element of the tensor is designated by its row number i, its column number j and its sheet number k */
  const Complex & operator () (const UnsignedInteger i,
                               const UnsignedInteger j,
                               const UnsignedInteger k) const;

  /** getSheet returns the sheet specified by its sheet number k */
  ComplexMatrix getSheet(const UnsignedInteger k) const;

  /** setSheet sets matrix m as the sheet specified by its sheet number k  */
  void setSheet(const UnsignedInteger k,
                const ComplexMatrix & m);

  /** getSheetSym returns the hermitian sheet specified by its sheet number k */
  HermitianMatrix getSheetSym(const UnsignedInteger k) const;

  /** setSheetSym sets hermitian matrix m as the sheet specified by its sheet number k  */
  void setSheetSym(const UnsignedInteger k,
                   const HermitianMatrix & m);

  /** Get the dimensions of the tensor */
  /** Number of rows */
  UnsignedInteger getNbRows() const;
  /** Number of columns */
  UnsignedInteger getNbColumns() const;
  /** Number of sheets */
  UnsignedInteger getNbSheets() const;

  /** Check for symmetry */
  Bool isSymmetric() const;

  /** Symmetrize ComplexTensorImplementation in case it is a hermitian tensor (stored as a set of triangular matrix sheets) */
  void hermitianize() const;

  /** Comparison operators */
  Bool operator == (const ComplexTensorImplementation & rhs) const;

  /** Empty returns true if there is no element in the tensor */
  Bool isEmpty() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Low-level data access */
  UnsignedInteger stride(const UnsignedInteger dim) const;

protected:

  /** ComplexTensorImplementation Dimensions */
  UnsignedInteger nbRows_;
  UnsignedInteger nbColumns_;
  UnsignedInteger nbSheets_;

  /** Position conversion function : the indices i & j are used to compute the actual position of the element in the collection */
  inline UnsignedInteger convertPosition (const UnsignedInteger i,
                                          const UnsignedInteger j,
                                          const UnsignedInteger k) const;

}; /* class ComplexTensorImplementation */

/** Constructor from range of external collection */
template <class InputIterator>
ComplexTensorImplementation::ComplexTensorImplementation(const UnsignedInteger rowDim,
    const UnsignedInteger colDim,
    const UnsignedInteger sheetDim,
    InputIterator first,
    InputIterator last)
  : PersistentCollection<Complex>(rowDim * colDim * sheetDim),
    nbRows_(rowDim),
    nbColumns_(colDim),
    nbSheets_(sheetDim)
{
  this->assign(first, last);
}

/** Inline functions */

inline UnsignedInteger ComplexTensorImplementation::convertPosition (const UnsignedInteger i,
    const UnsignedInteger j,
    const UnsignedInteger k) const
{
  return i + nbRows_ * (j + nbColumns_ * k) ;
}

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COMPLEXTENSORIMPLEMENTATION_HXX */
