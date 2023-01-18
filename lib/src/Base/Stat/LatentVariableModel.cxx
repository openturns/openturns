//                                               -*- C++ -*-
/**
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LatentVariableModel)

static const Factory<LatentVariableModel> Factory_LatentVariableModel;


/* Default constructor */
LatentVariableModel::LatentVariableModel(const UnsignedInteger n_levels)
  : CovarianceModelImplementation(Point(1, 1.0), Point(1, 1.0))
  , latent_dim_(2)
  , latCovMod_(latent_dim_)
  , n_levels_(n_levels)
  , latCovMat_(n_levels_)
{
  activeLatentCoordinateDim_ = 1 + latent_dim_ * (n_levels_ - 2);
  activeLatentVariables_ = Point(activeLatentCoordinateDim_, 0.0);
  fullLatentVariables_ = Sample(n_levels_,latent_dim_);
  Indices activeParameter = Indices(inputDimension_ + outputDimension_ + activeLatentCoordinateDim_);
  activeParameter.fill();
  setActiveParameter(activeParameter);
}

/** Parameters constructor */
LatentVariableModel::LatentVariableModel(const UnsignedInteger latent_dim,
						 const UnsignedInteger n_levels)
  : CovarianceModelImplementation(Point(1, 1.0), Point(1, 1.0))
  , latent_dim_(latent_dim)
  , latCovMod_(latent_dim_)
  , n_levels_(n_levels)
  , latCovMat_(n_levels_)
{
  activeLatentCoordinateDim_ = 1 + latent_dim_ * (n_levels_ - 2);
  activeLatentVariables_ = Point(activeLatentCoordinateDim_,0.);
  fullLatentVariables_ = Sample(n_levels_,latent_dim_);
  Indices activeParameter = Indices(inputDimension_ + outputDimension_ + activeLatentCoordinateDim_);
  activeParameter.fill();
  setActiveParameter(activeParameter);
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

  for (UnsignedInteger i = 0; i < n_levels_; ++i)
  {
    if (z1 == i) isLevelz1 = true;
  }

  for (UnsignedInteger i = 0; i < n_levels_; ++i)
  {
    if (z2 == i) isLevelz2 = true;
  }

  if (!isLevelz1 || !isLevelz2) throw InvalidArgumentException(HERE) << "Error: the input discrete variables values are not amongst the known levels";

  Scalar cov = latCovMat_(z1,z2);

  return cov;
}

Scalar LatentVariableModel::computeAsScalar(const Point & z1, const Point & z2) const
{
  /*check z being one of the levels*/
  bool isLevelz1 = false;
  bool isLevelz2 = false;

  for (UnsignedInteger i = 0; i < n_levels_; ++i)
  {
    if (z1[0] == i) isLevelz1 = true;
  }

  for (UnsignedInteger i = 0; i < n_levels_; ++i)
  {
    if (z2[0] == i) isLevelz2 = true;
  }

  if (!isLevelz1 || !isLevelz2) throw InvalidArgumentException(HERE) << "Error: the input discrete variables values are not among the known levels";

  Scalar cov = latCovMat_(z1[0],z2[0]);

  return cov;
}


Scalar LatentVariableModel::computeAsScalar(const Collection<Scalar>::const_iterator & z1_begin,
    const Collection<Scalar>::const_iterator & z2_begin) const
{

  Collection<Scalar>::const_iterator z1_it = z1_begin;
  Collection<Scalar>::const_iterator z2_it = z2_begin;

  /*check z being one of the levels*/
  bool isLevelz1 = false;
  bool isLevelz2 = false;

  for (UnsignedInteger i = 0; i < n_levels_; ++i)
  {
    if (*z1_it == i) isLevelz1 = true;
  }

  for (UnsignedInteger i = 0; i < n_levels_; ++i)
  {
    if (*z2_it == i) isLevelz2 = true;
  }

  if (!isLevelz1 || !isLevelz2) throw InvalidArgumentException(HERE) << "Error: the input discrete variables values are not amongst the known levels";

  Scalar cov = latCovMat_(*z1_it,*z2_it);
  return cov;
}


void LatentVariableModel::setFullParameter(const Point & parameter)
{
  /*
    Care! To make the method not bogus, the size of parameter argument
    should be :
     - Size of scale : here 1
     - Size of amplitude : here 1
     - Number of latent variable coordinates : latent_dim_*n_levels_-2*latent_dim_+1
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
  latCovMod_.setScale(Point(latent_dim_,scale_[0]));

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
  fullLatentVariables_[0] = Point(latent_dim_,0.);
  Point levelMapping = Point(latent_dim_,0.);
  levelMapping[0] = latentVariablesCoordinates[0];
  fullLatentVariables_[1] = levelMapping;

  UnsignedInteger count = 1;
  // Fix the coordinates of the remaining latent variables
  for (UnsignedInteger i = 2; i < n_levels_; ++i)
  {
    for (UnsignedInteger j = 0; j < latent_dim_; ++j)
    {
      levelMapping[j] = latentVariablesCoordinates[count];
      count++;
    }
    fullLatentVariables_[i] = levelMapping;
  }

  updateLatentCovarianceMatrix();

}

void LatentVariableModel::updateLatentCovarianceMatrix()
{
  for (UnsignedInteger i = 0; i < n_levels_; ++i)
  {
    for (UnsignedInteger j = 0; j <= i; ++j)
    {
      latCovMat_(i,j) = latCovMod_(fullLatentVariables_[i], fullLatentVariables_[j])(0,0);
    }
  }
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

  return latent_dim_;
}

/* NLevels accessor */
UnsignedInteger LatentVariableModel::getNLevels() const
{

  return n_levels_;
}

void LatentVariableModel::setScale(const Point & scale)
{
  if (scale.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "In LatentVariableModel::setScale: the given scale has a dimension=" << scale.getDimension() << " different from the input dimension=" << inputDimension_;
  for (UnsignedInteger index = 0; index < inputDimension_; ++index)
    if (!(scale[index] > 0.0))
      throw InvalidArgumentException(HERE) << "In LatentVariableModel::setScale: the component " << index << " of scale is non positive" ;
  scale_ = scale;
  latCovMod_.setScale(Point(latent_dim_,scale_[0]));
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

/* Method save() stores the object through the StorageManager */
void LatentVariableModel::save(Advocate & adv) const
{
  CovarianceModelImplementation::save(adv);
  adv.saveAttribute("fullLatentVariables_", fullLatentVariables_);
  adv.saveAttribute("activeLatentVariables_", activeLatentVariables_);
  adv.saveAttribute("n_levels_", n_levels_);
  adv.saveAttribute("latent_dim_", latent_dim_);
// ADD OTHER ATTRIBUTES
}

/* Method load() reloads the object from the StorageManager */
void LatentVariableModel::load(Advocate & adv)
{
  CovarianceModelImplementation::load(adv);
  adv.loadAttribute("fullLatentVariables_", fullLatentVariables_);
  adv.loadAttribute("activeLatentVariables_", activeLatentVariables_);
  adv.loadAttribute("n_levels_", n_levels_);
  adv.loadAttribute("latent_dim_", latent_dim_);
}

END_NAMESPACE_OPENTURNS
