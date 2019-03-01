//                                               -*- C++ -*-
/**
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/ProductCovarianceModel.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/AbsoluteExponential.hxx"

BEGIN_NAMESPACE_OPENTURNS

TEMPLATE_CLASSNAMEINIT(PersistentCollection< CovarianceModel >)

static const Factory<PersistentCollection<CovarianceModel> > Factory_PersistentCollection_CovarianceModel;

CLASSNAMEINIT(ProductCovarianceModel)

static const Factory<ProductCovarianceModel> Factory_ProductCovarianceModel;


/* Default constructor */
ProductCovarianceModel::ProductCovarianceModel(const UnsignedInteger inputDimension)
  : CovarianceModelImplementation(inputDimension)
  , collection_(inputDimension, AbsoluteExponential(1))
  , extraParameterNumber_(inputDimension)
{
  if (!(inputDimension > 0))
    throw InvalidArgumentException(HERE) << "Error: input dimension must be positive, here inputDimension=0";
  definesComputeStandardRepresentative_ = true;
  // scale attribut
  scale_ = Point(inputDimension, collection_[0].getScale()[0]);
  // Update the default values for the amplitude
  setAmplitude(Point(1, collection_[0].getAmplitude()[0]));
  // Active parameters : scale + amplitude
  activeParameter_ = Indices(inputDimension + 1);
  activeParameter_.fill();
}

/* Parameters constructor */
ProductCovarianceModel::ProductCovarianceModel(const CovarianceModelCollection & collection)
  : CovarianceModelImplementation()
  , collection_(0)
  , extraParameterNumber_(0)
{
  definesComputeStandardRepresentative_ = true;
  setCollection(collection);
}

/* Collection accessor */
void ProductCovarianceModel::setCollection(const CovarianceModelCollection & collection)
{
  // Check if the given models have an input dimension=1
  const UnsignedInteger size = collection.getSize();
  if (size == 0)
    throw InvalidArgumentException(HERE) << "Error: the collection must have a positive size, here size=0";
  // Scale & amplitude
  Point scale(0);
  Point amplitude(1, 1.0);
  inputDimension_ = 0;
  // Get dimension: should be the same for all elements
  outputDimension_ = 1;

  // checking if amplitude parameter should be active
  // Value is True if one of the marginal models activate it
  Bool isAmplitudeActive = false;
  // Handle 'specific' parameters
  extraParameterNumber_ = Indices(collection.getSize());

  // check that scale parametrizations match
  const ScaleParametrization scaleParametrization = collection[0].getScaleParametrization();
  for (UnsignedInteger i = 1; i < size; ++ i)
    if (collection[i].getScaleParametrization() != scaleParametrization)
      throw InvalidArgumentException(HERE) << "ProductCovarianceModel does not allow different scale parametrizations";

  // Filling the active parameters
  activeParameter_ = Indices(0);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger localOutputDimension = collection[i].getOutputDimension();
    if (outputDimension_ != localOutputDimension)
      throw InvalidArgumentException(HERE) << "In ProductCovarianceModel::setCollection, incompatible dimension of the element #" << i
                                           << " dimension of element = " << localOutputDimension << ", dimension of the model = " << outputDimension_;
    // Add element to the collection
    // Get its scale, which is seen as a concatenation of collection scale
    const UnsignedInteger localInputDimension = collection[i].getInputDimension();
    scale.add(collection[i].getScale());
    // Get amplitude as amplitude product
    const Scalar localAmplitude = collection[i].getAmplitude()[0];
    amplitude[0] *= localAmplitude;

    // Get local active parameter
    const Indices localActiveParameter(collection[i].getActiveParameter());
    // Check if scale are active
    for (UnsignedInteger j = 0; j < localInputDimension; ++j)
    {
      if (localActiveParameter.contains(j))
        activeParameter_.add(inputDimension_ + j);
    }
    inputDimension_ += localInputDimension;

    // Should we activate the amplitude parameter?
    isAmplitudeActive = isAmplitudeActive || localActiveParameter.contains(localInputDimension);

    // Number of specific parameter
    extraParameterNumber_[i] = collection[i].getFullParameter().getSize() - (localInputDimension + 1);
  }

  // Amplitude active
  if (isAmplitudeActive)
    activeParameter_.add(inputDimension_);

  // Handle active extra parameters
  UnsignedInteger index = inputDimension_ + 1;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger localInputDimension = collection[i].getInputDimension();
    const Indices localActiveParameter(collection[i].getActiveParameter());
    // if extraParameterNumber_[i] > 0, check if the parameters are active
    for (UnsignedInteger j = 0; j < extraParameterNumber_[i]; ++j)
    {
      if (localActiveParameter.contains(localInputDimension + j + 1))
        activeParameter_.add(index + j);
    }
    // update index
    index += extraParameterNumber_[i];
  }
  // Set collection
  collection_ = collection;
  // Set amplitude & scale
  scale_ = scale;
  setAmplitude(amplitude);
  // Fix all submodels as correlation models
  for (UnsignedInteger i = 0; i < size; ++i) collection_[i].setAmplitude(Point(1, 1.0));

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
Scalar ProductCovarianceModel::computeStandardRepresentative(const Point & s,
    const Point & t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;

  return computeStandardRepresentative(s.begin(), t.begin());
}

Scalar ProductCovarianceModel::computeStandardRepresentative(const Collection<Scalar>::const_iterator & s_begin,
    const Collection<Scalar>::const_iterator & t_begin) const
{
  Scalar rho = 1.0;
  UnsignedInteger start = 0;
  for (UnsignedInteger i = 0; i < collection_.getSize(); ++i)
  {
    const UnsignedInteger localInputDimension = collection_[i].getInputDimension();
    rho *= collection_[i].getImplementation()->computeStandardRepresentative(s_begin + start, t_begin + start);
    start += localInputDimension;
  }
  return rho;
}

/* Gradient */
Matrix ProductCovarianceModel::partialGradient(const Point & s,
    const Point & t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;

  const UnsignedInteger size = collection_.getSize();
  Point localCovariances(size);
  Scalar leftValue = 1.0;
  Scalar rightValue = 1.0;
  UnsignedInteger start = 0;
  Matrix gradient(inputDimension_, 1);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger localInputDimension = collection_[i].getInputDimension();
    const UnsignedInteger stop = start + localInputDimension;
    Point localS(localInputDimension);
    std::copy(s.begin() + start, s.begin() + stop, localS.begin());
    Point localT(localInputDimension);
    std::copy(t.begin() + start, t.begin() + stop, localT.begin());
    Matrix localGradient(collection_[i].partialGradient(localS, localT));
    for (UnsignedInteger j = 0; j < localInputDimension; ++j) localGradient(j, 0) *= leftValue;
    std::copy(localGradient.getImplementation()->begin(), localGradient.getImplementation()->end(), gradient.getImplementation()->begin() + start);
    localCovariances[i] = collection_[i](localS, localT)(0, 0);
    leftValue *= localCovariances[i];
    start = stop;
  }
  // Decrement
  start = getInputDimension();
  // Second step
  for (UnsignedInteger i = size; i > 0; --i)
  {
    const UnsignedInteger localInputDimension = collection_[i - 1].getInputDimension();
    start -= localInputDimension;
    for (UnsignedInteger j = 0; j < localInputDimension; ++j) gradient(start + j, 0) *= rightValue;
    rightValue *= localCovariances[i - 1];
  }
  return gradient * amplitude_[0] * amplitude_[0];
}

/* Parameters accessor */
void ProductCovarianceModel::setFullParameter(const Point & parameter)
{
  UnsignedInteger parameterDimension = inputDimension_ + 1;
  // Increase using the specific parameters
  for (UnsignedInteger i = 0; i < extraParameterNumber_.getSize(); ++i) parameterDimension += extraParameterNumber_[i];

  if (parameter.getDimension() != parameterDimension)
    throw InvalidArgumentException(HERE) << "Error: parameters dimension should be " << parameterDimension << " (, got " << parameter.getDimension() << ")";

  // Convention is the following :
  // Scale parameters then amplitude parameter and finally other parameters

  UnsignedInteger start = 0;
  // Index for extra parameters
  UnsignedInteger index = inputDimension_ + 1;
  Point scale(inputDimension_);
  for (UnsignedInteger i = 0; i < collection_.getSize(); ++i)
  {
    const UnsignedInteger atomInputDimension = collection_[i].getScale().getDimension();
    const UnsignedInteger stop = start + atomInputDimension;
    Point atomFullParameter(atomInputDimension);
    std::copy(parameter.begin() + start, parameter.begin() + stop, atomFullParameter.begin());
    // Duplicate scale
    std::copy(parameter.begin() + start, parameter.begin() + stop, scale.begin() + start);
    // Add 'local' amplitude
    atomFullParameter.add(1.0);
    // Set extra
    for (UnsignedInteger k = 0; k < extraParameterNumber_[i]; ++k)
    {
      atomFullParameter.add(parameter[index]);
      index += 1;
    }
    // update start index
    start = stop;
    collection_[i].setFullParameter(atomFullParameter);
  }
  // Copy scale (for get accessor)
  scale_ = scale;
  setAmplitude(Point(1, parameter[inputDimension_]));
}

void ProductCovarianceModel::setActiveParameter(const Indices & active)
{
  // Propagate information to marginal models
  // First, check if active contains the amplitude.
  Bool isAmplitudeActive = active.contains(inputDimension_);
  // variables that help to read active parameters
  const UnsignedInteger size = collection_.getSize();
  UnsignedInteger inputDimension = 0;
  UnsignedInteger index = inputDimension_ + 1;
  Indices localActiveParameter;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger localInputDimension = collection_[i].getInputDimension();
    localActiveParameter = Indices(0);
    // if extraParameterNumber_[i] > 0, check if the parameters are active
    for (UnsignedInteger j = 0; j < localInputDimension; ++j)
    {
      if (active.contains(inputDimension + j))
        localActiveParameter.add(j);
    }
    inputDimension += localInputDimension;
    if (isAmplitudeActive)
      localActiveParameter.add(localInputDimension);
    // Handle extra param
    for (UnsignedInteger j = 0; j < extraParameterNumber_[i]; ++j)
    {
      if (active.contains(index + j))
        localActiveParameter.add(localInputDimension + j + 1);
    }
    // update index
    index += extraParameterNumber_[i];
    // Update local active param
    collection_[i].setActiveParameter(localActiveParameter);
  }
  // Copy active (for get accessor)
  activeParameter_ = active;
}

Point ProductCovarianceModel::getFullParameter() const
{
  // Convention scale + amplitude + extraParam
  Point result(0);
  result.add(scale_);
  result.add(amplitude_);
  const UnsignedInteger size = extraParameterNumber_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if (extraParameterNumber_[i] > 0)
    {
      const UnsignedInteger localInputDimension = collection_[i].getInputDimension();
      const Point localFullParameter(collection_[i].getFullParameter());
      for (UnsignedInteger k = 0; k < extraParameterNumber_[i]; ++k)
        result.add(localFullParameter[localInputDimension + 1 + k]);
    }
  }
  return result;
}

Description ProductCovarianceModel::getFullParameterDescription() const
{
  const UnsignedInteger size = inputDimension_ + 1;
  Description description(size);
  for (UnsignedInteger i = 0; i < size - 1; ++i)
    description[i] = OSS() << "scale_" << i;
  // Last element is amplitude
  description[size - 1] = "amplitude_0";
  for (UnsignedInteger i = 0; i < extraParameterNumber_.getSize(); ++i)
  {
    if (extraParameterNumber_[i] > 0)
    {
      const UnsignedInteger localInputDimension = collection_[i].getInputDimension();
      const Description localFullParameterDescription(collection_[i].getFullParameterDescription());
      for (UnsignedInteger k = 0; k < extraParameterNumber_[i]; ++k)
        description.add(OSS() << localFullParameterDescription[localInputDimension + 1 + k] << "_" << i);
    }
  }

  return description;
}

void ProductCovarianceModel::setScale(const Point & scale)
{
  if (scale.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "Error: scale dimension should be " << inputDimension_ << ". Here we got " << scale.getDimension();
  // Set the scale
  UnsignedInteger start = 0;
  for (UnsignedInteger i = 0; i < collection_.getSize(); ++i)
  {
    const UnsignedInteger atomScaleDimension = collection_[i].getScale().getDimension();
    const UnsignedInteger stop = start + atomScaleDimension;
    Point atomScale(atomScaleDimension);
    std::copy(scale.begin() + start, scale.begin() + stop, atomScale.begin());
    start = stop;
    collection_[i].setScale(atomScale);
  }
  // Copy scale (for get accessor)
  scale_ = scale;
}

// Scale parametrization accessor
CovarianceModelImplementation::ScaleParametrization ProductCovarianceModel::getScaleParametrization() const
{
  return collection_[0].getScaleParametrization();
}

void ProductCovarianceModel::setScaleParametrization(const ScaleParametrization scaleParametrization)
{
  for (UnsignedInteger i = 0; i < collection_.getSize(); ++i)
  {
    collection_[i].setScaleParametrization(scaleParametrization);
  }

  // copy back scale
  Point scale;
  for (UnsignedInteger i = 0; i < collection_.getSize(); ++i)
    scale.add(collection_[i].getScale());
  scale_ = scale;
}

/* Is it a stationary model ? */
Bool ProductCovarianceModel::isStationary() const
{
  for (UnsignedInteger i = 0; i < collection_.getSize(); ++i)
    if (!collection_[i].isStationary()) return false;
  return true;
}

/* Is it a composite model ? */
Bool ProductCovarianceModel::isComposite() const
{
  return true;
}

/* String converter */
String ProductCovarianceModel::__repr__() const
{
  OSS oss;
  oss << "class=" << ProductCovarianceModel::GetClassName()
      << " input dimension=" << inputDimension_
      << " models=" << collection_;
  return oss;
}

/* String converter */
String ProductCovarianceModel::__str__(const String & ) const
{
  return __repr__();
}

/* Marginal accessor */
CovarianceModel ProductCovarianceModel::getMarginal(const UnsignedInteger index) const
{
  if (index >= outputDimension_) throw InvalidArgumentException(HERE) << "Error: index=" << index << " must be less than output dimension=" << outputDimension_;
  return collection_[index];
}

/* Method save() stores the object through the StorageManager */
void ProductCovarianceModel::save(Advocate & adv) const
{
  CovarianceModelImplementation::save(adv);
  adv.saveAttribute("collection_", collection_);
  adv.saveAttribute("extraParameterNumber_", extraParameterNumber_);
}

/* Method load() reloads the object from the StorageManager */
void ProductCovarianceModel::load(Advocate & adv)
{
  CovarianceModelImplementation::load(adv);
  adv.loadAttribute("collection_", collection_);
  adv.loadAttribute("extraParameterNumber_", extraParameterNumber_);
}

END_NAMESPACE_OPENTURNS
