//                                               -*- C++ -*-
/**
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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

CLASSNAMEINIT(TensorizedCovarianceModel)

static const Factory<TensorizedCovarianceModel> Factory_TensorizedCovarianceModel;


/* Default constructor */
TensorizedCovarianceModel::TensorizedCovarianceModel(const UnsignedInteger dimension)
  : CovarianceModelImplementation(1)
  , collection_(dimension, AbsoluteExponential(1))
{
  setAmplitude(Point(dimension, 1));

  activeParameter_ = Indices(getScale().getSize() + getAmplitude().getSize());
  activeParameter_.fill();
}

/* Parameters constructor */
TensorizedCovarianceModel::TensorizedCovarianceModel(const CovarianceModelCollection & collection)
  : CovarianceModelImplementation()
{
  setCollection(collection);
  activeParameter_ = Indices(getScale().getSize() + getAmplitude().getSize());
  activeParameter_.fill();
}

/** Parameters constructor */
TensorizedCovarianceModel::TensorizedCovarianceModel(const CovarianceModelCollection & collection,
    const Point & scale)
  : CovarianceModelImplementation()
{
  setCollection(collection);
  setScale(scale);

  activeParameter_ = Indices(getScale().getSize() + getAmplitude().getSize());
  activeParameter_.fill();
}

/* Collection accessor */
void TensorizedCovarianceModel::setCollection(const CovarianceModelCollection & collection)
{
  // Check if the given models have the same spatial dimension
  const UnsignedInteger size = collection.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "TensorizedCovarianceModel::setCollection: the collection must have a positive size, here size=0";
  Point amplitude(0);
  spatialDimension_ = collection[0].getSpatialDimension();
  // Get dimension: should be the same for all elements
  dimension_ = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger localSpatialDimension = collection[i].getSpatialDimension();
    if (spatialDimension_ != localSpatialDimension)
      throw InvalidArgumentException(HERE) << "In TensorizedCovarianceModel::setCollection, incompatible spatial dimension of the element #" << i
                                           << " spatial dimension of element = " << localSpatialDimension << ", spatial dimension of the model = " << spatialDimension_;

    const UnsignedInteger localDimension = collection[i].getDimension();
    dimension_ += localDimension;
    const Point localAmplitude(collection[i].getAmplitude());
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
CovarianceMatrix TensorizedCovarianceModel::operator() (const Point & s,
    const Point & t) const
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
    const UnsignedInteger localDimension = collection_[i].getDimension();
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
Matrix TensorizedCovarianceModel::partialGradient(const Point & s,
    const Point & t) const
{
  // Gradient definition results from definition of model
  // We should pay attention to the scaling factor scale_
  Matrix gradient(dimension_ * dimension_, spatialDimension_);
  MatrixImplementation & gradientImpl(*gradient.getImplementation());
  UnsignedInteger dimension = 0;
  const UnsignedInteger size = collection_.getSize();
  for(UnsignedInteger k = 0; k < size; ++k)
  {
    const CovarianceModel localCovariance = collection_[k];
    const Matrix gradient_k(localCovariance.partialGradient(s, t));
    const MatrixImplementation & gradient_k_Impl(*gradient_k.getImplementation());
    const UnsignedInteger localDimension = localCovariance.getDimension();
    // Gradient gradient_k is of size localDimension^2 x spatialDimension
    for (UnsignedInteger spatialIndex = 0; spatialIndex < spatialDimension_; ++spatialIndex)
    {
      for (UnsignedInteger localColumnIndex = 0; localColumnIndex < localDimension; ++localColumnIndex)
      {
        const UnsignedInteger localStart = localDimension * localColumnIndex;
        const UnsignedInteger start = dimension_ * (dimension + localColumnIndex) + dimension;
        std::copy(&gradient_k_Impl(localStart, spatialIndex), &gradient_k_Impl(localStart, spatialIndex) + localDimension, &gradientImpl(start, spatialIndex));
      }
    }
    // update dimension
    dimension += localDimension;
  }

  // Return gradient
  return gradient;
}

/* Parameters accessor */
void TensorizedCovarianceModel::setFullParameter(const Point & parameter)
{
  const UnsignedInteger parameterDimension = getParameter().getDimension();
  if (parameter.getDimension() != parameterDimension) throw InvalidArgumentException(HERE) << "Error: parameter dimension should be " << getParameter().getDimension()
        << " (got " << parameter.getDimension() << ")";
  Point scale(spatialDimension_);
  Point amplitude(dimension_);
  for (UnsignedInteger i = 0; i < scale_.getDimension(); ++i) scale[i] = parameter[i];
  for (UnsignedInteger i = 0; i < amplitude_.getDimension(); ++i) amplitude[i] = parameter[i + spatialDimension_];
  // set parameters
  setScale(scale);
  setAmplitude(amplitude);
}

Point TensorizedCovarianceModel::getFullParameter() const
{
  // Same convention : scale then amplitude parameters
  Point result(0);
  result.add(scale_);
  result.add(amplitude_);
  return result;
}

Description TensorizedCovarianceModel::getFullParameterDescription() const
{
  Description description(0);
  for (UnsignedInteger j = 0; j < scale_.getDimension(); ++ j) description.add(OSS() << "scale_" << j);
  for (UnsignedInteger j = 0; j < amplitude_.getDimension(); ++ j) description.add(OSS() << "amplitude_" << j);
  return description;
}

void TensorizedCovarianceModel::setScale(const Point & scale)
{
  if (scale.getDimension() != getSpatialDimension())
    throw InvalidArgumentException(HERE) << "In TensorizedCovarianceModel::setScale, incompatible dimension of the scale vector. Expected scale of size = " << spatialDimension_
                                         << ", vector size = " << scale.getDimension();

  Point scale0(collection_[0].getScale());
  collection_[0].setScale(scale);
  for (UnsignedInteger i = 1; i < collection_.getSize(); ++i)
  {
    Point newScale(collection_[i].getScale());
    for (UnsignedInteger j = 0; j < spatialDimension_; ++j)
      newScale[j] *= scale[j] / scale0[j];
    collection_[i].setScale(newScale);
  }
  // Copy of scale but not used
  scale_ = scale;
}

/** Amplitude accessor */
void TensorizedCovarianceModel::setAmplitude(const Point & amplitude)
{
  if (amplitude.getDimension() != getDimension())
    throw InvalidArgumentException(HERE) << "In TensorizedCovarianceModel::setAmplitude, incompatible dimension of the amplitude vector. Expected amplitude of size = " << dimension_
                                         << ", vector size = " << amplitude.getDimension();

  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < collection_.getSize(); ++i)
  {
    const UnsignedInteger localDimension = collection_[i].getDimension();
    Point localAmplitude(collection_[i].getAmplitude());
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
