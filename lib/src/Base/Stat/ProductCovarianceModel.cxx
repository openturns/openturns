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
#include "openturns/ProductCovarianceModel.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/AbsoluteExponential.hxx"
#include "openturns/SpecFunc.hxx"

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
  // scale parameter
  scale_ = Point(inputDimension, collection_[0].getScale()[0]);

  // nugget factor
  nuggetFactor_ = collection_[0].getNuggetFactor();
  for (UnsignedInteger i = 0; i < inputDimension; ++i)
  {
    collection_[i].setNuggetFactor(0.0);
    const Description description(collection_[i].getParameterDescription());
    const UnsignedInteger nuggetFactorIndex = description.find("nuggetFactor");
    if (nuggetFactorIndex < description.getSize()) // nuggetFactor is active, make it unactive
    {
      const Indices activeParameter(collection_[i].getActiveParameter());
      Indices newActiveParameter(description.getSize() - 1);
      for (UnsignedInteger j = 0; j < description.getSize(); ++j)
      {
        if (j < nuggetFactorIndex) newActiveParameter[j] = activeParameter[j];
        else if (j > nuggetFactorIndex) newActiveParameter[j - 1] = activeParameter[j];
      }
      collection_[i].setActiveParameter(newActiveParameter);
    }
  }

  // Update the default values for the amplitude
  setAmplitude(Point(1, collection_[0].getAmplitude()[0]));
  // Active parameters : scale + amplitude
  activeParameter_ = Indices(inputDimension + 1);
  activeParameter_.fill();
  activeParameter_[inputDimension] = inputDimension + 1;
  isStationary_ = true;
}

/* Parameters constructor */
ProductCovarianceModel::ProductCovarianceModel(const CovarianceModelCollection & collection)
  : CovarianceModelImplementation()
  , collection_(0)
  , extraParameterNumber_(0)
{
  setCollection(collection);
}

/* Collection accessor */
void ProductCovarianceModel::setCollection(const CovarianceModelCollection & collection)
{
  // Check if the given models have an input dimension=1
  const UnsignedInteger size = collection.getSize();
  if (!(size > 0))
    throw InvalidArgumentException(HERE) << "Error: the collection must have a positive size, here size=0";
  // Nugget factor
  nuggetFactor_ = collection[0].getNuggetFactor();
  // Scale & amplitude
  Point scale(0);
  Point amplitude(1, 1.0);
  inputDimension_ = 0;
  // Get dimension: should be the same for all elements
  outputDimension_ = 1;

  // checking if amplitude parameter should be active
  // Value is True if one of the marginal models activates it
  Bool isAmplitudeActive = false;

  // checking if nuggetFactor parameter should be active
  // Value is True if one of the marginal models activates it
  Bool isNuggetFactorActive = false;

  // Handle 'specific' parameters
  extraParameterNumber_ = Indices(collection.getSize());

  // Filling the active parameters
  activeParameter_ = Indices(0);
  isStationary_ = true;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger localOutputDimension = collection[i].getOutputDimension();
    if (outputDimension_ != localOutputDimension)
      throw InvalidArgumentException(HERE) << "In ProductCovarianceModel::setCollection, incompatible dimension of the element #" << i
                                           << " dimension of element = " << localOutputDimension << ", dimension of the model = " << outputDimension_;
    // Add element to the collection
    // Get its scale, which is seen as a concatenation of collection scale
    const UnsignedInteger localInputDimension = collection[i].getInputDimension();

    // Get amplitude as amplitude product
    const Scalar localAmplitude = collection[i].getAmplitude()[0];
    amplitude[0] *= localAmplitude;

    // Get local active parameter
    const Indices localActiveParameter(collection[i].getActiveParameter());
    // Check if scale are active
    for (UnsignedInteger j = 0; j < collection[i].getScale().getSize(); ++j)
    {
      if (localActiveParameter.contains(j))
        activeParameter_.add(scale.getSize() + j);
    }
    inputDimension_ += localInputDimension;
    scale.add(collection[i].getScale());

    // Should we activate the nuggetFactor parameter?
    isNuggetFactorActive = isNuggetFactorActive || localActiveParameter.contains(collection[i].getScale().getSize());

    // Should we activate the amplitude parameter?
    isAmplitudeActive = isAmplitudeActive || localActiveParameter.contains(collection[i].getScale().getSize() + 1);

    // Number of specific parameter
    extraParameterNumber_[i] = collection[i].getFullParameter().getSize() - (collection[i].getScale().getSize() + 2);

    // Check if model is stationary
    if (!collection[i].isStationary())
      isStationary_ = false;
  }

  // NuggetFactor active
  if (isNuggetFactorActive)
    activeParameter_.add(scale.getSize());

  // Amplitude active
  if (isAmplitudeActive)
    activeParameter_.add(scale.getSize() + 1);

  // Handle active extra parameters
  UnsignedInteger index = scale.getSize() + 2;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Indices localActiveParameter(collection[i].getActiveParameter());
    // if extraParameterNumber_[i] > 0, check if the parameters are active
    for (UnsignedInteger j = 0; j < extraParameterNumber_[i]; ++j)
    {
      if (localActiveParameter.contains(collection[i].getScale().getSize() + j + 2))
        activeParameter_.add(index + j);
    }
    // update index
    index += extraParameterNumber_[i];
  }
  // Set collection
  collection_ = collection;
  // Set amplitude & nuggetFactor & scale
  scale_ = scale;
  nuggetFactor_ = collection[0].getNuggetFactor();

  setAmplitude(amplitude);
  // Fix all submodels as correlation models
  for (UnsignedInteger i = 0; i < size; ++i) collection_[i].setAmplitude(Point(1, 1.0));

  // set all marginal model nugget factors to 0 and deactivate them
  LOGDEBUG(OSS(false) << "Set marginal nugget factors to 0 and deactivate them ");
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    collection_[i].setNuggetFactor(0.0);
    const Description description(collection_[i].getParameterDescription());
    const UnsignedInteger nuggetFactorIndex = description.find("nuggetFactor");
    if (nuggetFactorIndex < description.getSize()) // nuggetFactor is active, make it unactive
    {
      const Indices activeParameter(collection_[i].getActiveParameter());
      Indices newActiveParameter(description.getSize() - 1);
      for (UnsignedInteger j = 0; j < description.getSize(); ++j)
      {
        if (j < nuggetFactorIndex) newActiveParameter[j] = activeParameter[j];
        else if (j > nuggetFactorIndex) newActiveParameter[j - 1] = activeParameter[j];
      }
      collection_[i].setActiveParameter(newActiveParameter);
      LOGDEBUG(OSS(false) << "Collection[" << i << "] active parameter = " << collection_[i].getActiveParameter());
      LOGDEBUG(OSS(false) << "Collection[" << i << "] active parameter description = " << collection_[i].getParameterDescription());
    }
  }
}

ProductCovarianceModel::CovarianceModelCollection ProductCovarianceModel::getCollection() const
{
  return collection_;
}

/* Virtual constructor */
ProductCovarianceModel * ProductCovarianceModel::clone() const
{
  return new ProductCovarianceModel(*this);
}

Scalar ProductCovarianceModel::computeAsScalar(const Point & tau) const
{
  if (!isStationary())
    return CovarianceModelImplementation::computeAsScalar(tau);
  if (tau.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "ProductCovarianceModel::computeAsScalar(tau): the point s has dimension=" << tau.getDimension() << ", expected dimension=" << inputDimension_;
  Scalar rho = amplitude_[0] * amplitude_[0];
  UnsignedInteger start = 0;
  for (UnsignedInteger i = 0; i < collection_.getSize(); ++i)
  {
    // Compute as scalar returns the correlation function
    const UnsignedInteger localInputDimension = collection_[i].getInputDimension();
    const UnsignedInteger stop = start + localInputDimension;
    Point localTau(localInputDimension);
    std::copy(tau.begin() + start, tau.begin() + stop, localTau.begin());
    // Compute as scalar returns the correlation function
    rho *= collection_[i].getImplementation()->computeAsScalar(localTau);
    start += collection_[i].getInputDimension();
  }
  if (tau.norm() <= SpecFunc::ScalarEpsilon) rho *= (1.0 + getNuggetFactor());
  return rho;
}

Scalar ProductCovarianceModel::computeAsScalar(const Collection<Scalar>::const_iterator & s_begin,
    const Collection<Scalar>::const_iterator & t_begin) const
{
  Scalar rho = amplitude_[0] * amplitude_[0];
  Scalar squareNorm = 0.0;
  UnsignedInteger start = 0;
  Collection<Scalar>::const_iterator s_it = s_begin;
  Collection<Scalar>::const_iterator t_it = t_begin;
  for (UnsignedInteger i = 0; i < inputDimension_; ++i, ++s_it, ++t_it)
  {
    const Scalar dx = (*s_it - *t_it);
    squareNorm += dx * dx;
  }
  for (UnsignedInteger i = 0; i < collection_.getSize(); ++i)
  {
    // Compute as scalar returns the correlation function
    rho *= collection_[i].getImplementation()->computeAsScalar(s_begin + start, t_begin + start);
    start += collection_[i].getInputDimension();
  }
  if (squareNorm <= SpecFunc::ScalarEpsilon * SpecFunc::ScalarEpsilon) rho *= (1.0 + getNuggetFactor());
  return rho;
}

Scalar ProductCovarianceModel::computeAsScalar(const Scalar tau) const
{
  if (inputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has input dimension=" << inputDimension_ << ", expected input dimension=1.";
  if (outputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has output dimension=" << outputDimension_ << ", expected dimension=1.";
  Scalar rho = collection_[0].getImplementation()->computeAsScalar(tau);
  if (std::abs(tau) <= SpecFunc::ScalarEpsilon) rho *= (1.0 + getNuggetFactor());
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
  UnsignedInteger scaleSize = getScale().getSize();
  UnsignedInteger index = scaleSize + 2; // Index for extra parameters
  // Total parameter dimension
  UnsignedInteger parameterDimension = index;
  // Increase using the specific parameters
  for (UnsignedInteger i = 0; i < extraParameterNumber_.getSize(); ++i) parameterDimension += extraParameterNumber_[i];

  if (parameter.getDimension() != parameterDimension)
    throw InvalidArgumentException(HERE) << "Error: parameters dimension should be " << parameterDimension << " (, got " << parameter.getDimension() << ")";

  // Convention is the following :
  // Scale parameters then amplitude parameter and finally other parameters

  UnsignedInteger start = 0;
  Point scale(scaleSize);
  for (UnsignedInteger i = 0; i < collection_.getSize(); ++i)
  {
    const UnsignedInteger atomScaleDimension = collection_[i].getScale().getDimension();
    const UnsignedInteger stop = start + atomScaleDimension;
    Point atomFullParameter(atomScaleDimension);
    std::copy(parameter.begin() + start, parameter.begin() + stop, atomFullParameter.begin());
    // Duplicate scale
    std::copy(parameter.begin() + start, parameter.begin() + stop, scale.begin() + start);
    // Add 'local' nuggetFactor
    atomFullParameter.add(0.0);
    // Add 'local' amplitude
    atomFullParameter.add(1.0);
    // Set extra
    for (UnsignedInteger k = 0; k < extraParameterNumber_[i]; ++k)
    {
      atomFullParameter.add(parameter[index + k]);
    }
    index += extraParameterNumber_[i];
    // update start index
    start = stop;
    collection_[i].setFullParameter(atomFullParameter);
  }
  // Copy scale (for get accessor)
  scale_ = scale;
  setNuggetFactor(parameter[scaleSize]);
  setAmplitude(Point(1, parameter[scaleSize + 1]));
}

void ProductCovarianceModel::setActiveParameter(const Indices & active)
{
  // Propagate information to marginal models
  // First, check if active contains the nuggetFactor.
  const Bool isNuggetFactorActive = active.contains(getScale().getSize());
  // First, check if active contains the amplitude.
  const Bool isAmplitudeActive = active.contains(getScale().getSize() + 1);
  // variables that help to read active parameters
  const UnsignedInteger size = collection_.getSize();
  UnsignedInteger scaleSize = 0;
  UnsignedInteger index = getScale().getSize() + 2;
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    const UnsignedInteger localScaleSize = collection_[i].getScale().getSize();
    Indices localActiveParameter;
    // scale parameters
    for (UnsignedInteger j = 0; j < localScaleSize; ++ j)
    {
      if (active.contains(scaleSize + j))
        localActiveParameter.add(j);
    }
    scaleSize += localScaleSize;
    if (isNuggetFactorActive)
      localActiveParameter.add(localScaleSize);
    if (isAmplitudeActive)
      localActiveParameter.add(localScaleSize + 1);
    // Handle extra param
    for (UnsignedInteger j = 0; j < extraParameterNumber_[i]; ++j)
    {
      if (active.contains(index + j))
        localActiveParameter.add(localScaleSize + 2 + j);
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
  // Convention scale + amplitude + extras
  Point result(scale_);
  result.add(nuggetFactor_);
  result.add(amplitude_);
  const UnsignedInteger size = extraParameterNumber_.getSize();
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    if (extraParameterNumber_[i] > 0)
    {
      const Point localFullParameter(collection_[i].getFullParameter());
      for (UnsignedInteger k = 0; k < extraParameterNumber_[i]; ++ k)
        result.add(localFullParameter[collection_[i].getScale().getSize() + 2 + k]);
    }
  }
  return result;
}

Description ProductCovarianceModel::getFullParameterDescription() const
{
  const UnsignedInteger size = getScale().getSize();
  Description description(size);
  for (UnsignedInteger i = 0; i < size; ++i)
    description[i] = OSS() << "scale_" << i;
  description.add("nuggetFactor");
  // Last element is amplitude
  description.add("amplitude_0");
  for (UnsignedInteger i = 0; i < extraParameterNumber_.getSize(); ++ i)
  {
    if (extraParameterNumber_[i] > 0)
    {
      const Description localFullParameterDescription(collection_[i].getFullParameterDescription());
      for (UnsignedInteger k = 0; k < extraParameterNumber_[i]; ++ k)
        description.add(OSS() << localFullParameterDescription[collection_[i].getScale().getSize() + 2 + k] << "_" << i);
    }
  }

  return description;
}

void ProductCovarianceModel::setScale(const Point & scale)
{
  if (scale.getDimension() != getScale().getSize())
    throw InvalidArgumentException(HERE) << "Error: scale dimension should be " << getScale().getSize() << ". Here we got " << scale.getDimension();
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

/* Is it a stationary model ? */
Bool ProductCovarianceModel::isStationary() const
{
  for (UnsignedInteger i = 0; i < collection_.getSize(); ++i)
    if (!collection_[i].isStationary()) return false;
  return true;
}

/* Is it safe to compute discretize etc in parallel? */
Bool ProductCovarianceModel::isParallel() const
{
  for (UnsignedInteger i = 0; i < collection_.getSize(); ++i)
    if (!collection_[i].getImplementation()->isParallel()) return false;
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
  if (!(index < outputDimension_)) throw InvalidArgumentException(HERE) << "Error: index=" << index << " must be less than output dimension=" << outputDimension_;
  if (getOutputDimension() == 1)
    return clone();
  Collection<CovarianceModel> coll(collection_.getSize());
  for (UnsignedInteger i = 0; i < collection_.getSize(); ++i)
    coll[i] = collection_[i].getMarginal(index);
  return ProductCovarianceModel(coll);
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
