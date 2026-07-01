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
#include "openturns/CurlFreeModel.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Log.hxx"
#include "openturns/SquaredExponential.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(CurlFreeModel)

static const Factory<CurlFreeModel> Factory_CurlFreeModel;

/* Default constructor */
CurlFreeModel::CurlFreeModel(const UnsignedInteger inputDimension)
  : CovarianceModelImplementation(inputDimension)
  , model_(SquaredExponential(Point(inputDimension, 1.0)))
{
  if (inputDimension == 1)
    throw InvalidArgumentException(HERE) << "Error: the curl-free model is not defined for input dimension=1.";
  if (inputDimension > 3)
    LOGWARN(OSS() << "The curl-free model is proven for d=2,3; for d>3 it is a conjecture, see [scheuerer2012].");
  outputDimension_ = inputDimension;
  isStationary_ = model_.isStationary();
  amplitude_ = Point(outputDimension_, 1.0);
  activeParameter_ = model_.getActiveParameter();
  updateOutputCovariance();
}


/* Parameters constructor */
CurlFreeModel::CurlFreeModel(const CovarianceModel & model)
  : CovarianceModelImplementation(model.getInputDimension())
  , model_(model)
{
  if (model.getOutputDimension() != 1)
    throw InvalidArgumentException(HERE) << "Error: the underlying covariance model must be scalar (output dimension=1), got output dimension=" << model.getOutputDimension();
  if (model.getInputDimension() == 1)
    throw InvalidArgumentException(HERE) << "Error: the curl-free model is not defined for input dimension=1.";
  if (model.getInputDimension() > 3)
    LOGWARN(OSS() << "The curl-free model is proven for d=2,3; for d>3 it is a conjecture, see [scheuerer2012].");
  outputDimension_ = model.getInputDimension();
  scale_ = model.getScale();
  amplitude_ = Point(outputDimension_, 1.0);
  isStationary_ = model.isStationary();
  activeParameter_ = model.getActiveParameter();
  updateOutputCovariance();
}


/* Virtual constructor */
CurlFreeModel * CurlFreeModel::clone() const
{
  return new CurlFreeModel(*this);
}


SquareMatrix CurlFreeModel::operator()(const Point & s, const Point & t) const
{
  if (s.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;

  SquareMatrix result(outputDimension_);
  const Scalar epsilon = std::cbrt(SpecFunc::ScalarEpsilon);

  for (UnsignedInteger j = 0; j < inputDimension_; ++j)
  {
    Point tPlus(t);
    Point tMinus(t);
    tPlus[j] += epsilon;
    tMinus[j] -= epsilon;
    const Scalar delta = tPlus[j] - tMinus[j];
    const Matrix gradPlus(model_.partialGradient(s, tPlus));
    const Matrix gradMinus(model_.partialGradient(s, tMinus));
    for (UnsignedInteger i = 0; i < inputDimension_; ++i)
    {
      result(i, j) = (gradPlus(i, 0) - gradMinus(i, 0)) / delta;
    }
  }
  return result;
}


SquareMatrix CurlFreeModel::operator()(const Point & tau) const
{
  if (tau.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "Error: the point tau has dimension=" << tau.getDimension() << ", expected dimension=" << inputDimension_;

  SquareMatrix result(outputDimension_);
  const Scalar epsilon = std::cbrt(SpecFunc::ScalarEpsilon);
  const Point zero(inputDimension_);

  for (UnsignedInteger j = 0; j < inputDimension_; ++j)
  {
    Point tauPlus(tau);
    Point tauMinus(tau);
    tauPlus[j] += epsilon;
    tauMinus[j] -= epsilon;
    const Scalar delta = tauPlus[j] - tauMinus[j];
    const Matrix gradPlus(model_.partialGradient(tauPlus, zero));
    const Matrix gradMinus(model_.partialGradient(tauMinus, zero));
    for (UnsignedInteger i = 0; i < inputDimension_; ++i)
    {
      result(i, j) = -(gradPlus(i, 0) - gradMinus(i, 0)) / delta;
    }
  }
  return result;
}


Bool CurlFreeModel::isStationary() const
{
  return model_.isStationary();
}


void CurlFreeModel::setScale(const Point & scale)
{
  model_.setScale(scale);
  scale_ = model_.getScale();
}


/* String converter */
String CurlFreeModel::__repr__() const
{
  OSS oss;
  oss << "class=" << CurlFreeModel::GetClassName()
      << " input dimension=" << inputDimension_
      << " model=" << model_;
  return oss;
}


/* String converter */
String CurlFreeModel::__str__(const String & ) const
{
  OSS oss;
  oss << "CurlFreeModel(" << model_.__str__() << ")";
  return oss;
}


CovarianceModel CurlFreeModel::getCovarianceModel() const
{
  return model_;
}

Scalar CurlFreeModel::computeAsScalar(const Point & s, const Point & t) const
{
  return operator()(s, t)(0, 0);
}

Scalar CurlFreeModel::computeAsScalar(const Point & tau) const
{
  return operator()(tau)(0, 0);
}

Scalar CurlFreeModel::computeAsScalar(const Scalar, const Scalar) const
{
  throw NotDefinedException(HERE) << "Error: the covariance model has input dimension=" << inputDimension_ << ", this method is only for input dimension=1.";
}

Scalar CurlFreeModel::computeAsScalar(const Scalar) const
{
  throw NotDefinedException(HERE) << "Error: the covariance model has input dimension=" << inputDimension_ << ", this method is only for input dimension=1.";
}

Scalar CurlFreeModel::computeAsScalar(const Collection<Scalar>::const_iterator & s_begin,
                                      const Collection<Scalar>::const_iterator & t_begin) const
{
  const Point s(s_begin, s_begin + inputDimension_);
  const Point t(t_begin, t_begin + inputDimension_);
  return computeAsScalar(s, t);
}


/* Method save() stores the object through the StorageManager */
void CurlFreeModel::save(Advocate & adv) const
{
  CovarianceModelImplementation::save(adv);
  adv.saveAttribute("model_", model_);
}


/* Method load() reloads the object from the StorageManager */
void CurlFreeModel::load(Advocate & adv)
{
  CovarianceModelImplementation::load(adv);
  adv.loadAttribute("model_", model_);
}


END_NAMESPACE_OPENTURNS
