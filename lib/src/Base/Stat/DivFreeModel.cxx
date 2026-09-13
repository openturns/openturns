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
#include "openturns/DivFreeModel.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Log.hxx"
#include "openturns/SquaredExponential.hxx"

#include <limits>

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DivFreeModel)

static const Factory<DivFreeModel> Factory_DivFreeModel;

/** Compute two representable endpoints around value for a central difference stencil.
    If the nominal symmetric step collapses (huge coordinates) or overflows near the
    finite-coordinate limits, the endpoints fall back to adjacent representable
    neighbors, and the stencil becomes one-sided at the very ends of the range. */
static void divFreeStencil(Scalar & plus,
                                 Scalar & minus,
                                 const Scalar value,
                                 const Scalar targetStep)
{
  plus = value + targetStep;
  minus = value - targetStep;
  if ((plus == value) || !std::isfinite(plus))
    plus = std::nextafter(value, std::numeric_limits<Scalar>::infinity());
  if ((minus == value) || !std::isfinite(minus))
    minus = std::nextafter(value, -std::numeric_limits<Scalar>::infinity());
  if (!std::isfinite(plus))
    plus = std::nextafter(value, -std::numeric_limits<Scalar>::infinity());
  if (!std::isfinite(minus))
    minus = std::nextafter(value, std::numeric_limits<Scalar>::infinity());
  if (!(plus > minus))
  {
    plus = value;
    minus = std::nextafter(value, -std::numeric_limits<Scalar>::infinity());
  }
}


/** Helper: compute the Hessian of the underlying scalar model */
static SquareMatrix computeHessian(const CovarianceModel & model,
                                    const Point & s,
                                    const Point & t,
                                    const UnsignedInteger inputDimension)
{
  SquareMatrix hessian(inputDimension);
  const Scalar baseEps = std::cbrt(SpecFunc::ScalarEpsilon);
  const Point scale(model.getScale());
  const Bool isotropic = (scale.getSize() == 1);
  for (UnsignedInteger j = 0; j < inputDimension; ++j)
  {
    const Scalar epsilon = (isotropic ? scale[0] : scale[j]) * baseEps;
    Scalar tPlusValue = 0.0;
    Scalar tMinusValue = 0.0;
    divFreeStencil(tPlusValue, tMinusValue, t[j], epsilon);
    Point tPlus(t);
    Point tMinus(t);
    tPlus[j] = tPlusValue;
    tMinus[j] = tMinusValue;
    const Scalar delta = tPlusValue - tMinusValue;
    const Matrix gradPlus(model.partialGradient(s, tPlus));
    const Matrix gradMinus(model.partialGradient(s, tMinus));
    for (UnsignedInteger i = 0; i < inputDimension; ++i)
    {
      hessian(i, j) = (gradPlus(i, 0) - gradMinus(i, 0)) / delta;
    }
  }
  return hessian;
}


/** Helper: compute the Hessian of the underlying scalar model for stationary case */
static SquareMatrix computeHessianStationary(const CovarianceModel & model,
    const Point & tau,
    const UnsignedInteger inputDimension)
{
  SquareMatrix hessian(inputDimension);
  const Scalar baseEps = std::cbrt(SpecFunc::ScalarEpsilon);
  const Point scale(model.getScale());
  const Bool isotropic = (scale.getSize() == 1);
  const Point zero(inputDimension);
  for (UnsignedInteger j = 0; j < inputDimension; ++j)
  {
    const Scalar epsilon = (isotropic ? scale[0] : scale[j]) * baseEps;
    Scalar tauPlusValue = 0.0;
    Scalar tauMinusValue = 0.0;
    divFreeStencil(tauPlusValue, tauMinusValue, tau[j], epsilon);
    Point tauPlus(tau);
    Point tauMinus(tau);
    tauPlus[j] = tauPlusValue;
    tauMinus[j] = tauMinusValue;
    const Scalar delta = tauPlusValue - tauMinusValue;
    const Matrix gradPlus(model.partialGradient(tauPlus, zero));
    const Matrix gradMinus(model.partialGradient(tauMinus, zero));
    for (UnsignedInteger i = 0; i < inputDimension; ++i)
    {
      hessian(i, j) = -(gradPlus(i, 0) - gradMinus(i, 0)) / delta;
    }
  }
  return hessian;
}


/* Default constructor */
DivFreeModel::DivFreeModel(const UnsignedInteger inputDimension)
  : CovarianceModelImplementation(inputDimension)
  , model_(SquaredExponential(Point(inputDimension, 1.0)))
{
  if (inputDimension == 1)
    throw InvalidArgumentException(HERE) << "Error: the divergence-free model is not defined for input dimension=1.";
  if (inputDimension > 3)
    LOGWARN(OSS() << "The divergence-free model is proven for d=2,3; for d>3 it is a conjecture, see [scheuerer2012].");
  outputDimension_ = inputDimension;
  isStationary_ = model_.isStationary();
  activeParameter_ = model_.getActiveParameter();
  amplitude_ = Point(outputDimension_);
  const SquareMatrix C0(operator()(Point(inputDimension_)));
  for (UnsignedInteger j = 0; j < outputDimension_; ++j)
    amplitude_[j] = std::sqrt(std::abs(C0(j, j)));
  updateOutputCovariance();
}


/* Parameters constructor */
DivFreeModel::DivFreeModel(const CovarianceModel & model)
  : CovarianceModelImplementation(model.getInputDimension())
  , model_(model)
{
  if (model.getOutputDimension() != 1)
    throw InvalidArgumentException(HERE) << "Error: the underlying covariance model must be scalar (output dimension=1), got output dimension=" << model.getOutputDimension();
  if (model.getInputDimension() == 1)
    throw InvalidArgumentException(HERE) << "Error: the divergence-free model is not defined for input dimension=1.";
  if (model.getInputDimension() > 3)
    LOGWARN(OSS() << "The divergence-free model is proven for d=2,3; for d>3 it is a conjecture, see [scheuerer2012].");
  outputDimension_ = model.getInputDimension();
  scale_ = model.getScale();
  isStationary_ = model.isStationary();
  activeParameter_ = model.getActiveParameter();
  amplitude_ = Point(outputDimension_);
  const SquareMatrix C0(operator()(Point(inputDimension_)));
  for (UnsignedInteger j = 0; j < outputDimension_; ++j)
    amplitude_[j] = std::sqrt(std::abs(C0(j, j)));
  updateOutputCovariance();
}


/* Virtual constructor */
DivFreeModel * DivFreeModel::clone() const
{
  return new DivFreeModel(*this);
}


SquareMatrix DivFreeModel::operator()(const Point & s, const Point & t) const
{
  if (s.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;

  const SquareMatrix hessian(computeHessian(model_, s, t, inputDimension_));

  // C_div = trace(H) * I_d - H
  Scalar trace = 0.0;
  for (UnsignedInteger k = 0; k < inputDimension_; ++k) trace += hessian(k, k);

  SquareMatrix result(outputDimension_);
  for (UnsignedInteger i = 0; i < inputDimension_; ++i)
  {
    for (UnsignedInteger j = 0; j < inputDimension_; ++j)
    {
      if (i == j)
        result(i, j) = trace - hessian(i, j);
      else
        result(i, j) = -hessian(i, j);
    }
  }
  return result;
}


SquareMatrix DivFreeModel::operator()(const Point & tau) const
{
  if (tau.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "Error: the point tau has dimension=" << tau.getDimension() << ", expected dimension=" << inputDimension_;

  const SquareMatrix hessian(computeHessianStationary(model_, tau, inputDimension_));

  // C_div = trace(H) * I_d - H
  Scalar trace = 0.0;
  for (UnsignedInteger k = 0; k < inputDimension_; ++k) trace += hessian(k, k);

  SquareMatrix result(outputDimension_);
  for (UnsignedInteger i = 0; i < inputDimension_; ++i)
  {
    for (UnsignedInteger j = 0; j < inputDimension_; ++j)
    {
      if (i == j)
        result(i, j) = trace - hessian(i, j);
      else
        result(i, j) = -hessian(i, j);
    }
  }
  return result;
}


Bool DivFreeModel::isStationary() const
{
  return model_.isStationary();
}


void DivFreeModel::setScale(const Point & scale)
{
  model_.setScale(scale);
  scale_ = model_.getScale();
  const SquareMatrix C0(operator()(Point(inputDimension_)));
  for (UnsignedInteger j = 0; j < outputDimension_; ++j)
    amplitude_[j] = std::sqrt(std::abs(C0(j, j)));
  updateOutputCovariance();
}


/* String converter */
String DivFreeModel::__repr__() const
{
  OSS oss;
  oss << "class=" << DivFreeModel::GetClassName()
      << " input dimension=" << inputDimension_
      << " model=" << model_;
  return oss;
}


/* String converter */
String DivFreeModel::__str__(const String & ) const
{
  OSS oss;
  oss << "DivFreeModel(" << model_.__str__() << ")";
  return oss;
}


CovarianceModel DivFreeModel::getCovarianceModel() const
{
  return model_;
}

Scalar DivFreeModel::computeAsScalar(const Point & s, const Point & t) const
{
  return operator()(s, t)(0, 0);
}

Scalar DivFreeModel::computeAsScalar(const Point & tau) const
{
  return operator()(tau)(0, 0);
}

Scalar DivFreeModel::computeAsScalar(const Scalar, const Scalar) const
{
  throw NotDefinedException(HERE) << "Error: the covariance model has input dimension=" << inputDimension_ << ", this method is only for input dimension=1.";
}

Scalar DivFreeModel::computeAsScalar(const Scalar) const
{
  throw NotDefinedException(HERE) << "Error: the covariance model has input dimension=" << inputDimension_ << ", this method is only for input dimension=1.";
}

Scalar DivFreeModel::computeAsScalar(const Collection<Scalar>::const_iterator & s_begin,
                                     const Collection<Scalar>::const_iterator & t_begin) const
{
  const Point s(s_begin, s_begin + inputDimension_);
  const Point t(t_begin, t_begin + inputDimension_);
  return computeAsScalar(s, t);
}


/* Method save() stores the object through the StorageManager */
void DivFreeModel::save(Advocate & adv) const
{
  CovarianceModelImplementation::save(adv);
  adv.saveAttribute("model_", model_);
}


/* Method load() reloads the object from the StorageManager */
void DivFreeModel::load(Advocate & adv)
{
  CovarianceModelImplementation::load(adv);
  adv.loadAttribute("model_", model_);
}


END_NAMESPACE_OPENTURNS
