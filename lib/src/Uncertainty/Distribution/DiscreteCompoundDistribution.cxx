//                                               -*- C++ -*-
/**
 *  @brief The DiscreteCompoundDistribution
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
#include <cmath>
#include "openturns/DiscreteCompoundDistribution.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DiscreteCompoundDistribution)

static const Factory<DiscreteCompoundDistribution> Factory_DiscreteCompoundDistribution;

/* Default constructor */
DiscreteCompoundDistribution::DiscreteCompoundDistribution()
  : DiscreteDistribution()
{
  setName( "DiscreteCompoundDistribution" );

  setDimension( 1 );

  baseDistribution_ = Bernoulli(0.5);
  compoundDistribution_ = Poisson(10.0);

 
 // Distribution of the sum

  createIntegralCompound();
}

  /** Constructor using distributions */
DiscreteCompoundDistribution::DiscreteCompoundDistribution(const Distribution & base_distribution, const Distribution & compound_distribution)
  : DiscreteDistribution()
{
  setName( "DiscreteCompoundDistribution" );

  setDimension( 1 );

  if (!base_distribution.isIntegral()) throw InvalidArgumentException(HERE) << "Error: the base distribution must be integer-valued.";
  if (!compound_distribution.isIntegral()) throw InvalidArgumentException(HERE) << "Error: the compound distribution must be integer-valued.";

  baseDistribution_ = base_distribution;
  compoundDistribution_ = compound_distribution;

 
 // Distribution of the sum

  createIntegralCompound();
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
      << " name=" << getName();
      //<< " dimension=" << getDimension()
      //<< " p=" << p_;
  return oss;
}

String DiscreteCompoundDistribution::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "Base distribution " << baseDistribution_ << " Compound distribution" << compoundDistribution_;
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
  const Point lowerBound(1, 0.0);
  const Point upperBound(1, computeIntegerUpperBound());
  const Interval::BoolCollection finiteLowerBound(1, true);
  const Interval::BoolCollection finiteUpperBound(1, false);
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}

/* Get one realization of the distribution */
Point DiscreteCompoundDistribution::getRealization() const
{
  return distribution_.getRealization();
}


/* All values of the PDF */

Point DiscreteCompoundDistribution::computeProbabilities(const UnsignedInteger m, const Scalar epsilon) const // FIXME
{

  if (epsilon <= 0) throw InvalidArgumentException(HERE) << "Error parameter must be positive.";

  Scalar r = std::pow(epsilon, 1.0/m);


  FFT fft;
  FFT::ComplexCollection collection = FFT::ComplexCollection (m);
  for(UnsignedInteger i=0;i<m;i++)
  {
    collection[i] = computeGeneratingFunction(r * std::exp(2.0 * Complex(0.0,1.0) * (M_PI * i / m)));
  }

  collection = fft.transform(collection);

  Point probabilities(m);

  for(UnsignedInteger i=0;i<m;i++)
  {
    probabilities[i] = std::max(0.0, collection[i].real() / m / std::pow(r, (Scalar)i));
  }
  return probabilities;
}


UnsignedInteger DiscreteCompoundDistribution::computeIntegerUpperBound()
{
  return compoundDistribution_.getRange().getUpperBound()[0] * baseDistribution_.getRange().getUpperBound()[0];
}

void DiscreteCompoundDistribution::createIntegralCompound()
{
  UnsignedInteger m = computeIntegerUpperBound();

  Point weights = computeProbabilities(m);
  Sample points = Sample(m, 1);

  for(UnsignedInteger i=0;i<m;i++)
  {
	points(i, 0) = i;
  }
  
   distribution_ = UserDefined(points, weights);
   //std::cerr << "distribution_=" << distribution_ << std::endl;
}

/* Get the PDF of the distribution */
Scalar DiscreteCompoundDistribution::computePDF(const Point & point) const
{
  return distribution_.computePDF(point);
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
  //distribution_.save(adv); // FIXME
}

/* Method load() reloads the object from the StorageManager */
void DiscreteCompoundDistribution::load(Advocate & adv)
{
  DiscreteDistribution::load(adv);
  //distribution_.load(adv); // FIXME
}



END_NAMESPACE_OPENTURNS
