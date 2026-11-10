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
#include "openturns/MatrixImplementation.hxx"
#include "openturns/Log.hxx"
#include "openturns/SquaredExponential.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DivFreeModel)

static const Factory<DivFreeModel> Factory_DivFreeModel;

/** Helper: compute the Hessian of the underlying scalar model */
static SquareMatrix computeHessian(const CovarianceModel & model,
                                    const Point & s,
                                    const Point & t,
                                    const UnsignedInteger inputDimension)
{
  SquareMatrix hessian(inputDimension);
  const Scalar baseEps = std::cbrt(SpecFunc::ScalarEpsilon);
  const Point scale(model.getScale());
  const Bool isotropicScale = (scale.getDimension() != inputDimension);
  for (UnsignedInteger j = 0; j < inputDimension; ++j)
  {
    const Scalar epsilon = (isotropicScale ? scale[0] : scale[j]) * baseEps;
    Point tPlus(t);
    Point tMinus(t);
    tPlus[j] += epsilon;
    tMinus[j] -= epsilon;
    const Scalar delta = tPlus[j] - tMinus[j];
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
  const Bool isotropicScale = (scale.getDimension() != inputDimension);
  const Point zero(inputDimension);
  for (UnsignedInteger j = 0; j < inputDimension; ++j)
  {
    const Scalar epsilon = (isotropicScale ? scale[0] : scale[j]) * baseEps;
    Point tauPlus(tau);
    Point tauMinus(tau);
    tauPlus[j] += epsilon;
    tauMinus[j] -= epsilon;
    const Scalar delta = tauPlus[j] - tauMinus[j];
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

  if (isStationary())
  {
    // H~_ij(s, t) = -H_ij(s, t), hence C_div = H - trace(H) * I_d: exact, no finite difference
    const SymmetricMatrix hessian(model_.partialHessian(s, t));
    Scalar trace = 0.0;
    for (UnsignedInteger k = 0; k < inputDimension_; ++k) trace += hessian(k, k);
    SquareMatrix exact(outputDimension_);
    for (UnsignedInteger j = 0; j < inputDimension_; ++j)
      for (UnsignedInteger i = 0; i < inputDimension_; ++i)
        exact(i, j) = hessian(i, j) - (i == j ? trace : 0.0);
    return exact;
  }

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

  if (isStationary())
  {
    const Point zero(inputDimension_);
    const SymmetricMatrix hessian(model_.partialHessian(tau, zero));
    Scalar trace = 0.0;
    for (UnsignedInteger k = 0; k < inputDimension_; ++k) trace += hessian(k, k);
    SquareMatrix exact(outputDimension_);
    for (UnsignedInteger j = 0; j < inputDimension_; ++j)
      for (UnsignedInteger i = 0; i < inputDimension_; ++i)
        exact(i, j) = hessian(i, j) - (i == j ? trace : 0.0);
    return exact;
  }

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

/* The divergence couples the output components, so the discretized blocks are
   full and the diagonal fast paths of discretize()/computeCrossCovariance()
   must not apply. See the header for why the base flag is left as is. */
Bool DivFreeModel::isDiagonal() const
{
  return false;
}


/* Gradient */
Matrix DivFreeModel::partialGradient(const Point & s,
                                      const Point & t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;
  // The (i, j) covariance reads C_ij(s, t) = delta_ij tr(H~(s, t)) - H~_ij(s, t)
  // with H~_ij(s, t) = d^2 k(s, t) / ds_i dt_j, hence dC_ij/ds_a =
  // delta_ij sum_k G_akk - G_aij where G_aij = d/dt_j H_ai(s, t) and H is the
  // partial Hessian of the underlying scalar model. H is analytic, so a single
  // central finite difference in t yields a gradient exact in s, unlike the
  // nested finite differences of the base-class fallback.
  const Scalar baseEps = std::cbrt(SpecFunc::ScalarEpsilon);
  const Point scale(model_.getScale());
  const Bool isotropicScaleGrad = (scale.getDimension() != inputDimension_);
  Collection<SquareMatrix> thirdOrder(inputDimension_, SquareMatrix(outputDimension_));
  for (UnsignedInteger j = 0; j < inputDimension_; ++j)
  {
    const Scalar epsilon = (isotropicScaleGrad ? scale[0] : scale[j]) * baseEps;
    Point tPlus(t);
    Point tMinus(t);
    tPlus[j] += epsilon;
    tMinus[j] -= epsilon;
    const Scalar delta = tPlus[j] - tMinus[j];
    const SymmetricMatrix hessianPlus(model_.partialHessian(s, tPlus));
    const SymmetricMatrix hessianMinus(model_.partialHessian(s, tMinus));
    for (UnsignedInteger a = 0; a < inputDimension_; ++a)
      for (UnsignedInteger i = 0; i < inputDimension_; ++i)
        thirdOrder[a](i, j) = (hessianPlus(a, i) - hessianMinus(a, i)) / delta;
  }
  Collection<SquareMatrix> derivatives(inputDimension_, SquareMatrix(outputDimension_));
  for (UnsignedInteger a = 0; a < inputDimension_; ++a)
  {
    Scalar trace = 0.0;
    for (UnsignedInteger k = 0; k < inputDimension_; ++k)
      trace += thirdOrder[a](k, k);
    for (UnsignedInteger j = 0; j < inputDimension_; ++j)
      for (UnsignedInteger i = 0; i < inputDimension_; ++i)
        derivatives[a](i, j) = (i == j ? trace : 0.0) - thirdOrder[a](i, j);
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

/* Hessian of the (0, 0) component: C_00 = -sum_{k>0} H_kk, stencil on H */
SymmetricMatrix DivFreeModel::partialHessian(const Point & s,
    const Point & t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;
  if (!isStationary()) return CovarianceModelImplementation::partialHessian(s, t);
  // Second differences of values: optimal step is eps^(1/4).
  const Scalar baseEpsilon = std::sqrt(std::sqrt(SpecFunc::ScalarEpsilon));
  const Point scale(model_.getScale());
  const Bool isotropicScale = (scale.getDimension() != inputDimension_);
  SymmetricMatrix hessian(inputDimension_);
  const SymmetricMatrix hessianST(model_.partialHessian(s, t));
  Scalar centerValue = 0.0;
  for (UnsignedInteger k = 1; k < inputDimension_; ++k) centerValue += hessianST(k, k);
  for (UnsignedInteger i = 0; i < inputDimension_; ++i)
  {
    const Scalar epsilon = (isotropicScale ? scale[0] : scale[i]) * baseEpsilon;
    Point sPlus(s);
    Point sMinus(s);
    sPlus[i] += epsilon;
    sMinus[i] -= epsilon;
    const Scalar deltaI = sPlus[i] - sMinus[i];
    const SymmetricMatrix hessianPlus(model_.partialHessian(sPlus, t));
    const SymmetricMatrix hessianMinus(model_.partialHessian(sMinus, t));
    Scalar valuePlus = 0.0;
    Scalar valueMinus = 0.0;
    for (UnsignedInteger k = 1; k < inputDimension_; ++k)
    {
      valuePlus += hessianPlus(k, k);
      valueMinus += hessianMinus(k, k);
    }
    hessian(i, i) = -(valuePlus - 2.0 * centerValue + valueMinus) / (deltaI * deltaI / 4.0);
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
      Scalar valuePP = 0.0;
      Scalar valuePM = 0.0;
      Scalar valueMM = 0.0;
      Scalar valueMP = 0.0;
      const SymmetricMatrix hessianPP(model_.partialHessian(sPP, t));
      const SymmetricMatrix hessianPM(model_.partialHessian(sPM, t));
      const SymmetricMatrix hessianMM(model_.partialHessian(sMM, t));
      const SymmetricMatrix hessianMP(model_.partialHessian(sMP, t));
      for (UnsignedInteger k = 1; k < inputDimension_; ++k)
      {
        valuePP += hessianPP(k, k);
        valuePM += hessianPM(k, k);
        valueMM += hessianMM(k, k);
        valueMP += hessianMP(k, k);
      }
      const Scalar deltaJ = sPP[j] - sPM[j];
      hessian(i, j) = -(valuePP - valuePM - valueMP + valueMM) / (deltaI * deltaJ);
    }
  }
  return hessian;
}

/* Parameter gradient: C = H - tr(H) I, so dC/dp = dH/dp - tr(dH/dp) I */
Matrix DivFreeModel::parameterGradient(const Point & s,
    const Point & t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;
  if (!isStationary()) return CovarianceModelImplementation::parameterGradient(s, t);
  const Matrix baseParameterHessian(model_.getImplementation()->parameterHessian(s, t));
  const UnsignedInteger activeSize = baseParameterHessian.getNbRows();
  Matrix gradient(activeSize, outputDimension_ * outputDimension_);
  for (UnsignedInteger k = 0; k < activeSize; ++k)
  {
    Scalar trace = 0.0;
    for (UnsignedInteger q = 0; q < inputDimension_; ++q)
      trace += baseParameterHessian(k, q + inputDimension_ * q);
    UnsignedInteger index = 0;
    for (UnsignedInteger j = 0; j < outputDimension_; ++j)
      for (UnsignedInteger i = 0; i < outputDimension_; ++i, ++index)
        gradient(k, index) = baseParameterHessian(k, i + inputDimension_ * j) - (i == j ? trace : 0.0);
  }
  return gradient;
}


void DivFreeModel::setScale(const Point & scale)
{
  model_.setScale(scale);
  scale_ = scale;
  const SquareMatrix C0(operator()(Point(inputDimension_)));
  for (UnsignedInteger j = 0; j < outputDimension_; ++j)
    amplitude_[j] = std::sqrt(std::abs(C0(j, j)));
  updateOutputCovariance();
}


Point DivFreeModel::getParameter() const
{
  return model_.getParameter();
}


void DivFreeModel::setParameter(const Point & parameter)
{
  model_.setParameter(parameter);
  scale_ = model_.getScale();
  const SquareMatrix C0(operator()(Point(inputDimension_)));
  for (UnsignedInteger j = 0; j < outputDimension_; ++j)
    amplitude_[j] = std::sqrt(std::abs(C0(j, j)));
  updateOutputCovariance();
}


Point DivFreeModel::getFullParameter() const
{
  return model_.getFullParameter();
}


void DivFreeModel::setFullParameter(const Point & parameter)
{
  model_.setFullParameter(parameter);
  scale_ = model_.getScale();
  const SquareMatrix C0(operator()(Point(inputDimension_)));
  for (UnsignedInteger j = 0; j < outputDimension_; ++j)
    amplitude_[j] = std::sqrt(std::abs(C0(j, j)));
  updateOutputCovariance();
}


Description DivFreeModel::getFullParameterDescription() const
{
  return model_.getFullParameterDescription();
}


Description DivFreeModel::getParameterDescription() const
{
  return model_.getParameterDescription();
}


Indices DivFreeModel::getActiveParameter() const
{
  return model_.getActiveParameter();
}


void DivFreeModel::setActiveParameter(const Indices & active)
{
  model_.setActiveParameter(active);
}


/* Comparison operator */
Bool DivFreeModel::operator ==(const DivFreeModel & other) const
{
  if (this == &other) return true;
  return hasEqualBase(other) && (model_ == other.model_);
}


Bool DivFreeModel::equals(const CovarianceModelImplementation & other) const
{
  const DivFreeModel * p_other = dynamic_cast<const DivFreeModel *>(&other);
  return p_other && (*this == *p_other);
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
