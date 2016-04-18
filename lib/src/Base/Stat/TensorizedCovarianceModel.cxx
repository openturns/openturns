//                                               -*- C++ -*-
/**
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
#include "openturns/TensorizedCovarianceModel.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/AbsoluteExponential.hxx"
#include "openturns/ExponentialModel.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(TensorizedCovarianceModel);

static const Factory<TensorizedCovarianceModel> RegisteredFactory;


/* Default constructor */
TensorizedCovarianceModel::TensorizedCovarianceModel(const UnsignedInteger dimension)
  : CovarianceModelImplementation(1),
    collection_(dimension, AbsoluteExponential(1))
{
  setScale(NumericalPoint(spatialDimension_, 1.0));
  setAmplitude(NumericalPoint(dimension, 1));
}

/* Parameters constructor */
TensorizedCovarianceModel::TensorizedCovarianceModel(const CovarianceModelCollection & collection)
  : CovarianceModelImplementation()
{
  setCollection(collection);
  // Set same scale
  setScale(NumericalPoint(spatialDimension_, 1.0));
}

/** Parameters constructor */
TensorizedCovarianceModel::TensorizedCovarianceModel(const CovarianceModelCollection & collection,
    const NumericalPoint & scale)
{
  setCollection(collection);
  // Set same scale
  setScale(scale);
}

/* Collection accessor */
void TensorizedCovarianceModel::setCollection(const CovarianceModelCollection & collection)
{
  // Check if the given models have the same spatial dimension
  const UnsignedInteger size(collection.getSize());
  if (size == 0) throw InvalidArgumentException(HERE) << "TensorizedCovarianceModel::setCollection: the collection must have a positive size, here size=0";
  NumericalPoint amplitude(0);
  spatialDimension_ = collection[0].getSpatialDimension();
  // Get dimension: should be the same for all elements
  dimension_ = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger localSpatialDimension(collection[i].getSpatialDimension());
    if (spatialDimension_ != localSpatialDimension)
      throw InvalidArgumentException(HERE) << "In TensorizedCovarianceModel::setCollection, incompatible spatial dimension of the element #" << i
                                           << " spatial dimension of element = " << localSpatialDimension << ", spatial dimension of the model = " << spatialDimension_;

    const UnsignedInteger localDimension(collection[i].getDimension());
    dimension_ += localDimension;
    const NumericalPoint localAmplitude(collection[i].getAmplitude());
    amplitude.add(localAmplitude);
  }
  // Set collection
  collection_ = collection;
  // Set amplitude
  amplitude_ = amplitude;
}

const TensorizedCovarianceModel::CovarianceModelCollection & TensorizedCovarianceModel::getCollection() const
{
  return collection_;
}

/* Virtual constructor */
TensorizedCovarianceModel * TensorizedCovarianceModel::clone() const
{
  return new TensorizedCovarianceModel(*this);
}

/* Computation of the covariance density function */
CovarianceMatrix TensorizedCovarianceModel::operator() (const NumericalPoint & s,
    const NumericalPoint & t) const
{
  if (s.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << spatialDimension_;
  if (t.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << spatialDimension_;
  CovarianceMatrix covariance(getDimension());
  // Fill by block ==> block index
  UnsignedInteger blockIndex = 0;
  const UnsignedInteger size = collection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    // Compute the ith block
    const CovarianceMatrix localCovariance = collection_[i](s, t);
    const UnsignedInteger localDimension(collection_[i].getDimension());
    // Fill lower part of the covariance matrix
    for (UnsignedInteger localColumn = 0; localColumn < localDimension; ++localColumn)
      for (UnsignedInteger localRow = localColumn; localRow < localDimension; ++localRow)
        covariance(blockIndex + localRow, blockIndex + localColumn) = localCovariance(localRow, localColumn);
    // update blockIndex
    blockIndex += localDimension;
  }
  return covariance;
}

/* Gradient */
Matrix TensorizedCovarianceModel::partialGradient(const NumericalPoint & s,
    const NumericalPoint & t) const
{
  // Gradient definition results from definition of model
  // We should pay attention to the scaling factor scale_
  Matrix gradient(spatialDimension_, dimension_ * dimension_);
  UnsignedInteger dimension = 0;
  const UnsignedInteger size = collection_.getSize();
  for(UnsignedInteger k = 0; k < size; ++k)
  {
    const CovarianceModel localCovariance = collection_[k];
    const UnsignedInteger localDimension = localCovariance.getDimension();
    const Matrix gradient_k = localCovariance.partialGradient(s, t);
    // Gradient gradient_k is of size spatialDimension x localDimension^2
    for (UnsignedInteger localIndex = 0; localIndex < localDimension * localDimension; ++localIndex)
    {
      // Each row of the matrix gradient_k is a matrix localDimension x localDimension
      // presented as a collection of scalar of size localDimension^2
      // Objective is to get the corresponding localRowIndex & localColumnIndex
      const UnsignedInteger localRowIndex = localIndex % localDimension;
      const UnsignedInteger localColumnIndex = localIndex / localDimension;
      // We seek the corresponding rowIndex & localIndex, if the each row of the
      // matrix 'gradient' was a matrix of size dimension_ * dimension,
      const UnsignedInteger rowIndex = dimension + localRowIndex;
      const UnsignedInteger columnIndex = dimension + localColumnIndex;
      // With rowIndex & columnIndex, we get the index in collection
      // because data are presented as vector (or row matrix)
      const UnsignedInteger index = dimension_ * columnIndex + rowIndex;
      // Fill gradient matrix
      // Same index are used for all it rows
      for (UnsignedInteger spatialIndex = 0; spatialIndex < spatialDimension_; ++spatialIndex)
        gradient(spatialIndex, index) = gradient_k(spatialIndex, localIndex);
    }
    // update dimension
    dimension += localDimension;
  }

  // Return gradient
  return gradient;
}

/* Parameters accessor */
void TensorizedCovarianceModel::setParameter(const NumericalPoint & parameter)
{
  const UnsignedInteger parameterDimension(getParameter().getDimension());
  if (parameter.getDimension() != parameterDimension) throw InvalidArgumentException(HERE) << "Error: parameter dimension should be " << getParameter().getDimension()
        << " (got " << parameter.getDimension() << ")";
  NumericalPoint scale(spatialDimension_);
  NumericalPoint amplitude(dimension_);
  for (UnsignedInteger i = 0; i < scale_.getDimension(); ++i) scale[i] = parameter[i];
  for (UnsignedInteger i = 0; i < amplitude_.getDimension(); ++i) amplitude[i] = parameter[i + spatialDimension_];
  // set parameters
  setScale(scale);
  setAmplitude(amplitude);
}

NumericalPoint TensorizedCovarianceModel::getParameter() const
{
  // Same convention : scale than amplitude parameters
  NumericalPoint result(0);
  result.add(scale_);
  result.add(amplitude_);
  return result;

}

Description TensorizedCovarianceModel::getParameterDescription() const
{
  Description description(0);
  for (UnsignedInteger j = 0; j < scale_.getDimension(); ++j) description.add(OSS() << "scale_" << "_" << j);
  for (UnsignedInteger j = 0; j < amplitude_.getDimension(); ++j) description.add(OSS() << "amplitude_" << "_" << j);
  return description;

}

void TensorizedCovarianceModel::setScale(const NumericalPoint & scale)
{
  if (scale.getDimension() != getSpatialDimension())
    throw InvalidArgumentException(HERE) << "In TensorizedCovarianceModel::setScale, incompatible dimension of the scale vector. Expected scale of size = " << spatialDimension_
                                         << ", vector size = " << scale.getDimension();

  // Same scale fot all models
  for (UnsignedInteger i = 0; i < collection_.getSize(); ++i)
  {
    collection_[i].setScale(scale);
  }
  // Copy of scale but not used
  scale_ = scale;
}

/** Amplitude accessor */
void TensorizedCovarianceModel::setAmplitude(const NumericalPoint & amplitude)
{
  if (amplitude.getDimension() != getDimension())
    throw InvalidArgumentException(HERE) << "In TensorizedCovarianceModel::setAmplitude, incompatible dimension of the amplitude vector. Expected amplitude of size = " << dimension_
                                         << ", vector size = " << amplitude.getDimension();

  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < collection_.getSize(); ++i)
  {
    const UnsignedInteger localDimension(collection_[i].getDimension());
    NumericalPoint localAmplitude(collection_[i].getAmplitude());
    for (UnsignedInteger j = 0; j < localDimension; ++j)
    {
      localAmplitude[j] = amplitude[index];
      index += 1;
    }
    collection_[i].setAmplitude(localAmplitude);
  }
  // Copy of amplitude but not used
  amplitude_ = amplitude;
}

/* Is it a stationary model ? */
Bool TensorizedCovarianceModel::isStationary() const
{
  for (UnsignedInteger i = 0; i < collection_.getSize(); ++i)
    if (!collection_[i].isStationary()) return false;
  return true;
}

/** Is it a diagonal covariance model ? */
Bool TensorizedCovarianceModel::isDiagonal() const
{
  for (UnsignedInteger i = 0; i < collection_.getSize(); ++i)
  {
    if (!collection_[i].isDiagonal()) return false;
  }
  return true;
}

/* String converter */
String TensorizedCovarianceModel::__repr__() const
{
  OSS oss;
  oss << "class=" << TensorizedCovarianceModel::GetClassName()
      << " input dimension=" << spatialDimension_
      << ", dimension = " << dimension_
      << ", models=" << collection_;
  return oss;
}

/* String converter */
String TensorizedCovarianceModel::__str__(const String & offset) const
{
  return __repr__();
}

/* Marginal accessor */
TensorizedCovarianceModel::Implementation TensorizedCovarianceModel::getMarginal(const UnsignedInteger index) const
{
  if (index >= dimension_)
    throw InvalidArgumentException(HERE) << "Error: index=" << index << " must be less than output dimension=" << dimension_;
  UnsignedInteger size = collection_.getSize();
  UnsignedInteger start = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger localDimension = collection_[i].getDimension();
    const UnsignedInteger stop = start + localDimension;
    if (index < stop) return collection_[i].getMarginal(index - start).getImplementation();
    start = stop;
  }
  throw InternalException(HERE) << "In TensorizedCovarianceModel::getMarginal: could not return the marginal";
}

/* Method save() stores the object through the StorageManager */
void TensorizedCovarianceModel::save(Advocate & adv) const
{
  CovarianceModelImplementation::save(adv);
  adv.saveAttribute("collection_", collection_);
}

/* Method load() reloads the object from the StorageManager */
void TensorizedCovarianceModel::load(Advocate & adv)
{
  CovarianceModelImplementation::load(adv);
  adv.loadAttribute("collection_", collection_);
}

END_NAMESPACE_OPENTURNS
