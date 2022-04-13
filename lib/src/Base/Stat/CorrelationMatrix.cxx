//                                               -*- C++ -*-
/**
 *  @brief The class CorrelationMatrix implements correlation matrices
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/CorrelationMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(CorrelationMatrix)

/* Default constructor */
CorrelationMatrix::CorrelationMatrix()
  : CovarianceMatrix(0)
{
  // Nothing to do
}

/* Constructor with implementation */
CorrelationMatrix::CorrelationMatrix(const Implementation & i)
  : CovarianceMatrix(i)
{
  // Nothing to do
}


/* Constructor with implementation */
CorrelationMatrix::CorrelationMatrix(const MatrixImplementation & i)
  : CovarianceMatrix(i)
{
  // Nothing to do
}

/* Constructor from SymmetricMatrix */
CorrelationMatrix::CorrelationMatrix(const SymmetricMatrix & sym)
  : CovarianceMatrix(sym)
{
  // Nothing to do
}

/* Constructor with size (dim, which is the same for nbRows_ and nbColumns_ )*/
CorrelationMatrix::CorrelationMatrix(const UnsignedInteger dim)
  : CovarianceMatrix(dim)
{
  // Nothing to do
}

/* Constructor from external collection */
/* If the dimensions of the matrix and of the collection */
/* do not match, either the collection is truncated */
/* or the rest of the matrix is filled with zeros */
CorrelationMatrix::CorrelationMatrix(const UnsignedInteger dim,
                                     const Collection<Scalar> &elementsValues)
  : CovarianceMatrix(dim, elementsValues)
{
  // Nothing to do
}

/* String converter */
String CorrelationMatrix::__repr__() const
{
  checkSymmetry();
  return OSS() << "class=" << getClassName()
         << " dimension=" << this->getDimension()
         << " implementation=" << getImplementation()->__repr__();
}

/* CorrelationMatrix transpose */
CorrelationMatrix CorrelationMatrix::transpose () const
{
  return *this;
}

/* CorrelationMatrix multiplication (must have consistent dimensions) */
CorrelationMatrix CorrelationMatrix::operator * (const IdentityMatrix & ) const
{
  return *this;
}

END_NAMESPACE_OPENTURNS
