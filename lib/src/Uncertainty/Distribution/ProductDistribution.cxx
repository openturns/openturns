//                                               -*- C++ -*-
/**
 *  @brief The ProductDistribution distribution
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#include <cstdlib>
#include <cmath>

#include "ProductDistribution.hxx"
#include "GaussKronrod.hxx"
#include "PersistentObjectFactory.hxx"
#include "MethodBoundNumericalMathEvaluationImplementation.hxx"
#include "Uniform.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ProductDistribution);

static const Factory<ProductDistribution> RegisteredFactory;

/* Default constructor */
ProductDistribution::ProductDistribution()
  : ContinuousDistribution()
  , left_(Uniform(0.0, 1.0))
  , right_(Uniform(0.0, 1.0))
{
  setName("ProductDistribution");
  setDimension(1);
  // Adjust the truncation interval and the distribution range
  computeRange();
}

/* Parameters constructor to use when the two bounds are finite */
ProductDistribution::ProductDistribution(const Distribution & left,
    const Distribution & right)
  : ContinuousDistribution()
  , left_()
  , right_()
{
  setName("ProductDistribution");
  setLeft(left);
  setRight(right);
  computeRange();
}

/* Comparison operator */
Bool ProductDistribution::operator ==(const ProductDistribution & other) const
{
  if (this == &other) return true;
  return (left_ == other.getLeft()) && (right_ == other.getRight());
}

/* String converter */
String ProductDistribution::__repr__() const
{
  OSS oss;
  oss << "class=" << ProductDistribution::GetClassName()
      << " name=" << getName()
      << " left=" << left_
      << " right=" << right_;
  return oss;
}

String ProductDistribution::__str__(const String & offset) const
{
  OSS oss;
  oss << offset << getClassName() << "(" << left_.__str__() << " * " << right_.__str__() << ")";
  return oss;
}

/* Virtual constructor */
ProductDistribution * ProductDistribution::clone() const
{
  return new ProductDistribution(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void ProductDistribution::computeRange()
{
  const NumericalScalar a(left_.getRange().getLowerBound()[0]);
  const NumericalScalar b(left_.getRange().getUpperBound()[0]);
  const NumericalScalar c(right_.getRange().getLowerBound()[0]);
  const NumericalScalar d(right_.getRange().getUpperBound()[0]);
  const NumericalScalar ac(a * c);
  const NumericalScalar ad(a * d);
  const NumericalScalar bc(b * c);
  const NumericalScalar bd(b * d);
  setRange(Interval(std::min(std::min(ac, ad), std::min(bc, bd)), std::max(std::max(ac, ad), std::max(bc, bd))));
}

/* Get one realization of the distribution */
NumericalPoint ProductDistribution::getRealization() const
{
  return NumericalPoint(1, left_.getRealization()[0] * right_.getRealization()[0]);
}

/* Get the PDF of the distribution: PDF(x) = \int_R PDF_left(u) * PDF_right(x / u) * du / |u| */
NumericalScalar ProductDistribution::computePDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0]);
  const NumericalScalar a(getRange().getLowerBound()[0]);
  const NumericalScalar b(getRange().getUpperBound()[0]);
  if ((x < a) || (x > b)) return 0.0;
  const NumericalScalar aLeft(left_.getRange().getLowerBound()[0]);
  const NumericalScalar bLeft(left_.getRange().getUpperBound()[0]);
  const NumericalScalar aRight(right_.getRange().getLowerBound()[0]);
  const NumericalScalar bRight(right_.getRange().getUpperBound()[0]);
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
  const NumericalScalar q1(computePDFQ1(x, 0.0, bLeft, 0.0, bRight));
  const NumericalScalar q2(computePDFQ2(x, aLeft, 0.0, 0.0, bRight));
  const NumericalScalar q3(computePDFQ3(x, aLeft, 0.0, aRight, 0.0));
  const NumericalScalar q4(computePDFQ4(x, 0.0, bLeft, aRight, 0.0));
  return q1 + q2 + q3 + q4;
}

/* Get the PDF of the distribution: PDF(x) = \int_Q1 PDF_left(u) * PDF_right(x / u) * du / |u| when both right and left are positive */
NumericalScalar ProductDistribution::computePDFQ1(const NumericalScalar x,
    const NumericalScalar a,
    const NumericalScalar b,
    const NumericalScalar c,
    const NumericalScalar d) const
{
  const NumericalScalar ac(a * c);
  const NumericalScalar ad(a * d);
  const NumericalScalar bc(b * c);
  const NumericalScalar bd(b * d);
  GaussKronrod algo;
  const PDFKernelWrapper pdfKernelWrapper(left_, right_, x);
  const NumericalMathFunction pdfKernel(bindMethod<PDFKernelWrapper, NumericalPoint, NumericalPoint>(pdfKernelWrapper, &PDFKernelWrapper::eval, 1, 1));
  if (c == 0.0)
  {
    if ((x >= 0.0) && (x < ad)) return algo.integrate(pdfKernel, Interval(a, b), pdfEpsilon_)[0];
    if ((x >= ad) && (x <= bd)) return algo.integrate(pdfKernel, Interval(x / d, b), pdfEpsilon_)[0];
    return 0.0;
  }
  if (ad <= bc)
  {
    if ((x >= ac) && (x < ad)) return algo.integrate(pdfKernel, Interval(a, x / c), pdfEpsilon_)[0];
    if ((x >= ad) && (x < bc)) return algo.integrate(pdfKernel, Interval(x / d, x / c), pdfEpsilon_)[0];
    if ((x >= bc) && (x < bd)) return algo.integrate(pdfKernel, Interval(x / d, b), pdfEpsilon_)[0];
    return 0.0;
  }
  if ((x >= ac) && (x < bc)) return algo.integrate(pdfKernel, Interval(a, x / c), pdfEpsilon_)[0];
  if ((x >= bc) && (x < ad)) return algo.integrate(pdfKernel, Interval(a, b), pdfEpsilon_)[0];
  if ((x >= ad) && (x < bd)) return algo.integrate(pdfKernel, Interval(x / d, b), pdfEpsilon_)[0];
  return 0.0;
}

/* Get the PDF of the distribution: PDF(x) = \int_Q2 PDF_left(u) * PDF_right(x / u) * du / |u| when right is negative and left is positive */
NumericalScalar ProductDistribution::computePDFQ2(const NumericalScalar x,
    const NumericalScalar a,
    const NumericalScalar b,
    const NumericalScalar c,
    const NumericalScalar d) const
{
  const NumericalScalar ac(a * c);
  const NumericalScalar ad(a * d);
  const NumericalScalar bc(b * c);
  const NumericalScalar bd(b * d);
  GaussKronrod algo;
  const PDFKernelWrapper pdfKernelWrapper(left_, right_, x);
  const NumericalMathFunction pdfKernel(bindMethod<PDFKernelWrapper, NumericalPoint, NumericalPoint>(pdfKernelWrapper, &PDFKernelWrapper::eval, 1, 1));
  if (c == 0.0)
  {
    if ((x >= ad) && (x < bd)) return algo.integrate(pdfKernel, Interval(a, x / d), pdfEpsilon_)[0];
    if ((x >= bd) && (x <= 0.0)) return algo.integrate(pdfKernel, Interval(a, b), pdfEpsilon_)[0];
    return 0.0;
  }
  if (ac <= bd)
  {
    if ((x >= ad) && (x < ac)) return algo.integrate(pdfKernel, Interval(a, x / d), pdfEpsilon_)[0];
    if ((x >= ac) && (x < bd)) return algo.integrate(pdfKernel, Interval(x / c, x / d), pdfEpsilon_)[0];
    if ((x >= bd) && (x < bc)) return algo.integrate(pdfKernel, Interval(x / c, b), pdfEpsilon_)[0];
    return 0.0;
  }
  if ((x >= ad) && (x < bd)) return algo.integrate(pdfKernel, Interval(a, x / d), pdfEpsilon_)[0];
  if ((x >= bd) && (x < ac)) return algo.integrate(pdfKernel, Interval(a, b), pdfEpsilon_)[0];
  if ((x >= ac) && (x < bc)) return algo.integrate(pdfKernel, Interval(x / c, b), pdfEpsilon_)[0];
  return 0.0;
}

/* Get the PDF of the distribution: PDF(x) = \int_Q3 PDF_left(u) * PDF_right(x / u) * du / |u| when both right and left are negative */
NumericalScalar ProductDistribution::computePDFQ3(const NumericalScalar x,
    const NumericalScalar a,
    const NumericalScalar b,
    const NumericalScalar c,
    const NumericalScalar d) const
{
  const NumericalScalar ac(a * c);
  const NumericalScalar ad(a * d);
  const NumericalScalar bc(b * c);
  const NumericalScalar bd(b * d);
  GaussKronrod algo;
  const PDFKernelWrapper pdfKernelWrapper(left_, right_, x);
  const NumericalMathFunction pdfKernel(bindMethod<PDFKernelWrapper, NumericalPoint, NumericalPoint>(pdfKernelWrapper, &PDFKernelWrapper::eval, 1, 1));
  if (d == 0.0)
  {
    if ((x >= bc) && (x < ac)) return algo.integrate(pdfKernel, Interval(a, x / c), pdfEpsilon_)[0];
    if ((x >= 0.0) && (x < bc)) return algo.integrate(pdfKernel, Interval(a, b), pdfEpsilon_)[0];
    return 0.0;
  }
  if (ad <= bc)
  {
    if ((x >= ad) && (x < ac)) return algo.integrate(pdfKernel, Interval(a, x / c), pdfEpsilon_)[0];
    if ((x >= bc) && (x < ad)) return algo.integrate(pdfKernel, Interval(x / d, x / c), pdfEpsilon_)[0];
    if ((x >= bd) && (x < bc)) return algo.integrate(pdfKernel, Interval(x / d, b), pdfEpsilon_)[0];
    return 0.0;
  }
  if ((x >= bc) && (x < ac)) return algo.integrate(pdfKernel, Interval(a, x / c), pdfEpsilon_)[0];
  if ((x >= ad) && (x < bc)) return algo.integrate(pdfKernel, Interval(a, b), pdfEpsilon_)[0];
  if ((x >= bd) && (x < ad)) return algo.integrate(pdfKernel, Interval(x / d, b), pdfEpsilon_)[0];
  return 0.0;
}

/* Get the PDF of the distribution: PDF(x) = \int_Q4 PDF_left(u) * PDF_right(x / u) * du / |u| when right is positive and left is negative */
NumericalScalar ProductDistribution::computePDFQ4(const NumericalScalar x,
    const NumericalScalar a,
    const NumericalScalar b,
    const NumericalScalar c,
    const NumericalScalar d) const
{
  const NumericalScalar ac(a * c);
  const NumericalScalar ad(a * d);
  const NumericalScalar bc(b * c);
  const NumericalScalar bd(b * d);
  GaussKronrod algo;
  const PDFKernelWrapper pdfKernelWrapper(left_, right_, x);
  const NumericalMathFunction pdfKernel(bindMethod<PDFKernelWrapper, NumericalPoint, NumericalPoint>(pdfKernelWrapper, &PDFKernelWrapper::eval, 1, 1));
  if (d == 0.0)
  {
    if ((x >= ac) && (x <= 0.0)) return algo.integrate(pdfKernel, Interval(a, b), pdfEpsilon_)[0];
    if ((x >= bc) && (x < ac)) return algo.integrate(pdfKernel, Interval(x / c, b), pdfEpsilon_)[0];
    return 0.0;
  }
  if (bd <= ac)
  {
    if ((x >= bc) && (x < ac)) return algo.integrate(pdfKernel, Interval(a, x / d), pdfEpsilon_)[0];
    if ((x >= ad) && (x < bc)) return algo.integrate(pdfKernel, Interval(x / c, x / d), pdfEpsilon_)[0];
    if ((x >= bd) && (x < ad)) return algo.integrate(pdfKernel, Interval(x / c, b), pdfEpsilon_)[0];
    return 0.0;
  }
  if ((x >= bd) && (x < ad)) return algo.integrate(pdfKernel, Interval(a, x / d), pdfEpsilon_)[0];
  if ((x >= ac) && (x < bd)) return algo.integrate(pdfKernel, Interval(a, b), pdfEpsilon_)[0];
  if ((x >= bc) && (x < ac)) return algo.integrate(pdfKernel, Interval(x / c, b), pdfEpsilon_)[0];
  return 0.0;
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
NumericalComplex ProductDistribution::computeCharacteristicFunction(const NumericalScalar x) const
{
  const NumericalScalar muLeft(left_.getMean()[0]);
  const NumericalScalar muRight(right_.getMean()[0]);
  const NumericalScalar varLeft(left_.getCovariance()(0, 0));
  const NumericalScalar varRight(right_.getCovariance()(0, 0));
  if (x * x * (varLeft + muLeft * muLeft + varRight + muRight * muRight) < 2.0 * SpecFunc::NumericalScalarEpsilon) return NumericalComplex(1.0, -x * muLeft * muRight);
  if (std::abs(x) > ResourceMap::GetAsNumericalScalar("ProductDistribution-LargeCharacteristicFunctionArgument")) return ContinuousDistribution::computeCharacteristicFunction(x);
  NumericalComplex result(0.0);
  const NumericalScalar aLeft(left_.getRange().getLowerBound()[0]);
  const NumericalScalar bLeft(left_.getRange().getUpperBound()[0]);
  GaussKronrod algo;
  const CFKernelWrapper cfKernelWrapper(left_, right_, x);
  const NumericalMathFunction cfKernel(bindMethod<CFKernelWrapper, NumericalPoint, NumericalPoint>(cfKernelWrapper, &CFKernelWrapper::eval, 1, 2));
  NumericalScalar negativeError(0.0);
  const NumericalPoint negativePart(algo.integrate(cfKernel, Interval(aLeft, muLeft), negativeError));
  NumericalScalar positiveError(0.0);
  const NumericalPoint positivePart(algo.integrate(cfKernel, Interval(muLeft, bLeft), positiveError));
  NumericalComplex value(negativePart[0] + positivePart[0], negativePart[1] + positivePart[1]);
  return value;
}

/* Compute the mean of the distribution */
void ProductDistribution::computeMean() const
{
  mean_ = NumericalPoint(1, left_.getMean()[0] * right_.getMean()[0]);
  isAlreadyComputedMean_ = true;
}

/* Compute the covariance of the distribution */
void ProductDistribution::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  const NumericalScalar meanLeft(left_.getMean()[0]);
  const NumericalScalar meanRight(right_.getMean()[0]);
  const NumericalScalar varLeft(left_.getCovariance()(0, 0));
  const NumericalScalar varRight(right_.getCovariance()(0, 0));
  covariance_(0, 0) = meanLeft * meanLeft * varRight + meanRight * meanRight * varLeft + varLeft * varRight;
  isAlreadyComputedCovariance_ = true;
}


/* Parameters value accessor */
NumericalPoint ProductDistribution::getParameter() const
{
  NumericalPoint point(left_.getParameter());
  point.add(right_.getParameter());
  return point;
}

void ProductDistribution::setParameter(const NumericalPoint & parameter)
{
  const UnsignedInteger leftSize = left_.getParameterDimension();
  const UnsignedInteger rightSize = right_.getParameterDimension();
  if (parameter.getSize() != leftSize + rightSize)
    throw InvalidArgumentException(HERE) << "Error: expected "<< leftSize + rightSize << " values, got " << parameter.getSize();
  NumericalPoint newLeftParameters(leftSize);
  NumericalPoint newRightParameters(rightSize);
  std::copy(parameter.begin(), parameter.begin() + leftSize, newLeftParameters.begin());
  std::copy(parameter.begin() + leftSize, parameter.end(), newRightParameters.begin());
  Distribution newLeft(left_);
  Distribution newRight(right_);
  newLeft.setParameter(newLeftParameters);
  newRight.setParameter(newRightParameters);
  const NumericalScalar w = getWeight();
  *this = ProductDistribution(newLeft, newRight);
  setWeight(w);
}

/* Parameters description accessor */
Description ProductDistribution::getParameterDescription() const
{
  Description description(left_.getParameterDescription());
  description.add(right_.getParameterDescription());
  return description;
}

/* Left accessor */
void ProductDistribution::setLeft(const Distribution & left)
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

Distribution ProductDistribution::getLeft() const
{
  return left_;
}

/* Right accessor */
void ProductDistribution::setRight(const Distribution & right)
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

Distribution ProductDistribution::getRight() const
{
  return right_;
}

Bool ProductDistribution::isContinuous() const
{
  return left_.isContinuous() && right_.isContinuous();
}

/* Tell if the distribution is discrete */
Bool ProductDistribution::isDiscrete() const
{
  return left_.isDiscrete() && right_.isDiscrete();
}

/* Tell if the distribution is integer valued */
Bool ProductDistribution::isIntegral() const
{
  return left_.isIntegral() && right_.isIntegral();
}

/* Method save() stores the object through the StorageManager */
void ProductDistribution::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "left_", left_ );
  adv.saveAttribute( "right_", right_ );
}

/* Method load() reloads the object from the StorageManager */
void ProductDistribution::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "left_", left_ );
  adv.loadAttribute( "right_", right_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
