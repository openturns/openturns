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
#include "openturns/StationaryFunctionalCovarianceModel.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(StationaryFunctionalCovarianceModel)

static const Factory<StationaryFunctionalCovarianceModel> Factory_StationaryFunctionalCovarianceModel;


/* Constructor based on input dimension */
StationaryFunctionalCovarianceModel::StationaryFunctionalCovarianceModel()
  : CovarianceModelImplementation(Point(1, 1.0), Point(1, 1.0))
{
  isStationary_ = true;
}

/** Parameters constructor */
StationaryFunctionalCovarianceModel::StationaryFunctionalCovarianceModel(const Point & scale,
    const Point & amplitude,
    const Function & rho)
  : CovarianceModelImplementation(scale, amplitude)
{
  isStationary_ = true;
  if (getOutputDimension() != 1)
    throw InvalidArgumentException(HERE) << "Only models with one-dimensional output should be defined"
                                         << " (got output dimension=" << getOutputDimension() << ")";
  setRho(rho);
}

/* Virtual constructor */
StationaryFunctionalCovarianceModel * StationaryFunctionalCovarianceModel::clone() const
{
  return new StationaryFunctionalCovarianceModel(*this);
}

/* Computation of the covariance function */
Scalar StationaryFunctionalCovarianceModel::computeAsScalar(const Point & tau) const
{
  if (tau.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "Error: expected a shift of dimension=" << inputDimension_ << ", got dimension=" << tau.getDimension();
  Point tauOverTheta(tau);
  for (UnsignedInteger i = 0; i < inputDimension_; ++ i)
    tauOverTheta[i] /= scale_[i];
  // The model is stationary
  // Thus we should care about value for tau=0
  const Scalar tauOverThetaNorm = tauOverTheta.norm();
  const CovarianceMatrix & outputCovariance = outputCovariance_;
  if (tauOverThetaNorm <= SpecFunc::ScalarEpsilon)
    return outputCovariance(0, 0) * (1.0 + nuggetFactor_);
  return outputCovariance(0, 0) * rho_(tauOverTheta)[0];
}

Scalar StationaryFunctionalCovarianceModel::computeAsScalar(const Collection<Scalar>::const_iterator & s_begin,
    const Collection<Scalar>::const_iterator & t_begin) const
{
  Point tauOverTheta(inputDimension_);
  Collection<Scalar>::const_iterator s_it = s_begin;
  Collection<Scalar>::const_iterator t_it = t_begin;
  for (UnsignedInteger i = 0; i < inputDimension_; ++ i, ++ s_it, ++ t_it)
    tauOverTheta[i] = (*s_it - *t_it) / scale_[i];
  // The model is stationary
  // Thus we should care about value for tau=0
  const Scalar tauOverThetaNorm = tauOverTheta.norm();
  const CovarianceMatrix & outputCovariance = outputCovariance_;
  if (tauOverThetaNorm <= SpecFunc::ScalarEpsilon)
    return outputCovariance(0, 0) * (1.0 + nuggetFactor_);
  return outputCovariance(0, 0) * rho_(tauOverTheta)[0];
}

Scalar StationaryFunctionalCovarianceModel::computeAsScalar(const Scalar tau) const
{
  if (inputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has input dimension=" << inputDimension_ << ", expected input dimension=1.";
  if (outputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has output dimension=" << outputDimension_ << ", expected dimension=1.";

  const Scalar tauOverThetaNorm = std::abs(tau / scale_[0]);
  const CovarianceMatrix & outputCovariance = outputCovariance_;
  if (tauOverThetaNorm <= SpecFunc::ScalarEpsilon)
    return outputCovariance(0, 0) * (1.0 + nuggetFactor_);
  const Point tauOverTheta(1, tau / scale_[0]);
  return outputCovariance(0, 0) * rho_(tauOverTheta)[0];
}

/* Gradient */
Matrix StationaryFunctionalCovarianceModel::partialGradient(const Point & s,
    const Point & t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected to match the input dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected to match the input dimension=" << inputDimension_;
  const Point tau(s - t);
  Point tauOverTheta(inputDimension_);
  for (UnsignedInteger i = 0; i < inputDimension_; ++ i) tauOverTheta[i] = std::abs(tau[i]) / scale_[i];
  Matrix grad(amplitude_[0] * amplitude_[0] * rho_.gradient(tauOverTheta));
  for (UnsignedInteger i = 0; i < inputDimension_; ++ i)
    if (tau[i] < 0.0) grad(i, 0) *= -1.0;
  return grad;
}

/* Correlation function accessor */
Function StationaryFunctionalCovarianceModel::getRho() const
{
  return rho_;
}

void StationaryFunctionalCovarianceModel::setRho(const Function & rho)
{
  if (rho.getInputDimension() != getInputDimension())
    throw InvalidArgumentException(HERE) << "In StationaryFunctionalCovarianceModel, function input dimension must match input dimension (" << getInputDimension()
                                         << ") Here, (got inputDimension=" << rho.getInputDimension() << ")";
  if (rho.getOutputDimension() != 1)
    throw InvalidArgumentException(HERE) << "In StationaryFunctionalCovarianceModel, function output dimension must be 1"
                                         << " Here, (got dimension=" << rho.getOutputDimension() << ")";
  rho_ = rho;
}


void StationaryFunctionalCovarianceModel::setFullParameter(const Point & parameter)
{
  CovarianceModelImplementation::setFullParameter(parameter);
  Point functionParameter(rho_.getParameter().getDimension());
  std::copy(parameter.begin() + getInputDimension() + getOutputDimension(), parameter.end(), functionParameter.begin());
  rho_.setParameter(functionParameter);
}

Point StationaryFunctionalCovarianceModel::getFullParameter() const
{
  // Get the generic parameter
  Point parameter(CovarianceModelImplementation::getFullParameter());
  // Add the specific one
  parameter.add(rho_.getParameter());
  return parameter;
}

Description StationaryFunctionalCovarianceModel::getFullParameterDescription() const
{
  // Description of the generic parameter
  Description description(CovarianceModelImplementation::getFullParameterDescription());
  // Description of the specific parameter
  description.add(rho_.getParameterDescription());
  return description;
}

/* Is it safe to compute discretize etc in parallel? */
Bool StationaryFunctionalCovarianceModel::isParallel() const
{
  return rho_.getImplementation()->isParallel();
}

/* String converter */
String StationaryFunctionalCovarianceModel::__repr__() const
{
  OSS oss;
  oss << "class=" << StationaryFunctionalCovarianceModel::GetClassName()
      << " scale=" << scale_
      << " amplitude=" << amplitude_
      << " rho=" << rho_;
  return oss;
}

/* String converter */
String StationaryFunctionalCovarianceModel::__str__(const String & ) const
{
  OSS oss;
  oss << StationaryFunctionalCovarianceModel::GetClassName()
      << "(scale=" << scale_.__str__()
      << ", amplitude=" << amplitude_.__str__()
      << ", rho=" << rho_.__str__()
      << ")";
  return oss;
}

/* Method save() stores the object through the StorageManager */
void StationaryFunctionalCovarianceModel::save(Advocate & adv) const
{
  CovarianceModelImplementation::save(adv);
  adv.saveAttribute( "rho_", rho_);
}

/* Method load() reloads the object from the StorageManager */
void StationaryFunctionalCovarianceModel::load(Advocate & adv)
{
  CovarianceModelImplementation::load(adv);
  adv.loadAttribute( "rho_", rho_);
}

END_NAMESPACE_OPENTURNS
