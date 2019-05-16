//                                               -*- C++ -*-
/**
 *  @brief Discrete compound distribution
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/DiscreteCompoundDistribution.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/FFT.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Poisson.hxx"
#include "openturns/Bernoulli.hxx"
#include "openturns/DistFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DiscreteCompoundDistribution)

static const Factory<DiscreteCompoundDistribution> Factory_DiscreteCompoundDistribution;

/* Default constructor */
DiscreteCompoundDistribution::DiscreteCompoundDistribution()
  : DiscreteDistribution()
{
  setName("DiscreteCompoundDistribution");
  setDimension(1);
  setBaseAndCompoundDistributions(Bernoulli(0.5), Poisson(10.0));
}

/** Constructor using distributions */
DiscreteCompoundDistribution::DiscreteCompoundDistribution(const Distribution & baseDistribution,
                                                           const Distribution & compoundDistribution)
  : DiscreteDistribution()
{
  setName("DiscreteCompoundDistribution");
  setDimension(1);
  setBaseAndCompoundDistributions(baseDistribution, compoundDistribution);
}

/* Constructs PDF */

/* Comparison operator */
Bool DiscreteCompoundDistribution::operator ==(const DiscreteCompoundDistribution & other) const
{
  if (this == &other) return true;
  return baseDistribution_ == other.baseDistribution_ && compoundDistribution_ == other.compoundDistribution_;
}

Bool DiscreteCompoundDistribution::equals(const DistributionImplementation & other) const
{
  const DiscreteCompoundDistribution* p_other = dynamic_cast<const DiscreteCompoundDistribution*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String DiscreteCompoundDistribution::__repr__() const
{
  OSS oss(true);
  oss << "class=" << DiscreteCompoundDistribution::GetClassName()
      << " name=" << getName()
      << " baseDistribution=" << baseDistribution_
      << " compoundDistribution=" << compoundDistribution_;
  return oss;
}

String DiscreteCompoundDistribution::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(base=" << baseDistribution_ << ", compound=" << compoundDistribution_ << ")";
  return oss;
}

/* Virtual constructor */
DiscreteCompoundDistribution * DiscreteCompoundDistribution::clone() const
{
  return new DiscreteCompoundDistribution(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void DiscreteCompoundDistribution::computeRange()
{
  const Interval::BoolCollection finiteLowerBound(1, true);
  const Interval::BoolCollection finiteUpperBound(1, baseDistribution_.getRange().getFiniteUpperBound()[0] && compoundDistribution_.getRange().getFiniteUpperBound()[0]);
  setRange(Interval(distribution_.getRange().getLowerBound(), distribution_.getRange().getUpperBound(), finiteLowerBound, finiteUpperBound));
}

/* Get one realization of the distribution */
Point DiscreteCompoundDistribution::getRealization() const
{
  const UnsignedInteger index = base_.getSize() ? DistFunc::rDiscrete(base_, alias_) : DistFunc::rDiscrete(probabilities_, base_, alias_);
  return Point(1, index);
}


/* All values of the PDF */

Point DiscreteCompoundDistribution::computeProbabilities(const UnsignedInteger m) const
{
  const Scalar epsilon = ResourceMap::GetAsScalar("DiscreteCompoundDistribution-DefaultEpsilon");
  if (!(epsilon > 0.0)) throw InvalidArgumentException(HERE) << "Error parameter must be positive.";

  const Scalar r = std::pow(epsilon, 1.0 / m);
  FFT fft;
  FFT::ComplexCollection collection(m);
  for (UnsignedInteger i = 0; i < m; ++ i)
  {
    collection[i] = computeGeneratingFunction(r * std::exp(2.0 * Complex(0.0, 1.0) * (M_PI * i / m)));
  }
  collection = fft.transform(collection);

  probabilities_ = Point(m);
  for (UnsignedInteger i = 0; i < m; ++ i)
  {
    probabilities_[i] = std::min(1.0, std::max(0.0, collection[i].real() / m / std::pow(r, (Scalar)i)));
  }
  return probabilities_;
}


UnsignedInteger DiscreteCompoundDistribution::computeIntegerUpperBound()
{
  return compoundDistribution_.getRange().getUpperBound()[0] * baseDistribution_.getRange().getUpperBound()[0];
}

void DiscreteCompoundDistribution::setBaseAndCompoundDistributions(const Distribution & baseDistribution,
                                                                   const Distribution & compoundDistribution)
{
  if (!baseDistribution.isIntegral()) throw InvalidArgumentException(HERE) << "Error: the base distribution must be integer-valued.";
  if (!compoundDistribution.isIntegral()) throw InvalidArgumentException(HERE) << "Error: the compound distribution must be integer-valued.";

  baseDistribution_ = baseDistribution;
  compoundDistribution_ = compoundDistribution;

  const UnsignedInteger m = computeIntegerUpperBound();

  const Point weights(computeProbabilities(m));
  Sample points(m, 1);
  for(UnsignedInteger i = 0; i < m; ++ i)
  {
    points(i, 0) = i;
  }
  distribution_ = UserDefined(points, weights);
  computeRange();
}

/* Get the PDF of the distribution */
Scalar DiscreteCompoundDistribution::computePDF(const Point & point) const
{
  const Scalar x = point[0];
  const UnsignedInteger ix = static_cast<UnsignedInteger>(x);
  if (ix >= probabilities_.getSize()) return 0.0;
  if (std::abs(ix - x) > pdfEpsilon_) return 0.0;
  return probabilities_[ix];
}


/* Get the CDF of the distribution */
Scalar DiscreteCompoundDistribution::computeCDF(const Point & point) const
{
  return distribution_.computeCDF(point);
}

Scalar DiscreteCompoundDistribution::computeComplementaryCDF(const Point & point) const
{
  return distribution_.computeComplementaryCDF(point);
}

/* Get the PDFGradient of the distribution */
Point DiscreteCompoundDistribution::computePDFGradient(const Point & point) const
{
  return distribution_.computePDFGradient(point);
}

/* Get the CDFGradient of the distribution */
Point DiscreteCompoundDistribution::computeCDFGradient(const Point & point) const
{
  return distribution_.computeCDFGradient(point);
}

/* Get the quantile of the distribution */
Scalar DiscreteCompoundDistribution::computeScalarQuantile(const Scalar prob,
                                        const Bool tail) const
{
  return distribution_.computeQuantile(prob, tail)[0];
}

/* Compute the entropy of the distribution */
Scalar DiscreteCompoundDistribution::computeEntropy() const
{
  return distribution_.computeEntropy();
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
Complex DiscreteCompoundDistribution::computeCharacteristicFunction(const Scalar x) const
{
  return distribution_.computeCharacteristicFunction(x);
}

/* Get the generating function of the distribution, i.e. psi(z) = E(z^X) */
Complex DiscreteCompoundDistribution::computeGeneratingFunction(const Complex & z) const
{
  return compoundDistribution_.computeGeneratingFunction(baseDistribution_.computeGeneratingFunction(z));
}

/* Get the support of a discrete distribution that intersect a given interval */
Sample DiscreteCompoundDistribution::getSupport(const Interval & interval) const
{
  return distribution_.getSupport(interval);
}

/* Get the standard deviation of the distribution */
Point DiscreteCompoundDistribution::getStandardDeviation() const
{
  return distribution_.getStandardDeviation();
}

/* Get the skewness of the distribution */
Point DiscreteCompoundDistribution::getSkewness() const
{
  return distribution_.getSkewness();
}

/* Get the kurtosis of the distribution */
Point DiscreteCompoundDistribution::getKurtosis() const
{
  return distribution_.getKurtosis();
}

/* Parameters value accessor */
Point DiscreteCompoundDistribution::getParameter() const
{
  return distribution_.getParameter();
}

void DiscreteCompoundDistribution::setParameter(const Point & parameter)
{
  return distribution_.setParameter(parameter);
}

/* Parameters description accessor */
Description DiscreteCompoundDistribution::getParameterDescription() const
{
  return distribution_.getParameterDescription();
}

/* Method save() stores the object through the StorageManager */
void DiscreteCompoundDistribution::save(Advocate & adv) const
{
  DiscreteDistribution::save(adv);
  adv.saveAttribute("baseDistribution_", baseDistribution_);
  adv.saveAttribute("compoundDistribution_", compoundDistribution_);
  adv.saveAttribute("distribution_", distribution_);
}

/* Method load() reloads the object from the StorageManager */
void DiscreteCompoundDistribution::load(Advocate & adv)
{
  DiscreteDistribution::load(adv);
  adv.loadAttribute("baseDistribution_", baseDistribution_);
  adv.loadAttribute("compoundDistribution_", compoundDistribution_);
  adv.loadAttribute("distribution_", distribution_);
}

/* Base distribution accessor */
Distribution DiscreteCompoundDistribution::getBaseDistribution() const
{
  return baseDistribution_;
}

/* Compound distribution accessor */
Distribution DiscreteCompoundDistribution::getCompoundDistribution() const
{
  return compoundDistribution_;
}

END_NAMESPACE_OPENTURNS
