//                                               -*- C++ -*-
/**
 *  @brief A class that implements an independent copula
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
#include "openturns/IndependentCopula.hxx"
#include "openturns/Normal.hxx"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/NatafIndependentCopulaEvaluation.hxx"
#include "openturns/NatafIndependentCopulaGradient.hxx"
#include "openturns/NatafIndependentCopulaHessian.hxx"
#include "openturns/InverseNatafIndependentCopulaEvaluation.hxx"
#include "openturns/InverseNatafIndependentCopulaGradient.hxx"
#include "openturns/InverseNatafIndependentCopulaHessian.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/AnalyticalFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(IndependentCopula);

static const Factory<IndependentCopula> Factory_IndependentCopula;

/* Default constructor */
IndependentCopula::IndependentCopula(const UnsignedInteger dimension)
  : CopulaImplementation()
{
  setName( "IndependentCopula" );
  setDimension(dimension);
  computeRange();
}

/* Comparison operator */
Bool IndependentCopula::operator ==(const IndependentCopula & other) const
{
  if (this == &other) return true;
  return getDimension() == other.getDimension();
}

Bool IndependentCopula::equals(const DistributionImplementation & other) const
{
  const IndependentCopula* p_other = dynamic_cast<const IndependentCopula*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String IndependentCopula::__repr__() const
{
  OSS oss(true);
  oss << "class=" << IndependentCopula::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension();
  return oss;
}

/* String converter */
String IndependentCopula::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(dimension = " << getDimension() << ")";
  return oss;
}

/* Virtual constructor */
IndependentCopula * IndependentCopula::clone() const
{
  return new IndependentCopula(*this);
}

/* Get one realization of the distribution */
NumericalPoint IndependentCopula::getRealization() const
{
  return RandomGenerator::Generate(getDimension());
}

/* Get the DDF of the distribution */
NumericalPoint IndependentCopula::computeDDF(const NumericalPoint & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  return NumericalPoint(dimension, 0.0);
}

/* Compute the probability content of an interval */
NumericalScalar IndependentCopula::computeProbability(const Interval & interval) const
{
  const UnsignedInteger dimension = getDimension();
  if (interval.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given interval must have dimension=" << dimension << ", here dimension=" << interval.getDimension();

  // Reduce the given interval to the support of the distribution, which is the nD unit cube
  const Interval intersect(interval.intersect(Interval(dimension)));
  // If the intersection is empty
  if (intersect.isNumericallyEmpty()) return 0.0;
  const NumericalPoint lower(intersect.getLowerBound());
  const NumericalPoint upper(intersect.getUpperBound());
  NumericalScalar value = 1.0;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    value *= upper[i] - lower[i];
  }
  return value;
}

/* Get the PDF of the distribution */
NumericalScalar IndependentCopula::computePDF(const NumericalPoint & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  for (UnsignedInteger i = 0; i < dimension; i++)
  {
    NumericalScalar x = point[i];
    // If one component is outside of the support, the PDF is null
    if ((x <= 0.0) || (x >= 1.0)) return 0.0;
  }
  // The point is in the support
  return 1.0;
}

/* Get the CDF of the distribution */
NumericalScalar IndependentCopula::computeCDF(const NumericalPoint & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  NumericalScalar value = 1.0;
  for (UnsignedInteger i = 0; i < dimension; i++)
  {
    NumericalScalar x = point[i];
    // If one component is at the left of the support of its matginal distribution, the CDF is null
    if (x <= 0.0) return 0.0;
    // If the component is inside of the support, multiply the value of the CDF by x
    // FIXME
    if (x < 1.0) value *= x;
  }
  return value;
}

/* Get the survival function of the distribution */
NumericalScalar IndependentCopula::computeSurvivalFunction(const NumericalPoint & point) const
{
  return computeCDF(NumericalPoint(getDimension(), 1.0) - point);
}

/* Get the Kendall concordance of the distribution */
CorrelationMatrix IndependentCopula::getKendallTau() const
{
  return IdentityMatrix(getDimension());
}

/* Get the PDF gradient of the distribution */
NumericalPoint IndependentCopula::computePDFGradient(const NumericalPoint & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  return NumericalPoint(0, 0.0);
}

/* Get the CDF gradient of the distribution */
NumericalPoint IndependentCopula::computeCDFGradient(const NumericalPoint & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  return NumericalPoint(0, 0.0);
}


/* Get the quantile of the distribution */
NumericalPoint IndependentCopula::computeQuantile(const NumericalScalar prob,
    const Bool tail,
    NumericalScalar & marginalProb) const
{
  if (prob < 0.0 || prob > 1.0) throw InvalidArgumentException(HERE) << "Error: cannot compute a quantile for a probability level outside of [0, 1]";
  const NumericalScalar q = tail ? 1.0 - prob : prob;
  marginalProb = std::pow(q, 1.0 / dimension_);
  if (q == 0.0) return NumericalPoint(dimension_, 0.0);
  if (q == 1.0) return NumericalPoint(dimension_, 1.0);
  return NumericalPoint(dimension_, marginalProb);
}

/** Get the product minimum volume interval containing a given probability of the distribution */
Interval IndependentCopula::computeMinimumVolumeIntervalWithMarginalProbability(const NumericalScalar prob, NumericalScalar & marginalProb) const
{
  return computeBilateralConfidenceIntervalWithMarginalProbability(prob, marginalProb);
}

/** Get the product bilateral confidence interval containing a given probability of the distribution */
Interval IndependentCopula::computeBilateralConfidenceIntervalWithMarginalProbability(const NumericalScalar prob, NumericalScalar & marginalProb) const
{
  marginalProb = std::pow(prob, 1.0 / dimension_);
  return Interval(NumericalPoint(dimension_, 0.5 * (1.0 - marginalProb)), NumericalPoint(dimension_, 0.5 * (1.0 + marginalProb)));
}

/** Get the minimum volume level set containing a given probability of the distribution */
LevelSet IndependentCopula::computeMinimumVolumeLevelSetWithThreshold(const NumericalScalar prob, NumericalScalar & threshold) const
{
  const Description inVars(Description::BuildDefault(dimension_, "x"));
  OSS formula;
  formula << "2*max(abs(" << inVars[0] << "-0.5";
  for (UnsignedInteger i = 1; i < dimension_; ++i)
    formula << "),abs(" << inVars[i] << "-0.5";
  formula << "))";
  threshold = std::pow(prob, 1.0 / dimension_);
  return LevelSet(AnalyticalFunction(inVars, Description(1, formula)), threshold);
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
IndependentCopula::Implementation IndependentCopula::getMarginal(const Indices & indices) const
{
  UnsignedInteger dimension = getDimension();
  if (!indices.check(dimension)) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal distribution must be in the range [0, dim-1] and must be different";
  // General case
  return new IndependentCopula(indices.getSize());
}

/* Compute the covariance of the distribution */
void IndependentCopula::computeCovariance() const
{
  const UnsignedInteger dimension = getDimension();
  covariance_ = CovarianceMatrix(dimension);
  for (UnsignedInteger i = 0; i < dimension; i++)
    covariance_(i, i) = 1.0 / 12.0;
  isAlreadyComputedCovariance_ = true;
}

/* Compute the DDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
NumericalScalar IndependentCopula::computeConditionalDDF(const NumericalScalar x, const NumericalPoint & y) const
{
  return 0.0;
}

/* Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
NumericalScalar IndependentCopula::computeConditionalPDF(const NumericalScalar x, const NumericalPoint & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional PDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  if (x < 0.0) return 0.0;
  if (x < 1.0) return 1.0;
  return 0.0;
}

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
NumericalScalar IndependentCopula::computeConditionalCDF(const NumericalScalar x, const NumericalPoint & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile with a conditioning point of dimension greater or equal to the distribution dimension.";
  if (x < 0.0) return 0.0;
  if (x < 1.0) return x;
  return 1.0;
}

/* Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
NumericalScalar IndependentCopula::computeConditionalQuantile(const NumericalScalar q, const NumericalPoint & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile with a conditioning point of dimension greater or equal to the distribution dimension.";
  if ((q < 0.0) || (q > 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile for a probability level outside of [0, 1]";
  return q;
}

/* Get the isoprobabilist transformation */
IndependentCopula::IsoProbabilisticTransformation IndependentCopula::getIsoProbabilisticTransformation() const
{
  IsoProbabilisticTransformation transformation;
  transformation.setEvaluation(new NatafIndependentCopulaEvaluation(getDimension()));
  transformation.setGradient(new NatafIndependentCopulaGradient(getDimension()));
  transformation.setHessian(new NatafIndependentCopulaHessian(getDimension()));

  return transformation;
}

/* Get the inverse isoprobabilistic transformation */
IndependentCopula::InverseIsoProbabilisticTransformation IndependentCopula::getInverseIsoProbabilisticTransformation() const
{
  InverseIsoProbabilisticTransformation transformation;
  transformation.setEvaluation(new InverseNatafIndependentCopulaEvaluation(getDimension()));
  transformation.setGradient(new InverseNatafIndependentCopulaGradient(getDimension()));
  transformation.setHessian(new InverseNatafIndependentCopulaHessian(getDimension()));

  return transformation;
}

/* Tell if the distribution is elliptical */
Bool IndependentCopula::isElliptical() const
{
  return dimension_ == 1;
}

/* Tell if the distribution has elliptical copula */
Bool IndependentCopula::hasEllipticalCopula() const
{
  return true;
}

/* Tell if the distribution has independent copula */
Bool IndependentCopula::hasIndependentCopula() const
{
  return true;
}

/* Method save() stores the object through the StorageManager */
void IndependentCopula::save(Advocate & adv) const
{
  CopulaImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void IndependentCopula::load(Advocate & adv)
{
  CopulaImplementation::load(adv);
  computeRange();
}

END_NAMESPACE_OPENTURNS

