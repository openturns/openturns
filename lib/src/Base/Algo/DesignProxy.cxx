//                                               -*- C++ -*-
/**
 *  @brief Design matrix cached evaluation
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/DesignProxy.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DesignProxy)

static const Factory<DesignProxy> Factory_DesignProxy;

/* Default constructor */
DesignProxy::DesignProxy ()
  : PersistentObject()
  , x_()
  , basis_()
  , designCache_(0, 0)
  , alreadyComputed_(0)
  , rowFilter_(0)
{
  // Nothing to do
}

/* Parameters constructor */
DesignProxy::DesignProxy(const Sample & x,
                         const FunctionCollection & basis)
  : PersistentObject()
  , x_(x)
  , basis_(basis)
  , designCache_(0, 0)
  , alreadyComputed_(0)
  , rowFilter_(0)
{
  // keep initialization in the ctor for designCache_ to be shared among DesignProxy copies
  initialize();
}


/* Parameters constructor */
DesignProxy::DesignProxy(const Matrix & matrix)
  : PersistentObject()
  , x_()
  , basis_()
  , designCache_(matrix)
  , alreadyComputed_(0)
  , rowFilter_(0)
{
  // Here we bypass the size constraint on the cache so we don't call initialize()
  alreadyComputed_ = Indices(matrix.getNbColumns());
  alreadyComputed_.fill();
}


void DesignProxy::initialize() const
{
  // allocate cache
  UnsignedInteger cacheSize = ResourceMap::GetAsUnsignedInteger("DesignProxy-DefaultCacheSize");
  UnsignedInteger nbRows = x_.getSize();
  if (!(nbRows > 0)) throw InvalidArgumentException(HERE) << "Cannot initialize a DesignProxy with an empty sample";

  UnsignedInteger nbCols = cacheSize / nbRows;
  // The cache stores at least the first function values
  if (nbCols <= 0) nbCols = 1;
  if (nbCols > basis_.getSize()) nbCols = basis_.getSize();
  designCache_ = Matrix(nbRows, nbCols);
  alreadyComputed_ = Indices(nbCols, nbCols);
}


/* Virtual constructor */
DesignProxy * DesignProxy::clone() const
{
  return new DesignProxy( *this );
}


/* String converter */
String DesignProxy::__repr__() const
{
  return OSS() << "class=" << GetClassName();
}


/* Compute the design matrix with the provided basis terms indices */
Matrix DesignProxy::computeDesign(const Indices & indices) const
{
  // Quick return if the cache is *exactly* the design matrix
  // In particular, it is the case if the DesignProxy has been
  // built from a matrix
  if ((indices == alreadyComputed_) && !hasRowFilter())
    return *designCache_.getImplementation();
  const UnsignedInteger allowedSize = basis_.getSize() ? basis_.getSize() : designCache_.getNbColumns();
  if (!indices.check(allowedSize))
    throw InvalidArgumentException(HERE) << "Requested indices exceed basis size (" << allowedSize << ")";
  MatrixImplementation design;
  const UnsignedInteger indicesSize = indices.getSize();
  const UnsignedInteger xSize = x_.getSize();
  if (indices == alreadyComputed_)
    design = *designCache_.getImplementation();
  else
  {
    // Copy or compute the needed parts
    const UnsignedInteger cacheSize = designCache_.getNbColumns();
    design = MatrixImplementation(xSize, indicesSize);
    MatrixImplementation::iterator startDesign(design.begin());
    for (UnsignedInteger j = 0; j < indicesSize; ++j)
    {
      const UnsignedInteger phiIndex = indices[j];
      // If the index is too large for the cache, compute the column and copy it directly in the design matrix
      if (phiIndex >= cacheSize)
      {
        const Point column(basis_[phiIndex](x_).getImplementation()->getData());
        std::copy(column.begin(), column.end(), startDesign);
      } // Exceeds cache capacity
      else
      {
        MatrixImplementation::iterator startCache(designCache_.getImplementation()->begin() + phiIndex * xSize);
        // If the current index is already in the cache
        if (alreadyComputed_[phiIndex] != alreadyComputed_.getSize())
        {
          // Simply copy the cache content in the design matrix
          std::copy(startCache, startCache + xSize, startDesign);
        } // Values in the cache
        else
          // The value is not in the cache
        {
          // Compute the values
          Point column(basis_[phiIndex](x_).getImplementation()->getData());
          // Copy the values in the cache
          alreadyComputed_[phiIndex] = phiIndex;
          std::copy(column.begin(), column.end(), startCache);// copyOnWrite not called
          // And copy the value in the design matrix
          std::copy(column.begin(), column.end(), startDesign);
        } // values stored in the cache
      } // Index in cache capacity
      startDesign += xSize;
    } // j
  } // indices != alreadyComputed_
  // Apply row filter if needed
  if (hasRowFilter())
  {
    const UnsignedInteger newRowDim = rowFilter_.getSize();
    MatrixImplementation filteredDesign(newRowDim, indicesSize);
    UnsignedInteger linearIndex = 0;
    UnsignedInteger shift = 0;
    for (UnsignedInteger j = 0; j < indicesSize; ++ j)
    {
      for (UnsignedInteger i = 0; i < newRowDim; ++ i)
      {
        filteredDesign[linearIndex] = design[shift + rowFilter_[i]];
        ++ linearIndex;
      }
      shift += xSize;
    }
    design = filteredDesign;
  } // hasRowFilter()
  return design;
}

/* Input sample accessor */
Sample DesignProxy::getInputSample() const
{
  return x_;
}

/* Basis accessor */
DesignProxy::FunctionCollection DesignProxy::getBasis() const
{
  return basis_;
}

DesignProxy::FunctionCollection DesignProxy::getBasis(const Indices & indices) const
{
  if (!indices.check(basis_.getSize()))
    throw InvalidArgumentException(HERE) << "Requested indices exceed basis size (" << basis_.getSize() << ")";
  const UnsignedInteger activeSize = indices.getSize();
  FunctionCollection selectedBasis(activeSize);
  for (UnsignedInteger i = 0; i < activeSize; ++i)
    selectedBasis[i] = basis_[indices[i]];
  return selectedBasis;
}

/* Row filter accessor */
void DesignProxy::setRowFilter(const Indices & rowFilter)
{
  rowFilter.check(designCache_.getNbRows());
  rowFilter_ = rowFilter;
}

Indices DesignProxy::getRowFilter() const
{
  return rowFilter_;
}

/* Row filter flag accessor */
Bool DesignProxy::hasRowFilter() const
{
  return rowFilter_.getSize() > 0;
}

/* Effective size of sample */
UnsignedInteger DesignProxy::getSampleSize() const
{
  // Here we use the number of rows in design cache as it covers
  // both cases when the DesignProxy is constructed using a sample
  // or a matrix
  return hasRowFilter() ? rowFilter_.getSize() : designCache_.getNbRows();
}


/* Method save() stores the object through the StorageManager */
void DesignProxy::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "x_", x_ );
  adv.saveAttribute( "basis_", basis_ );
  adv.saveAttribute( "designCache_", designCache_ );
  adv.saveAttribute( "alreadyComputed_", alreadyComputed_ );
  adv.saveAttribute( "rowFilter_", rowFilter_ );
}


/* Method load() reloads the object from the StorageManager */
void DesignProxy::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "x_", x_ );
  adv.loadAttribute( "basis_", basis_ );
  adv.loadAttribute( "designCache_", designCache_ );
  adv.loadAttribute( "alreadyComputed_", alreadyComputed_ );
  adv.loadAttribute( "rowFilter_", rowFilter_ );
}
END_NAMESPACE_OPENTURNS
