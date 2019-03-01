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
#include "openturns/MaternModel.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MaternModel)

static const Factory<MaternModel> Factory_MaternModel;


/* Default constructor */
MaternModel::MaternModel(const UnsignedInteger inputDimension)
  : StationaryCovarianceModel(Point(inputDimension, ResourceMap::GetAsScalar("MaternModel-DefaultTheta")), Point(1, 1.0))
  , nu_(ResourceMap::GetAsScalar("MaternModel-DefaultNu"))
  , sqrt2nuOverTheta_(Point(inputDimension, sqrt(2.0 * nu_) / ResourceMap::GetAsScalar("MaternModel-DefaultTheta") ))
{
  definesComputeStandardRepresentative_ = true;
  // Compute the normalization factor
  computeLogNormalizationFactor();
  // Compute useful scaling factor
  computeSqrt2nuOverTheta();
}

/** Parameters constructor */
MaternModel::MaternModel(const Point & scale,
                         const Scalar nu)
  : StationaryCovarianceModel(scale, Point(1, 1.0))
  , nu_(0.0)
  , sqrt2nuOverTheta_(Point(scale.getDimension(), 0.0))
{
  definesComputeStandardRepresentative_ = true;
  setNu(nu);
}

/** Parameters constructor */
MaternModel::MaternModel(const Point & scale,
                         const Point & amplitude,
                         const Scalar nu)
  : StationaryCovarianceModel(scale, amplitude)
  , nu_(0.0)
  , sqrt2nuOverTheta_(Point(scale.getDimension(), 0.0))
{
  if (getOutputDimension() != 1)
    throw InvalidArgumentException(HERE) << "In MaternModel::MaternModel, only unidimensional models should be defined."
                                         << " Here, (got dimension=" << getOutputDimension() << ")";
  definesComputeStandardRepresentative_ = true;
  setNu(nu);
}

void MaternModel::computeLogNormalizationFactor()
{
  // Compute the normalization factor
  logNormalizationFactor_ = (1.0 - nu_) * std::log(2.0) - SpecFunc::LogGamma(nu_);
}

void MaternModel::computeSqrt2nuOverTheta()
{
  Point theta(scale_);
  switch (scaleParametrization_)
  {
    case STANDARD:
      // nothing to do
      break;
    case INVERSE:
      for(UnsignedInteger i = 0; i < inputDimension_; ++i)
        theta[i] = 1.0 / scale_[i];
      break;
    case LOGINVERSE:
      for(UnsignedInteger i = 0; i < inputDimension_; ++i)
        theta[i] = std::exp(- scale_[i]);
      break;
  }

  // Compute useful scaling factor
  for(UnsignedInteger i = 0; i < inputDimension_; ++i) sqrt2nuOverTheta_[i] = sqrt(2.0 * nu_) / theta[i];
}

/* Virtual constructor */
MaternModel * MaternModel::clone() const
{
  return new MaternModel(*this);
}

/* Computation of the covariance  function */
Scalar MaternModel::computeStandardRepresentative(const Point & tau) const
{
  if (tau.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: expected a shift of dimension=" << inputDimension_ << ", got dimension=" << tau.getDimension();
  Point scaledTau(inputDimension_);
  for(UnsignedInteger i = 0; i < inputDimension_; ++i) scaledTau[i] = tau[i] * sqrt2nuOverTheta_[i];
  const Scalar scaledPoint = scaledTau.norm();
  if (scaledPoint <= SpecFunc::ScalarEpsilon)
    return 1.0 + nuggetFactor_;
  else
    return exp(logNormalizationFactor_ + nu_ * std::log(scaledPoint) + SpecFunc::LogBesselK(nu_, scaledPoint));
}

Scalar MaternModel::computeStandardRepresentative(const Collection<Scalar>::const_iterator & s_begin,
    const Collection<Scalar>::const_iterator & t_begin) const
{
  Scalar scaledPoint = 0;
  Collection<Scalar>::const_iterator s_it = s_begin;
  Collection<Scalar>::const_iterator t_it = t_begin;
  for (UnsignedInteger i = 0; i < inputDimension_; ++i, ++s_it, ++t_it)
  {
    const Scalar dx = (*s_it - *t_it) * sqrt2nuOverTheta_[i];
    scaledPoint += dx * dx;
  }
  scaledPoint = sqrt(scaledPoint);
  if (scaledPoint <= SpecFunc::ScalarEpsilon)
    return 1.0 + nuggetFactor_;
  else
    return exp(logNormalizationFactor_ + nu_ * std::log(scaledPoint) + SpecFunc::LogBesselK(nu_, scaledPoint));
}

void MaternModel::setScaleParametrization(const ScaleParametrization scaleParametrization)
{
  CovarianceModelImplementation::setScaleParametrization(scaleParametrization);
  computeSqrt2nuOverTheta();
}


/* Gradient */
Matrix MaternModel::partialGradient(const Point & s,
                                    const Point & t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;
  const Point tau(s - t);
  Point scaledTau(inputDimension_);
  for(UnsignedInteger i = 0; i < inputDimension_; ++i) scaledTau[i] = tau[i] * sqrt2nuOverTheta_[i];
  const Scalar scaledTauNorm = scaledTau.norm();
  const Scalar norm2 = scaledTauNorm * scaledTauNorm;

  // For zero norm
  if (norm2 == 0.0)
  {
    // Infinite gradient for nu < 1/2
    if (nu_ < 0.5) return Matrix(inputDimension_, 1, Point(inputDimension_, -SpecFunc::MaxScalar));
    // Non-zero gradient for nu = 1/2
    if (nu_ == 0.5)
    {
      Matrix gradient(inputDimension_, 1);
      for (UnsignedInteger i = 0; i < inputDimension_; ++i) gradient(i, 0) = -amplitude_[0] * amplitude_[0] / scale_[i];
      return gradient;
    }
    // Zero gradient for p > 1
    return Matrix(inputDimension_, 1);
  }
  // General case
  const Scalar value = std::exp(logNormalizationFactor_ + nu_ * std::log(scaledTauNorm)) * (nu_ * SpecFunc::BesselK(nu_, scaledTauNorm) + SpecFunc::BesselKDerivative(nu_, scaledTauNorm) * scaledTauNorm) / norm2;
  Point tauDotsquareSqrt2nuOverTheta(inputDimension_);
  for(UnsignedInteger i = 0; i < inputDimension_; ++i) tauDotsquareSqrt2nuOverTheta[i] = tau[i] * sqrt2nuOverTheta_[i] * sqrt2nuOverTheta_[i];
  return Matrix(inputDimension_, 1, tauDotsquareSqrt2nuOverTheta * value) * amplitude_[0] * amplitude_[0];
}

void MaternModel::setScale(const Point & scale)
{
  // First set scale
  StationaryCovarianceModel::setScale(scale);
  // Update scaling factor
  computeSqrt2nuOverTheta();
}

void MaternModel::setFullParameter(const Point & parameter)
{
  /*
    Care! To make the method not bogus, the size of paramter argument
    should be :
     - Size of scale : inputDimension_
     - Size of amplitude : here 1
     - Nu parameter : 1
    CovarianceModelImplementation::setFullParameter checks that size is
    equal to inputDimension_ + dimension_
  As we set the Nu parameter it is not coherant.
  We should check that totalSize = generic totalSize + 1
  */
  // Check the size
  const UnsignedInteger totalSize = inputDimension_ + outputDimension_  + 1;
  if (parameter.getSize() < totalSize)
    throw InvalidArgumentException(HERE) << "In MaternModel::setFullParameter, points have incompatible size. Point size = " << parameter.getSize()
                                         << " whereas expected size = " << totalSize ;
  // First set the generic parameter using CovarianceModelImplementation::setFullParameter
  CovarianceModelImplementation::setFullParameter(parameter);
  // We recompute the internal parameter (see ticket 905)
  computeSqrt2nuOverTheta();
  // We set the Nu parameter
  // If Nu is the same as the model's one, nothing will be done
  setNu(parameter[parameter.getSize() - 1]);
}

Point MaternModel::getFullParameter() const
{
  // Get the generic parameter
  Point parameter(CovarianceModelImplementation::getFullParameter());
  // Add the specific one
  parameter.add(nu_);
  return parameter;
}

Description MaternModel::getFullParameterDescription() const
{
  // Description of the generic parameter
  Description description(CovarianceModelImplementation::getFullParameterDescription());
  // Description of the specific parameter
  description.add("nu");
  return description;
}

/* String converter */
String MaternModel::__repr__() const
{
  OSS oss;
  oss << "class=" << MaternModel::GetClassName()
      << " scale=" << scale_
      << " amplitude=" << amplitude_
      << " nu=" << nu_;
  return oss;
}

/* String converter */
String MaternModel::__str__(const String & ) const
{
  OSS oss;
  oss << MaternModel::GetClassName()
      << "(scale=" << scale_.__str__()
      << ", amplitude=" << amplitude_.__str__()
      << ", nu=" << nu_
      << ")";
  return oss;
}

/* Nu accessor */
Scalar MaternModel::getNu() const
{
  return nu_;
}

void MaternModel::setNu(const Scalar nu)
{
  if (!(nu > 0.0)) throw InvalidArgumentException(HERE) << "Error: nu must be positive.";
  if (!(nu == nu_))
  {
    nu_ = nu;
    // Compute the normalization factor
    computeLogNormalizationFactor();
    // Compute useful scaling factor
    computeSqrt2nuOverTheta();
  }
}

/* Method save() stores the object through the StorageManager */
void MaternModel::save(Advocate & adv) const
{
  StationaryCovarianceModel::save(adv);
  adv.saveAttribute("nu_", nu_);
  adv.saveAttribute("logNormalizationFactor_", logNormalizationFactor_);
  adv.saveAttribute("sqrt2nuOverTheta_", sqrt2nuOverTheta_);
}

/* Method load() reloads the object from the StorageManager */
void MaternModel::load(Advocate & adv)
{
  StationaryCovarianceModel::load(adv);
  adv.loadAttribute("nu_", nu_);
  adv.loadAttribute("logNormalizationFactor_", logNormalizationFactor_);
  adv.loadAttribute("sqrt2nuOverTheta_", sqrt2nuOverTheta_);
}

END_NAMESPACE_OPENTURNS
