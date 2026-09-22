//                                               -*- C++ -*-
/**
 *  @brief The PiecewiseLinearDistribution distribution
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
#include <cmath>
#include <algorithm>
#include "openturns/PiecewiseLinearDistribution.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PiecewiseLinearDistribution)

static const Factory<PiecewiseLinearDistribution> Factory_PiecewiseLinearDistribution;

/* Kahan compensated summation helper */
struct KahanSum
{
  Scalar value;
  Scalar compensation;
  KahanSum() : value(0.0), compensation(0.0) {}
  void add(Scalar term)
  {
    const Scalar y = term - compensation;
    const Scalar t = value + y;
    compensation = (t - value) - y;
    value = t;
  }
};

/* Default constructor */
PiecewiseLinearDistribution::PiecewiseLinearDistribution()
  : DistributionImplementation()
  , x_({-1.0, 1.0})
  , y_({0.5, 0.5})
  , yNorm_({0.5, 0.5})
  , probabilities_(1)
{
  setName("PiecewiseLinearDistribution");
  setDimension(1);
  update();
}

/* Parameters constructor */
PiecewiseLinearDistribution::PiecewiseLinearDistribution(const Point & x,
    const Point & y)
  : DistributionImplementation()
  , x_(x)
  , y_(y)
  , yNorm_(y)
  , probabilities_(0)
{
  setName("PiecewiseLinearDistribution");
  if (x.getSize() < 2)
    throw InvalidArgumentException(HERE) << "Error: x must have at least 2 elements, here x.getSize()=" << x.getSize();
  if (y.getSize() != x.getSize())
    throw InvalidArgumentException(HERE) << "Error: y must have the same size as x, here x.getSize()=" << x.getSize() << " and y.getSize()=" << y.getSize();
  // Check strictly increasing
  for (UnsignedInteger i = 1; i < x.getSize(); ++i)
  {
    if (x[i] <= x[i - 1])
      throw InvalidArgumentException(HERE) << "Error: x must be strictly increasing, here x[" << i - 1 << "]=" << x[i - 1] << " and x[" << i << "]=" << x[i];
  }
  // Check nonnegativity
  for (UnsignedInteger i = 0; i < y.getSize(); ++i)
  {
    if (y[i] < 0.0)
      throw InvalidArgumentException(HERE) << "Error: y must be nonnegative, here y[" << i << "]=" << y[i];
  }
  // Check at least one positive value
  Scalar maxY = 0.0;
  for (UnsignedInteger i = 0; i < y.getSize(); ++i)
  {
    if (y[i] > maxY) maxY = y[i];
  }
  if (maxY <= 0.0)
    throw InvalidArgumentException(HERE) << "Error: at least one y value must be strictly positive";
  setDimension(1);
  update();
}

/* Comparison operator */
Bool PiecewiseLinearDistribution::operator ==(const PiecewiseLinearDistribution & other) const
{
  if (this == &other) return true;
  return (x_ == other.x_) && (y_ == other.y_);
}

Bool PiecewiseLinearDistribution::equals(const DistributionImplementation & other) const
{
  const PiecewiseLinearDistribution * p_other = dynamic_cast<const PiecewiseLinearDistribution *>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String PiecewiseLinearDistribution::__repr__() const
{
  OSS oss;
  oss << "class=" << PiecewiseLinearDistribution::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " x=" << x_
      << " y=" << y_;
  return oss;
}

String PiecewiseLinearDistribution::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "(x = " << x_ << ", y = " << y_ << ")";
  return oss;
}

/* Virtual constructor */
PiecewiseLinearDistribution * PiecewiseLinearDistribution::clone() const
{
  return new PiecewiseLinearDistribution(*this);
}

/* Update precomputed quantities */
void PiecewiseLinearDistribution::update()
{
  const UnsignedInteger n = x_.getSize();
  // Compute normalized y_ into yNorm_
  Scalar totalIntegral = 0.0;
  for (UnsignedInteger i = 0; i < n - 1; ++i)
  {
    const Scalar dx = x_[i + 1] - x_[i];
    totalIntegral += 0.5 * (y_[i] + y_[i + 1]) * dx;
  }
  if (totalIntegral > 0.0)
  {
    for (UnsignedInteger i = 0; i < n; ++i)
      yNorm_[i] = y_[i] / totalIntegral;
  }
  else
  {
    yNorm_ = y_;
  }
  // Compute segment probabilities using normalized values
  probabilities_ = Point(n - 1);
  for (UnsignedInteger i = 0; i < n - 1; ++i)
  {
    const Scalar dx = x_[i + 1] - x_[i];
    probabilities_[i] = 0.5 * (yNorm_[i] + yNorm_[i + 1]) * dx;
  }
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  computeRange();
}

/* Compute the numerical range of the distribution given the parameters values */
void PiecewiseLinearDistribution::computeRange()
{
  Interval range(x_[0], x_[x_.getSize() - 1]);
  range.setFiniteLowerBound(Interval::BoolCollection(1, false));
  range.setFiniteUpperBound(Interval::BoolCollection(1, false));
  setRange(range);
}

/* Get one realization of the distribution */
Point PiecewiseLinearDistribution::getRealization() const
{
  return Point(1, computeScalarQuantile(RandomGenerator::Generate()));
}

/* Get the DDF of the distribution */
Point PiecewiseLinearDistribution::computeDDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  const Scalar x = point[0];
  if ((x < x_[0]) || (x > x_[x_.getSize() - 1])) return Point(1, 0.0);
  const UnsignedInteger n = x_.getSize();
  UnsignedInteger iLeft = 0;
  for (UnsignedInteger i = 0; i < n - 1; ++i)
  {
    if (x >= x_[i] && x <= x_[i + 1])
    {
      iLeft = i;
      break;
    }
  }
  const Scalar dx = x_[iLeft + 1] - x_[iLeft];
  if (dx == 0.0) return Point(1, 0.0);
  return Point(1, (yNorm_[iLeft + 1] - yNorm_[iLeft]) / dx);
}


/* Get the PDF of the distribution */
Scalar PiecewiseLinearDistribution::computePDF(const Scalar x) const
{
  if ((x < x_[0]) || (x > x_[x_.getSize() - 1])) return 0.0;
  const UnsignedInteger n = x_.getSize();
  UnsignedInteger iLeft = 0;
  for (UnsignedInteger i = 0; i < n - 1; ++i)
  {
    if (x >= x_[i] && x <= x_[i + 1])
    {
      iLeft = i;
      break;
    }
  }
  const Scalar dx = x_[iLeft + 1] - x_[iLeft];
  if (dx == 0.0) return yNorm_[iLeft];
  const Scalar t = (x - x_[iLeft]) / dx;
  return yNorm_[iLeft] + (yNorm_[iLeft + 1] - yNorm_[iLeft]) * t;
}

Scalar PiecewiseLinearDistribution::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  return computePDF(point[0]);
}


/* Get the CDF of the distribution */
Scalar PiecewiseLinearDistribution::computeCDF(const Scalar x) const
{
  if (x <= x_[0]) return 0.0;
  const UnsignedInteger n = x_.getSize();
  if (x >= x_[n - 1]) return 1.0;
  Scalar result = 0.0;
  UnsignedInteger iLeft = 0;
  for (UnsignedInteger i = 0; i < n - 1; ++i)
  {
    if (x >= x_[i + 1])
    {
      result += probabilities_[i];
    }
    else
    {
      iLeft = i;
      break;
    }
  }
  // Partial segment
  const Scalar dx = x_[iLeft + 1] - x_[iLeft];
  if (dx > 0.0)
  {
    const Scalar t = (x - x_[iLeft]) / dx;
    result += dx * (yNorm_[iLeft] * t + 0.5 * (yNorm_[iLeft + 1] - yNorm_[iLeft]) * t * t);
  }
  return result;
}

Scalar PiecewiseLinearDistribution::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  return computeCDF(point[0]);
}


/* Get the quantile of the distribution */
Scalar PiecewiseLinearDistribution::computeScalarQuantile(const Scalar prob,
    const Bool tail) const
{
  if (!((prob >= 0.0) && (prob <= 1.0)))
    throw InvalidArgumentException(HERE) << "computeScalarQuantile expected prob to belong to [0,1], but is " << prob;
  const Scalar q = tail ? 1.0 - prob : prob;
  if (q <= 0.0) return x_[0];
  if (q >= 1.0) return x_[x_.getSize() - 1];
  Scalar cumProb = 0.0;
  const UnsignedInteger n = x_.getSize();
  for (UnsignedInteger i = 0; i < n - 1; ++i)
  {
    if (cumProb + probabilities_[i] >= q)
    {
      // Target is within this segment
      const Scalar dx = x_[i + 1] - x_[i];
      if (dx <= 0.0) return x_[i];
      const Scalar remain = (q - cumProb) / dx;
      const Scalar a = 0.5 * (yNorm_[i + 1] - yNorm_[i]);
      const Scalar b = yNorm_[i];
      Scalar t = 0.0;
      if (std::abs(a) < SpecFunc::ScalarEpsilon)
      {
        // Linear case: b*t = remain
        t = (b > 0.0) ? remain / b : 0.0;
        if (t > 1.0) t = 1.0;
      }
      else
      {
        // Quadratic: a*t^2 + b*t - remain = 0
        const Scalar discriminant = b * b + 4.0 * a * remain;
        t = (-b + std::sqrt(std::max(0.0, discriminant))) / (2.0 * a);
        if (t < 0.0) t = 0.0;
        if (t > 1.0) t = 1.0;
      }
      return x_[i] + t * (x_[i + 1] - x_[i]);
    }
    cumProb += probabilities_[i];
  }
  return x_[n - 1];
}


Scalar PiecewiseLinearDistribution::computeProbability(const Interval & interval) const
{
  if (interval.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "computeProbability expected an interval of dimension=" << dimension_ << ", got dimension=" << interval.getDimension();
  if (interval.getUpperBound()[0] <= interval.getLowerBound()[0])
    return 0.0;
  return computeProbabilityGeneral1D(interval.getLowerBound()[0], interval.getUpperBound()[0]);
}


/* Compute the entropy of the distribution */
Scalar PiecewiseLinearDistribution::computeEntropy() const
{
  Scalar entropy = 0.0;
  const UnsignedInteger n = x_.getSize();
  for (UnsignedInteger i = 0; i < n - 1; ++i)
  {
    const Scalar dx = x_[i + 1] - x_[i];
    const Scalar y0 = yNorm_[i];
    const Scalar y1 = yNorm_[i + 1];
    if (std::abs(y1 - y0) < SpecFunc::ScalarEpsilon * (std::abs(y0) + std::abs(y1) + SpecFunc::ScalarEpsilon))
    {
      // Nearly constant segment: integral of c*log(c) = c*log(c)
      if (y0 > 0.0)
        entropy -= dx * y0 * std::log(y0);
    }
    else
    {
      // integral of (a+bt)*log(a+bt) dt from 0 to 1, where a=y0, b=y1-y0
      // antiderivative: (a+bt)^2/(2b) * (log(a+bt) - 1/2)
      // definite integral: [y1^2*(log(y1)-1/2) - y0^2*(log(y0)-1/2)] / (2*d)
      //                  = [y1^2*log(y1) - y0^2*log(y0)]/(2*d) - (y0+y1)/4
      const Scalar d = y1 - y0;
      // Use the fact that lim(x->0) x^2*log(x) = 0
      const Scalar y0_2_log_y0 = (y0 > 0.0) ? y0 * y0 * std::log(y0) : 0.0;
      const Scalar y1_2_log_y1 = (y1 > 0.0) ? y1 * y1 * std::log(y1) : 0.0;
      entropy -= dx * ((y1_2_log_y1 - y0_2_log_y0) / (2.0 * d) - (y0 + y1) / 4.0);
    }
  }
  return entropy;
}


/* Get the roughness, i.e. the L2-norm of the PDF */
Scalar PiecewiseLinearDistribution::getRoughness() const
{
  Scalar roughness = 0.0;
  const UnsignedInteger n = x_.getSize();
  for (UnsignedInteger i = 0; i < n - 1; ++i)
  {
    const Scalar dx = x_[i + 1] - x_[i];
    const Scalar y0 = yNorm_[i];
    const Scalar y1 = yNorm_[i + 1];
    roughness += dx * (y0 * y0 + y0 * y1 + y1 * y1) / 3.0;
  }
  return roughness;
}


/* Compute the mean of the distribution */
void PiecewiseLinearDistribution::computeMean() const
{
  KahanSum mSum;
  const UnsignedInteger n = x_.getSize();
  for (UnsignedInteger i = 0; i < n - 1; ++i)
  {
    const Scalar dx = x_[i + 1] - x_[i];
    const Scalar x0 = x_[i];
    const Scalar x1 = x_[i + 1];
    const Scalar y0 = yNorm_[i];
    const Scalar y1 = yNorm_[i + 1];
    mSum.add(dx * (y0 * (2.0 * x0 + x1) + y1 * (x0 + 2.0 * x1)) / 6.0);
  }
  mean_ = Point(1, mSum.value);
  isAlreadyComputedMean_ = true;
}


/* Compute the covariance of the distribution */
void PiecewiseLinearDistribution::computeCovariance() const
{
  KahanSum m2Sum;
  const UnsignedInteger n = x_.getSize();
  for (UnsignedInteger i = 0; i < n - 1; ++i)
  {
    const Scalar dx = x_[i + 1] - x_[i];
    const Scalar x0 = x_[i];
    const Scalar x1 = x_[i + 1];
    const Scalar x0_2 = x0 * x0;
    const Scalar x1_2 = x1 * x1;
    const Scalar y0 = yNorm_[i];
    const Scalar y1 = yNorm_[i + 1];
    m2Sum.add(dx * (y0 * (3.0 * x0_2 + 2.0 * x0 * x1 + x1_2)
                     + y1 * (x0_2 + 2.0 * x0 * x1 + 3.0 * x1_2)) / 12.0);
  }
  covariance_ = CovarianceMatrix(1);
  const Scalar mu = getMean()[0];
  covariance_(0, 0) = m2Sum.value - mu * mu;
  isAlreadyComputedCovariance_ = true;
}


/* Get the standard deviation of the distribution */
Point PiecewiseLinearDistribution::getStandardDeviation() const
{
  return Point(1, std::sqrt(getCovariance()(0, 0)));
}


/* Get the skewness of the distribution */
Point PiecewiseLinearDistribution::getSkewness() const
{
  KahanSum m3Sum;
  const UnsignedInteger n = x_.getSize();
  for (UnsignedInteger i = 0; i < n - 1; ++i)
  {
    const Scalar dx = x_[i + 1] - x_[i];
    const Scalar x0 = x_[i];
    const Scalar x1 = x_[i + 1];
    const Scalar x0_2 = x0 * x0;
    const Scalar x0_3 = x0_2 * x0;
    const Scalar x1_2 = x1 * x1;
    const Scalar x1_3 = x1_2 * x1;
    const Scalar y0 = yNorm_[i];
    const Scalar y1 = yNorm_[i + 1];
    m3Sum.add(dx * (y0 * (4.0 * x0_3 + 3.0 * x0_2 * x1 + 2.0 * x0 * x1_2 + x1_3)
                    + y1 * (x0_3 + 2.0 * x0_2 * x1 + 3.0 * x0 * x1_2 + 4.0 * x1_3)) / 20.0);
  }
  const Scalar mu = getMean()[0];
  const Scalar sigma2 = getCovariance()(0, 0);
  const Scalar sigma = std::sqrt(sigma2);
  if (sigma == 0.0) return Point(1, 0.0);
  return Point(1, (m3Sum.value - 3.0 * mu * sigma2 - mu * mu * mu) / (sigma2 * sigma));
}


/* Get the kurtosis of the distribution */
Point PiecewiseLinearDistribution::getKurtosis() const
{
  KahanSum m4Sum;
  const UnsignedInteger n = x_.getSize();
  for (UnsignedInteger i = 0; i < n - 1; ++i)
  {
    const Scalar dx = x_[i + 1] - x_[i];
    const Scalar x0 = x_[i];
    const Scalar x1 = x_[i + 1];
    const Scalar x0_2 = x0 * x0;
    const Scalar x0_3 = x0_2 * x0;
    const Scalar x0_4 = x0_2 * x0_2;
    const Scalar x1_2 = x1 * x1;
    const Scalar x1_3 = x1_2 * x1;
    const Scalar x1_4 = x1_2 * x1_2;
    const Scalar y0 = yNorm_[i];
    const Scalar y1 = yNorm_[i + 1];
    m4Sum.add(dx * (y0 * (5.0 * x0_4 + 4.0 * x0_3 * x1 + 3.0 * x0_2 * x1_2 + 2.0 * x0 * x1_3 + x1_4)
                    + y1 * (x0_4 + 2.0 * x0_3 * x1 + 3.0 * x0_2 * x1_2 + 4.0 * x0 * x1_3 + 5.0 * x1_4)) / 30.0);
  }
  const Scalar mu = getMean()[0];
  const Scalar sigma2 = getCovariance()(0, 0);
  if (sigma2 == 0.0) return Point(1, 0.0);
  const Scalar sigma4 = sigma2 * sigma2;
  KahanSum m3Sum;
  for (UnsignedInteger i = 0; i < n - 1; ++i)
  {
    const Scalar dx = x_[i + 1] - x_[i];
    const Scalar x0 = x_[i];
    const Scalar x1 = x_[i + 1];
    const Scalar x0_2 = x0 * x0;
    const Scalar x0_3 = x0_2 * x0;
    const Scalar x1_2 = x1 * x1;
    const Scalar x1_3 = x1_2 * x1;
    const Scalar y0 = yNorm_[i];
    const Scalar y1 = yNorm_[i + 1];
    m3Sum.add(dx * (y0 * (4.0 * x0_3 + 3.0 * x0_2 * x1 + 2.0 * x0 * x1_2 + x1_3)
                    + y1 * (x0_3 + 2.0 * x0_2 * x1 + 3.0 * x0 * x1_2 + 4.0 * x1_3)) / 20.0);
  }
  const Scalar mu4 = m4Sum.value;
  const Scalar mu3 = m3Sum.value;
  return Point(1, (mu4 - 4.0 * mu * mu3 + 6.0 * mu * mu * sigma2 + 3.0 * mu * mu * mu * mu) / sigma4);
}


/* Parameters value accessors */
Point PiecewiseLinearDistribution::getParameter() const
{
  const UnsignedInteger n = x_.getSize();
  Point parameter(2 * n);
  std::copy(x_.begin(), x_.end(), parameter.begin());
  std::copy(y_.begin(), y_.end(), parameter.begin() + n);
  return parameter;
}

void PiecewiseLinearDistribution::setParameter(const Point & parameter)
{
  const UnsignedInteger n = x_.getSize();
  if (parameter.getSize() != 2 * n) throw InvalidArgumentException(HERE) << "Error: expected " << 2 * n << " values, got " << parameter.getSize();
  const Scalar w = getWeight();
  Point x(n);
  Point y(n);
  std::copy(parameter.begin(), parameter.begin() + n, x.begin());
  std::copy(parameter.begin() + n, parameter.end(), y.begin());
  *this = PiecewiseLinearDistribution(x, y);
  setWeight(w);
}


/* Parameters description accessor */
Description PiecewiseLinearDistribution::getParameterDescription() const
{
  const UnsignedInteger n = x_.getSize();
  Description description(Description::BuildDefault(n, "x"));
  description.add(Description::BuildDefault(n, "y"));
  return description;
}


/* X accessor */
void PiecewiseLinearDistribution::setX(const Point & x)
{
  if (x.getSize() != x_.getSize()) throw InvalidArgumentException(HERE) << "Error: new x must have the same size as the current x";
  for (UnsignedInteger i = 1; i < x.getSize(); ++i)
  {
    if (x[i] <= x[i - 1])
      throw InvalidArgumentException(HERE) << "Error: x must be strictly increasing";
  }
  if (x != x_)
  {
    x_ = x;
    update();
  }
}

Point PiecewiseLinearDistribution::getX() const
{
  return x_;
}


/* Y accessor */
void PiecewiseLinearDistribution::setY(const Point & y)
{
  if (y.getSize() != y_.getSize()) throw InvalidArgumentException(HERE) << "Error: new y must have the same size as the current y";
  for (UnsignedInteger i = 0; i < y.getSize(); ++i)
  {
    if (y[i] < 0.0)
      throw InvalidArgumentException(HERE) << "Error: y must be nonnegative";
  }
  if (*std::max_element(y.begin(), y.end()) <= 0.0)
    throw InvalidArgumentException(HERE) << "Error: at least one y value must be strictly positive";
  if (y != y_)
  {
    y_ = y;
    update();
  }
}

Point PiecewiseLinearDistribution::getY() const
{
  return y_;
}


/* Get the PDF singularities inside of the range - 1D only */
Point PiecewiseLinearDistribution::getSingularities() const
{
  const UnsignedInteger n = x_.getSize();
  if (n <= 2) return Point(0);
  Point singularities(n - 2);
  for (UnsignedInteger i = 1; i < n - 1; ++i)
    singularities[i - 1] = x_[i];
  return singularities;
}


/* Method save() stores the object through the StorageManager */
void PiecewiseLinearDistribution::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute("x_", x_);
  adv.saveAttribute("y_", y_);
}

/* Method load() reloads the object from the StorageManager */
void PiecewiseLinearDistribution::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute("x_", x_);
  adv.loadAttribute("y_", y_);
  if (x_.getSize() < 2)
    throw InvalidArgumentException(HERE) << "Error: x must have at least 2 elements, here x.getSize()=" << x_.getSize();
  if (y_.getSize() != x_.getSize())
    throw InvalidArgumentException(HERE) << "Error: y must have the same size as x, here x.getSize()=" << x_.getSize() << " and y.getSize()=" << y_.getSize();
  for (UnsignedInteger i = 1; i < x_.getSize(); ++i)
  {
    if (x_[i] <= x_[i - 1])
      throw InvalidArgumentException(HERE) << "Error: x must be strictly increasing, here x[" << i - 1 << "]=" << x_[i - 1] << " and x[" << i << "]=" << x_[i];
  }
  Scalar maxY = 0.0;
  for (UnsignedInteger i = 0; i < y_.getSize(); ++i)
  {
    if (y_[i] < 0.0)
      throw InvalidArgumentException(HERE) << "Error: y must be nonnegative, here y[" << i << "]=" << y_[i];
    if (y_[i] > maxY) maxY = y_[i];
  }
  if (maxY <= 0.0)
    throw InvalidArgumentException(HERE) << "Error: at least one y value must be strictly positive";
  yNorm_ = y_;
  probabilities_ = Point(x_.getSize() - 1);
  update();
}


END_NAMESPACE_OPENTURNS
