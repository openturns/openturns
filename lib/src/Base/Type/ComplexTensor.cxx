//                                               -*- C++ -*-
/**
 *  @brief ComplexTensor implements the classical mathematical tensor
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
#include "openturns/ComplexTensor.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(ComplexTensor)

/* Default constructor */
ComplexTensor::ComplexTensor()
  : TypedInterfaceObject<ComplexTensorImplementation>(new ComplexTensorImplementation())
{
  // Nothing to do
}

/* Constructor with size (rowDim, colDim and sheetDim) */
/* The tensor is made up of a collection of rowDim*colDim*sheetDim elements */
/* The tensor is viewed as a set of column vectors read one after another, one sheet after another */
ComplexTensor::ComplexTensor(const UnsignedInteger rowDim,
                             const UnsignedInteger colDim,
                             const UnsignedInteger sheetDim)
  : TypedInterfaceObject<ComplexTensorImplementation>(new ComplexTensorImplementation(rowDim, colDim, sheetDim))
{
  // Nothing to do
}

/* Constructor from external collection */
ComplexTensor::ComplexTensor(const UnsignedInteger rowDim,
                             const UnsignedInteger colDim,
                             const UnsignedInteger sheetDim,
                             const Collection<Complex> & elementsValues)
  : TypedInterfaceObject<ComplexTensorImplementation>(new ComplexTensorImplementation(rowDim, colDim, sheetDim, elementsValues))
{
  // Nothing to do
}

/* Constructor with implementation */
ComplexTensor::ComplexTensor(const Implementation & i)
  : TypedInterfaceObject<ComplexTensorImplementation>(i)
{
  // Nothing to do
}

/* Set small elements to zero */
ComplexTensor ComplexTensor::clean(const Scalar & threshold) const
{
  return Pointer<ComplexTensorImplementation>(getImplementation()->clean(threshold).clone());
}

/* String converter */
String ComplexTensor::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << " implementation=" << getImplementation()->__repr__();
}

/* String converter */
String ComplexTensor::__str__(const String & offset) const
{
  OSS oss(false);
  const UnsignedInteger rows   = getNbRows();
  const UnsignedInteger cols   = getNbColumns();
  const UnsignedInteger sheets = getNbSheets();
  if ( (rows   >= ResourceMap::GetAsUnsignedInteger("ComplexTensor-size-visible-in-str-from")) ||
       (cols   >= ResourceMap::GetAsUnsignedInteger("ComplexTensor-size-visible-in-str-from")) ||
       (sheets >= ResourceMap::GetAsUnsignedInteger("ComplexTensor-size-visible-in-str-from")) )
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
    oss << nl << "sheet #" << k << Os::GetEndOfLine() << offset;
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
Complex & ComplexTensor::operator () (const UnsignedInteger i,
                                      const UnsignedInteger j,
                                      const UnsignedInteger k)
{
  copyOnWrite();
  return (*getImplementation())(i, j, k);
}

/* Operator () gives access to the elements of the tensor (read only) */
/* The element of the tensor is designated by its row number i, its column number j and its sheet number k */
const Complex & ComplexTensor::operator () (const UnsignedInteger i,
    const UnsignedInteger j,
    const UnsignedInteger k) const
{
  return (*getImplementation())(i, j, k);
}


/* getSheet returns the sheet specified by its sheet number k */
ComplexMatrix ComplexTensor::getSheet(const UnsignedInteger k) const
{
  return getImplementation()->getSheet(k);
}

/* setSheet sets matrix m as the sheet specified by its sheet number k  */
void ComplexTensor::setSheet(const UnsignedInteger k,
                             const ComplexMatrix & m)
{
  copyOnWrite();
  getImplementation()->setSheet(k, m);
}

/* Get the dimensions of the tensor : number of rows */
UnsignedInteger ComplexTensor::getNbRows() const
{
  return getImplementation()->getNbRows();
}

/* Get the dimensions of the ComplexTensor : number of columns */
UnsignedInteger ComplexTensor::getNbColumns() const
{
  return getImplementation()->getNbColumns();
}

/* Get the dimensions of the ComplexTensor : number of sheets */
UnsignedInteger ComplexTensor::getNbSheets() const
{
  return getImplementation()->getNbSheets();
}

/* Empty returns true if there is no element in the ComplexTensor */
Bool ComplexTensor::isEmpty() const
{
  return getImplementation()->isEmpty();
}

/* Comparison operator */
Bool ComplexTensor::operator == (const ComplexTensor & rhs) const
{
  const ComplexTensor &lhs(*this);
  return (*(lhs.getImplementation()) == *(rhs.getImplementation()) );
}

const Complex* ComplexTensor::__baseaddress__() const
{
  return getImplementation()->__baseaddress__();
}

UnsignedInteger ComplexTensor::__elementsize__() const
{
  return getImplementation()->__elementsize__();
}

UnsignedInteger ComplexTensor::__stride__(UnsignedInteger dim) const
{
  return getImplementation()->__stride__(dim);
}


END_NAMESPACE_OPENTURNS
