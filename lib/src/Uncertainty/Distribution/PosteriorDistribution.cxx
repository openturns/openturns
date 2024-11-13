//                                               -*- C++ -*-
/**
 *  @brief The PosteriorDistribution distribution
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/PosteriorDistribution.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/GaussLegendre.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(PosteriorDistribution)

static const Factory<PosteriorDistribution> Factory_PosteriorDistribution;


/* Default constructor */
PosteriorDistribution::PosteriorDistribution()
  : DistributionImplementation(),
    deconditionedDistribution_(),
    observations_()
{
  setName("PosteriorDistribution");
  // First, set the observations
  observations_ = Sample(1, deconditionedDistribution_.getConditionedDistribution().getMean());
  // Then, set the deconditioned distribution. It also set the dimension.
  setDeconditionedDistribution(deconditionedDistribution_);
  computeRange();
}

/* Parameters constructor */
PosteriorDistribution::PosteriorDistribution(const DeconditionedDistribution & deconditionedDistribution,
    const Sample & observations)
  : DistributionImplementation(),
    deconditionedDistribution_(),
    observations_(observations)
{
  setName("PosteriorDistribution");
  if (observations.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a posterior distribution with no observation.";
  setDeconditionedDistribution(deconditionedDistribution);
}

/* Comparison operator */
Bool PosteriorDistribution::operator ==(const PosteriorDistribution & other) const
{
  if (this == &other) return true;
  return (deconditionedDistribution_ == other.deconditionedDistribution_) && (observations_ == other.observations_);
}

Bool PosteriorDistribution::equals(const DistributionImplementation & other) const
{
  const PosteriorDistribution* p_other = dynamic_cast<const PosteriorDistribution*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String PosteriorDistribution::__repr__() const
{
  OSS oss;
  oss << "class=" << PosteriorDistribution::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " deconditioned distribution=" << deconditionedDistribution_
      << " observations=" << observations_;
  return oss;
}

String PosteriorDistribution::__str__(const String & offset) const
{
  OSS oss;
  oss << getClassName() << "(deconditioned distribution = " << deconditionedDistribution_.__str__() << ", observations =\n" << offset << observations_.__str__(offset) << ")";
  return oss;
}

/* Virtual constructor */
PosteriorDistribution * PosteriorDistribution::clone() const
{
  return new PosteriorDistribution(*this);
}

/* Compute the likelihood of the observations */
Point PosteriorDistribution::computeLikelihood(const Point & y) const
{
  return Point(1, std::exp(computeLogLikelihood(y)));
}

/* Compute the log-likelihood of the observations */
Scalar PosteriorDistribution::computeLogLikelihood(const Point & y) const
{
  Distribution conditionedDistribution(deconditionedDistribution_.getConditionedDistribution());
  const Point theta(deconditionedDistribution_.getLinkFunction()(y));
  conditionedDistribution.setParameter(theta);
  Scalar logLikelihood = 0.0;
  const UnsignedInteger size = observations_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar atomicValue = conditionedDistribution.computeLogPDF(observations_[i]);
    logLikelihood += atomicValue;
  }
  return logLikelihood;
}

/* Get the log PDF of the distribution */
Scalar PosteriorDistribution::computeLogPDF(const Point & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();

  return deconditionedDistribution_.getConditioningDistribution().computeLogPDF(point) - logNormalizationFactor_ + computeLogLikelihood(point);
}

/* Get the PDF of the distribution */
Scalar PosteriorDistribution::computePDF(const Point & point) const
{
  return std::exp(computeLogPDF(point));
}

/* Get the CDF of the distribution */
Scalar PosteriorDistribution::computeCDF(const Point & point) const
{
  // FIXME: computeExpectation seems to incorrectly compute the CDF of the prior
  if (deconditionedDistribution_.getConditioningDistribution().isContinuous())
    return DistributionImplementation::computeCDF(point);

  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();

  Description inputDescription(getDimension());
  for (UnsignedInteger i = 0; i < getDimension(); ++i) inputDescription[i] = String(OSS() << "x" << i);
  const SymbolicFunction f(inputDescription, Description(1, "1"));
  const Scalar cdf = deconditionedDistribution_.computeExpectation(f, point)[0];
  return cdf;
}

/* Parameters value and description accessor */
PosteriorDistribution::PointWithDescriptionCollection PosteriorDistribution::getParametersCollection() const
{
  throw NotYetImplementedException(HERE) << "In PosteriorDistribution::getParametersCollection() const";
}

void PosteriorDistribution::setParametersCollection(const PointCollection & )
{
  throw NotYetImplementedException(HERE) << "In PosteriorDistribution::setParametersCollection(const PointCollection & parametersCollection)";
}


/* Deconditioned distribution accessor */
void PosteriorDistribution::setDeconditionedDistribution(const DeconditionedDistribution & deconditionedDistribution)
{
  if (observations_.getDimension() != deconditionedDistribution.getDimension()) throw InvalidArgumentException(HERE) << "Error: the conditioned distribution defining the deconditioned distribution must have the same dimension as the observations.";
  deconditionedDistribution_ = deconditionedDistribution;
  setDimension(deconditionedDistribution.getConditioningDistribution().getDimension());
  logNormalizationFactor_ = 0.0;
  const UnsignedInteger size = observations_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
    logNormalizationFactor_ += deconditionedDistribution_.computeLogPDF(observations_[i]);
  computeRange();
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;

  // FIXME: tweak normalization factor
  if (deconditionedDistribution_.getConditioningDistribution().isContinuous())
  {
    GaussLegendre integrationAlgorithm(getDimension());
    const Scalar iPDF = integrationAlgorithm.integrate(PDFWrapper(this), getRange())[0];
    logNormalizationFactor_ += std::log(iPDF);
  }
}

DeconditionedDistribution PosteriorDistribution::getDeconditionedDistribution() const
{
  return deconditionedDistribution_;
}


/* Observations accessor */
void PosteriorDistribution::setObservations(const Sample & observations)
{
  if (observations.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot use a posterior distribution with no observation.";
  if (observations.getDimension() != deconditionedDistribution_.getDimension()) throw InvalidArgumentException(HERE) << "Error: the conditioned distribution defining the deconditioned distribution must have the same dimension as the observations.";
  observations_ = observations;
  setDeconditionedDistribution(deconditionedDistribution_);
}

Sample PosteriorDistribution::getObservations() const
{
  return observations_;
}

/* Log normalization factor accessor */
Scalar PosteriorDistribution::getLogNormalizationFactor() const
{
  return logNormalizationFactor_;
}

/* Compute the numerical range of the distribution given the parameters values */
void PosteriorDistribution::computeRange()
{
  setRange(deconditionedDistribution_.conditioningDistribution_.getRange());
}

class PosteriorDistributionLikelihoodEvaluation : public EvaluationImplementation
{
public:
  PosteriorDistributionLikelihoodEvaluation(const PosteriorDistribution & distribution) : EvaluationImplementation(), distribution_(distribution) {}
  PosteriorDistributionLikelihoodEvaluation * clone() const override
  {
    return new PosteriorDistributionLikelihoodEvaluation(*this);
  }
  UnsignedInteger getInputDimension() const override
  {
    return distribution_.getDimension();
  }
  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

  Point operator()(const Point & inP) const override
  {
    return distribution_.computeLikelihood(inP);
  }

private:
  const PosteriorDistribution & distribution_;
};

/* Compute the mean of the distribution */
void PosteriorDistribution::computeMean() const
{
  Description inputDescription(Description::BuildDefault(getDimension(), "x"));
  const SymbolicFunction meanFunction(inputDescription, inputDescription);
  const Function likelihood(PosteriorDistributionLikelihoodEvaluation(*this));
  mean_ = deconditionedDistribution_.computeExpectation(likelihood * meanFunction, getRange().getUpperBound()) / std::exp(logNormalizationFactor_);
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point PosteriorDistribution::getStandardDeviation() const
{
  // To insure that the covariance has been computed
  getCovariance();
  Point sigma(getDimension());
  for (UnsignedInteger i = 0; i < getDimension(); ++i) sigma[i] = std::sqrt(covariance_(i, i));
  return sigma;
}

/* Get the skewness of the distribution */
Point PosteriorDistribution::getSkewness() const
{
  throw NotYetImplementedException(HERE) << "In PosteriorDistribution::getSkewness() const";
}

/* Get the kurtosis of the distribution */
Point PosteriorDistribution::getKurtosis() const
{
  throw NotYetImplementedException(HERE) << "In PosteriorDistribution::getKurtosis() const";
}

/* Compute the covariance of the distribution */
void PosteriorDistribution::computeCovariance() const
{
  const UnsignedInteger dimension = getDimension();
  covariance_ = CovarianceMatrix(dimension);
  // To insure that the mean has been computed
  getMean();
  Description inputDescription(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i) inputDescription[i] = String(OSS() << "x" << i);
  Description formulas((dimension * (dimension + 1)) / 2);
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar muI = mean_[i];
    for (UnsignedInteger j = 0; j <= i; ++j)
    {
      const Scalar muJ = mean_[j];
      formulas[index] = String(OSS() << "(x" << i << "-" << muI << ")*(x" << j << "-" << muJ << ")");
      ++index;
    }
  }
  const SymbolicFunction covarianceFunction(inputDescription, formulas);
  const Function likelihood(PosteriorDistributionLikelihoodEvaluation(*this));
  const Point result(deconditionedDistribution_.computeExpectation(likelihood * covarianceFunction, getRange().getUpperBound()) / std::exp(logNormalizationFactor_));
  index = 0;
  for (UnsignedInteger i = 0; i < dimension; ++i)
    for (UnsignedInteger j = 0; j <= i; ++j)
    {
      covariance_(i, j) = result[index];
      ++index;
    }
  isAlreadyComputedCovariance_ = true;
}

/* Method save() stores the object through the StorageManager */
void PosteriorDistribution::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute( "deconditionedDistribution_", deconditionedDistribution_ );
  adv.saveAttribute( "observations_", observations_ );
  adv.saveAttribute( "logNormalizationFactor_", logNormalizationFactor_ );
}

/* Method load() reloads the object from the StorageManager */
void PosteriorDistribution::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute( "deconditionedDistribution_", deconditionedDistribution_ );
  adv.loadAttribute( "observations_", observations_ );
  adv.loadAttribute( "logNormalizationFactor_", logNormalizationFactor_ );
  computeRange();
}


END_NAMESPACE_OPENTURNS
