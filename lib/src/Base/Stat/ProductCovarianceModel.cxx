//                                               -*- C++ -*-
/**
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
#include "ProductCovarianceModel.hxx"
#include "PersistentObjectFactory.hxx"
#include "Exception.hxx"
#include "AbsoluteExponential.hxx"

BEGIN_NAMESPACE_OPENTURNS

TEMPLATE_CLASSNAMEINIT(PersistentCollection< CovarianceModel >);

static const Factory<PersistentCollection<CovarianceModel> > RegisteredFactory1;

CLASSNAMEINIT(ProductCovarianceModel);

static const Factory<ProductCovarianceModel> RegisteredFactory;


/* Default constructor */
ProductCovarianceModel::ProductCovarianceModel(const UnsignedInteger spatialDimension)
  : CovarianceModelImplementation(spatialDimension)
  , collection_(spatialDimension, AbsoluteExponential(1))
{
  // Update the default values for the amplitude
  setAmplitude(NumericalPoint(spatialDimension, collection_[0].getAmplitude()[0]));
}

/* Parameters constructor */
ProductCovarianceModel::ProductCovarianceModel(const CovarianceModelCollection & collection)
  : CovarianceModelImplementation()
{
  setCollection(collection);
}

/* Collection accessor */
void ProductCovarianceModel::setCollection(const CovarianceModelCollection & collection)
{
  // Check if the given models have a spatial dimension=1
  const UnsignedInteger size(collection.getSize());
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: the collection must have a positive size, here size=0";
  NumericalPoint amplitude(0);
  spatialDimension_ = 0;
  // Get dimension: should be the same for all elements
  dimension_ = 1;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger localDimension(collection[i].getDimension());
    if (dimension_ != localDimension)
      throw InvalidArgumentException(HERE) << "In ProductCovarianceModel::setCollection, incompatible dimension of the element #" << i
                                           << " dimension of element = " << localDimension << ", dimension of the model = " << dimension_;
    const UnsignedInteger localSpatialDimension(collection[i].getSpatialDimension());
    const NumericalPoint localAmplitude(collection[i].getAmplitude());
    for (UnsignedInteger j = 0; j < localDimension; ++j)
    {
      amplitude.add(localAmplitude[j]);
    }
    spatialDimension_ += localSpatialDimension;
  }
  setAmplitude(amplitude);
  collection_ = collection;
}

const ProductCovarianceModel::CovarianceModelCollection & ProductCovarianceModel::getCollection() const
{
  return collection_;
}

/* Virtual constructor */
ProductCovarianceModel * ProductCovarianceModel::clone() const
{
  return new ProductCovarianceModel(*this);
}

/* Computation of the covariance density function */
CovarianceMatrix ProductCovarianceModel::operator() (const NumericalPoint & s,
    const NumericalPoint & t) const
{
  if (s.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << spatialDimension_;
  if (t.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << spatialDimension_;

  NumericalScalar sigma2 = 1.0;
  UnsignedInteger start(0);
  for (UnsignedInteger i = 0; i < collection_.getSize(); ++i)
  {
    const UnsignedInteger localSpatialDimension(collection_[i].getSpatialDimension());
    const UnsignedInteger stop(start + localSpatialDimension);
    NumericalPoint localS(localSpatialDimension);
    std::copy(s.begin() + start, s.begin() + stop, localS.begin());
    NumericalPoint localT(localSpatialDimension);
    std::copy(t.begin() + start, t.begin() + stop, localT.begin());
    sigma2 *= collection_[i](localS, localT)(0, 0);
    start = stop;
  }
  CovarianceMatrix covariance(1);
  covariance(0, 0) = sigma2;
  return covariance;
}

/* Gradient */
Matrix ProductCovarianceModel::partialGradient(const NumericalPoint & s,
    const NumericalPoint & t) const
{
  if (s.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << spatialDimension_;
  if (t.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << spatialDimension_;

  const UnsignedInteger size(collection_.getSize());
  NumericalPoint localCovariances(size);
  NumericalScalar leftValue(1.0);
  NumericalScalar rightValue(1.0);
  UnsignedInteger start = 0;
  Matrix gradient(spatialDimension_, 1);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger localSpatialDimension(collection_[i].getSpatialDimension());
    const UnsignedInteger stop(start + localSpatialDimension);
    NumericalPoint localS(localSpatialDimension);
    std::copy(s.begin() + start, s.begin() + stop, localS.begin());
    NumericalPoint localT(localSpatialDimension);
    std::copy(t.begin() + start, t.begin() + stop, localT.begin());
    Matrix localGradient(collection_[i].partialGradient(localS, localT));
    for (UnsignedInteger j = 0; j < localSpatialDimension; ++j) localGradient(j, 0) *= leftValue;
    std::copy(localGradient.getImplementation()->begin(), localGradient.getImplementation()->end(), gradient.getImplementation()->begin() + start);
    localCovariances[i] = collection_[i](localS, localT)(0, 0);
    leftValue *= localCovariances[i];
    start = stop;
  }
  // Decrement
  start = getSpatialDimension();
  // Second step
  for (UnsignedInteger i = size; i > 0; --i)
  {
    const UnsignedInteger localSpatialDimension(collection_[i - 1].getSpatialDimension());
    start -= localSpatialDimension;
    for (UnsignedInteger j = 0; j < localSpatialDimension; ++j) gradient(start + j, 0) *= rightValue;
    rightValue *= localCovariances[i - 1];
  }
  return gradient;
}

/* Parameters accessor */
void ProductCovarianceModel::setParameters(const NumericalPoint & parameters)
{
  const UnsignedInteger parametersDimension(getParameters().getDimension());
  if (parameters.getDimension() != parametersDimension) throw InvalidArgumentException(HERE) << "Error: parameters dimension should be 1 (got " << parameters.getDimension() << ")";
  UnsignedInteger start(0);
  for (UnsignedInteger i = 0; i < collection_.getSize(); ++i)
  {
    const UnsignedInteger atomParametersDimension(collection_[i].getParameters().getDimension());
    const UnsignedInteger stop(start + atomParametersDimension);
    NumericalPoint atomParameters(atomParametersDimension);
    std::copy(parameters.begin() + start, parameters.begin() + stop, atomParameters.begin());
    start = stop;
    collection_[i].setParameters(atomParameters);
  }
}

NumericalPointWithDescription ProductCovarianceModel::getParameters() const
{
  NumericalPointWithDescription result(0);
  Description description(0);
  const UnsignedInteger size(collection_.getSize());
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const NumericalPointWithDescription atomParameters(collection_[i].getParameters());
    const Description atomDescription(atomParameters.getDescription());
    result.add(atomParameters);
    for (UnsignedInteger j = 0; j < atomDescription.getSize(); ++j)
      description.add(OSS() << "model_" << i << "_" << atomDescription[j]);
  }
  result.setDescription(description);
  return result;
}

void ProductCovarianceModel::setScale(const NumericalPoint & scale)
{
  const UnsignedInteger scaleDimension(getScale().getDimension());
  if (scale.getDimension() != scaleDimension) throw InvalidArgumentException(HERE) << "Error: scale dimension should be " << scaleDimension << ". Here we got " << scale.getDimension();
  UnsignedInteger start(0);
  for (UnsignedInteger i = 0; i < collection_.getSize(); ++i)
  {
    const UnsignedInteger atomScaleDimension(collection_[i].getScale().getDimension());
    const UnsignedInteger stop(start + atomScaleDimension);
    NumericalPoint atomScale(atomScaleDimension);
    std::copy(scale.begin() + start, scale.begin() + stop, atomScale.begin());
    start = stop;
    collection_[i].setScale(atomScale);
  }
}

NumericalPoint ProductCovarianceModel::getScale() const
{
  NumericalPoint scale(0);
  const UnsignedInteger size(collection_.getSize());
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const NumericalPoint atomScale(collection_[i].getScale());
    scale.add(atomScale);
  }
  return scale;
}

/* Is it a stationary model ? */
Bool ProductCovarianceModel::isStationary() const
{
  for (UnsignedInteger i = 0; i < collection_.getSize(); ++i)
    if (!collection_[i].isStationary()) return false;
  return true;
}

/* String converter */
String ProductCovarianceModel::__repr__() const
{
  OSS oss;
  oss << "class=" << ProductCovarianceModel::GetClassName()
      << " input dimension=" << spatialDimension_
      << " models=" << collection_;
  return oss;
}

/* String converter */
String ProductCovarianceModel::__str__(const String & offset) const
{
  return __repr__();
}

/* Marginal accessor */
ProductCovarianceModel::Implementation ProductCovarianceModel::getMarginal(const UnsignedInteger index) const
{
  if (index >= dimension_) throw InvalidArgumentException(HERE) << "Error: index=" << index << " must be less than output dimension=" << dimension_;
  return collection_[index].getImplementation();
}

/* Method save() stores the object through the StorageManager */
void ProductCovarianceModel::save(Advocate & adv) const
{
  CovarianceModelImplementation::save(adv);
  adv.saveAttribute("collection_", collection_);
}

/* Method load() reloads the object from the StorageManager */
void ProductCovarianceModel::load(Advocate & adv)
{
  CovarianceModelImplementation::load(adv);
  adv.loadAttribute("collection_", collection_);
}

END_NAMESPACE_OPENTURNS
