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

/** Parameters constructor */
LatentVariableModel::LatentVariableModel(const UnsignedInteger nLevels,
										 const UnsignedInteger latentDim)
  : CovarianceModelImplementation(Point(1, 1.0), Point(1, 1.0))
  , latentDim_(latentDim)
  , nLevels_(nLevels)
  , latentCovarianceMatrix_(nLevels_)
  , latentCovarianceModel_(latentDim_)
{
  if (latentDim_ < 1) throw InvalidArgumentException(HERE) << "Error: the dimension of the latent space must be >= 1";
  if (nLevels_ < 2) throw InvalidArgumentException(HERE) << "Error: the number of discrete levels must be >= 2";
  activeLatentCoordinateDim_ = 1 + latentDim_ * (nLevels_ - 2);
  activeLatentVariables_ = Point(activeLatentCoordinateDim_, 0.0);
  fullLatentVariables_ = Sample(nLevels_, latentDim_);
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
  /*checks z1 and z2 being among the known levels*/
  const bool isLevelz1 = (z1 >= 0 && z1 < nLevels_ && (z1 - floor(z1)) == 0);
  const bool isLevelz2 = (z2 >= 0 && z2 < nLevels_ && (z2 - floor(z2)) == 0);

  if (!isLevelz1 || !isLevelz2) throw InvalidArgumentException(HERE) << "Error: the input discrete variables values: " << z1 
  << ", and/or " << z2 << " are not among the known levels. They should both present integer values between 0 and l-1.";

  Scalar cov = latentCovarianceMatrix_(z1, z2);

  return cov;
}

Scalar LatentVariableModel::computeAsScalar(const Point & z1, const Point & z2) const
{
  return computeAsScalar(z1[0], z2[0]);
}


Scalar LatentVariableModel::computeAsScalar(const Collection<Scalar>::const_iterator & z1_begin,
    const Collection<Scalar>::const_iterator & z2_begin) const
{

  Collection<Scalar>::const_iterator z1_it = z1_begin;
  Collection<Scalar>::const_iterator z2_it = z2_begin;

  return computeAsScalar(*z1_it, *z2_it);
}


void LatentVariableModel::setFullParameter(const Point & parameter)
{
  /*
    Care! To make the method not bogus, the size of parameter argument
    should be :
     - Size of scale : here 1
     - Size of amplitude : here 1
     - Number of latent variable coordinates : latentDim_ * nLevels_ - 2 * latentDim_ + 1
    CovarianceModelImplementation::setFullParameter checks that size is
    equal to the total number of parameters : 2 + the number of latent variable coordinates
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
  latentCovarianceModel_.setAmplitude(amplitude_);
  latentCovarianceModel_.setScale(Point(latentDim_, scale_[0]));

  // Third the latent variable coordinates
  Point activeLatentVariables = Point(activeLatentCoordinateDim_, 1.0);
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
  latentCovarianceMatrix_ = latentCovarianceModel_.discretize(fullLatentVariables_);
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
UnsignedInteger LatentVariableModel::getLevelNumber() const
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
  latentCovarianceModel_.setScale(Point(latentDim_, scale_[0]));
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
  latentCovarianceModel_.setAmplitude(amplitude_);
  updateLatentCovarianceMatrix();
}

void LatentVariableModel::setNuggetFactor(const Scalar nuggetFactor)
{
  if (!(nuggetFactor >= 0.0)) throw InvalidArgumentException(HERE) << "Error: the nugget factor=" << nuggetFactor << " is negative";
  latentCovarianceModel_.setNuggetFactor(nuggetFactor_);
  nuggetFactor_ = nuggetFactor;
  updateLatentCovarianceMatrix();
}

/* Drawing method */
Graph LatentVariableModel::draw(const UnsignedInteger /*rowIndex*/,
    const UnsignedInteger /*columnIndex*/,
    const Scalar /*zMin*/,
    const Scalar /*zMax*/,
    const UnsignedInteger /*pointNumber*/,
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
