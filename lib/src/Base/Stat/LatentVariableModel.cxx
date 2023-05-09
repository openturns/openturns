//                                               -*- C++ -*-
/**
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
#include "openturns/LatentVariableModel.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Polygon.hxx"
#include "openturns/Text.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LatentVariableModel)

static const Factory<LatentVariableModel> Factory_LatentVariableModel;


/* Default constructor */
LatentVariableModel::LatentVariableModel(const UnsignedInteger nLevels)
  : CovarianceModelImplementation(Point(1, 1.0), Point(1, 1.0))
  , latentDim_(2)
  , nLevels_(nLevels)
  , latCovMat_(nLevels_)
  , latCovMod_(latentDim_)
{
  if (nLevels_ < 2) throw InvalidArgumentException(HERE) << "Error: the number of discrete levels must be >= 2";
  activeLatentCoordinateDim_ = 1 + latentDim_ * (nLevels_ - 2);
  activeLatentVariables_ = Point(activeLatentCoordinateDim_, 0.0);
  fullLatentVariables_ = Sample(nLevels_,latentDim_);
  Indices activeParameter = Indices(inputDimension_ + outputDimension_ + activeLatentCoordinateDim_);
  activeParameter.fill();
  setActiveParameter(activeParameter);
  updateLatentCovarianceMatrix();
}

/** Parameters constructor */
LatentVariableModel::LatentVariableModel(const UnsignedInteger nLevels,
						 const UnsignedInteger latentDim)
  : CovarianceModelImplementation(Point(1, 1.0), Point(1, 1.0))
  , latentDim_(latentDim)
  , nLevels_(nLevels)
  , latCovMat_(nLevels_)
  , latCovMod_(latentDim_)
{
  if (latentDim_ < 1) throw InvalidArgumentException(HERE) << "Error: the dimension of the latent space must be >= 1";
  if (nLevels_ < 2) throw InvalidArgumentException(HERE) << "Error: the number of discrete levels must be >= 2";
  activeLatentCoordinateDim_ = 1 + latentDim_ * (nLevels_ - 2);
  activeLatentVariables_ = Point(activeLatentCoordinateDim_,0.);
  fullLatentVariables_ = Sample(nLevels_,latentDim_);
  Indices activeParameter = Indices(inputDimension_ + outputDimension_ + activeLatentCoordinateDim_);
  activeParameter.fill();
  setActiveParameter(activeParameter);
  updateLatentCovarianceMatrix();
}

/* Virtual constructor */
LatentVariableModel * LatentVariableModel::clone() const
{
  return new LatentVariableModel(*this);
}

/* Computation of the covariance  function */
Scalar LatentVariableModel::computeAsScalar(const Scalar z1, const Scalar z2) const
{
  /*check z being one of the levels*/
  bool isLevelz1 = false;
  bool isLevelz2 = false;

  for (UnsignedInteger i = 0; i < nLevels_; ++i)
  {
    if (z1 == i) 
    {
      isLevelz1 = true;
      break;
    }
  }

  for (UnsignedInteger i = 0; i < nLevels_; ++i)
  {
    if (z2 == i)
    {
      isLevelz2 = true;
      break;
    }
  }

  if (!isLevelz1 || !isLevelz2) throw InvalidArgumentException(HERE) << "Error: the input discrete variables values are not amongst the known levels";

  Scalar cov = latCovMat_(z1,z2);

  return cov;
}

Scalar LatentVariableModel::computeAsScalar(const Point & z1, const Point & z2) const
{
  return computeAsScalar(z1[0],z2[0]);
}


Scalar LatentVariableModel::computeAsScalar(const Collection<Scalar>::const_iterator & z1_begin,
    const Collection<Scalar>::const_iterator & z2_begin) const
{

  Collection<Scalar>::const_iterator z1_it = z1_begin;
  Collection<Scalar>::const_iterator z2_it = z2_begin;

  return computeAsScalar(*z1_it,*z2_it);
}


void LatentVariableModel::setFullParameter(const Point & parameter)
{
  /*
    Care! To make the method not bogus, the size of parameter argument
    should be :
     - Size of scale : here 1
     - Size of amplitude : here 1
     - Number of latent variable coordinates : latentDim_*nLevels_-2*latentDim_+1
    CovarianceModelImplementation::setFullParameter checks that size is
    equal to the total number of parameters
  */
  // Check the size
  const UnsignedInteger totalSize = 2 + activeLatentCoordinateDim_;
  if (!(parameter.getSize() == totalSize))
    throw InvalidArgumentException(HERE) << "In LatentVariableModel::setFullParameter, points have incompatible size. Point size = " << parameter.getSize()
                                         << " whereas expected size = " << totalSize ;
  UnsignedInteger index = 0;
  // First set the scale parameter
  for (UnsignedInteger i = 0; i < inputDimension_; ++ i)
  {
    if (!(parameter[index] > 0.0))
      throw InvalidArgumentException(HERE) << "In LatentVariableModel::setFullParameter, the component " << index << " of scale is non positive";
    scale_[i] = parameter[index];
    ++ index;
  }
  // Second the amplitude parameter
  for (UnsignedInteger i = 0; i < outputDimension_; ++ i)
  {
    if (!(parameter[index] > 0.0))
      throw InvalidArgumentException(HERE) << "In LatentVariableModel::setFullParameter, the component " << index << " of amplitude is non positive";
    amplitude_[i] = parameter[index];
    ++ index;
  }

  // Update the latent covariance model
  latCovMod_.setAmplitude(amplitude_);
  latCovMod_.setScale(Point(latentDim_,scale_[0]));

  // Third the latent variable coordinates
  Point activeLatentVariables = Point(activeLatentCoordinateDim_,1.0);
  for (UnsignedInteger i = 0; i < activeLatentCoordinateDim_; ++ i)
  {
    activeLatentVariables[i] = parameter[index];
    ++ index;
  }
  setLatentVariables(activeLatentVariables);


}

Point LatentVariableModel::getFullParameter() const
{
  // Get the generic parameter
  Point parameter(CovarianceModelImplementation::getFullParameter());
  // Add the specific one
  parameter.add(activeLatentVariables_);
  return parameter;
}

Description LatentVariableModel::getFullParameterDescription() const
{
  // Description of the generic parameter
  Description description(CovarianceModelImplementation::getFullParameterDescription());
  // Description of the specific parameter
  for (UnsignedInteger i = 0; i < activeLatentCoordinateDim_; ++i)
    description.add(OSS() << "activeLatentVariable_" << i);
  return description;
}

/* String converter */
String LatentVariableModel::__repr__() const
{
  OSS oss;
  oss << "class=" << LatentVariableModel::GetClassName()
      << " scale=" << scale_
      << " amplitude=" << amplitude_
      << " activeLatentVariables=" << activeLatentVariables_;
  return oss;
}

/* String converter */
String LatentVariableModel::__str__(const String & ) const
{
  OSS oss;
  oss << LatentVariableModel::GetClassName()
      << "(scale=" << scale_.__str__()
      << ", amplitude=" << amplitude_.__str__()
      << ", activeLatentVariables=" << activeLatentVariables_.__str__()
      << ")";
  return oss;
}


/* latentVariables accessor */
void LatentVariableModel::setLatentVariables(const Point & latentVariablesCoordinates)
{
  if (!(latentVariablesCoordinates.getSize() == activeLatentCoordinateDim_))
    throw InvalidArgumentException(HERE) << "In LatentVariableModel::setLatentVariables, samples have incompatible size. Sample size = " << latentVariablesCoordinates.getSize()
                                         << " whereas expected size = " << activeLatentCoordinateDim_;
  // Set the active variables
  activeLatentVariables_ = latentVariablesCoordinates;

  // Set the full sample of latent coordinates
  // Fix the coordinates of the first two latent variables
  fullLatentVariables_ = Sample(nLevels_, latentDim_);
  fullLatentVariables_(1,0) = latentVariablesCoordinates[0];

  UnsignedInteger count = 1;
  // Fix the coordinates of the remaining latent variables
  for (UnsignedInteger i = 2; i < nLevels_; ++i)
  {
    for (UnsignedInteger j = 0; j < latentDim_; ++j)
    {
      fullLatentVariables_(i,j) = latentVariablesCoordinates[count];
      count++;
    }
  }
  updateLatentCovarianceMatrix();
}

void LatentVariableModel::updateLatentCovarianceMatrix()
{
  latCovMat_ = latCovMod_.discretize(fullLatentVariables_);
}

/* latentVariables accessor */
Sample LatentVariableModel::getFullLatentVariables() const
{
  return fullLatentVariables_;
}

Point LatentVariableModel::getActiveLatentVariables() const
{
  return activeLatentVariables_;
}

/* latentDimension accessor */
UnsignedInteger LatentVariableModel::getLatentDimension() const
{

  return latentDim_;
}

/* NLevels accessor */
UnsignedInteger LatentVariableModel::getNLevels() const
{

  return nLevels_;
}

void LatentVariableModel::setScale(const Point & scale)
{
  if (scale.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "In LatentVariableModel::setScale: the given scale has a dimension=" << scale.getDimension() << " different from the input dimension=" << inputDimension_;
  for (UnsignedInteger index = 0; index < inputDimension_; ++index)
    if (!(scale[index] > 0.0))
      throw InvalidArgumentException(HERE) << "In LatentVariableModel::setScale: the component " << index << " of scale is non positive" ;
  scale_ = scale;
  latCovMod_.setScale(Point(latentDim_,scale_[0]));
  updateLatentCovarianceMatrix();
}

void LatentVariableModel::setAmplitude(const Point & amplitude)
{
  if (amplitude.getDimension() != outputDimension_) throw InvalidArgumentException(HERE) << "In LatentVariableModel::setAmplitude: the given amplitude has a dimension=" << amplitude.getDimension() << " different from the dimension=" << outputDimension_;
  for (UnsignedInteger index = 0; index < outputDimension_; ++index)
    if (!(amplitude[index] > 0.0))
      throw InvalidArgumentException(HERE) << "In LatentVariableModel::setAmplitude, the component " << index << " of amplitude=" << amplitude << " is non positive" ;
  amplitude_ = amplitude;
  updateOutputCovariance();
  latCovMod_.setAmplitude(amplitude_);
  updateLatentCovarianceMatrix();
}

void LatentVariableModel::setNuggetFactor(const Scalar nuggetFactor)
{
  if (!(nuggetFactor >= 0.0)) throw InvalidArgumentException(HERE) << "Error: the nugget factor=" << nuggetFactor << " is negative";
  latCovMod_.setNuggetFactor(nuggetFactor_);
  nuggetFactor_ = nuggetFactor;
  updateLatentCovarianceMatrix();
}

/* Drawing method */
Graph LatentVariableModel::draw(const UnsignedInteger rowIndex,
    const UnsignedInteger columnIndex,
    const Scalar /*zMin*/,
    const Scalar /*zMax*/,
    const UnsignedInteger pointNumber,
    const Bool /*asStationary*/,
    const Bool /*correlationFlag*/) const
{
  throw NotYetImplementedException(HERE) << "The latentVariableModel class does not possess a draw method.";
}

/* Method save() stores the object through the StorageManager */
void LatentVariableModel::save(Advocate & adv) const
{
  CovarianceModelImplementation::save(adv);
  adv.saveAttribute("fullLatentVariables_", fullLatentVariables_);
  adv.saveAttribute("activeLatentVariables_", activeLatentVariables_);
  adv.saveAttribute("nLevels_", nLevels_);
  adv.saveAttribute("latentDim_", latentDim_);
// ADD OTHER ATTRIBUTES
}

/* Method load() reloads the object from the StorageManager */
void LatentVariableModel::load(Advocate & adv)
{
  CovarianceModelImplementation::load(adv);
  adv.loadAttribute("fullLatentVariables_", fullLatentVariables_);
  adv.loadAttribute("activeLatentVariables_", activeLatentVariables_);
  adv.loadAttribute("nLevels_", nLevels_);
  adv.loadAttribute("latentDim_", latentDim_);
}

END_NAMESPACE_OPENTURNS
