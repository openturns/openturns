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
#include "openturns/IsotropicCovarianceModel.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(IsotropicCovarianceModel)

static const Factory<IsotropicCovarianceModel> Factory_IsotropicCovarianceModel;

IsotropicCovarianceModel::IsotropicCovarianceModel()
  : IsotropicCovarianceModel(CovarianceModel(), 1)
{
  // Nothing to do
}

/* Parameters constructor */
IsotropicCovarianceModel::IsotropicCovarianceModel(const CovarianceModel & oneDimensional,
    const UnsignedInteger inputDimension)
  : CovarianceModelImplementation(inputDimension)
{
  setKernel(oneDimensional);
}

/* 1D covariance kernel accessor */
void IsotropicCovarianceModel::setKernel(const CovarianceModel & oneDimensional)
{
  // Check if the given covariance kernel is stationary.
  if (!oneDimensional.isStationary())
  {
    throw InvalidArgumentException(HERE) << "In IsotropicCovarianceModel::setKernel, the supplied covariance kernel must be stationary.";
  }
  // Check if the given stationary covariance kernel has both input and output dimension equal to 1.
  const UnsignedInteger kernelInputDimension = oneDimensional.getInputDimension();
  if (kernelInputDimension != 1)
  {
    throw InvalidArgumentException(HERE) << "In IsotropicCovarianceModel::setKernel, the supplied covariance kernel must have input dimension 1, not " << kernelInputDimension << ".";
  }
  const UnsignedInteger kernelOutputDimension = oneDimensional.getOutputDimension();
  if (kernelOutputDimension != 1)
  {
    throw InvalidArgumentException(HERE) << "In IsotropicCovarianceModel::setKernel, the supplied covariance kernel must have output dimension 1, not " << kernelOutputDimension << ".";
  }
  outputDimension_ = kernelOutputDimension;
  kernel_ = oneDimensional;
  setActiveParameter(oneDimensional.getActiveParameter());
  setScale(oneDimensional.getScale());
  setAmplitude(oneDimensional.getAmplitude());
  setNuggetFactor(oneDimensional.getNuggetFactor());
}

CovarianceModel IsotropicCovarianceModel::getKernel() const
{
  return kernel_;
}

/* Virtual constructor */
IsotropicCovarianceModel * IsotropicCovarianceModel::clone() const
{
  return new IsotropicCovarianceModel(*this);
}

/* Computation of the covariance function */
Scalar IsotropicCovarianceModel::computeAsScalar(const Point & tau) const
{
  if (tau.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "Error: expected a shift of dimension=" << inputDimension_ << ", got dimension=" << tau.getDimension();
  return kernel_.computeAsScalar(tau.norm());
}

/* Computation of the covariance function when inputDimension_==1 (the IsotropicCovarianceModel class is useless in this case). */
Scalar IsotropicCovarianceModel::computeAsScalar(const Scalar tau) const
{
  if (inputDimension_ != 1)
    throw InvalidArgumentException(HERE) << "Error: expected a shift of dimension=" << inputDimension_ << ", got the scalar " << tau << " instead.";
  return kernel_.computeAsScalar(tau);
}

Scalar IsotropicCovarianceModel::computeAsScalar(const Collection<Scalar>::const_iterator & s_begin,
    const Collection<Scalar>::const_iterator & t_begin) const
{
  Scalar tauNormSquare = 0;
  Collection<Scalar>::const_iterator s_it = s_begin;
  Collection<Scalar>::const_iterator t_it = t_begin;
  for (UnsignedInteger i = 0; i < inputDimension_; ++i, ++s_it, ++t_it)
  {
    const Scalar dx = *s_it - *t_it;
    tauNormSquare += dx * dx;
  }
  return kernel_.computeAsScalar(std::sqrt(tauNormSquare));
}

/* Gradient */
Matrix IsotropicCovarianceModel::partialGradient(const Point & s,
    const Point & t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;
  const Point tau(s - t);
  const Scalar tauNorm = tau.norm();
  const Point normGradient(tau / tauNorm);
  const Matrix kernelPartialGradient(kernel_.partialGradient(Point(1, tauNorm), Point(1)));
  return Matrix(inputDimension_, 1, normGradient) * kernelPartialGradient(0, 0);
}

void IsotropicCovarianceModel::setFullParameter(const Point & parameter)
{
  // First update the parameters of kernel_
  kernel_.setFullParameter(parameter);

  // The safest way to set IsotropicCovarianceModel attributes is to synchronize with the updated kernel_ attributes
  scale_ = kernel_.getScale();
  amplitude_ = kernel_.getAmplitude();
  nuggetFactor_ = kernel_.getNuggetFactor();
  updateOutputCovariance();
}

Point IsotropicCovarianceModel::getFullParameter() const
{
  return kernel_.getFullParameter();
}

Description IsotropicCovarianceModel::getFullParameterDescription() const
{
  return kernel_.getFullParameterDescription();
}

/** Scale accessor */
void IsotropicCovarianceModel::setScale(const Point & scale)
{
  if (scale.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the scale should have dimension 1, not " << scale.getDimension() << ".";
  if (!(scale[0] > 0))
    throw InvalidArgumentException(HERE) << "In IsotropicCovarianceModel::setScale";
  kernel_.setScale(scale);
  scale_ = scale;
}

/* Nugget factor accessor */
void IsotropicCovarianceModel::setNuggetFactor(const Scalar nuggetFactor)
{
  kernel_.setNuggetFactor(nuggetFactor);
  nuggetFactor_ = nuggetFactor;
}

/** Indices of the active parameters */
void IsotropicCovarianceModel::setActiveParameter(const Indices & active)
{
  kernel_.setActiveParameter(active);
  activeParameter_ = active;
}

/** Amplitude accessor */
void IsotropicCovarianceModel::setAmplitude(const Point & amplitude)
{
  if (amplitude.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the amplitude should have dimension 1, not " << amplitude.getDimension() << ".";
  kernel_.setAmplitude(amplitude);
  amplitude_ = amplitude;
  updateOutputCovariance();
}

/* String converter */
String IsotropicCovarianceModel::__repr__() const
{
  OSS oss;
  oss << "class=" << IsotropicCovarianceModel::GetClassName()
      << " input dimension=" << inputDimension_
      << ", dimension = " << getOutputDimension()
      << ", kernel =" << kernel_;
  return oss;
}

/* Is it a stationary model ? */
Bool IsotropicCovarianceModel::isStationary() const
{
  return true;
}

Bool IsotropicCovarianceModel::isParallel() const
{
  return kernel_.getImplementation()->isParallel();
}

/* String converter */
String IsotropicCovarianceModel::__str__(const String & ) const
{
  return __repr__();
}

/* Method save() stores the object through the StorageManager */
void IsotropicCovarianceModel::save(Advocate & adv) const
{
  CovarianceModelImplementation::save(adv);
  adv.saveAttribute("kernel_", kernel_);
}

/* Method load() reloads the object from the StorageManager */
void IsotropicCovarianceModel::load(Advocate & adv)
{
  CovarianceModelImplementation::load(adv);
  adv.loadAttribute("kernel_", kernel_);
}

END_NAMESPACE_OPENTURNS
