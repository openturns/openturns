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
#include "openturns/RankMCovarianceModel.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/ConstantBasisFactory.hxx"
#include "openturns/Lapack.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(RankMCovarianceModel)

static const Factory<RankMCovarianceModel> Factory_RankMCovarianceModel;


/* Default constructor */
RankMCovarianceModel::RankMCovarianceModel(const UnsignedInteger inputDimension)
  : CovarianceModelImplementation(inputDimension)
  , variance_(1, 1.0)
  , covariance_(0)
  , basis_()
  , functions_(0)
{
  // Use a constant basis for the default constructor
  setBasis(ConstantBasisFactory(1).build());
}

/* Parameters constructor */
RankMCovarianceModel::RankMCovarianceModel(const Point & variance,
    const Basis & basis)
  : CovarianceModelImplementation()
  , variance_(variance)
  , covariance_(0)
  , basis_()
  , functions_(0)
{
  if (!(variance.getDimension() > 0)) throw InvalidArgumentException(HERE) << "Error: expected a vector of marginal variances of positive dimension, got dimension=0.";
  setBasis(basis);
}

/* Parameters constructor */
RankMCovarianceModel::RankMCovarianceModel(const CovarianceMatrix & covariance,
    const Basis & basis)
  : CovarianceModelImplementation()
  , variance_(0)
  , covariance_(0)
  , basis_()
  , functions_(0)
{
  if (!(covariance.getDimension() > 0)) throw InvalidArgumentException(HERE) << "Error: expected a covariance of positive dimension, got dimension=0.";
  // Check if the covariance is diagonal
  if (covariance.isDiagonal())
  {
    variance_ = Point(covariance.getDimension());
    for (UnsignedInteger i = 0; i < variance_.getDimension(); ++i)
      variance_[i] = covariance(i, i);
  }
  else covariance_ = covariance;
  setBasis(basis);
}

/* Virtual constructor */
RankMCovarianceModel * RankMCovarianceModel::clone() const
{
  return new RankMCovarianceModel(*this);
}

/* Comparison operators */
Bool RankMCovarianceModel::operator ==(const RankMCovarianceModel & other) const
{
  if (this == &other) return true;
  return hasEqualBase(other) && (variance_ == other.variance_) && (covariance_ == other.covariance_);
}

Bool RankMCovarianceModel::equals(const CovarianceModelImplementation & other) const
{
  const RankMCovarianceModel * p_other = dynamic_cast<const RankMCovarianceModel *>(&other);
  return p_other && (*this == *p_other);
}

/* Computation of the covariance function
 * Suppose that X(\omega,t)=\sum_{i=1}^M\xi_i(\omega)\phi_i(t)
 * with E[\xi_i]=0. Then:
 * C(s,t)=E[X(\omega,s)X(\omega,t)^t]
 *       =\sum_{i=1}^M\sum_{j=1}^M\phi_i(s)\phi_j(t)^tE[\xi_i\xi_j]
 * and when the components of (\xi_1,\dots,\xi_M) are decorrelated with unit variance, it reduces to:
 * C(s,t)=\sum_{i=1}^M\alpha_i^2\phi_i(s)\phi_i(t)^t
 */
SquareMatrix RankMCovarianceModel::operator() (const Point & s,
    const Point & t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;

  MatrixImplementation result(outputDimension_, outputDimension_);
  const UnsignedInteger size = functions_.getSize();
  int dim = static_cast<int>(outputDimension_);
  Scalar plusOne = 1.0;
  int increment = 1;
  // If the variance are uncorrelated
  if (covariance_.getDimension() == 0)
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const Point phiS(functions_[i](s));
      const Point phiT(functions_[i](t) * variance_[i]);
      dger_(&dim, &dim, &plusOne,
            const_cast<double*>(&phiS[0]), &increment,
            const_cast<double*>(&phiT[0]), &increment,
            &result[0], &dim);
    }
  else
  {
    MatrixImplementation phiT(outputDimension_, size);
    Collection<Scalar>::iterator itPhiT = phiT.begin();
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const Point evalPhiT(functions_[i](t));
      std::copy(evalPhiT.begin(), evalPhiT.end(), itPhiT);
      itPhiT += outputDimension_;
    }
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const Point phiS(functions_[i](s));
      itPhiT = phiT.begin();
      for (UnsignedInteger j = 0; j < size; ++j)
      {
        const Point ptPhiT(Point(Collection<Scalar>(itPhiT, itPhiT + outputDimension_)) * covariance_(i, j));
        dger_(&dim, &dim, &plusOne,
              const_cast<double*>(&phiS[0]), &increment,
              const_cast<double*>(&ptPhiT[0]), &increment,
              &result[0], &dim);
        itPhiT += outputDimension_;
      }
    }
  } // covariance dimension > 0
  return result;
}

/* Gradient */
Matrix RankMCovarianceModel::partialGradient(const Point & s,
    const Point & t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;

  Matrix gradient(inputDimension_, outputDimension_ * outputDimension_);
  const UnsignedInteger size = functions_.getSize();
  // Uncorrelated case: C_ab(s, t) = sum_i variance_i phi_i(s)_a phi_i(t)_b
  if (covariance_.getDimension() == 0)
  {
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const Matrix phiGradient(functions_[i].gradient(s));
      const Point phiT(functions_[i](t) * variance_[i]);
      for (UnsignedInteger b = 0; b < outputDimension_; ++b)
        for (UnsignedInteger a = 0; a < outputDimension_; ++a)
          for (UnsignedInteger j = 0; j < inputDimension_; ++j)
            gradient(j, b * outputDimension_ + a) += phiGradient(j, a) * phiT[b];
    }
    return gradient;
  }
  // Correlated case: C_ab(s, t) = sum_ik phi_i(s)_a covariance_(i, k) phi_k(t)_b
  Matrix phiT(outputDimension_, size);
  for (UnsignedInteger k = 0; k < size; ++k)
  {
    const Point evalPhiT(functions_[k](t));
    for (UnsignedInteger b = 0; b < outputDimension_; ++b)
      phiT(b, k) = evalPhiT[b];
  }
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Matrix phiGradient(functions_[i].gradient(s));
    Point weightedPhiT(outputDimension_, 0.0);
    for (UnsignedInteger k = 0; k < size; ++k)
      for (UnsignedInteger b = 0; b < outputDimension_; ++b)
        weightedPhiT[b] += covariance_(i, k) * phiT(b, k);
    for (UnsignedInteger b = 0; b < outputDimension_; ++b)
      for (UnsignedInteger a = 0; a < outputDimension_; ++a)
        for (UnsignedInteger j = 0; j < inputDimension_; ++j)
          gradient(j, b * outputDimension_ + a) += phiGradient(j, a) * weightedPhiT[b];
  }
  return gradient;
}

/** Hessian wrt s, for the first output component */
SymmetricMatrix RankMCovarianceModel::partialHessian(const Point & s,
    const Point & t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;

  SymmetricMatrix hessian(inputDimension_);
  const UnsignedInteger size = functions_.getSize();
  // Uncorrelated case: C_00(s, t) = sum_i variance_i phi_i(s)_0 phi_i(t)_0
  if (covariance_.getDimension() == 0)
  {
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const SymmetricTensor phiHessian(functions_[i].hessian(s));
      const Scalar phiT0 = functions_[i](t)[0] * variance_[i];
      for (UnsignedInteger k = 0; k < inputDimension_; ++k)
        for (UnsignedInteger j = 0; j <= k; ++j)
          hessian(j, k) += phiHessian(j, k, 0) * phiT0;
    }
    return hessian;
  }
  // Correlated case: C_00(s, t) = sum_ik phi_i(s)_0 covariance_(i, k) phi_k(t)_0
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const SymmetricTensor phiHessian(functions_[i].hessian(s));
    Scalar weightedPhiT0 = 0.0;
    for (UnsignedInteger k = 0; k < size; ++k)
      weightedPhiT0 += covariance_(i, k) * functions_[k](t)[0];
    for (UnsignedInteger k = 0; k < inputDimension_; ++k)
      for (UnsignedInteger j = 0; j <= k; ++j)
        hessian(j, k) += phiHessian(j, k, 0) * weightedPhiT0;
  }
  return hessian;
}

CovarianceMatrix RankMCovarianceModel::discretize(const Sample & vertices) const
{
  if (vertices.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the given sample has a dimension=" << vertices.getDimension() << " different from the input dimension=" << inputDimension_;
  const UnsignedInteger size = vertices.getSize();
  const UnsignedInteger fullSize = size * outputDimension_;
  // Precompute the discretizations of the functions over the vertices
  const UnsignedInteger basisSize = functions_.getSize();
  MatrixImplementation basisDiscretization(fullSize, basisSize);
  MatrixImplementation::iterator start = basisDiscretization.begin();
  // If the covariance is diagonal
  if (covariance_.getDimension() == 0)
  {
    for (UnsignedInteger i = 0; i < basisSize; ++i)
    {
      const Point data(functions_[i](vertices).getImplementation()->getData() * std::sqrt(variance_[i]));
      std::copy(data.begin(), data.end(), start);
      start += fullSize;
    }
    // C = M.M^t
    return basisDiscretization.computeGram(false);
  }
  // Here covariance_ is left untouched by computeCholesky(), but the method in not const
  for (UnsignedInteger i = 0; i < basisSize; ++i)
  {
    const Point data(functions_[i](vertices).getImplementation()->getData());
    std::copy(data.begin(), data.end(), start);
    start += fullSize;
  }
  return (covariance_.getImplementation()->symProd(basisDiscretization, 'R')).genProd(basisDiscretization, false, true);
}

/* Covariance accessor */
CovarianceMatrix RankMCovarianceModel::getCovariance() const
{
  return covariance_;
}

Point RankMCovarianceModel::getVariance() const
{
  return variance_;
}

/* Basis accessor */
Basis RankMCovarianceModel::getBasis() const
{
  return basis_;
}

Basis::FunctionCollection RankMCovarianceModel::getFunctions() const
{
  return functions_;
}

/* Basis accessor */
void RankMCovarianceModel::setBasis(const Basis & basis)
{
  const UnsignedInteger size = std::max(variance_.getSize(), covariance_.getDimension());
  if (!(size > 0)) throw InvalidArgumentException(HERE) << "Error: there must be at least one coefficient";
  functions_ = Basis::FunctionCollection(size);
  for (UnsignedInteger i = 0; i < size; ++i)
    functions_[i] = basis.build(i);
  inputDimension_ = functions_[0].getInputDimension();
  scale_ = Point(inputDimension_, 1.0);
  outputDimension_ = functions_[0].getOutputDimension();
  amplitude_ = Point(outputDimension_, 1.0);
  basis_ = basis;
}

/* String converter */
String RankMCovarianceModel::__repr__() const
{
  OSS oss;
  oss << "class=" << RankMCovarianceModel::GetClassName()
      << ", variance=" << variance_
      << ", covariance=" << covariance_
      << ", basis=" << basis_
      << ", functions=" << functions_;
  return oss;
}

/* String converter */
String RankMCovarianceModel::__str__(const String & ) const
{
  OSS oss(false);
  oss << "class=" << RankMCovarianceModel::GetClassName();
  if (covariance_.getDimension() > 0) oss << ", covariance=" << covariance_;
  else oss << ", variance=" << variance_;
  oss << ", basis=" << basis_
      << ", functions=" << functions_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void RankMCovarianceModel::save(Advocate & adv) const
{
  CovarianceModelImplementation::save(adv);
  adv.saveAttribute("variance_", variance_);
  adv.saveAttribute("covariance_", covariance_);
  adv.saveAttribute("basis_", basis_);
  adv.saveAttribute("functions_", functions_);
}

/* Method load() reloads the object from the StorageManager */
void RankMCovarianceModel::load(Advocate & adv)
{
  CovarianceModelImplementation::load(adv);
  adv.loadAttribute("variance_", variance_);
  adv.loadAttribute("covariance_", covariance_);
  adv.loadAttribute("basis_", basis_);
  adv.loadAttribute("functions_", functions_);
}

END_NAMESPACE_OPENTURNS
