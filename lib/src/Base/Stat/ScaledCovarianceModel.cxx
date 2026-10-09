//                                               -*- C++ -*-
/**
 *  @brief Covariance model scaled by a positive factor
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
#include "openturns/ScaledCovarianceModel.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/AbsoluteExponential.hxx"
#include "openturns/OSS.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ScaledCovarianceModel)

static const Factory<ScaledCovarianceModel> Factory_ScaledCovarianceModel;

/* Default constructor */
ScaledCovarianceModel::ScaledCovarianceModel()
  : CovarianceModelImplementation(1)
  , kernel_(AbsoluteExponential(1))
  , factor_(1.0)
{
  setKernel(kernel_);
}

/* Parameters constructor: the inner model is kept as-is */
ScaledCovarianceModel::ScaledCovarianceModel(const CovarianceModel & kernel,
    const Scalar factor)
  : CovarianceModelImplementation()
  , kernel_(kernel)
  , factor_(1.0)
{
  setScaleFactor(factor);
  setKernel(kernel);
}

/* Kernel accessor */
CovarianceModel ScaledCovarianceModel::getKernel() const
{
  return kernel_;
}

void ScaledCovarianceModel::setKernel(const CovarianceModel & kernel)
{
  kernel_ = kernel;
  inputDimension_ = kernel_.getInputDimension();
  outputDimension_ = kernel_.getOutputDimension();
  isStationary_ = kernel_.isStationary();
  isDiagonal_ = kernel_.isDiagonal();
  // Dummy base fields for persistence compatibility; authoritative
  // values are delegated to the inner model, see the accessors below.
  scale_ = Point(inputDimension_, 1.0);
  amplitude_ = Point(outputDimension_, 1.0);
  nuggetFactor_ = 0.0;
  outputCorrelation_ = CorrelationMatrix();
  updateOutputCovariance();
  // Default active set: inner actives plus the trailing factor
  const Indices innerActive(kernel_.getActiveParameter());
  const UnsignedInteger innerFullSize = kernel_.getFullParameter().getSize();
  activeParameter_ = Indices(0);
  for (UnsignedInteger j = 0; j < innerActive.getSize(); ++j)
    activeParameter_.add(innerActive[j]);
  activeParameter_.add(innerFullSize);
}

/* Factor accessor */
Scalar ScaledCovarianceModel::getScaleFactor() const
{
  return factor_;
}

void ScaledCovarianceModel::setScaleFactor(const Scalar factor)
{
  if (!(factor > 0.0))
    throw InvalidArgumentException(HERE) << "Error: the scale factor must be positive, here factor=" << factor;
  factor_ = factor;
}

/* Virtual constructor */
ScaledCovarianceModel * ScaledCovarianceModel::clone() const
{
  return new ScaledCovarianceModel(*this);
}

/* Comparison operators */
Bool ScaledCovarianceModel::operator ==(const ScaledCovarianceModel & other) const
{
  if (this == &other) return true;
  if (!hasEqualBase(other)) return false;
  return (kernel_ == other.kernel_) && (factor_ == other.factor_);
}

Bool ScaledCovarianceModel::equals(const CovarianceModelImplementation & other) const
{
  const ScaledCovarianceModel * p_other = dynamic_cast<const ScaledCovarianceModel *>(&other);
  return p_other && (*this == *p_other);
}

/* Evaluation */
SquareMatrix ScaledCovarianceModel::operator()(const Point & s,
    const Point & t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;
  SquareMatrix result(kernel_(s, t));
  result = result * factor_;
  return result;
}

SquareMatrix ScaledCovarianceModel::operator()(const Point & tau) const
{
  if (!isStationary()) return CovarianceModelImplementation::operator()(tau);
  if (tau.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the lag has dimension=" << tau.getDimension() << ", expected dimension=" << inputDimension_;
  SquareMatrix result(kernel_(tau));
  result = result * factor_;
  return result;
}

Scalar ScaledCovarianceModel::computeAsScalar(const Point & s,
    const Point & t) const
{
  if (outputDimension_ != 1)
    throw InvalidArgumentException(HERE) << "Error: computeAsScalar is only defined for output dimension 1, here output dimension=" << outputDimension_;
  return factor_ * kernel_.computeAsScalar(s, t);
}

Scalar ScaledCovarianceModel::computeAsScalar(const Point & tau) const
{
  if (outputDimension_ != 1)
    throw InvalidArgumentException(HERE) << "Error: computeAsScalar is only defined for output dimension 1, here output dimension=" << outputDimension_;
  return factor_ * kernel_.computeAsScalar(tau);
}

Scalar ScaledCovarianceModel::computeAsScalar(const Collection<Scalar>::const_iterator & s_begin,
    const Collection<Scalar>::const_iterator & t_begin) const
{
  if (outputDimension_ != 1)
    throw InvalidArgumentException(HERE) << "Error: computeAsScalar is only defined for output dimension 1, here output dimension=" << outputDimension_;
  return factor_ * kernel_.getImplementation()->computeAsScalar(s_begin, t_begin);
}

Scalar ScaledCovarianceModel::computeAsScalar(const Scalar s,
    const Scalar t) const
{
  if (inputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has input dimension=" << inputDimension_ << ", expected input dimension=1.";
  if (outputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has output dimension=" << outputDimension_ << ", expected dimension=1.";
  return factor_ * kernel_.computeAsScalar(s, t);
}

Scalar ScaledCovarianceModel::computeAsScalar(const Scalar tau) const
{
  if (inputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has input dimension=" << inputDimension_ << ", expected input dimension=1.";
  if (outputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has output dimension=" << outputDimension_ << ", expected dimension=1.";
  return factor_ * kernel_.computeAsScalar(tau);
}

/* Gradient with respect to the input */
Matrix ScaledCovarianceModel::partialGradient(const Point & s,
    const Point & t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;
  Matrix result(kernel_.partialGradient(s, t));
  result = result * factor_;
  return result;
}

/* Gradient with respect to the parameters: scaled inner gradient plus
 * the inner covariance as derivative row when the factor is active */
Matrix ScaledCovarianceModel::parameterGradient(const Point & s,
    const Point & t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;
  const UnsignedInteger innerSize = kernel_.getFullParameter().getSize();
  const Matrix innerGradient(kernel_.parameterGradient(s, t));
  const UnsignedInteger columns = innerGradient.getNbColumns();
  const Indices innerActive(kernel_.getActiveParameter());
  Matrix result(activeParameter_.getSize(), columns);
  for (UnsignedInteger r = 0; r < activeParameter_.getSize(); ++r)
  {
    const UnsignedInteger global = activeParameter_[r];
    if (global < innerSize)
    {
      UnsignedInteger innerRow = innerActive.getSize();
      for (UnsignedInteger j = 0; j < innerActive.getSize(); ++j)
        if (innerActive[j] == global)
        {
          innerRow = j;
          break;
        }
      if (!(innerRow < innerGradient.getNbRows()))
        throw InternalException(HERE) << "In ScaledCovarianceModel::parameterGradient: inconsistent active parameters";
      for (UnsignedInteger c = 0; c < columns; ++c)
        result(r, c) = factor_ * innerGradient(innerRow, c);
    }
    else
    {
      // Derivative with respect to the factor: the inner covariance,
      // flattened column-major as in the base finite-difference version
      if (outputDimension_ == 1)
        result(r, 0) = kernel_.computeAsScalar(s, t);
      else
      {
        const SquareMatrix covariance(kernel_(s, t));
        UnsignedInteger index = 0;
        for (UnsignedInteger j = 0; j < outputDimension_; ++j)
          for (UnsignedInteger i = 0; i < outputDimension_; ++i)
          {
            result(r, index) = covariance(i, j);
            ++index;
          }
      }
    }
  }
  return result;
}

/* Marginal accessor */
CovarianceModel ScaledCovarianceModel::getMarginal(const UnsignedInteger index) const
{
  if (!(index < outputDimension_)) throw InvalidArgumentException(HERE) << "Error: index=" << index << " must be less than output dimension=" << outputDimension_;
  return ScaledCovarianceModel(kernel_.getMarginal(index), factor_);
}

/* Marginal accessor */
CovarianceModel ScaledCovarianceModel::getMarginal(const Indices & indices) const
{
  if (!indices.check(outputDimension_)) throw InvalidArgumentException(HERE) << "Error: indices=" << indices << " must be less than output dimension=" << outputDimension_;
  return ScaledCovarianceModel(kernel_.getMarginal(indices), factor_);
}

/* Delegated accessors */
Point ScaledCovarianceModel::getScale() const
{
  return kernel_.getScale();
}

void ScaledCovarianceModel::setScale(const Point & scale)
{
  kernel_.setScale(scale);
}

Point ScaledCovarianceModel::getAmplitude() const
{
  return kernel_.getAmplitude();
}

void ScaledCovarianceModel::setAmplitude(const Point & amplitude)
{
  kernel_.setAmplitude(amplitude);
}

CorrelationMatrix ScaledCovarianceModel::getOutputCorrelation() const
{
  return kernel_.getOutputCorrelation();
}

void ScaledCovarianceModel::setOutputCorrelation(const CorrelationMatrix & correlation)
{
  kernel_.setOutputCorrelation(correlation);
}

Scalar ScaledCovarianceModel::getNuggetFactor() const
{
  return kernel_.getNuggetFactor();
}

void ScaledCovarianceModel::setNuggetFactor(const Scalar nuggetFactor)
{
  kernel_.setNuggetFactor(nuggetFactor);
}

/* Flags */
Bool ScaledCovarianceModel::isStationary() const
{
  return kernel_.isStationary();
}

Bool ScaledCovarianceModel::isDiagonal() const
{
  return kernel_.isDiagonal();
}

Bool ScaledCovarianceModel::isParallel() const
{
  return kernel_.getImplementation()->isParallel();
}

/* String converters */
String ScaledCovarianceModel::__repr__() const
{
  OSS oss;
  oss << "class=" << ScaledCovarianceModel::GetClassName()
      << " factor=" << factor_
      << " kernel=" << kernel_;
  return oss;
}

String ScaledCovarianceModel::__str__(const String &) const
{
  return __repr__();
}

/* Full parameter: inner full parameter plus trailing factor */
Point ScaledCovarianceModel::getFullParameter() const
{
  Point result(kernel_.getFullParameter());
  result.add(factor_);
  return result;
}

void ScaledCovarianceModel::setFullParameter(const Point & parameter)
{
  const UnsignedInteger innerSize = kernel_.getFullParameter().getSize();
  if (parameter.getSize() != innerSize + 1)
    throw InvalidArgumentException(HERE) << "Error: parameter dimension should be " << innerSize + 1 << ", got " << parameter.getDimension();
  Point innerParameter(innerSize);
  std::copy(parameter.begin(), parameter.begin() + innerSize, innerParameter.begin());
  kernel_.setFullParameter(innerParameter);
  setScaleFactor(parameter[innerSize]);
}

Description ScaledCovarianceModel::getFullParameterDescription() const
{
  Description description(kernel_.getFullParameterDescription());
  description.add("factor");
  return description;
}

void ScaledCovarianceModel::setActiveParameter(const Indices & active)
{
  const UnsignedInteger innerSize = kernel_.getFullParameter().getSize();
  const UnsignedInteger totalSize = innerSize + 1;
  Indices sorted(active);
  std::sort(sorted.begin(), sorted.end());
  const Indices::const_iterator last = std::unique(sorted.begin(), sorted.end());
  if (last != sorted.end())
    throw InvalidArgumentException(HERE) << "Active parameter indices must be unique";
  for (UnsignedInteger j = 0; j < active.getSize(); ++j)
    if (!(active[j] < totalSize))
      throw InvalidArgumentException(HERE) << "Active parameter index " << active[j] << " is out of range [0, " << totalSize << ")";
  Indices innerActive(0);
  for (UnsignedInteger j = 0; j < active.getSize(); ++j)
    if (active[j] < innerSize) innerActive.add(active[j]);
  kernel_.setActiveParameter(innerActive);
  activeParameter_ = active;
}

/* Persistence */
void ScaledCovarianceModel::save(Advocate & adv) const
{
  CovarianceModelImplementation::save(adv);
  adv.saveAttribute("kernel_", kernel_);
  adv.saveAttribute("factor_", factor_);
}

void ScaledCovarianceModel::load(Advocate & adv)
{
  CovarianceModelImplementation::load(adv);
  adv.loadAttribute("kernel_", kernel_);
  Scalar factor = 1.0;
  adv.loadAttribute("factor_", factor);
  setScaleFactor(factor);
}

END_NAMESPACE_OPENTURNS
