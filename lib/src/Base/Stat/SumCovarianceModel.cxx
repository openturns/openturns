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
#include "openturns/SumCovarianceModel.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/AbsoluteExponential.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

static const Factory<PersistentCollection<CovarianceModel> > Factory_PersistentCollection_CovarianceModel;

CLASSNAMEINIT(SumCovarianceModel)

static const Factory<SumCovarianceModel> Factory_SumCovarianceModel;


/* Default constructor */
SumCovarianceModel::SumCovarianceModel(const UnsignedInteger inputDimension)
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
SumCovarianceModel::SumCovarianceModel(const CovarianceModelCollection & collection)
  : CovarianceModelImplementation()
  , collection_(0)
  , extraParameterNumber_(0)
{
  setCollection(collection);
}

/* Collection accessor */
void SumCovarianceModel::setCollection(const CovarianceModelCollection & collection)
{
  // Check if the given models have an input dimension=1
  const UnsignedInteger size = collection.getSize();
  if (!(size > 0))
    throw InvalidArgumentException(HERE) << "Error: the collection must have a positive size, here size=0";
    
  // Nugget factor
  nuggetFactor_ = collection[0].getNuggetFactor();
  
  // Input Dimension
  inputDimension_ = collection[0].getInputDimension();

  // checking if nuggetFactor parameter should be active
  // Value is True if one of the marginal models activates it
  Bool isNuggetFactorActive = false;

  // Handle 'specific' parameters
  extraParameterNumber_ = Indices(collection.getSize());

  isStationary_ = true;
  
  // Check output dimension and other parameters
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if (inputDimension_ != collection[i].getInputDimension())
      throw InvalidArgumentException(HERE) << "Error: the collection items have different input sizes";
      
    const Indices localActiveParameter(collection[i].getActiveParameter());
    
    // Should we activate the nuggetFactor parameter?
    isNuggetFactorActive = isNuggetFactorActive || localActiveParameter.contains(collection[i].getScale().getSize());

    // Number of specific parameter
    extraParameterNumber_[i] = collection[i].getFullParameter().getSize() - (collection[i].getScale().getSize() + 2);

    // Check if model is stationary
    if (!collection[i].isStationary())
      isStationary_ = false;
  }
  
  // Get dimension: should be the same for all elements
  outputDimension_ = 1;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if (outputDimension_ != collection[i].getOutputDimension())
      throw InvalidArgumentException(HERE) << "In SumCovarianceModel::setCollection, incompatible dimension of the element #" << i
                                           << " dimension of element = " << collection[i].getOutputDimension() << ", dimension of the model = " << outputDimension_;
  }
  // Scale & amplitude
  Point scale(inputDimension_,1.0); //Dummy scale parameters with the correct size
  Point amplitude(1, 1.0); //Dummy amplitude parameter with the correct size

  // Filling the active parameters. All active parameters are treated as extra parameters, except for thge nugget
  activeParameter_ = Indices(0);
  
  // NuggetFactor active
  if (isNuggetFactorActive)
    activeParameter_.add(inputDimension_);
    
  UnsignedInteger index = inputDimension_ + 2; // We start filling active parameters after scales, nugget and amplitude, which are dummy parameters
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    // Get local active parameter
    const Indices localActiveParameter(collection[i].getActiveParameter());
    // Check if scale are active
    for (UnsignedInteger j = 0; j < collection[i].getScale().getSize(); ++j)
    {
      if (localActiveParameter.contains(j))
        activeParameter_.add(index);
      index ++;
    }
  }

  // Filling amplitudes. One per item in the collection
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    // Get local active parameter
    const Indices localActiveParameter(collection[i].getActiveParameter());
    // Check if scale are active
    UnsignedInteger j = collection[i].getScale().getSize()+1;
    if (localActiveParameter.contains(j))
      activeParameter_.add(index);
    index ++;
  }

  // Handle active extra parameters
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Indices localActiveParameter(collection[i].getActiveParameter());
    // if extraParameterNumber_[i] > 0, check if the parameters are active
    for (UnsignedInteger j = 0; j < extraParameterNumber_[i]; ++j)
    {
      if (localActiveParameter.contains(collection[i].getScale().getSize() + j + 2))
        activeParameter_.add(index);
      index ++;
    }
  }
  
  // Set collection
  collection_ = collection;
  
  // Set amplitude & nuggetFactor & scale
  scale_ = scale;
  amplitude_ = amplitude;

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

SumCovarianceModel::CovarianceModelCollection SumCovarianceModel::getCollection() const
{
  return collection_;
}

/* Virtual constructor */
SumCovarianceModel * SumCovarianceModel::clone() const
{
  return new SumCovarianceModel(*this);
}

Scalar SumCovarianceModel::computeAsScalar(const Point & tau) const
{
  if (tau.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "SumCovarianceModel::computeAsScalar(tau): the point s has dimension=" << tau.getDimension() << ", expected dimension=" << inputDimension_;
  Scalar rho = 0.0;
  for (UnsignedInteger i = 0; i < collection_.getSize(); ++i)
  {
    // Compute as scalar returns the correlation function
    rho += collection_[i].getImplementation()->computeAsScalar(tau);
  }
  if (tau.norm() <= SpecFunc::ScalarEpsilon) rho *= (1.0 + getNuggetFactor());
  return rho;
}

Scalar SumCovarianceModel::computeAsScalar(const Collection<Scalar>::const_iterator & s_begin,
    const Collection<Scalar>::const_iterator & t_begin) const
{
  Scalar rho = 0.0;
  Scalar squareNorm = 0.0;
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
    rho += collection_[i].getImplementation()->computeAsScalar(s_begin, t_begin);
  }
  if (squareNorm <= SpecFunc::ScalarEpsilon * SpecFunc::ScalarEpsilon) rho *= (1.0 + getNuggetFactor());
  return rho;
}

Scalar SumCovarianceModel::computeAsScalar(const Scalar tau) const
{
  if (inputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has input dimension=" << inputDimension_ << ", expected input dimension=1.";
  if (outputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has output dimension=" << outputDimension_ << ", expected dimension=1.";
  Scalar rho = collection_[0].getImplementation()->computeAsScalar(tau);
  if (std::abs(tau) <= SpecFunc::ScalarEpsilon) rho *= (1.0 + getNuggetFactor());
  return rho;
}

/* Parameters accessor */
void SumCovarianceModel::setFullParameter(const Point & parameter)
{
  const UnsignedInteger size = collection_.getSize();
  // Total parameter dimension, starts with dummy lengthscales, nugget and dummy amplitude
  UnsignedInteger parameterDimension = inputDimension_ + 2;
  // Adding non-dummy lengthscales and amplitudes
  for (UnsignedInteger i = 0; i < size; ++i) parameterDimension += collection_[i].getScale().getDimension() + 1;
  // Increase using the specific parameters
  for (UnsignedInteger i = 0; i < extraParameterNumber_.getSize(); ++i) parameterDimension += extraParameterNumber_[i];

  if (parameter.getDimension() != parameterDimension)
    throw InvalidArgumentException(HERE) << "Error: parameters dimension should be " << parameterDimension << " (, got " << parameter.getDimension() << ")";

  // Cumulated size of the scales of all items in the collection
  UnsignedInteger totalScaleSize = 0;
  for (UnsignedInteger i = 0; i < size; ++i) totalScaleSize += collection_[i].getScale().getSize();
   
  // Convention is the following :
  // Scale parameters then nugget, then amplitude parameter and finally other parameters

  const UnsignedInteger start = inputDimension_ + 2;
  UnsignedInteger extraParameterIndexCounter = 0;
  UnsignedInteger scaleIndexCounter = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    UnsignedInteger localScaleSize = collection_[i].getScale().getSize();
    Point atomFullParameter(0);

    // Scales
    for (UnsignedInteger j = 0; j < localScaleSize; ++j)
      {
        atomFullParameter.add(parameter[start+scaleIndexCounter]);
        scaleIndexCounter ++;
      }
      
    // Nugget
    atomFullParameter.add(parameter[inputDimension_]);
    
    // Amplitude
    atomFullParameter.add(parameter[start+totalScaleSize+i]);

    // Extra parameters
    for (UnsignedInteger j = 0; j < extraParameterNumber_[i]; ++j)
    {
      atomFullParameter.add(parameter[start+totalScaleSize+size+extraParameterIndexCounter]);
      extraParameterIndexCounter++;
    }
    collection_[i].setFullParameter(atomFullParameter);
  }

  setNuggetFactor(parameter[inputDimension_]);
  // Scale & amplitude
  Point scale(inputDimension_,1.0); //Dummy scale parameters with the correct size
  Point amplitude(size, 1.0); //Dummy amplitude parameter with the correct size
}

void SumCovarianceModel::setActiveParameter(const Indices & active)
{
  // variables that help to read active parameters
  const UnsignedInteger size = collection_.getSize();
  const UnsignedInteger start = inputDimension_ + 2;
  UnsignedInteger extraParameterIndexCounter = 0;
  UnsignedInteger scaleIndexCounter = 0;
  
  // Cumulated size of the scales of all items in the collection
  UnsignedInteger totalScaleSize = 0;
  for (UnsignedInteger i = 0; i < size; ++i) totalScaleSize += collection_[i].getScale().getSize();
  
  // Convention is the following :
  // Scale parameters then nugget, then amplitude parameter and finally other parameters
  
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    UnsignedInteger localScaleSize = collection_[i].getScale().getSize();
    Indices localActiveParameter;

    // Scales
    for (UnsignedInteger j = 0; j < localScaleSize; ++j)
      {
      if (active.contains(start+scaleIndexCounter))
        localActiveParameter.add(scaleIndexCounter);
      scaleIndexCounter ++;
      }
      
    // Nugget
    if (active.contains(inputDimension_))
      localActiveParameter.add(localScaleSize);
         
    // Amplitude
    if (active.contains(start+totalScaleSize+i))
      localActiveParameter.add(localScaleSize+1);

    // Extra parameters
    for (UnsignedInteger j = 0; j < extraParameterNumber_[i]; ++j)
    {
      if (active.contains(start+totalScaleSize+size+extraParameterIndexCounter))
        localActiveParameter.add(localScaleSize + 2 + j);
      extraParameterIndexCounter++;
    }
    collection_[i].setActiveParameter(localActiveParameter);
  }
  
  // Copy active (for get accessor)
  activeParameter_ = active;
}

Point SumCovarianceModel::getFullParameter() const
{
  // Convention scale + amplitude + extras
  Point result(scale_);
  result.add(nuggetFactor_);
  result.add(amplitude_);
  
  for (UnsignedInteger i = 0; i < collection_.getSize(); ++ i)
  {
    result.add(collection_[i].getScale());
  }

  for (UnsignedInteger i = 0; i < collection_.getSize(); ++ i)
  {
    result.add(collection_[i].getAmplitude());
  }

  for (UnsignedInteger i = 0; i < extraParameterNumber_.getSize(); ++ i)
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

Description SumCovarianceModel::getFullParameterDescription() const
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

void SumCovarianceModel::setScale(const Point & scale)
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
Bool SumCovarianceModel::isStationary() const
{
  for (UnsignedInteger i = 0; i < collection_.getSize(); ++i)
    if (!collection_[i].isStationary()) return false;
  return true;
}

/* Is it safe to compute discretize etc in parallel? */
Bool SumCovarianceModel::isParallel() const
{
  for (UnsignedInteger i = 0; i < collection_.getSize(); ++i)
    if (!collection_[i].getImplementation()->isParallel()) return false;
  return true;
}

/* String converter */
String SumCovarianceModel::__repr__() const
{
  OSS oss;
  oss << "class=" << SumCovarianceModel::GetClassName()
      << " input dimension=" << inputDimension_
      << " models=" << collection_;
  return oss;
}

/* String converter */
String SumCovarianceModel::__str__(const String & ) const
{
  return __repr__();
}

/* Marginal accessor */
CovarianceModel SumCovarianceModel::getMarginal(const UnsignedInteger index) const
{
  if (!(index < outputDimension_)) throw InvalidArgumentException(HERE) << "Error: index=" << index << " must be less than output dimension=" << outputDimension_;
  if (getOutputDimension() == 1)
    return clone();
  Collection<CovarianceModel> coll(collection_.getSize());
  for (UnsignedInteger i = 0; i < collection_.getSize(); ++i)
    coll[i] = collection_[i].getMarginal(index);
  return SumCovarianceModel(coll);
}

/* Method save() stores the object through the StorageManager */
void SumCovarianceModel::save(Advocate & adv) const
{
  CovarianceModelImplementation::save(adv);
  adv.saveAttribute("collection_", collection_);
  adv.saveAttribute("extraParameterNumber_", extraParameterNumber_);
}

/* Method load() reloads the object from the StorageManager */
void SumCovarianceModel::load(Advocate & adv)
{
  CovarianceModelImplementation::load(adv);
  adv.loadAttribute("collection_", collection_);
  adv.loadAttribute("extraParameterNumber_", extraParameterNumber_);
}

END_NAMESPACE_OPENTURNS
