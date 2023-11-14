//                                               -*- C++ -*-
/**
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
  isStationary_ = true;
}

/* Parameters constructor */
TensorizedCovarianceModel::TensorizedCovarianceModel(const CovarianceModelCollection & collection)
  : CovarianceModelImplementation()
{
  setCollection(collection);
  scale_ = Point(inputDimension_, 1.0);
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
  // Check if the given models have the same input dimension
  const UnsignedInteger size = collection.getSize();
  if (!(size > 0)) throw InvalidArgumentException(HERE) << "TensorizedCovarianceModel::setCollection: the collection must have a positive size, here size=0";
  Point amplitude(0);
  inputDimension_ = collection[0].getInputDimension();
  // Get dimension: should be the same for all elements
  // Since 1.17, collection should be a list of 1d output models
  outputDimension_ = size;
  amplitude_ = Point(size);
  isStationary_ = true;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger localSpatialDimension = collection[i].getInputDimension();
    if (inputDimension_ != localSpatialDimension)
      throw InvalidArgumentException(HERE) << "In TensorizedCovarianceModel::setCollection, incompatible input dimension of the element #" << i
                                           << " input dimension of element = " << localSpatialDimension << ", input dimension of the model = " << inputDimension_;

    const UnsignedInteger localDimension = collection[i].getOutputDimension();
    if (localDimension != 1)
      throw InvalidArgumentException(HERE) << "In TensorizedCovarianceModel::setCollection, collection should only contains models of output dimension = 1."
                                           << " The output dimension of the #" << i << " element is = " << localDimension;
    const Scalar localAmplitude = collection[i].getAmplitude()[0];
    // Set amplitude
    amplitude_[i] = localAmplitude;
    if (!collection[i].isStationary())
      isStationary_ = false;
  }
  // Set collection
  collection_ = collection;
}

TensorizedCovarianceModel::CovarianceModelCollection TensorizedCovarianceModel::getCollection() const
{
  return collection_;
}

/* Virtual constructor */
TensorizedCovarianceModel * TensorizedCovarianceModel::clone() const
{
  return new TensorizedCovarianceModel(*this);
}

Scalar TensorizedCovarianceModel::computeAsScalar(const Point &s,
    const Point &t) const
{
  if (outputDimension_ != 1)
    throw InvalidArgumentException(HERE) << "TensorizedCovarianceModel::computeAsScalar(s,t) should be used only if output dimension is 1."
                                         << "Here, output dimension = " << outputDimension_;
  return collection_[0].computeAsScalar(s, t);
}

Scalar TensorizedCovarianceModel::computeAsScalar(const Point &tau) const
{
  if (outputDimension_ != 1)
    throw InvalidArgumentException(HERE) << "TensorizedCovarianceModel::computeAsScalar(s,t) should be used only if output dimension is 1."
                                         << "Here, output dimension = " << outputDimension_;
  return collection_[0].computeAsScalar(tau);
}

Scalar TensorizedCovarianceModel::computeAsScalar(const Collection<Scalar>::const_iterator &s_begin,
    const Collection<Scalar>::const_iterator &t_begin) const
{
  if (outputDimension_ != 1)
    throw InvalidArgumentException(HERE) << "TensorizedCovarianceModel::computeAsScalar(s,t) should be used only if output dimension is 1."
                                         << "Here, output dimension = " << outputDimension_;
  return collection_[0].getImplementation()->computeAsScalar(s_begin, t_begin);
}
Scalar TensorizedCovarianceModel::computeAsScalar(const Scalar tau) const
{
  if (inputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has input dimension=" << inputDimension_ << ", expected input dimension=1.";
  if (outputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has output dimension=" << outputDimension_ << ", expected dimension=1.";
  return collection_[0].getImplementation()->computeAsScalar(tau);
}

/* Computation of the covariance density function */
SquareMatrix TensorizedCovarianceModel::operator() (const Point & s,
    const Point & t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;
  SquareMatrix covariance(getOutputDimension());
  const UnsignedInteger size = collection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    covariance(i, i) = collection_[i].computeAsScalar(s, t);
  }
  return covariance;
}

/* Computation of the covariance density function */
SquareMatrix TensorizedCovarianceModel::operator()(const Point &tau) const
{
  if (!isStationary())
    return CovarianceModelImplementation::operator()(tau);
  if (tau.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "Error: the point tau has dimension=" << tau.getDimension() << ", expected dimension=" << inputDimension_;
  SquareMatrix covariance(getOutputDimension());
  const UnsignedInteger size = collection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    covariance(i, i) = collection_[i].computeAsScalar(tau);
  }
  return covariance;
}

/* Gradient */
Matrix TensorizedCovarianceModel::partialGradient(const Point & s,
    const Point & t) const
{
  // Gradient definition results from definition of model
  // We should pay attention to the scaling factor scale_
  Matrix gradient(inputDimension_, outputDimension_ * outputDimension_);
  const UnsignedInteger size = collection_.getSize();
  for(UnsignedInteger k = 0; k < size; ++k)
  {
    const CovarianceModel localCovariance(collection_[k]);
    const Matrix gradient_k(localCovariance.partialGradient(s, t));
    // Gradient gradient_k is of size inputDimension x 1
    // operator() yields diagonal matrix ==> cov(k,k)
    // cov(k, k) corresponds to the element of index k * dim + k
    const UnsignedInteger columnIndex = k + outputDimension_ * k;
    for (UnsignedInteger i = 0; i < inputDimension_; ++i)
      gradient(i, columnIndex) = gradient_k(i, 0);
  }
  // Return gradient
  return gradient;
}

/* Parameters accessor */
void TensorizedCovarianceModel::setFullParameter(const Point & parameter)
{
  const UnsignedInteger parameterDimension = getFullParameter().getSize();

  if (parameter.getDimension() != parameterDimension) throw InvalidArgumentException(HERE) << "Error: parameter dimension should be " << getParameter().getDimension()
        << " (got " << parameter.getDimension() << ")";
  Point scale(inputDimension_);
  Point amplitude(outputDimension_);
  for (UnsignedInteger i = 0; i < scale_.getDimension(); ++i) scale[i] = parameter[i];
  for (UnsignedInteger i = 0; i < amplitude_.getDimension(); ++i) amplitude[i] = parameter[i + inputDimension_];
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
  if (scale.getDimension() != getInputDimension())
    throw InvalidArgumentException(HERE) << "In TensorizedCovarianceModel::setScale, incompatible dimension of the scale vector. Expected scale of size = " << inputDimension_
                                         << ", vector size = " << scale.getDimension();

  Point scale0(collection_[0].getScale());
  collection_[0].setScale(scale);
  for (UnsignedInteger i = 1; i < collection_.getSize(); ++i)
  {
    Point newScale(collection_[i].getScale());
    for (UnsignedInteger j = 0; j < inputDimension_; ++j)
      newScale[j] *= scale[j] / scale0[j];
    collection_[i].setScale(newScale);
  }
  // Copy of scale but not used
  scale_ = scale;
}

/** Amplitude accessor */
void TensorizedCovarianceModel::setAmplitude(const Point & amplitude)
{
  if (amplitude.getDimension() != getOutputDimension())
    throw InvalidArgumentException(HERE) << "In TensorizedCovarianceModel::setAmplitude, incompatible dimension of the amplitude vector. Expected amplitude of size = " << outputDimension_
                                         << ", vector size = " << amplitude.getDimension();

  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < collection_.getSize(); ++i)
  {
    const UnsignedInteger localDimension = collection_[i].getOutputDimension();
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
  return isStationary_;
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

/* Is it safe to compute discretize etc in parallel? */
Bool TensorizedCovarianceModel::isParallel() const
{
  for (UnsignedInteger i = 0; i < collection_.getSize(); ++i)
    if (!collection_[i].getImplementation()->isParallel()) return false;
  return true;
}

/* String converter */
String TensorizedCovarianceModel::__repr__() const
{
  OSS oss;
  oss << "class=" << TensorizedCovarianceModel::GetClassName()
      << " input dimension=" << inputDimension_
      << ", dimension = " << outputDimension_
      << ", models=" << collection_;
  return oss;
}

/* String converter */
String TensorizedCovarianceModel::__str__(const String & ) const
{
  return __repr__();
}

/* Marginal accessor */
CovarianceModel TensorizedCovarianceModel::getMarginal(const UnsignedInteger index) const
{
  if (!(index < outputDimension_))
    throw InvalidArgumentException(HERE) << "Error: index=" << index << " must be less than output dimension=" << outputDimension_;
  UnsignedInteger size = collection_.getSize();
  UnsignedInteger start = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger localDimension = collection_[i].getOutputDimension();
    const UnsignedInteger stop = start + localDimension;
    if (index < stop) return collection_[i].getMarginal(index - start);
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
