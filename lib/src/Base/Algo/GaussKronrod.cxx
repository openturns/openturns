//                                               -*- C++ -*-
/**
 *  @brief Implement the Gauss-Kronrod adaptive integration method for functions
 *         with 1D argument.
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/GaussKronrod.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class GaussKronrod
 */

CLASSNAMEINIT(GaussKronrod)

static const Factory<GaussKronrod> Factory_GaussKronrod;

/* Constructor without parameters */
GaussKronrod::GaussKronrod()
  : IntegrationAlgorithmImplementation()
  , maximumSubIntervals_(ResourceMap::GetAsUnsignedInteger("GaussKronrod-MaximumSubIntervals"))
  , maximumError_(ResourceMap::GetAsScalar("GaussKronrod-MaximumError"))
  , rule_()
{
  // Check the maximum number of sub-intervals
  if (maximumSubIntervals_ < 2) throw InvalidArgumentException(HERE) << "Error: the maximum number of sub-intervals must be at least 2, here maximumSubIntervals=" << maximumSubIntervals_ << ". Check the value of the key 'GaussKronrod-MaximumSubIntervals' in ResourceMap.";
}

/* Parameters constructor */
GaussKronrod::GaussKronrod(const UnsignedInteger maximumSubIntervals,
                           const Scalar maximumError,
                           const GaussKronrodRule & rule)
  : IntegrationAlgorithmImplementation()
  , maximumSubIntervals_(maximumSubIntervals)
  , maximumError_(maximumError)
  , rule_(rule)
{
  // Check the maximum number of sub-intervals
  if (maximumSubIntervals < 2) throw InvalidArgumentException(HERE) << "Error: the maximum number of sub-intervals must be at least 2, here maximumSubIntervals=" << maximumSubIntervals;
}

/* Virtual constructor */
GaussKronrod * GaussKronrod::clone() const
{
  return new GaussKronrod(*this);
}

/* Compute an approximation of \int_{[a,b]}f(x)dx, where [a,b]
 * is an 1D interval and f a scalar function
 */
Point GaussKronrod::integrate(const Function & function,
                              const Interval & interval,
                              Scalar & error) const
{
  if (interval.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given interval should be 1D, here dimension=" << interval.getDimension();
  Point ai(0);
  Point bi(0);
  Sample fi(0, 0);
  Point ei(0);
  return integrate(function, interval.getLowerBound()[0], interval.getUpperBound()[0], error, ai, bi, fi, ei);
}

Point GaussKronrod::integrate(const Function & function,
                              const Scalar a,
                              const Scalar b,
                              Scalar & error,
                              Point & ai,
                              Point & bi,
                              Sample & fi,
                              Point & ei) const
{
  if (function.getInputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can integrate only 1D function, here input dimension=" << function.getInputDimension();
  const UnsignedInteger outputDimension = function.getOutputDimension();
  if (outputDimension == 0) throw InvalidArgumentException(HERE) << "Error: can integrate only non-zero output dimension function";
  Point result(outputDimension);
  ai = Point(maximumSubIntervals_);
  ai[0] = a;
  bi = Point(maximumSubIntervals_);
  bi[0] = b;
  fi = Sample(maximumSubIntervals_, outputDimension);
  ei = Point(maximumSubIntervals_);
  UnsignedInteger ip = 0;
  UnsignedInteger im = 0;
  error = maximumError_;
  while ((error > 0.25 * maximumError_) && (im < maximumSubIntervals_ - 1))
  {
    ++im;
    bi[im] = bi[ip];
    ai[im] = 0.5 * (ai[ip] + bi[ip]);
    bi[ip] = ai[im];
    fi[ip] = computeRule(function, ai[ip], bi[ip], ei[ip]);
    fi[im] = computeRule(function, ai[im], bi[im], ei[im]);
    UnsignedInteger iErrorMax = 0;
    Scalar errorMax = 0.0;
    error = 0.0;
    result = Point(outputDimension);
    for (UnsignedInteger i = 0; i <= im; ++i)
    {
      const Scalar localError = ei[i];
      for (UnsignedInteger j = 0; j < outputDimension; ++j) result[j] += fi(i, j);
      error += localError * localError;
      // Add a test on the integration interval length to avoid too short intervals
      if ((localError > errorMax) && (bi[i] - ai[i] > maximumError_))
      {
        errorMax = localError;
        iErrorMax = i;
      }
    }
    ip = iErrorMax;
    error = sqrt(error);
  } // while (error >...)
  ai.resize(im + 1);
  bi.resize(im + 1);
  ei.resize(im + 1);
  fi.erase(im + 1, maximumSubIntervals_);
  if (error > maximumError_) LOGINFO(OSS() << "The GaussKronrod algorithm was not able to reach the requested error=" << maximumError_ << ", the achieved error is " << error);
  return result;
}

Point GaussKronrod::integrate(const Function & function,
                              const Scalar a,
                              const Scalar b,
                              Point & error,
                              Point & ai,
                              Point & bi,
                              Sample & fi,
                              Point & ei) const
{
  // Here we initialize the error to a 1D Point in order to use the interface with scalar error
  error = Point(1);
  return integrate(function, a, b, error[0], ai, bi, fi, ei);
}

/* Compute the local GaussKronrod rule over [a, b]. */
Point GaussKronrod::computeRule(const Function & function,
                                const Scalar a,
                                const Scalar b,
                                Scalar & localError) const
{
  const Scalar width = 0.5 * (b - a);
  const Scalar center = 0.5 * (a + b);
  // Generate the set of points
  Sample x(2 * rule_.order_ + 1, 1);
  x(0, 0) = center;
  for (UnsignedInteger i = 0; i < rule_.order_; ++i)
  {
    const Scalar t = width * rule_.otherKronrodNodes_[i];
    x(2 * i + 1, 0) = center - t;
    x(2 * i + 2, 0) = center + t;
  }
  // Use possible parallelization of the evaluation
  const Sample y(function(x));
  Point value(y[0]);
  Point resultGauss(value * rule_.zeroGaussWeight_);
  Point resultGaussKronrod(value * rule_.zeroKronrodWeight_);
  for (UnsignedInteger j = 0; j < (rule_.order_ - 1) / 2; ++j)
  {
    value = y[4 * j + 1] + y[4 * j + 2];
    resultGaussKronrod += value * rule_.otherKronrodWeights_[2 * j];
    value = y[4 * j + 3] + y[4 * j + 4];
    resultGaussKronrod += value * rule_.otherKronrodWeights_[2 * j + 1];
    resultGauss += value * rule_.otherGaussWeights_[j];
  }
  value = y[2 * rule_.order_ - 1] + y[2 * rule_.order_];
  resultGaussKronrod = (resultGaussKronrod + rule_.otherKronrodWeights_[rule_.order_ - 1] * value) * width;
  localError = (resultGaussKronrod - resultGauss * width).norm1();
  return resultGaussKronrod;
}

/* Special case for univariate functions to avoid using costly Point */
Scalar GaussKronrod::integrate(const UniVariateFunction & function,
			       const Scalar a,
			       const Scalar b) const
{
  Scalar result = 0.0;
  Collection<Scalar> ai(maximumSubIntervals_);
  ai[0] = a;
  Collection<Scalar> bi(maximumSubIntervals_);
  bi[0] = b;
  Collection<Scalar> fi(maximumSubIntervals_);
  Collection<Scalar> ei(maximumSubIntervals_);
  UnsignedInteger ip = 0;
  UnsignedInteger im = 0;
  Scalar error = maximumError_;
  while ((error > 0.25 * maximumError_) && (im < maximumSubIntervals_ - 1))
  {
    ++im;
    bi[im] = bi[ip];
    ai[im] = 0.5 * (ai[ip] + bi[ip]);
    bi[ip] = ai[im];
    fi[ip] = computeScalarRule(function, ai[ip], bi[ip], ei[ip]);
    fi[im] = computeScalarRule(function, ai[im], bi[im], ei[im]);
    UnsignedInteger iErrorMax = 0;
    Scalar errorMax = 0.0;
    error = 0.0;
    result = 0.0;
    for (UnsignedInteger i = 0; i <= im; ++i)
    {
      const Scalar localError = ei[i];
      result += fi[i];
      error += localError * localError;
      // Add a test on the integration interval length to avoid too short intervals
      if ((localError > errorMax) && (bi[i] - ai[i] > maximumError_))
      {
        errorMax = localError;
        iErrorMax = i;
      }
    }
    ip = iErrorMax;
    error = sqrt(error);
  } // while (error >...)
  if (error > maximumError_) LOGINFO(OSS() << "The GaussKronrod algorithm was not able to reach the requested error=" << maximumError_ << ", the achieved error is " << error);
  return result;
}

/* Compute the local GaussKronrod rule over [a, b]. */
Scalar GaussKronrod::computeScalarRule(const UniVariateFunction & function,
				       const Scalar a,
				       const Scalar b,
				       Scalar & localError) const
{
  const Scalar width = 0.5 * (b - a);
  const Scalar center = 0.5 * (a + b);
  // Generate the set of points
  const UnsignedInteger size = 2 * rule_.order_ + 1;
  Collection<Scalar> x(size);
  x[0] = center;
  for (UnsignedInteger i = 0; i < rule_.order_; ++i)
  {
    const Scalar t = width * rule_.otherKronrodNodes_[i];
    x[2 * i + 1] = center - t;
    x[2 * i + 2] = center + t;
  }
  Collection<Scalar> y(size);
  for (UnsignedInteger i = 0; i < size; ++i)
    y[i] = function(x[i]);
  Scalar value = y[0];
  Scalar resultGauss = value * rule_.zeroGaussWeight_;
  Scalar resultGaussKronrod = value * rule_.zeroKronrodWeight_;
  for (UnsignedInteger j = 0; j < (rule_.order_ - 1) / 2; ++j)
  {
    value = y[4 * j + 1] + y[4 * j + 2];
    resultGaussKronrod += value * rule_.otherKronrodWeights_[2 * j];
    value = y[4 * j + 3] + y[4 * j + 4];
    resultGaussKronrod += value * rule_.otherKronrodWeights_[2 * j + 1];
    resultGauss += value * rule_.otherGaussWeights_[j];
  }
  value = y[2 * rule_.order_ - 1] + y[2 * rule_.order_];
  resultGaussKronrod = (resultGaussKronrod + rule_.otherKronrodWeights_[rule_.order_ - 1] * value) * width;
  localError = std::abs(resultGaussKronrod - resultGauss * width);
  return resultGaussKronrod;
}

/* Maximum sub-intervals accessor */
UnsignedInteger GaussKronrod::getMaximumSubIntervals() const
{
  return maximumSubIntervals_;
}

void GaussKronrod::setMaximumSubIntervals(const UnsignedInteger maximumSubIntervals)
{
  if (maximumSubIntervals < 1) throw InvalidArgumentException(HERE) << "Error: the number of intervals must be at least 1.";
  maximumSubIntervals_ = maximumSubIntervals;
}

/* Maximum error accessor */
Scalar GaussKronrod::getMaximumError() const
{
  return maximumError_;
}

void GaussKronrod::setMaximumError(const Scalar maximumError)
{
  if (!(maximumError >= 0.0)) throw InvalidArgumentException(HERE) << "Error: the maximum error must be nonnegative, here maximum error=" << maximumError;
  maximumError_ = maximumError;
}

/* Rule accessor */
GaussKronrodRule GaussKronrod::getRule() const
{
  return rule_;
}

void GaussKronrod::setRule(const GaussKronrodRule & rule)
{
  rule_ = rule;
}

/* String converter */
String GaussKronrod::__repr__() const
{
  OSS oss(true);
  oss << "class=" << GaussKronrod::GetClassName()
      << ", maximum sub intervals=" << maximumSubIntervals_
      << ", maximum error=" << maximumError_
      << ", rule=" << rule_;
  return oss;
}

/* String converter */
String GaussKronrod::__str__(const String & offset) const
{
  OSS oss(false);
  oss << GaussKronrod::GetClassName()
      << "(maximum sub intervals=" << maximumSubIntervals_
      << ", maximum error=" << maximumError_
      << ", rule=" << rule_.__str__(offset)
      << ")";
  return oss;
}

END_NAMESPACE_OPENTURNS
