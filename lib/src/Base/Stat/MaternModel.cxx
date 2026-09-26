//                                               -*- C++ -*-
/**
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
  : CovarianceModelImplementation(Point(inputDimension, ResourceMap::GetAsScalar("MaternModel-DefaultTheta")), Point(1, 1.0))
  , nu_(ResourceMap::GetAsScalar("MaternModel-DefaultNu"))
  , sqrt2nuOverTheta_(Point(inputDimension, sqrt(2.0 * nu_) / ResourceMap::GetAsScalar("MaternModel-DefaultTheta") ))
{
  isStationary_ = true;
  // Compute the normalization factor
  computeLogNormalizationFactor();
  // Compute useful scaling factor
  computeSqrt2nuOverTheta();
}

/** Parameters constructor */
MaternModel::MaternModel(const Point & scale,
                         const Scalar nu)
  : CovarianceModelImplementation(scale, Point(1, 1.0))
  , nu_(0.0)
  , sqrt2nuOverTheta_(Point(scale.getDimension(), 0.0))
{
  isStationary_ = true;
  setNu(nu);
}

/** Parameters constructor */
MaternModel::MaternModel(const Point & scale,
                         const Point & amplitude,
                         const Scalar nu)
  : CovarianceModelImplementation(scale, amplitude)
  , nu_(0.0)
  , sqrt2nuOverTheta_(Point(scale.getDimension(), 0.0))
{
  isStationary_ = true;
  if (getOutputDimension() != 1)
    throw InvalidArgumentException(HERE) << "In MaternModel::MaternModel, only unidimensional models should be defined."
                                         << " Here, (got dimension=" << getOutputDimension() << ")";
  setNu(nu);
}

void MaternModel::computeLogNormalizationFactor()
{
  // Compute the normalization factor
  logNormalizationFactor_ = (1.0 - nu_) * std::log(2.0) - SpecFunc::LogGamma(nu_);
}

void MaternModel::computeSqrt2nuOverTheta()
{
  // Compute useful scaling factor
  for(UnsignedInteger i = 0; i < inputDimension_; ++i) sqrt2nuOverTheta_[i] = sqrt(2.0 * nu_) / scale_[i];
  // The closed form of the covariance value only depends on nu
  updateClosedForm();
}

/* Virtual constructor */
MaternModel * MaternModel::clone() const
{
  return new MaternModel(*this);
}

/* Comparison operators */
Bool MaternModel::operator ==(const MaternModel & other) const
{
  if (this == &other) return true;
  return hasEqualBase(other) && (nu_ == other.nu_);
}

Bool MaternModel::equals(const CovarianceModelImplementation & other) const
{
  const MaternModel * p_other = dynamic_cast<const MaternModel *>(&other);
  return p_other && (*this == *p_other);
}

/* Computation of the covariance  function */
Scalar MaternModel::computeAsScalar(const Point & tau) const
{
  if (tau.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: expected a shift of dimension=" << inputDimension_ << ", got dimension=" << tau.getDimension();
  Point scaledTau(inputDimension_);
  for(UnsignedInteger i = 0; i < inputDimension_; ++i) scaledTau[i] = tau[i] * sqrt2nuOverTheta_[i];
  const Scalar scaledPoint = scaledTau.norm();
  const CovarianceMatrix & outputCovariance = outputCovariance_;
  if (scaledPoint <= SpecFunc::ScalarEpsilon)
    return outputCovariance(0, 0) * (1.0 + nuggetFactor_);
  else
    return outputCovariance(0, 0) * computeCovarianceValue(scaledPoint);
}

Scalar MaternModel::computeAsScalar(const Collection<Scalar>::const_iterator & s_begin,
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
  // The diagonal of the output covariance is the squared amplitude by
  // construction (see CovarianceModelImplementation::updateOutputCovariance);
  // reading amplitude_ avoids a matrix access in the evaluation paths.
  const Scalar amplitudeSquare = amplitude_[0] * amplitude_[0];
  if (scaledPoint <= SpecFunc::ScalarEpsilon)
    return amplitudeSquare * (1.0 + nuggetFactor_);
  else
    return amplitudeSquare * computeCovarianceValue(scaledPoint);
}

// Entry point for the bulk evaluations of the compressed matrix assemblies:
// same operations, in the same order, as the iterator based version above,
// with nothing to build and no virtual call below this one.
Scalar MaternModel::computeAsScalar(const Scalar * s, const Scalar * t) const
{
  if (outputDimension_ != 1)
    throw InvalidArgumentException(HERE) << "Error: in MaternModel::computeAsScalar(s, t), the covariance model is of dimension=" << outputDimension_ << ", expected dimension=1.";
  Scalar scaledPoint = 0;
  for (UnsignedInteger i = 0; i < inputDimension_; ++i)
  {
    const Scalar dx = (s[i] - t[i]) * sqrt2nuOverTheta_[i];
    scaledPoint += dx * dx;
  }
  scaledPoint = std::sqrt(scaledPoint);
  const Scalar amplitudeSquare = amplitude_[0] * amplitude_[0];
  if (scaledPoint <= SpecFunc::ScalarEpsilon)
    return amplitudeSquare * (1.0 + nuggetFactor_);
  else
    return amplitudeSquare * computeCovarianceValue(scaledPoint);
}

Scalar MaternModel::computeAsScalar(const Scalar tau) const
{
  if (inputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has input dimension=" << inputDimension_ << ", expected input dimension=1.";
  if (outputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has output dimension=" << outputDimension_ << ", expected dimension=1.";
  const Scalar scaledPoint = std::abs(tau * sqrt2nuOverTheta_[0]);
  const Scalar amplitudeSquare = amplitude_[0] * amplitude_[0];
  if (scaledPoint <= SpecFunc::ScalarEpsilon)
    return amplitudeSquare * (1.0 + nuggetFactor_);
  else
    return amplitudeSquare * computeCovarianceValue(scaledPoint);
}

// Select the closed form once and for all: the evaluation paths only test
// closedForm_, which is set whenever a parameter changes, instead of comparing
// nu_ against the half-integer values on every call.
void MaternModel::updateClosedForm()
{
  if (nu_ == 0.5)
    closedForm_ = 1;
  else if (nu_ == 1.5)
    closedForm_ = 2;
  else if (nu_ == 2.5)
    closedForm_ = 3;
  else
    closedForm_ = 0;
}

// Exact value of the Matern covariance for the scaled distance scaledPoint =
// sqrt(2 nu) * ||tau|| / scale. For the half-integer smoothness nu = p + 1/2
// the modified Bessel function K_nu reduces to exp(-s) times a polynomial of
// degree p in s, which is cheaper and numerically more stable than the
// general formula in terms of log and LogBesselK. Which form applies has
// already been decided by updateClosedForm().
Scalar MaternModel::computeCovarianceValue(const Scalar scaledPoint) const
{
  if (closedForm_ == 1)
    return std::exp(-scaledPoint);
  if (closedForm_ == 2)
  {
    const Scalar s = scaledPoint;
    return std::exp(-s) * (1.0 + s);
  }
  if (closedForm_ == 3)
  {
    const Scalar s = scaledPoint;
    return std::exp(-s) * (1.0 + s * (1.0 + s / 3.0));
  }
  return std::exp(logNormalizationFactor_ + nu_ * std::log(scaledPoint) + SpecFunc::LogBesselK(nu_, scaledPoint));
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
    if (nu_ < 0.5) return Matrix(inputDimension_, 1, Point(inputDimension_, SpecFunc::LowestScalar));
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
  CovarianceModelImplementation::setScale(scale);
  // Update scaling factor
  computeSqrt2nuOverTheta();
}

void MaternModel::setFullParameter(const Point & parameter)
{
  /*
    Care! To make the method not bogus, the size of parameter argument
    should be :
     - Size of scale : inputDimension_
     - Size of amplitude : here 1
     - Nu parameter : 1
    CovarianceModelImplementation::setFullParameter checks that size is
    equal to inputDimension_ + dimension_
  As we set the Nu parameter it is not coherent.
  We should check that totalSize = generic totalSize + 1
  */
  // Check the size
  const UnsignedInteger totalSize = inputDimension_ + outputDimension_  + 2;
  if (parameter.getSize() != totalSize)
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
  CovarianceModelImplementation::save(adv);
  adv.saveAttribute("nu_", nu_);
  adv.saveAttribute("logNormalizationFactor_", logNormalizationFactor_);
  adv.saveAttribute("sqrt2nuOverTheta_", sqrt2nuOverTheta_);
}

/* Method load() reloads the object from the StorageManager */
void MaternModel::load(Advocate & adv)
{
  CovarianceModelImplementation::load(adv);
  adv.loadAttribute("nu_", nu_);
  adv.loadAttribute("logNormalizationFactor_", logNormalizationFactor_);
  adv.loadAttribute("sqrt2nuOverTheta_", sqrt2nuOverTheta_);
  // The closed form is derived from nu_, it is not stored
  updateClosedForm();
}

END_NAMESPACE_OPENTURNS
