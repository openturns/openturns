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
#include "openturns/MatrixImplementation.hxx"
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
  activeParameter_ = model_.getActiveParameter();
  amplitude_ = Point(outputDimension_);
  const SquareMatrix C0(operator()(Point(inputDimension_)));
  for (UnsignedInteger j = 0; j < outputDimension_; ++j)
    amplitude_[j] = std::sqrt(std::abs(C0(j, j)));
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
  isStationary_ = model.isStationary();
  activeParameter_ = model.getActiveParameter();
  amplitude_ = Point(outputDimension_);
  const SquareMatrix C0(operator()(Point(inputDimension_)));
  for (UnsignedInteger j = 0; j < outputDimension_; ++j)
    amplitude_[j] = std::sqrt(std::abs(C0(j, j)));
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

  if (isStationary())
  {
    // H~_ij(s, t) = d^2 k(s, t) / ds_i dt_j = -H_ij(s, t): exact, no finite difference
    const SymmetricMatrix hessian(model_.partialHessian(s, t));
    SquareMatrix exact(outputDimension_);
    for (UnsignedInteger j = 0; j < inputDimension_; ++j)
      for (UnsignedInteger i = 0; i < inputDimension_; ++i)
        exact(i, j) = -hessian(i, j);
    return exact;
  }

  SquareMatrix result(outputDimension_);
  const Scalar baseEps = std::cbrt(SpecFunc::ScalarEpsilon);
  const Point scale(model_.getScale());
  const Bool isotropicScale = (scale.getDimension() != inputDimension_);

  for (UnsignedInteger j = 0; j < inputDimension_; ++j)
  {
    const Scalar epsilon = (isotropicScale ? scale[0] : scale[j]) * baseEps;
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

  if (isStationary())
  {
    const Point zero(inputDimension_);
    const SymmetricMatrix hessian(model_.partialHessian(tau, zero));
    SquareMatrix exact(outputDimension_);
    for (UnsignedInteger j = 0; j < inputDimension_; ++j)
      for (UnsignedInteger i = 0; i < inputDimension_; ++i)
        exact(i, j) = -hessian(i, j);
    return exact;
  }

  SquareMatrix result(outputDimension_);
  const Scalar baseEps = std::cbrt(SpecFunc::ScalarEpsilon);
  const Point scale(model_.getScale());
  const Bool isotropicScale2 = (scale.getDimension() != inputDimension_);
  const Point zero(inputDimension_);

  for (UnsignedInteger j = 0; j < inputDimension_; ++j)
  {
    const Scalar epsilon = (isotropicScale2 ? scale[0] : scale[j]) * baseEps;
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

/* The curl couples the output components, so the discretized blocks are
   full and the diagonal fast paths of discretize()/computeCrossCovariance()
   must not apply. See the header for why the base flag is left as is. */
Bool CurlFreeModel::isDiagonal() const
{
  return false;
}


/* Gradient */
Matrix CurlFreeModel::partialGradient(const Point & s,
                                       const Point & t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;
  // The (i, j) covariance reads C_ij(s, t) = d^2 k(s, t) / ds_i dt_j, hence
  // dC_ij/ds_a = d/dt_j H_ai(s, t) where H is the partial Hessian of the
  // underlying scalar model. H is analytic, so a single central finite
  // difference in t yields a gradient exact in s, unlike the nested finite
  // differences of the base-class fallback.
  const Scalar baseEps = std::cbrt(SpecFunc::ScalarEpsilon);
  const Point scale(model_.getScale());
  const Bool isotropicScale3 = (scale.getDimension() != inputDimension_);
  Collection<SquareMatrix> derivatives(inputDimension_, SquareMatrix(outputDimension_));
  for (UnsignedInteger j = 0; j < inputDimension_; ++j)
  {
    const Scalar epsilon = (isotropicScale3 ? scale[0] : scale[j]) * baseEps;
    Point tPlus(t);
    Point tMinus(t);
    tPlus[j] += epsilon;
    tMinus[j] -= epsilon;
    const Scalar delta = tPlus[j] - tMinus[j];
    const SymmetricMatrix hessianPlus(model_.partialHessian(s, tPlus));
    const SymmetricMatrix hessianMinus(model_.partialHessian(s, tMinus));
    for (UnsignedInteger a = 0; a < inputDimension_; ++a)
      for (UnsignedInteger i = 0; i < inputDimension_; ++i)
        derivatives[a](i, j) = (hessianPlus(a, i) - hessianMinus(a, i)) / delta;
  }
  // Same layout as the base class: symmetrized covariance flattened column-wise
  Matrix gradient(inputDimension_, outputDimension_ * outputDimension_);
  for (UnsignedInteger a = 0; a < inputDimension_; ++a)
  {
    MatrixImplementation flat(*derivatives[a].getImplementation());
    flat.symmetrize();
    const Point values(flat);
    for (UnsignedInteger k = 0; k < values.getSize(); ++k)
      gradient(a, k) = values[k];
  }
  return gradient;
}

/* Hessian of the (0, 0) component: C_00 = -H_00, second-order stencil on H */
SymmetricMatrix CurlFreeModel::partialHessian(const Point & s,
    const Point & t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;
  if (!isStationary()) return CovarianceModelImplementation::partialHessian(s, t);
  // C_00(s, t) = -H_00(s, t) with H analytic and nugget-free, so a
  // value stencil on H is safe at s == t (never evaluates k at centre).
  // Second differences of values: optimal step is eps^(1/4).
  const Scalar baseEpsilon = std::sqrt(std::sqrt(SpecFunc::ScalarEpsilon));
  const Point scale(model_.getScale());
  const Bool isotropicScale = (scale.getDimension() != inputDimension_);
  SymmetricMatrix hessian(inputDimension_);
  const Scalar hessianST = model_.partialHessian(s, t)(0, 0);
  for (UnsignedInteger i = 0; i < inputDimension_; ++i)
  {
    const Scalar epsilon = (isotropicScale ? scale[0] : scale[i]) * baseEpsilon;
    Point sPlus(s);
    Point sMinus(s);
    sPlus[i] += epsilon;
    sMinus[i] -= epsilon;
    const Scalar deltaI = sPlus[i] - sMinus[i];
    const Scalar valuePlus = model_.partialHessian(sPlus, t)(0, 0);
    const Scalar valueMinus = model_.partialHessian(sMinus, t)(0, 0);
    hessian(i, i) = -(valuePlus - 2.0 * hessianST + valueMinus) / (deltaI * deltaI / 4.0);
    for (UnsignedInteger j = 0; j < i; ++j)
    {
      const Scalar epsilonJ = (isotropicScale ? scale[0] : scale[j]) * baseEpsilon;
      Point sPP(s);
      Point sPM(s);
      Point sMM(s);
      Point sMP(s);
      sPP[i] += epsilon;
      sPP[j] += epsilonJ;
      sPM[i] += epsilon;
      sPM[j] -= epsilonJ;
      sMM[i] -= epsilon;
      sMM[j] -= epsilonJ;
      sMP[i] -= epsilon;
      sMP[j] += epsilonJ;
      const Scalar valuePP = model_.partialHessian(sPP, t)(0, 0);
      const Scalar valuePM = model_.partialHessian(sPM, t)(0, 0);
      const Scalar valueMM = model_.partialHessian(sMM, t)(0, 0);
      const Scalar valueMP = model_.partialHessian(sMP, t)(0, 0);
      const Scalar deltaJ = sPP[j] - sPM[j];
      hessian(i, j) = -(valuePP - valuePM - valueMP + valueMM) / (deltaI * deltaJ);
    }
  }
  return hessian;
}

/* Parameter gradient: C_ij = -H_ij, so dC/dp = -dH/dp via base parameterHessian */
Matrix CurlFreeModel::parameterGradient(const Point & s,
    const Point & t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;
  if (!isStationary()) return CovarianceModelImplementation::parameterGradient(s, t);
  // Base parameterHessian rows are already filtered to base active parameters,
  // which coincide with ours (forwarded active set). Columns hold dH_ab/dp
  // with a + d*b layout, matching our output flattening i + d*j.
  const Matrix baseParameterHessian(model_.getImplementation()->parameterHessian(s, t));
  const UnsignedInteger activeSize = baseParameterHessian.getNbRows();
  Matrix gradient(activeSize, outputDimension_ * outputDimension_);
  for (UnsignedInteger k = 0; k < activeSize; ++k)
  {
    UnsignedInteger index = 0;
    for (UnsignedInteger j = 0; j < outputDimension_; ++j)
      for (UnsignedInteger i = 0; i < outputDimension_; ++i, ++index)
        gradient(k, index) = -baseParameterHessian(k, i + inputDimension_ * j);
  }
  return gradient;
}


void CurlFreeModel::setScale(const Point & scale)
{
  model_.setScale(scale);
  scale_ = scale;
  const SquareMatrix C0(operator()(Point(inputDimension_)));
  for (UnsignedInteger j = 0; j < outputDimension_; ++j)
    amplitude_[j] = std::sqrt(std::abs(C0(j, j)));
  updateOutputCovariance();
}


Point CurlFreeModel::getParameter() const
{
  return model_.getParameter();
}


void CurlFreeModel::setParameter(const Point & parameter)
{
  model_.setParameter(parameter);
  scale_ = model_.getScale();
  const SquareMatrix C0(operator()(Point(inputDimension_)));
  for (UnsignedInteger j = 0; j < outputDimension_; ++j)
    amplitude_[j] = std::sqrt(std::abs(C0(j, j)));
  updateOutputCovariance();
}


Point CurlFreeModel::getFullParameter() const
{
  return model_.getFullParameter();
}


void CurlFreeModel::setFullParameter(const Point & parameter)
{
  model_.setFullParameter(parameter);
  scale_ = model_.getScale();
  const SquareMatrix C0(operator()(Point(inputDimension_)));
  for (UnsignedInteger j = 0; j < outputDimension_; ++j)
    amplitude_[j] = std::sqrt(std::abs(C0(j, j)));
  updateOutputCovariance();
}


Description CurlFreeModel::getFullParameterDescription() const
{
  return model_.getFullParameterDescription();
}


Description CurlFreeModel::getParameterDescription() const
{
  return model_.getParameterDescription();
}


Indices CurlFreeModel::getActiveParameter() const
{
  return model_.getActiveParameter();
}


void CurlFreeModel::setActiveParameter(const Indices & active)
{
  model_.setActiveParameter(active);
}


/* Comparison operator */
Bool CurlFreeModel::operator ==(const CurlFreeModel & other) const
{
  if (this == &other) return true;
  return hasEqualBase(other) && (model_ == other.model_);
}


Bool CurlFreeModel::equals(const CovarianceModelImplementation & other) const
{
  const CurlFreeModel * p_other = dynamic_cast<const CurlFreeModel *>(&other);
  return p_other && (*this == *p_other);
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
