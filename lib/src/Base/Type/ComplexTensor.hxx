//                                               -*- C++ -*-
/**
 *  @brief ComplexTensor implements the classical mathematical tensor
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_COMPLEXTENSOR_HXX
#define OPENTURNS_COMPLEXTENSOR_HXX

#include "ComplexTensorImplementation.hxx"
#include "TypedInterfaceObject.hxx"


BEGIN_NAMESPACE_OPENTURNS



class ComplexTensorImplementation;

/**
 * @class ComplexTensor
 *
 * ComplexTensor implements the classical mathematical ComplexTensor
 */

class OT_API ComplexTensor :
  public TypedInterfaceObject<ComplexTensorImplementation>

{
  CLASSNAME;

public:

  typedef TypedInterfaceObject<ComplexTensorImplementation>::Implementation Implementation;
  typedef Collection<NumericalComplex>                          NumericalComplexCollection;

  /** Default constructor */
  ComplexTensor();

  /** Constructor with size (rowDim, colDim, sheetDim) */
  ComplexTensor(const UnsignedInteger rowDim,
                const UnsignedInteger colDim,
                const UnsignedInteger sheetDim);

  /** Constructor from range of external collection */
  template <class InputIterator>
  ComplexTensor(const UnsignedInteger rowDim,
                const UnsignedInteger colDim,
                const UnsignedInteger sheetDim,
                InputIterator first,
                InputIterator last);

  /** Constructor from external collection */
  ComplexTensor(const UnsignedInteger rowDim,
                const UnsignedInteger colDim,
                const UnsignedInteger sheetDim,
                const NumericalComplexCollection & elementsValues);

  /** Constructor with implementation */
  ComplexTensor(const Implementation & i);

  /** Set small elements to zero */
  ComplexTensor clean(const NumericalScalar & threshold) const;

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

#ifndef SWIG
  /** Operator () gives access to the elements of the tensor (to modify these elements) */
  /** The element of the tensor is designated by its row number i, its column number j and its sheet number k */
  NumericalComplex & operator () (const UnsignedInteger i,
                                  const UnsignedInteger j,
                                  const UnsignedInteger k);

  /** Operator () gives access to the elements of the tensor (read only) */
  /** The element of the tensor is designated by its row number i, its column number j and its sheet number k */
  const NumericalComplex & operator () (const UnsignedInteger i,
                                        const UnsignedInteger j,
                                        const UnsignedInteger k) const;
#endif

  /** getSheet returns the sheet specified by its sheet number k */
  ComplexMatrix getSheet(const UnsignedInteger k) const;

  /** setSheet sets matrix m as the sheet specified by its sheet number k  */
  void setSheet(const UnsignedInteger k,
                const ComplexMatrix & m);

  /** Get the dimensions of the tensor */
  /** Number of rows */
  UnsignedInteger getNbRows() const;
  /** Number of columns */
  UnsignedInteger getNbColumns() const;
  /** Number of sheets */
  UnsignedInteger getNbSheets() const;

  /** Comparison operators */
  Bool operator == (const ComplexTensor & rhs) const;

  /** Empty returns true if there is no element in the tensor */
  Bool isEmpty() const;

  // These functions are only intended to be used by SWIG, DO NOT use them for your own purpose !
  // INTENTIONALY NOT DOCUMENTED
  const NumericalComplex * __baseaddress__ () const;
  UnsignedInteger __elementsize__ () const;
  UnsignedInteger __stride__ (UnsignedInteger dim) const;

protected:

}; /* class ComplexTensor */



/** Constructor with size (rowDim, colDim, sheetDim) */
template <class InputIterator>
ComplexTensor::ComplexTensor(const UnsignedInteger rowDim,
                             const UnsignedInteger colDim,
                             const UnsignedInteger sheetDim,
                             InputIterator first,
                             InputIterator last)
  : TypedInterfaceObject<ComplexTensorImplementation>(new ComplexTensorImplementation(rowDim, colDim, sheetDim, first, last))
{
  // nothing to do
}

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COMPLEXTENSOR_HXX */
