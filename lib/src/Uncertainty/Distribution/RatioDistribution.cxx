//                                               -*- C++ -*-
/**
 *  @brief The RatioDistribution distribution
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
#include <cstdlib>
#include <cmath>

#include "openturns/RatioDistribution.hxx"
#include "openturns/GaussKronrod.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/MethodBoundEvaluation.hxx"
#include "openturns/Uniform.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(RatioDistribution)

static const Factory<RatioDistribution> Factory_RatioDistribution;

/* Default constructor */
RatioDistribution::RatioDistribution()
  : ContinuousDistribution()
  , left_(Uniform(0.0, 1.0))
  , right_(Uniform(0.0, 1.0))
{
  setName("RatioDistribution");
  setDimension(1);
  // Adjust the truncation interval and the distribution range
  computeRange();
}

/* Parameters constructor to use when the two bounds are finite */
RatioDistribution::RatioDistribution(const Distribution & left,
                                     const Distribution & right)
  : ContinuousDistribution()
  , left_()
  , right_()
{
  setName("RatioDistribution");
  setLeft(left);
  setRight(right);
  computeRange();
}

/* Comparison operator */
Bool RatioDistribution::operator ==(const RatioDistribution & other) const
{
  if (this == &other) return true;
  return (left_ == other.getLeft()) && (right_ == other.getRight());
}

Bool RatioDistribution::equals(const DistributionImplementation & other) const
{
  const RatioDistribution* p_other = dynamic_cast<const RatioDistribution*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String RatioDistribution::__repr__() const
{
  OSS oss;
  oss << "class=" << RatioDistribution::GetClassName()
      << " name=" << getName()
      << " left=" << left_
      << " right=" << right_;
  return oss;
}

String RatioDistribution::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "(" << left_.__str__() << " * " << right_.__str__() << ")";
  return oss;
}

/* Virtual constructor */
RatioDistribution * RatioDistribution::clone() const
{
  return new RatioDistribution(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void RatioDistribution::computeRange()
{
  const Scalar a = left_.getRange().getLowerBound()[0];
  const Scalar b = left_.getRange().getUpperBound()[0];
  const Scalar c = right_.getRange().getLowerBound()[0];
  const Scalar d = right_.getRange().getUpperBound()[0];
  if ((c > 0.0) || (d < 0.0))
  {
    const Scalar aOverC = a / c;
    const Scalar aOverD = a / d;
    const Scalar bOverC = b / c;
    const Scalar bOverD = b / d;
    setRange(Interval(std::min(std::min(aOverC, aOverD), std::min(bOverC, bOverD)), std::max(std::max(aOverC, aOverD), std::max(bOverC, bOverD))));
  }
  const Scalar ac = a * c;
  const Scalar ad = a * d;
  const Scalar bc = b * c;
  const Scalar bd = b * d;
  setRange(Interval(std::min(std::min(ac, ad), std::min(bc, bd)), std::max(std::max(ac, ad), std::max(bc, bd))));
}

/* Get one realization of the distribution */
Point RatioDistribution::getRealization() const
{
  return Point(1, left_.getRealization()[0] * right_.getRealization()[0]);
}

/* Get the PDF of the distribution: PDF(x) = \int_R PDF_left(u) * PDF_right(x / u) * du / |u| */
Scalar RatioDistribution::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  const Scalar a = getRange().getLowerBound()[0];
  const Scalar b = getRange().getUpperBound()[0];
  if ((x < a) || (x > b)) return 0.0;
  const Scalar aLeft = left_.getRange().getLowerBound()[0];
  const Scalar bLeft = left_.getRange().getUpperBound()[0];
  const Scalar aRight = right_.getRange().getLowerBound()[0];
  const Scalar bRight = right_.getRange().getUpperBound()[0];
  // First, the case where the joint support of left and right is included in a unique quadrant
  if ((aLeft >= 0.0) && (aRight >= 0.0)) return computePDFQ1(x, aLeft, bLeft, aRight, bRight);
  if ((bLeft <= 0.0) && (aRight >= 0.0)) return computePDFQ2(x, aLeft, bLeft, aRight, bRight);
  if ((bLeft <= 0.0) && (bRight <= 0.0)) return computePDFQ3(x, aLeft, bLeft, aRight, bRight);
  if ((aLeft >= 0.0) && (bRight <= 0.0)) return computePDFQ4(x, aLeft, bLeft, aRight, bRight);
  // Second, the case where the support is in Q1 U Q2
  if (aRight > 0.0) return computePDFQ1(x, 0.0, bLeft, aRight, bRight) + computePDFQ2(x, aLeft, 0.0, aRight, bRight);
  // Third, the case where the support is in Q3 U Q4
  if (bRight < 0.0) return computePDFQ3(x, 0.0, bLeft, aRight, bRight) + computePDFQ4(x, aLeft, 0.0, aRight, bRight);
  // Fourth, the case where the support is in Q1 U Q4
  if (aLeft > 0.0) return computePDFQ1(x, aLeft, bLeft, 0.0, bRight) + computePDFQ4(x, aLeft, bLeft, aRight, 0.0);
  // Fifth, the case where the support is in Q2 U Q3
  if (bLeft < 0.0) return computePDFQ2(x, aLeft, bLeft, 0.0, bRight) + computePDFQ3(x, aLeft, bLeft, aRight, 0.0);
  // Sixth, the case where the support is in Q1 U Q2 U Q3 U Q4
  const Scalar q1 = computePDFQ1(x, 0.0, bLeft, 0.0, bRight);
  const Scalar q2 = computePDFQ2(x, aLeft, 0.0, 0.0, bRight);
  const Scalar q3 = computePDFQ3(x, aLeft, 0.0, aRight, 0.0);
  const Scalar q4 = computePDFQ4(x, 0.0, bLeft, aRight, 0.0);
  return q1 + q2 + q3 + q4;
}

/* Get the PDF of the distribution: PDF(x) = \int_Q1 PDF_left(u) * PDF_right(x / u) * du / |u| when both right and left are positive */
Scalar RatioDistribution::computePDFQ1(const Scalar x,
                                       const Scalar a,
                                       const Scalar b,
                                       const Scalar c,
                                       const Scalar d) const
{
  const Scalar ac = a * c;
  const Scalar ad = a * d;
  const Scalar bc = b * c;
  const Scalar bd = b * d;
  GaussKronrod algo;
  const PDFKernelWrapper pdfKernelWrapper(left_, right_, x);
  const Function pdfKernel(bindMethod<PDFKernelWrapper, Point, Point>(pdfKernelWrapper, &PDFKernelWrapper::eval, 1, 1));
  if (c == 0.0)
  {
    if ((x >= 0.0) && (x < ad)) return algo.integrate(pdfKernel, Interval(a, b))[0];
    if ((x >= ad) && (x <= bd)) return algo.integrate(pdfKernel, Interval(x / d, b))[0];
    return 0.0;
  }
  if (ad <= bc)
  {
    if ((x >= ac) && (x < ad)) return algo.integrate(pdfKernel, Interval(a, x / c))[0];
    if ((x >= ad) && (x < bc)) return algo.integrate(pdfKernel, Interval(x / d, x / c))[0];
    if ((x >= bc) && (x < bd)) return algo.integrate(pdfKernel, Interval(x / d, b))[0];
    return 0.0;
  }
  if ((x >= ac) && (x < bc)) return algo.integrate(pdfKernel, Interval(a, x / c))[0];
  if ((x >= bc) && (x < ad)) return algo.integrate(pdfKernel, Interval(a, b))[0];
  if ((x >= ad) && (x < bd)) return algo.integrate(pdfKernel, Interval(x / d, b))[0];
  return 0.0;
}

/* Get the PDF of the distribution: PDF(x) = \int_Q2 PDF_left(u) * PDF_right(x / u) * du / |u| when right is negative and left is positive */
Scalar RatioDistribution::computePDFQ2(const Scalar x,
                                       const Scalar a,
                                       const Scalar b,
                                       const Scalar c,
                                       const Scalar d) const
{
  const Scalar ac = a * c;
  const Scalar ad = a * d;
  const Scalar bc = b * c;
  const Scalar bd = b * d;
  GaussKronrod algo;
  const PDFKernelWrapper pdfKernelWrapper(left_, right_, x);
  const Function pdfKernel(bindMethod<PDFKernelWrapper, Point, Point>(pdfKernelWrapper, &PDFKernelWrapper::eval, 1, 1));
  if (c == 0.0)
  {
    if ((x >= ad) && (x < bd)) return algo.integrate(pdfKernel, Interval(a, x / d))[0];
    if ((x >= bd) && (x <= 0.0)) return algo.integrate(pdfKernel, Interval(a, b))[0];
    return 0.0;
  }
  if (ac <= bd)
  {
    if ((x >= ad) && (x < ac)) return algo.integrate(pdfKernel, Interval(a, x / d))[0];
    if ((x >= ac) && (x < bd)) return algo.integrate(pdfKernel, Interval(x / c, x / d))[0];
    if ((x >= bd) && (x < bc)) return algo.integrate(pdfKernel, Interval(x / c, b))[0];
    return 0.0;
  }
  if ((x >= ad) && (x < bd)) return algo.integrate(pdfKernel, Interval(a, x / d))[0];
  if ((x >= bd) && (x < ac)) return algo.integrate(pdfKernel, Interval(a, b))[0];
  if ((x >= ac) && (x < bc)) return algo.integrate(pdfKernel, Interval(x / c, b))[0];
  return 0.0;
}

/* Get the PDF of the distribution: PDF(x) = \int_Q3 PDF_left(u) * PDF_right(x / u) * du / |u| when both right and left are negative */
Scalar RatioDistribution::computePDFQ3(const Scalar x,
                                       const Scalar a,
                                       const Scalar b,
                                       const Scalar c,
                                       const Scalar d) const
{
  const Scalar ac = a * c;
  const Scalar ad = a * d;
  const Scalar bc = b * c;
  const Scalar bd = b * d;
  GaussKronrod algo;
  const PDFKernelWrapper pdfKernelWrapper(left_, right_, x);
  const Function pdfKernel(bindMethod<PDFKernelWrapper, Point, Point>(pdfKernelWrapper, &PDFKernelWrapper::eval, 1, 1));
  if (d == 0.0)
  {
    if ((x >= bc) && (x < ac)) return algo.integrate(pdfKernel, Interval(a, x / c))[0];
    if ((x >= 0.0) && (x < bc)) return algo.integrate(pdfKernel, Interval(a, b))[0];
    return 0.0;
  }
  if (ad <= bc)
  {
    if ((x >= ad) && (x < ac)) return algo.integrate(pdfKernel, Interval(a, x / c))[0];
    if ((x >= bc) && (x < ad)) return algo.integrate(pdfKernel, Interval(x / d, x / c))[0];
    if ((x >= bd) && (x < bc)) return algo.integrate(pdfKernel, Interval(x / d, b))[0];
    return 0.0;
  }
  if ((x >= bc) && (x < ac)) return algo.integrate(pdfKernel, Interval(a, x / c))[0];
  if ((x >= ad) && (x < bc)) return algo.integrate(pdfKernel, Interval(a, b))[0];
  if ((x >= bd) && (x < ad)) return algo.integrate(pdfKernel, Interval(x / d, b))[0];
  return 0.0;
}

/* Get the PDF of the distribution: PDF(x) = \int_Q4 PDF_left(u) * PDF_right(x / u) * du / |u| when right is positive and left is negative */
Scalar RatioDistribution::computePDFQ4(const Scalar x,
                                       const Scalar a,
                                       const Scalar b,
                                       const Scalar c,
                                       const Scalar d) const
{
  const Scalar ac = a * c;
  const Scalar ad = a * d;
  const Scalar bc = b * c;
  const Scalar bd = b * d;
  GaussKronrod algo;
  const PDFKernelWrapper pdfKernelWrapper(left_, right_, x);
  const Function pdfKernel(bindMethod<PDFKernelWrapper, Point, Point>(pdfKernelWrapper, &PDFKernelWrapper::eval, 1, 1));
  if (d == 0.0)
  {
    if ((x >= ac) && (x <= 0.0)) return algo.integrate(pdfKernel, Interval(a, b))[0];
    if ((x >= bc) && (x < ac)) return algo.integrate(pdfKernel, Interval(x / c, b))[0];
    return 0.0;
  }
  if (bd <= ac)
  {
    if ((x >= bc) && (x < ac)) return algo.integrate(pdfKernel, Interval(a, x / d))[0];
    if ((x >= ad) && (x < bc)) return algo.integrate(pdfKernel, Interval(x / c, x / d))[0];
    if ((x >= bd) && (x < ad)) return algo.integrate(pdfKernel, Interval(x / c, b))[0];
    return 0.0;
  }
  if ((x >= bd) && (x < ad)) return algo.integrate(pdfKernel, Interval(a, x / d))[0];
  if ((x >= ac) && (x < bd)) return algo.integrate(pdfKernel, Interval(a, b))[0];
  if ((x >= bc) && (x < ac)) return algo.integrate(pdfKernel, Interval(x / c, b))[0];
  return 0.0;
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
Complex RatioDistribution::computeCharacteristicFunction(const Scalar x) const
{
  const Scalar muLeft = left_.getMean()[0];
  const Scalar muRight = right_.getMean()[0];
  const Scalar varLeft = left_.getCovariance()(0, 0);
  const Scalar varRight = right_.getCovariance()(0, 0);
  if (x * x * (varLeft + muLeft * muLeft + varRight + muRight * muRight) < 2.0 * SpecFunc::ScalarEpsilon) return Complex(1.0, -x * muLeft * muRight);
  if (std::abs(x) > ResourceMap::GetAsScalar("RatioDistribution-LargeCharacteristicFunctionArgument")) return ContinuousDistribution::computeCharacteristicFunction(x);
  const Scalar aLeft = left_.getRange().getLowerBound()[0];
  const Scalar bLeft = left_.getRange().getUpperBound()[0];
  GaussKronrod algo;
  const CFKernelWrapper cfKernelWrapper(left_, right_, x);
  const Function cfKernel(bindMethod<CFKernelWrapper, Point, Point>(cfKernelWrapper, &CFKernelWrapper::eval, 1, 2));
  Scalar negativeError = 0.0;
  const Point negativePart(algo.integrate(cfKernel, Interval(aLeft, muLeft), negativeError));
  Scalar positiveError = 0.0;
  const Point positivePart(algo.integrate(cfKernel, Interval(muLeft, bLeft), positiveError));
  Complex value(negativePart[0] + positivePart[0], negativePart[1] + positivePart[1]);
  return value;
}

/* Compute the mean of the distribution */
void RatioDistribution::computeMean() const
{
  mean_ = Point(1, left_.getMean()[0] * right_.getMean()[0]);
  isAlreadyComputedMean_ = true;
}

/* Compute the covariance of the distribution */
void RatioDistribution::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  const Scalar meanLeft = left_.getMean()[0];
  const Scalar meanRight = right_.getMean()[0];
  const Scalar varLeft = left_.getCovariance()(0, 0);
  const Scalar varRight = right_.getCovariance()(0, 0);
  covariance_(0, 0) = meanLeft * meanLeft * varRight + meanRight * meanRight * varLeft + varLeft * varRight;
  isAlreadyComputedCovariance_ = true;
}

/* Parameters value accessor */
Point RatioDistribution::getParameter() const
{
  Point point(left_.getParameter());
  point.add(right_.getParameter());
  return point;
}

void RatioDistribution::setParameter(const Point & parameter)
{
  const UnsignedInteger leftSize = left_.getParameterDimension();
  const UnsignedInteger rightSize = right_.getParameterDimension();
  if (parameter.getSize() != leftSize + rightSize)
    throw InvalidArgumentException(HERE) << "Error: expected " << leftSize + rightSize << " values, got " << parameter.getSize();
  Point newLeftParameters(leftSize);
  Point newRightParameters(rightSize);
  std::copy(parameter.begin(), parameter.begin() + leftSize, newLeftParameters.begin());
  std::copy(parameter.begin() + leftSize, parameter.end(), newRightParameters.begin());
  Distribution newLeft(left_);
  Distribution newRight(right_);
  newLeft.setParameter(newLeftParameters);
  newRight.setParameter(newRightParameters);
  const Scalar w = getWeight();
  *this = RatioDistribution(newLeft, newRight);
  setWeight(w);
}

/* Parameters description accessor */
Description RatioDistribution::getParameterDescription() const
{
  Description description(left_.getParameterDescription());
  description.add(right_.getParameterDescription());
  return description;
}

/* Left accessor */
void RatioDistribution::setLeft(const Distribution & left)
{
  if (left.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can multiply only distribution with dimension=1, here dimension=" << left.getDimension();
  if (!left.isContinuous()) throw InvalidArgumentException(HERE) << "Error: can multiply only continuous distributions";
  left_ = left;
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  isAlreadyCreatedGeneratingFunction_ = false;
  isParallel_ = left_.getImplementation()->isParallel();
  computeRange();
}

Distribution RatioDistribution::getLeft() const
{
  return left_;
}

/* Right accessor */
void RatioDistribution::setRight(const Distribution & right)
{
  if (right.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can multiply only distribution with dimension=1, here dimension=" << right.getDimension();
  if (!right.isContinuous()) throw InvalidArgumentException(HERE) << "Error: can multiply only continuous distributions";
  right_ = right;
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  isAlreadyCreatedGeneratingFunction_ = false;
  isParallel_ = right_.getImplementation()->isParallel();
  computeRange();
}

Distribution RatioDistribution::getRight() const
{
  return right_;
}

Bool RatioDistribution::isContinuous() const
{
  return left_.isContinuous() && right_.isContinuous();
}

/* Tell if the distribution is integer valued */
Bool RatioDistribution::isDiscrete() const
{
  return left_.isDiscrete() && right_.isDiscrete();
}

/* Tell if the distribution is integer valued */
Bool RatioDistribution::isIntegral() const
{
  return left_.isIntegral() && right_.isIntegral();
}

/* Method save() stores the object through the StorageManager */
void RatioDistribution::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "left_", left_ );
  adv.saveAttribute( "right_", right_ );
}

/* Method load() reloads the object from the StorageManager */
void RatioDistribution::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "left_", left_ );
  adv.loadAttribute( "right_", right_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
