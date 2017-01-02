//                                               -*- C++ -*-
/**
 *  @brief The NormalGamma distribution
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <cmath>
#include "openturns/NormalGamma.hxx"
#include "openturns/Gamma.hxx"
#include "openturns/Normal.hxx"
#include "openturns/AnalyticalFunction.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(NormalGamma);

static const Factory<NormalGamma> Factory_NormalGamma;


/* Default constructor */
NormalGamma::NormalGamma()
  : BayesDistribution()
  , mu_(0.0)
  , kappa_(1.0)
  , alpha_(1.0)
  , beta_(1.0)
{
  setName("NormalGamma");
  const Description inVars(1, "lambda");
  Description formulas(2);
  OSS oss;
  oss.setPrecision(17);
  formulas[0] = String(oss << mu_);
  oss.clear();
  formulas[1] = String(oss << "1.0 / (" << kappa_ << "*lambda)");
  const AnalyticalFunction link(inVars, formulas);
  setConditionedAndConditioningDistributionsAndLinkFunction(Normal(), Gamma(alpha_, beta_), link);
}

/* Parameters constructor */
NormalGamma::NormalGamma(const NumericalScalar mu,
                         const NumericalScalar kappa,
                         const NumericalScalar alpha,
                         const NumericalScalar beta)
  : BayesDistribution()
  , mu_(mu)
  , kappa_(kappa)
  , alpha_(alpha)
  , beta_(beta)
{
  if (kappa_ <= 0.0) throw InvalidArgumentException(HERE) << "Error: kappa must be positive, here kappa=" << kappa;
  if (alpha_ <= 0.0) throw InvalidArgumentException(HERE) << "Error: alpha must be positive, here alpha=" << alpha;
  if (beta_ <= 0.0) throw InvalidArgumentException(HERE) << "Error: beta must be positive, here beta=" << beta;
  setName("NormalGamma");
  const Description inVars(1, "lambda");
  Description formulas(2);
  OSS oss;
  oss.setPrecision(17);
  formulas[0] = String(oss << mu_);
  oss.clear();
  formulas[1] = String(oss << "1.0 / sqrt(" << kappa_ << "*lambda)");
  const AnalyticalFunction link(inVars, formulas);
  setConditionedAndConditioningDistributionsAndLinkFunction(Normal(), Gamma(alpha_, beta_), link);
  computeRange();
}

/* Compute the numerical range of the distribution given the parameters values */
void NormalGamma::computeRange()
{
  const NumericalScalar epsilon = ResourceMap::GetAsNumericalScalar("Distribution-DefaultQuantileEpsilon");
  // Lower bound
  NumericalPoint lowerBound(2, 0.0);
  //const NumericalScalar lambdaMin(conditioningDistribution_.computeQuantile(epsilon)[0]);
  const NumericalScalar lambdaMax = conditioningDistribution_.computeQuantile(epsilon, true)[0];
  const Normal deconditionedDistribution(mu_, 1.0 / std::sqrt(kappa_ * lambdaMax));
  lowerBound[0] = deconditionedDistribution.computeQuantile(epsilon)[0];
  // Upper bound
  NumericalPoint upperBound(2, lambdaMax);
  upperBound[0] = deconditionedDistribution.computeQuantile(epsilon, true)[0];
  Interval::BoolCollection finiteLowerBound(2, false);
  finiteLowerBound[1] = true;
  const Interval::BoolCollection finiteUpperBound(2, false);
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}

/* Comparison operator */
Bool NormalGamma::operator ==(const NormalGamma & other) const
{
  if (this == &other) return true;
  return (mu_ == other.mu_) && (kappa_ == other.kappa_) && (alpha_ == other.alpha_) && (beta_ == other.beta_);
}

Bool NormalGamma::equals(const DistributionImplementation & other) const
{
  const NormalGamma* p_other = dynamic_cast<const NormalGamma*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String NormalGamma::__repr__() const
{
  OSS oss(true);
  oss << "class=" << NormalGamma::GetClassName()
      << " name=" << getName()
      << " mu=" << mu_
      << " kappa=" << kappa_
      << " alpha=" << alpha_
      << " beta=" << beta_;
  return oss;
}

String NormalGamma::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(mu=" << mu_ << ", kappa=" << kappa_ << ", alpha=" << alpha_ << ", beta=" << beta_ << ")";
  return oss;
}

/* Virtual constructor */
NormalGamma * NormalGamma::clone() const
{
  return new NormalGamma(*this);
}

/* Mu accessor */
void NormalGamma::setMu(const NumericalScalar mu)
{
  if (mu != mu_) mu_ = mu;
}

NumericalScalar NormalGamma::getMu() const
{
  return mu_;
}


/* Kappa accessor */
void NormalGamma::setKappa(const NumericalScalar kappa)
{
  if (kappa != kappa_)
  {
    if (kappa_ <= 0.0) throw InvalidArgumentException(HERE) << "Error: kappa must be positive, here kappa=" << kappa;
    kappa_ = kappa;
  }
}

NumericalScalar NormalGamma::getKappa() const
{
  return kappa_;
}

/* Alpha accessor */
void NormalGamma::setAlpha(const NumericalScalar alpha)
{
  if (!(alpha == alpha_))
  {
    if (alpha_ <= 0.0) throw InvalidArgumentException(HERE) << "Error: alpha must be positive, here alpha=" << alpha;
    alpha_ = alpha;
  }
}

NumericalScalar NormalGamma::getAlpha() const
{
  return alpha_;
}

/* Beta accessor */
void NormalGamma::setBeta(const NumericalScalar beta)
{
  if (!(beta == beta_))
  {
    if (beta_ <= 0.0) throw InvalidArgumentException(HERE) << "Error: beta must be positive, here beta=" << beta;
    beta_ = beta;
  }
}

NumericalScalar NormalGamma::getBeta() const
{
  return beta_;
}

/* Method save() stores the object through the StorageManager */
void NormalGamma::save(Advocate & adv) const
{
  BayesDistribution::save(adv);
  adv.saveAttribute( "mu_", mu_ );
  adv.saveAttribute( "kappa_", kappa_ );
  adv.saveAttribute( "alpha_", alpha_ );
  adv.saveAttribute( "beta_", beta_ );
}

/* Method load() reloads the object from the StorageManager */
void NormalGamma::load(Advocate & adv)
{
  BayesDistribution::load(adv);
  adv.loadAttribute( "mu_", mu_ );
  adv.loadAttribute( "kappa_", kappa_ );
  adv.loadAttribute( "alpha_", alpha_ );
  adv.loadAttribute( "beta_", beta_ );
  // The range is computed using the upper class
}


END_NAMESPACE_OPENTURNS
