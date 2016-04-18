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
#include "openturns/ProductCovarianceModel.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/AbsoluteExponential.hxx"

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
  const UnsignedInteger size = collection.getSize();
  if (size == 0)
    throw InvalidArgumentException(HERE) << "Error: the collection must have a positive size, here size=0";
  // Scale & amplitude
  NumericalPoint scale(0);
  NumericalPoint amplitude(1, 1.0);
  spatialDimension_ = 0;
  // Get dimension: should be the same for all elements
  dimension_ = 1;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger localDimension = collection[i].getDimension();
    if (dimension_ != localDimension)
      throw InvalidArgumentException(HERE) << "In ProductCovarianceModel::setCollection, incompatible dimension of the element #" << i
                                           << " dimension of element = " << localDimension << ", dimension of the model = " << dimension_;
    // Add element to the collection
    // Get its scale, which is seen as a concatenation of collection scale
    const UnsignedInteger localSpatialDimension = collection[i].getSpatialDimension();
    spatialDimension_ += localSpatialDimension;
    scale.add(collection[i].getScale());
    // Get amplitude as amplitude product
    const NumericalScalar localAmplitude = collection[i].getAmplitude()[0];
    amplitude[0] *= localAmplitude;
  }
  // Set collection
  collection_ = collection;
  // Set amplitude & scale
  scale_ = scale;
  setAmplitude(amplitude);
  // Fix all submodels as correlation models
  for (UnsignedInteger i = 0; i < size; ++i) collection_[i].setAmplitude(NumericalPoint(1, 1.0));
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
NumericalScalar ProductCovarianceModel::computeStandardRepresentative(const NumericalPoint & s,
                                                                      const NumericalPoint & t) const
{
  if (s.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << spatialDimension_;
  if (t.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << spatialDimension_;

  NumericalScalar rho = 1.0;
  UnsignedInteger start = 0;
  for (UnsignedInteger i = 0; i < collection_.getSize(); ++i)
  {
    const UnsignedInteger localSpatialDimension = collection_[i].getSpatialDimension();
    const UnsignedInteger stop = start + localSpatialDimension;
    NumericalPoint localS(localSpatialDimension);
    std::copy(s.begin() + start, s.begin() + stop, localS.begin());
    NumericalPoint localT(localSpatialDimension);
    std::copy(t.begin() + start, t.begin() + stop, localT.begin());
    rho *= collection_[i].computeStandardRepresentative(localS, localT);
    start = stop;
  }
  return rho;
}

/* Gradient */
Matrix ProductCovarianceModel::partialGradient(const NumericalPoint & s,
                                               const NumericalPoint & t) const
{
  if (s.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << spatialDimension_;
  if (t.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << spatialDimension_;

  const UnsignedInteger size = collection_.getSize();
  NumericalPoint localCovariances(size);
  NumericalScalar leftValue = 1.0;
  NumericalScalar rightValue = 1.0;
  UnsignedInteger start = 0;
  Matrix gradient(spatialDimension_, 1);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger localSpatialDimension = collection_[i].getSpatialDimension();
    const UnsignedInteger stop = start + localSpatialDimension;
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
    const UnsignedInteger localSpatialDimension = collection_[i - 1].getSpatialDimension();
    start -= localSpatialDimension;
    for (UnsignedInteger j = 0; j < localSpatialDimension; ++j) gradient(start + j, 0) *= rightValue;
    rightValue *= localCovariances[i - 1];
  }
  return gradient;
}

/* Parameters accessor */
void ProductCovarianceModel::setParameter(const NumericalPoint & parameter)
{
  const UnsignedInteger parameterDimension = getParameter().getDimension();
  if (parameter.getDimension() != parameterDimension)
      throw InvalidArgumentException(HERE) << "Error: parameters dimension should be 1 (got " << parameter.getDimension() << ")";
  // Convention is the following :
  // Scale parameters than amplitude parameter
  // As it is a 1 d model, scale size = parameterDimension - 1
  NumericalPoint scale(parameterDimension - 1);
  for (UnsignedInteger i = 0; i < parameterDimension - 1; ++i)
  {
    scale[i] = parameter[i];
  }
  setScale(scale);
  // last value of corresponds to amplitude
  setAmplitude(NumericalPoint(1, parameter[parameterDimension - 1]));
}

NumericalPoint ProductCovarianceModel::getParameter() const
{
  // Convention scale + amplitude
  // local amplitudes is 1
  // amplitude vector is of size 1
  NumericalPoint result(0);
  result.add(scale_);
  result.add(amplitude_);
  return result;
}

Description ProductCovarianceModel::getParameterDescription() const
{
  const UnsignedInteger size = spatialDimension_ + 1;
  Description description(size);
  for (UnsignedInteger i = 0; i < size - 1; ++i)
      description[i] = OSS() << "scale_" << i;
  // Last element is amplitude
  description[size-1] = "amplitude";
  return description;
}

void ProductCovarianceModel::setScale(const NumericalPoint & scale)
{
  if (scale.getDimension() != spatialDimension_)
    throw InvalidArgumentException(HERE) << "Error: scale dimension should be " << spatialDimension_ << ". Here we got " << scale.getDimension();
  // Set the scale
  UnsignedInteger start = 0;
  for (UnsignedInteger i = 0; i < collection_.getSize(); ++i)
  {
    const UnsignedInteger atomScaleDimension = collection_[i].getScale().getDimension();
    const UnsignedInteger stop = start + atomScaleDimension;
    NumericalPoint atomScale(atomScaleDimension);
    std::copy(scale.begin() + start, scale.begin() + stop, atomScale.begin());
    start = stop;
    collection_[i].setScale(atomScale);
  }
  // Copy scale (for get accessor)
  scale_ = scale;
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
