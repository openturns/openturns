//                                               -*- C++ -*-
/**
 *  @brief IterativeMoments implements the iterative computation of centered moments
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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


#include "openturns/IterativeAlgorithm.hxx"
#include "openturns/Log.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Point.hxx"
#include "openturns/IterativeMoments.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(IterativeMoments)

static const Factory<IterativeMoments> Factory_IterativeMoments;

/** Default constructor */
IterativeMoments::IterativeMoments(const UnsignedInteger orderMax, const UnsignedInteger dimension)
  : IterativeAlgorithmImplementation(dimension)
  , orderMax_(orderMax)
  , centeredMoments_(orderMax, dimension_)
{
  // Nothing to do
}

/* Virtual constructor */
IterativeMoments * IterativeMoments::clone() const
{
  return new IterativeMoments(*this);
}

/* String converter */
String IterativeMoments::__repr__() const
{
  OSS oss(true);
  oss << "class=" << IterativeMoments::GetClassName()
      << " iteration=" << iteration_
      << " dimension=" << dimension_
      << " order max=" << orderMax_
      << " centered moments=" << centeredMoments_.__repr__();
  return oss;
}

/* String converter */
String IterativeMoments::__str__(const String & offset) const
{
  return getCenteredMoments().__str__(offset);
}

/*  Accessor to the centered moments */
Sample IterativeMoments::getCenteredMoments() const
{
  return centeredMoments_;
}

/* Accessor to the maximum order declared */
UnsignedInteger IterativeMoments::getOrder() const
{
  return orderMax_;
}

/* Accessor to the mean */
Point IterativeMoments::getMean() const
{
  LOGDEBUG(OSS() << "IterativeMoments::getMean()");
  if (!(iteration_ > 0)) throw InternalException(HERE) << "Error: cannot compute the mean per component of an empty sample.";
  return centeredMoments_[0] / iteration_;
}

/* Accessor to the unbiased variance */
Point IterativeMoments::getVariance() const
{
  LOGDEBUG(OSS() << "IterativeMoments::getVariance()");
  if (!(iteration_ > 0)) throw InternalException(HERE) << "Error: cannot compute the variance per component of an empty sample.";
  if (!(orderMax_ >= 2)) throw InternalException(HERE) << "Error: unavailable method, the declared maximum should be at least 2.";

  /* Special case for a size 1 */
  if (iteration_ == 1) return Point(dimension_, 0.0);

  return centeredMoments_[1] / (iteration_ - 1);
}

/* Accessor to the unbiased estimator of the skewness */
Point IterativeMoments::getSkewness() const
{
  LOGDEBUG(OSS() << "IterativeMoments::getSkewness()");
  if (!(iteration_ >= 2)) throw InternalException(HERE) << "Error: cannot compute the skewness per component of a sample of size less than 2.";
  if (!(orderMax_ >= 3)) throw InternalException(HERE) << "Error: unavailable method, the declared maximum order should be at least 3.";

  /* Special case for a size 2 */
  if (iteration_ == 2) return Point(dimension_, 0.0);
  
  Point result(dimension_);
  const Point varianceEstimator(getVariance());
  for(UnsignedInteger d = 0; d < dimension_; ++d)
  {
    if (!(varianceEstimator[d] < 0.0 || varianceEstimator[d] > 0.0)) throw NotDefinedException(HERE) << "Error: the sample has component " << d << " constant. The skewness is not defined.";
    result[d] = iteration_ / ((iteration_ - 1.0) * (iteration_ - 2.0)) * centeredMoments_(2, d) / std::pow(varianceEstimator[d], 1.5) ;
  }
  return result;
}

/* Accessor to the unbiased estimator of the kurtosis */
Point IterativeMoments::getKurtosis() const
{
  LOGDEBUG(OSS() << "IterativeMoments::getKurtosis()");
  if (!(iteration_ >= 3)) throw InternalException(HERE) << "Error: cannot compute the kurtosis per component of a sample of size less than 4.";
  if (!(orderMax_ >= 4)) throw InternalException(HERE) << "Error: unavailable method, the declared maximum order is lower than 4.";

  /* Special case for a size 3 */
  if (iteration_ == 3) return Point(dimension_, 0.0);

  Point result(dimension_);
  const Point varianceEstimator(getVariance());
  const Point skewnessEstimator(getSkewness());
  const UnsignedInteger n = iteration_;

  const Scalar factor1 = n * (n + 1.0) / ((n - 1.0) * (n - 2.0) * (n - 3.0));
  const Scalar factor2 = -3.0 * (3.0 * n - 5) / ((n - 2.0) * (n - 3.0));

  for(UnsignedInteger d = 0; d < dimension_; ++d)
  {
    LOGDEBUG(OSS() << "varianceEstimator[d] = " << varianceEstimator[d]);
    if (!(varianceEstimator[d] < 0.0 || varianceEstimator[d] > 0.0)) throw NotDefinedException(HERE) << "Error: the sample has component " << d << " constant. The kurtosis is not defined.";
    result[d] = factor1 * centeredMoments_(3, d) / std::pow(varianceEstimator[d], 2) + factor2;
  }
  return result;
}

/*  Increment method from a Point */
void IterativeMoments::increment(const Point & newData)
{
  /* centered new data */
  Point delta(newData);
  if (iteration_ > 0)
  {
    /* mean at the previous step */
    const Point mu_previous(getMean());
    delta -= mu_previous;
  }
  ++ iteration_;
  const Point delta_over_n = delta / iteration_;

  for(UnsignedInteger d = 0; d < dimension_; ++d)
  {
    centeredMoments_(0, d) += newData[d];
  }
  if(orderMax_ > 1) updateHigherMoments(orderMax_, delta, delta_over_n);
}

/*  Increment method from a Sample */
void IterativeMoments::increment(const Sample & newData)
{
  for(UnsignedInteger j = 0; j < newData.getSize(); ++j)
  {
    increment(newData[j]);
  }
}

/* Update the centered moments higher than 2 using a recurrence relation for 3 and more
    see: Simpler Online Updates for Arbitrary-Order Central Moments, Meng (2015)
*/
void IterativeMoments::updateHigherMoments(UnsignedInteger orderMax, const Point & delta, const Point & delta_over_n)
{
  /* special case: second centered moment */
  if(orderMax == 2)
  {
    for(UnsignedInteger d = 0; d < dimension_; ++d)
    {
      centeredMoments_(1, d) += delta[d] * (delta[d] - delta_over_n[d]);
    }
  }
  else
  {
    /* general case for order >= 2 */
    for(UnsignedInteger d = 0; d < dimension_; ++d)
    {
      centeredMoments_(1, d) += delta[d] * (delta[d] - delta_over_n[d]);
      for(UnsignedInteger order = 3 ; order <= orderMax ; ++order)
      {
        Scalar tmp = 0.0;
        for(UnsignedInteger l = 1; l <= order - 2; ++l)
        {
          const UnsignedInteger binomialCoeff = SpecFunc::BinomialCoefficient(order, l) ;
          tmp += std::pow(delta_over_n[d], l) * centeredMoments_(order - l - 1, d) * binomialCoeff ;
        }
        centeredMoments_(order - 1, d) += delta[d] * (std::pow(delta[d], order - 1) - std::pow(delta_over_n[d], order - 1)) - tmp;
      }
    }
  }
}

/* Accessor to the coefficient of variation */
Point IterativeMoments::getCoefficientOfVariation() const
{
  if ((orderMax_ < 2)) throw InternalException(HERE) << "Error: unavailable method, the declared maximum order is lower than 2.";

  Point coeficientOfVariationData(dimension_, 0.0);
  const Point meanData(getMean());
  const Point varData(getVariance());
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    coeficientOfVariationData[i] = std::pow(varData[i], 0.5) / meanData[i];
  }

  return coeficientOfVariationData;
}

/* Accessor to the standard error of the mean */
Point IterativeMoments::getStandardErrorOfTheMean() const
{
  if ((orderMax_ < 2)) throw InternalException(HERE) << "Error: unavailable method, the declared maximum order is lower than 2.";

  Point standardErrorOfTheMean(dimension_, 0.0);
  const Point varData(getVariance());

  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    standardErrorOfTheMean[i] = std::pow(varData[i], 0.5) / std::pow(iteration_, 0.5);
  }

  return standardErrorOfTheMean;
}

/* Accessor to the standard deviation */
Point IterativeMoments::getStandardDeviation() const
{
  if ((orderMax_ < 2)) throw InternalException(HERE) << "Error: unavailable method, the declared maximum order is lower than 2.";

  Point standardDeviationData(dimension_, 0.0);
  const Point varData(getVariance());

  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    standardDeviationData[i] = std::pow(varData[i], 0.5);
  }

  return standardDeviationData;
}

/* Method save() stores the object through the StorageManager */
void IterativeMoments::save(Advocate & adv) const
{
  IterativeAlgorithmImplementation::save(adv);
  adv.saveAttribute( "orderMax_", orderMax_);
  adv.saveAttribute( "centeredMoments_", centeredMoments_);
}

/* Method load() reloads the object from the StorageManager */
void IterativeMoments::load(Advocate & adv)
{
  IterativeAlgorithmImplementation::load(adv);
  adv.loadAttribute( "orderMax_", orderMax_);
  adv.loadAttribute( "centeredMoments_", centeredMoments_);
}

END_NAMESPACE_OPENTURNS
