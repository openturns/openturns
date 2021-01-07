//                                               -*- C++ -*-
/**
 *  @brief Tensor implements the classical mathematical tensor
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
#include "openturns/Tensor.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(Tensor)

/* Default constructor */
Tensor::Tensor()
  : TypedInterfaceObject<TensorImplementation>(new TensorImplementation())
{
  // Nothing to do
}

/* Constructor with size (rowDim, colDim and sheetDim) */
/* The tensor is made up of a collection of rowDim*colDim*sheetDim elements */
/* The tensor is viewed as a set of column vectors read one after another, one sheet after another */
Tensor::Tensor(const UnsignedInteger rowDim,
               const UnsignedInteger colDim,
               const UnsignedInteger sheetDim)
  : TypedInterfaceObject<TensorImplementation>(new TensorImplementation(rowDim, colDim, sheetDim))
{
  // Nothing to do
}

/* Constructor from external collection */
Tensor::Tensor(const UnsignedInteger rowDim,
               const UnsignedInteger colDim,
               const UnsignedInteger sheetDim,
               const Collection<Scalar> & elementsValues)
  : TypedInterfaceObject<TensorImplementation>(new TensorImplementation(rowDim, colDim, sheetDim, elementsValues))
{
  // Nothing to do
}

/* Constructor with implementation */
Tensor::Tensor(const Implementation & i)
  : TypedInterfaceObject<TensorImplementation>(i)
{
  // Nothing to do
}

/* Set small elements to zero */
Tensor Tensor::clean(const Scalar & threshold) const
{
  return Pointer<TensorImplementation>(getImplementation()->clean(threshold).clone());
}

/* String converter */
String Tensor::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << " implementation=" << getImplementation()->__repr__();
}

/* String converter */
String Tensor::__str__(const String & offset) const
{
  OSS oss(false);
  const UnsignedInteger rows   = getNbRows();
  const UnsignedInteger cols   = getNbColumns();
  const UnsignedInteger sheets = getNbSheets();
  if ( (rows   >= ResourceMap::GetAsUnsignedInteger("Tensor-size-visible-in-str-from")) ||
       (cols   >= ResourceMap::GetAsUnsignedInteger("Tensor-size-visible-in-str-from")) ||
       (sheets >= ResourceMap::GetAsUnsignedInteger("Tensor-size-visible-in-str-from")) )
    oss << rows << "x" << cols << "x" << sheets << Os::GetEndOfLine();

  size_t lwidth = 0;
  size_t rwidth = 0;
  for( UnsignedInteger k = 0; k < sheets; ++k )
    for( UnsignedInteger i = 0; i < rows; ++i )
      for( UnsignedInteger j = 0; j < cols; ++j )
      {
        String st = OSS() << (*this)(i, j, k);
        size_t dotpos = st.find( '.' );
        lwidth = std::max( lwidth, (dotpos != String::npos) ? dotpos             : st.size() );
        rwidth = std::max( rwidth, (dotpos != String::npos) ? st.size() - dotpos : 0         );
      }

  const char * nl = "";
  for( UnsignedInteger k = 0; k < sheets; ++k, nl = Os::GetEndOfLine() )
  {
    oss << nl << "sheet #" << k << Os::GetEndOfLine();
    const char * bracket = "[";
    const char * newline = "";
    for( UnsignedInteger i = 0; i < rows; ++i, newline = Os::GetEndOfLine(), bracket = " " )
    {
      oss << newline << offset << bracket << "[ ";
      const char * sep = "";
      for( UnsignedInteger j = 0; j < cols; ++j, sep = " " )
      {
        String st = OSS() << (*this)(i, j, k);
        size_t dotpos = st.find( '.' );
        oss << sep << String( lwidth - ((dotpos != String::npos) ? dotpos : st.size()), ' ' )
            << (*this)(i, j, k)
            << String( rwidth - ((dotpos != String::npos) ? st.size() - dotpos : 0), ' ' );
      }
      oss << " ]";
    }
    oss << "]";
  }
  return oss;
}

/* Operator () gives access to the elements of the tensor (to modify these elements) */
/* The element of the tensor is designated by its row number i, its column number j and its sheet number k */
Scalar & Tensor::operator () (const UnsignedInteger i,
                              const UnsignedInteger j,
                              const UnsignedInteger k)
{
  copyOnWrite();
  return (*getImplementation())(i, j, k);
}

/* Operator () gives access to the elements of the tensor (read only) */
/* The element of the tensor is designated by its row number i, its column number j and its sheet number k */
const Scalar & Tensor::operator () (const UnsignedInteger i,
                                    const UnsignedInteger j,
                                    const UnsignedInteger k) const
{
  return (*getImplementation())(i, j, k);
}


/* getSheet returns the sheet specified by its sheet number k */
Matrix Tensor::getSheet(const UnsignedInteger k) const
{
  return getImplementation()->getSheet(k);
}

/* setSheet sets matrix m as the sheet specified by its sheet number k  */
void Tensor::setSheet(const UnsignedInteger k,
                      const Matrix & m)
{
  copyOnWrite();
  getImplementation()->setSheet(k, m);
}

/* Get the dimensions of the tensor : number of rows */
UnsignedInteger Tensor::getNbRows() const
{
  return getImplementation()->getNbRows();
}

/* Get the dimensions of the Tensor : number of columns */
UnsignedInteger Tensor::getNbColumns() const
{
  return getImplementation()->getNbColumns();
}

/* Get the dimensions of the Tensor : number of sheets */
UnsignedInteger Tensor::getNbSheets() const
{
  return getImplementation()->getNbSheets();
}

/* Empty returns true if there is no element in the Tensor */
Bool Tensor::isEmpty() const
{
  return getImplementation()->isEmpty();
}

/* Comparison operator */
Bool Tensor::operator == (const Tensor & rhs) const
{
  const Tensor &lhs(*this);
  return (*(lhs.getImplementation()) == *(rhs.getImplementation()) );
}

const Scalar* Tensor::data() const
{
  return getImplementation()->data();
}

UnsignedInteger Tensor::elementSize() const
{
  return getImplementation()->elementSize();
}

UnsignedInteger Tensor::stride(UnsignedInteger dim) const
{
  return getImplementation()->stride(dim);
}


END_NAMESPACE_OPENTURNS
