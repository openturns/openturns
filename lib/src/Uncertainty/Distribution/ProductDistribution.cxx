//                                               -*- C++ -*-
/**
 *  @brief The ProductDistribution distribution
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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

#include "openturns/ProductDistribution.hxx"
#include "openturns/GaussKronrod.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/MethodBoundEvaluation.hxx"
#include "openturns/Uniform.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ProductDistribution)

static const Factory<ProductDistribution> Factory_ProductDistribution;

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

Bool ProductDistribution::equals(const DistributionImplementation & other) const
{
  const ProductDistribution* p_other = dynamic_cast<const ProductDistribution*>(&other);
  return p_other && (*this == *p_other);
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

String ProductDistribution::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "(" << left_.__str__() << " * " << right_.__str__() << ")";
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
  const Scalar a = left_.getRange().getLowerBound()[0];
  const Scalar b = left_.getRange().getUpperBound()[0];
  const Scalar c = right_.getRange().getLowerBound()[0];
  const Scalar d = right_.getRange().getUpperBound()[0];
  const Scalar ac = a * c;
  const Scalar ad = a * d;
  const Scalar bc = b * c;
  const Scalar bd = b * d;
  setRange(Interval(std::min(std::min(ac, ad), std::min(bc, bd)), std::max(std::max(ac, ad), std::max(bc, bd))));
}

/* Get one realization of the distribution */
Point ProductDistribution::getRealization() const
{
  return Point(1, left_.getRealization()[0] * right_.getRealization()[0]);
}

/* Get the PDF of the distribution: PDF(x) = \int_R PDF_left(u) * PDF_right(x / u) * du / |u| */
Scalar ProductDistribution::computePDF(const Point & point) const
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
  if ((aLeft >= 0.0) && (aRight >= 0.0))
  {
    LOGDEBUG("In ProductDistribution::computePDF, Q1");
    const Scalar value = computePDFQ1(x, aLeft, bLeft, aRight, bRight);
    LOGDEBUG(OSS() << "pdf=" << value);
    return value;
  }
  if ((bLeft <= 0.0) && (aRight >= 0.0))
  {
    LOGDEBUG("In ProductDistribution::computePDF, Q2");
    const Scalar value = computePDFQ2(x, aLeft, bLeft, aRight, bRight);
    LOGDEBUG(OSS() << "pdf=" << value);
    return value;
  }
  if ((bLeft <= 0.0) && (bRight <= 0.0))
  {
    LOGDEBUG("In ProductDistribution::computePDF, Q3");
    const Scalar value = computePDFQ3(x, aLeft, bLeft, aRight, bRight);
    LOGDEBUG(OSS() << "pdf=" << value);
    return value;
  }
  if ((aLeft >= 0.0) && (bRight <= 0.0))
  {
    LOGDEBUG("In ProductDistribution::computePDF, Q3");
    const Scalar value = computePDFQ4(x, aLeft, bLeft, aRight, bRight);
    LOGDEBUG(OSS() << "pdf=" << value);
    return value;
  }
  // Second, the case where the support is in Q1 U Q2
  if (aRight > 0.0)
  {
    LOGDEBUG("In ProductDistribution::computePDF, Q1 U Q2");
    const Scalar q1 = computePDFQ1(x, 0.0, bLeft, aRight, bRight);
    const Scalar q2 = computePDFQ2(x, aLeft, 0.0, aRight, bRight);
    LOGDEBUG(OSS() << "value Q1=" << q1 << ", value Q2=" << q2 << ", pdf=" << q1 + q2);
    return q1 + q2;
  }
  // Third, the case where the support is in Q3 U Q4
  if (bRight <= 0.0)
  {
    LOGDEBUG("In ProductDistribution::computePDF, Q3 U Q4");
    const Scalar q3 = computePDFQ3(x, aLeft, 0.0, aRight, bRight);
    const Scalar q4 = computePDFQ4(x, 0.0, bLeft, aRight, bRight);
    LOGDEBUG(OSS() << "value Q3=" << q3 << ", value Q4=" << q4 << ", pdf=" << q3 + q4);
    return q3 + q4;
  }
  // Fourth, the case where the support is in Q1 U Q4
  if (aLeft >= 0.0)
  {
    LOGDEBUG("In ProductDistribution::computePDF, Q1 U Q4");
    const Scalar q1 = computePDFQ1(x, aLeft, bLeft, 0.0, bRight);
    const Scalar q4 = computePDFQ4(x, aLeft, bLeft, aRight, 0.0);
    LOGDEBUG(OSS() << "value Q1=" << q1 << ", value Q4=" << q4 << ", pdf=" << q1 + q4);
    return q1 + q4;
  }
  // Fifth, the case where the support is in Q2 U Q3
  if (bLeft <= 0.0)
  {
    LOGDEBUG("In ProductDistribution::computePDF, Q2 U Q3");
    const Scalar q2 = computePDFQ2(x, aLeft, bLeft, 0.0, bRight);
    const Scalar q3 = computePDFQ3(x, aLeft, bLeft, aRight, 0.0);
    LOGDEBUG(OSS() << "value Q2=" << q2 << ", value Q3=" << q3 << ", pdf=" << q2 + q3);
    return q2 + q3;
  }
  // Sixth, the case where the support is in Q1 U Q2 U Q3 U Q4
  LOGDEBUG("In ProductDistribution::computePDF, Q1 U Q2 U Q3 U Q4");
  const Scalar q1 = computePDFQ1(x, 0.0, bLeft, 0.0, bRight);
  const Scalar q2 = computePDFQ2(x, aLeft, 0.0, 0.0, bRight);
  const Scalar q3 = computePDFQ3(x, aLeft, 0.0, aRight, 0.0);
  const Scalar q4 = computePDFQ4(x, 0.0, bLeft, aRight, 0.0);
  LOGDEBUG(OSS() << "value Q1=" << q1 << "value Q2=" << q2 << ", value Q3=" << q3 << "value Q4=" << q4 << ", pdf=" << q1 + q2 + q3 + q4);
  return q1 + q2 + q3 + q4;
}

/* Get the PDF of the distribution: PDF(x) = \int_Q1 PDF_left(u) * PDF_right(x / u) * du / |u| when left >= 0, right >= 0 */
Scalar ProductDistribution::computePDFQ1(const Scalar x,
    const Scalar a,
    const Scalar b,
    const Scalar c,
    const Scalar d) const
{
  LOGDEBUG(OSS() << "In ProductDistribution::computePDFQ1, x=" << x << ", a=" << a << ", b=" << b << ", c=" << c << ", d=" << d);
  const Scalar ac = a * c;
  const Scalar ad = a * d;
  const Scalar bc = b * c;
  const Scalar bd = b * d;
  LOGDEBUG(OSS() << "ac=" << ac << ", ad=" << ad << ", bc=" << bc << ", bd=" << bd);
  // Here the support is included into [ac, bd]
  if ((x < ac) || (x >= bd)) return 0.0;
  GaussKronrod algo;
  const PDFKernelWrapper pdfKernelWrapper(left_, right_, x);
  const Function pdfKernel(bindMethod<PDFKernelWrapper, Point, Point>(pdfKernelWrapper, &PDFKernelWrapper::eval, 1, 1));
  if (c == 0.0)
  {
    LOGDEBUG("c == 0.0");
    if (x < ad)
    {
      LOGDEBUG(OSS() << x << " < " << ad);
      const Scalar value = algo.integrate(pdfKernel, Interval(a, b))[0];
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    LOGDEBUG(OSS() << ad << " <= " << x);
    const Scalar value = algo.integrate(pdfKernel, Interval(x / d, b))[0];
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if (ad <= bc)
  {
    LOGDEBUG("ad <= bc");
    if (x < ad)
    {
      LOGDEBUG(OSS() << x << " < " << ad);
      const Scalar value = algo.integrate(pdfKernel, Interval(a, x / c))[0];
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if (x < bc)
    {
      LOGDEBUG(OSS() << x << " < " << bc);
      const Scalar value = algo.integrate(pdfKernel, Interval(x / d, x / c))[0];
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    LOGDEBUG(OSS() << x << " < " << bd);
    const Scalar value = algo.integrate(pdfKernel, Interval(x / d, b))[0];
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  LOGDEBUG("ad > bc");
  if (x < bc)
  {
    LOGDEBUG(OSS() << x << " < " << bc);
    const Scalar value = algo.integrate(pdfKernel, Interval(a, x / c))[0];
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if (x < ad)
  {
    LOGDEBUG(OSS() << x << " < " << ad);
    const Scalar value = algo.integrate(pdfKernel, Interval(a, b))[0];
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  LOGDEBUG(OSS() << x << " < " << bd);
  const Scalar value = algo.integrate(pdfKernel, Interval(x / d, b))[0];
  LOGDEBUG(OSS() << "value=" << value);
  return value;
}

/* Get the PDF of the distribution: PDF(x) = \int_Q2 PDF_left(u) * PDF_right(x / u) * du / |u| when left <= 0, right >= 0 */
Scalar ProductDistribution::computePDFQ2(const Scalar x,
    const Scalar a,
    const Scalar b,
    const Scalar c,
    const Scalar d) const
{
  LOGDEBUG(OSS() << "In ProductDistribution::computePDFQ2, x=" << x << ", a=" << a << ", b=" << b << ", c=" << c << ", d=" << d);
  const Scalar ac = a * c;
  const Scalar ad = a * d;
  const Scalar bc = b * c;
  const Scalar bd = b * d;
  LOGDEBUG(OSS() << "ac=" << ac << ", ad=" << ad << ", bc=" << bc << ", bd=" << bd);
  // Here the support is included into [ad, bc]
  if ((x < ad) || (x >= bc)) return 0.0;
  GaussKronrod algo;
  const PDFKernelWrapper pdfKernelWrapper(left_, right_, x);
  const Function pdfKernel(bindMethod<PDFKernelWrapper, Point, Point>(pdfKernelWrapper, &PDFKernelWrapper::eval, 1, 1));
  if (c == 0.0)
  {
    LOGDEBUG("c == 0.0");
    if (x < bd)
    {
      LOGDEBUG(OSS() << x << " < " << bd);
      const Scalar value = algo.integrate(pdfKernel, Interval(a, x / d))[0];
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    LOGDEBUG(OSS() << x << " < " << 0.0);
    const Scalar value = algo.integrate(pdfKernel, Interval(a, b))[0];
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if (ac <= bd)
  {
    LOGDEBUG("ac <= bd");
    if (x < ac)
    {
      LOGDEBUG(OSS() << x << " < " << ac);
      const Scalar value = algo.integrate(pdfKernel, Interval(a, x / d))[0];
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if (x < bd)
    {
      LOGDEBUG(OSS() << x << " < " << bd);
      const Scalar value = algo.integrate(pdfKernel, Interval(x / c, x / d))[0];
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    LOGDEBUG(OSS() << x << " < " << bc);
    const Scalar value = algo.integrate(pdfKernel, Interval(x / c, b))[0];
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  LOGDEBUG("ac > bd");
  if (x < bd)
  {
    LOGDEBUG(OSS() << x << " < " << bd);
    const Scalar value = algo.integrate(pdfKernel, Interval(a, x / d))[0];
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if (x < ac)
  {
    LOGDEBUG(OSS() << x << " < " << ac);
    const Scalar value = algo.integrate(pdfKernel, Interval(a, b))[0];
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  LOGDEBUG(OSS() << x << " < " << bc);
  const Scalar value = algo.integrate(pdfKernel, Interval(x / c, b))[0];
  LOGDEBUG(OSS() << "value=" << value);
  return value;
}

/* Get the PDF of the distribution: PDF(x) = \int_Q3 PDF_left(u) * PDF_right(x / u) * du / |u| when left <= 0, right <= 0 */
Scalar ProductDistribution::computePDFQ3(const Scalar x,
    const Scalar a,
    const Scalar b,
    const Scalar c,
    const Scalar d) const
{
  LOGDEBUG(OSS() << "In ProductDistribution::computePDFQ3, x=" << x << ", a=" << a << ", b=" << b << ", c=" << c << ", d=" << d);
  const Scalar ac = a * c;
  const Scalar ad = a * d;
  const Scalar bc = b * c;
  const Scalar bd = b * d;
  LOGDEBUG(OSS() << "ac=" << ac << ", ad=" << ad << ", bc=" << bc << ", bd=" << bd);
  // Here the support is included into [bd, ac]
  if ((x < bd) || (x >= ac)) return 0.0;
  GaussKronrod algo;
  const PDFKernelWrapper pdfKernelWrapper(left_, right_, x);
  const Function pdfKernel(bindMethod<PDFKernelWrapper, Point, Point>(pdfKernelWrapper, &PDFKernelWrapper::eval, 1, 1));
  if (d == 0.0)
  {
    LOGDEBUG("d == 0.0");
    if (x < bc)
    {
      LOGDEBUG(OSS() << x << " < " << bc);
      const Scalar value = algo.integrate(pdfKernel, Interval(a, b))[0];
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    LOGDEBUG(OSS() << x << " < " << ac);
    const Scalar value = algo.integrate(pdfKernel, Interval(a, x / c))[0];
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if (ad <= bc)
  {
    LOGDEBUG("ad <= bc");
    if (x < ad)
    {
      LOGDEBUG(OSS() << x << " < " << ad);
      const Scalar value = algo.integrate(pdfKernel, Interval(x / d, b))[0];
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if (x < bc)
    {
      LOGDEBUG(OSS() << x << " < " << bc);
      const Scalar value = algo.integrate(pdfKernel, Interval(a, b))[0];
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    LOGDEBUG(OSS() << x << " < " << ac);
    const Scalar value = algo.integrate(pdfKernel, Interval(a, x / c))[0];
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  LOGDEBUG("ad > bc");
  if (x < bc)
  {
    LOGDEBUG(OSS() << x << " < " << bc);
    const Scalar value = algo.integrate(pdfKernel, Interval(x / d, b))[0];
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if (x < ad)
  {
    LOGDEBUG(OSS() << x << " < " << ad);
    const Scalar value = algo.integrate(pdfKernel, Interval(x / d, x / c))[0];
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  LOGDEBUG(OSS() << x << " < " << ac);
  const Scalar value = algo.integrate(pdfKernel, Interval(a, x / c))[0];
  LOGDEBUG(OSS() << "value=" << value);
  return value;
}

/* Get the PDF of the distribution: PDF(x) = \int_Q4 PDF_left(u) * PDF_right(x / u) * du / |u| when left >= 0, right <= 0 */
Scalar ProductDistribution::computePDFQ4(const Scalar x,
    const Scalar a,
    const Scalar b,
    const Scalar c,
    const Scalar d) const
{
  LOGDEBUG(OSS() << "In ProductDistribution::computePDFQ4, x=" << x << ", a=" << a << ", b=" << b << ", c=" << c << ", d=" << d);
  const Scalar ac = a * c;
  const Scalar ad = a * d;
  const Scalar bc = b * c;
  const Scalar bd = b * d;
  LOGDEBUG(OSS() << "ac=" << ac << ", ad=" << ad << ", bc=" << bc << ", bd=" << bd);
  // Here the support is included into [bc, ad]
  if ((x < bc) || (x >= ad)) return 0.0;
  GaussKronrod algo;
  const PDFKernelWrapper pdfKernelWrapper(left_, right_, x);
  const Function pdfKernel(bindMethod<PDFKernelWrapper, Point, Point>(pdfKernelWrapper, &PDFKernelWrapper::eval, 1, 1));
  if (d == 0.0)
  {
    LOGDEBUG("d == 0.0");
    if (x < ac)
    {
      LOGDEBUG(OSS() << x << " < " << ac);
      const Scalar value = algo.integrate(pdfKernel, Interval(x / c, b))[0];
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    LOGDEBUG(OSS() << x << " < " << 0.0);
    const Scalar value = algo.integrate(pdfKernel, Interval(a, b))[0];
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if (bd <= ac)
  {
    LOGDEBUG("bd <= ac");
    if (x < bd)
    {
      LOGDEBUG(OSS() << x << " < " << bd);
      const Scalar value = algo.integrate(pdfKernel, Interval(x / c, b))[0];
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if (x < ac)
    {
      LOGDEBUG(OSS() << x << " < " << ac);
      const Scalar value = algo.integrate(pdfKernel, Interval(x / c, x / d))[0];
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    LOGDEBUG(OSS() << x << " < " << ad);
    const Scalar value = algo.integrate(pdfKernel, Interval(a, x / d))[0];
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  LOGDEBUG("In ProductDistribution::computePDFQ4, bd > ac");
  LOGDEBUG("bd > ac");
  if (x < ac)
  {
    LOGDEBUG(OSS() << x << " < " << ac);
    const Scalar value = algo.integrate(pdfKernel, Interval(x / c, b))[0];
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if (x < bd)
  {
    LOGDEBUG(OSS() << x << " < " << bd);
    const Scalar value = algo.integrate(pdfKernel, Interval(a, b))[0];
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  LOGDEBUG(OSS() << bd << " <= " << x << " < " << ad);
  const Scalar value = algo.integrate(pdfKernel, Interval(a, x / d))[0];
  LOGDEBUG(OSS() << "value=" << value);
  return value;
}

/* Get the CDF of the distribution */
Scalar ProductDistribution::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  const Scalar a = getRange().getLowerBound()[0];
  const Scalar b = getRange().getUpperBound()[0];
  if (x <= a) return 0.0;
  if (x >= b) return 1.0;
  const Scalar aLeft = left_.getRange().getLowerBound()[0];
  const Scalar bLeft = left_.getRange().getUpperBound()[0];
  Scalar value = 0.0;
  // First, compute the negative part
  GaussKronrod algo;
  const CDFKernelWrapper cdfKernelWrapper(left_, right_, x);
  if (aLeft < 0)
  {
    const Function cdfKernel(bindMethod<CDFKernelWrapper, Point, Point>(cdfKernelWrapper, &CDFKernelWrapper::evalComplementary, 1, 1));
    value += algo.integrate(cdfKernel, Interval(aLeft, std::min(bLeft, 0.0)))[0];
  }
  if (bLeft >= 0)
  {
    const Function cdfKernel(bindMethod<CDFKernelWrapper, Point, Point>(cdfKernelWrapper, &CDFKernelWrapper::eval, 1, 1));
    value += algo.integrate(cdfKernel, Interval(std::max(0.0, aLeft), bLeft))[0];
  }
  return value;
}

/* Get the CDF of the distribution: CDF(x) = \int_Q1 PDF_left(u) * CDF_right(x / u) * du when left >= 0, right >= 0 */
Scalar ProductDistribution::computeCDFQ1(const Scalar x,
    const Scalar a,
    const Scalar b,
    const Scalar c,
    const Scalar d) const
{
  LOGDEBUG(OSS() << "In ProductDistribution::computeCDFQ1, x=" << x << ", a=" << a << ", b=" << b << ", c=" << c << ", d=" << d);
  const Scalar ac = a * c;
  const Scalar ad = a * d;
  const Scalar bc = b * c;
  const Scalar bd = b * d;
  LOGDEBUG(OSS() << "ac=" << ac << ", ad=" << ad << ", bc=" << bc << ", bd=" << bd);
  GaussKronrod algo;
  const CDFKernelWrapper cdfKernelWrapper(left_, right_, x);
  const Function cdfKernel(bindMethod<CDFKernelWrapper, Point, Point>(cdfKernelWrapper, &CDFKernelWrapper::eval, 1, 1));
  if (c == 0.0)
  {
    LOGDEBUG("c == 0.0");
    if ((x >= 0.0) && (x < ad))
    {
      LOGDEBUG(OSS() << 0.0 << " <= " << x << " < " << ad);
      const Scalar value = algo.integrate(cdfKernel, Interval(a, b))[0];
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if ((x >= ad) && (x < bd))
    {
      LOGDEBUG(OSS() << ad << " <= " << x << " < " << bd);
      const Scalar value = left_.computeCDF(x / d) + algo.integrate(cdfKernel, Interval(x / d, b))[0];
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    LOGDEBUG(OSS() << x << " not in [" << ac << ", " << 0.0 << "]");
    return 0.0;
  }
  if (ad <= bc)
  {
    LOGDEBUG("ad <= bc");
    if ((x >= ac) && (x < ad))
    {
      LOGDEBUG(OSS() << ac << " <= " << x << " < " << ad);
      const Scalar value = algo.integrate(cdfKernel, Interval(a, x / c))[0];
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if ((x >= ad) && (x < bc))
    {
      LOGDEBUG(OSS() << ad << " <= " << x << " < " << bc);
      const Scalar value = left_.computeCDF(x / d) + algo.integrate(cdfKernel, Interval(x / d, x / c))[0];
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if ((x >= bc) && (x < bd))
    {
      LOGDEBUG(OSS() << bc << " <= " << x << " < " << bd);
      const Scalar value = left_.computeCDF(x / d) + algo.integrate(cdfKernel, Interval(x / d, b))[0];
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    LOGDEBUG(OSS() << x << " not in [" << ac << ", " << bd << "]");
    return 0.0;
  }
  LOGDEBUG("ad > bc");
  if ((x >= ac) && (x < bc))
  {
    LOGDEBUG(OSS() << ac << " <= " << x << " < " << bc);
    const Scalar value = algo.integrate(cdfKernel, Interval(a, x / c))[0];
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if ((x >= bc) && (x < ad))
  {
    LOGDEBUG(OSS() << bc << " <= " << x << " < " << ad);
    const Scalar value = algo.integrate(cdfKernel, Interval(a, b))[0];
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if ((x >= ad) && (x < bd))
  {
    LOGDEBUG(OSS() << ad << " <= " << x << " < " << bd);
    const Scalar value = left_.computeCDF(x / d) + algo.integrate(cdfKernel, Interval(x / d, b))[0];
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  LOGDEBUG(OSS() << x << " not in [" << ac << ", " << bd << "]");
  return 0.0;
}

/* Get the CDF of the distribution: CDF(x) = 1 - \int_Q2 PDF_left(u) * CDF_right(x / u) * du when left <= 0, right >= 0 */
Scalar ProductDistribution::computeCDFQ2(const Scalar x,
    const Scalar a,
    const Scalar b,
    const Scalar c,
    const Scalar d) const
{
  LOGDEBUG(OSS() << "In ProductDistribution::computeCDFQ2, x=" << x << ", a=" << a << ", b=" << b << ", c=" << c << ", d=" << d);
  const Scalar ac = a * c;
  const Scalar ad = a * d;
  const Scalar bc = b * c;
  const Scalar bd = b * d;
  LOGDEBUG(OSS() << "ac=" << ac << ", ad=" << ad << ", bc=" << bc << ", bd=" << bd);
  GaussKronrod algo;
  const CDFKernelWrapper cdfKernelWrapper(left_, right_, x);
  const Function cdfKernel(bindMethod<CDFKernelWrapper, Point, Point>(cdfKernelWrapper, &CDFKernelWrapper::eval, 1, 1));
  if (c == 0.0)
  {
    LOGDEBUG("c == 0.0");
    if ((x >= ad) && (x < bd))
    {
      LOGDEBUG(OSS() << ad << " <= " << x << " < " << bd);
      const Scalar value = left_.computeCDF(x / d) - algo.integrate(cdfKernel, Interval(a, x / d))[0];
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if ((x >= bd) && (x < 0.0))
    {
      LOGDEBUG(OSS() << ad << " <= " << x << " < " << bd);
      const Scalar value = 1.0 - algo.integrate(cdfKernel, Interval(a, b))[0];
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    LOGDEBUG(OSS() << x << " not in [" << ad << ", " << 0.0 << "]");
    return 0.0;
  }
  if (ac <= bd)
  {
    LOGDEBUG("ac <= bd");
    if ((x >= ad) && (x < ac))
    {
      LOGDEBUG(OSS() << ad << " <= " << x << " < " << ac);
      const Scalar value = left_.computeCDF(x / d) - algo.integrate(cdfKernel, Interval(a, x / d))[0];
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if ((x >= ac) && (x < bd))
    {
      LOGDEBUG(OSS() << ac << " <= " << x << " < " << bd);
      const Scalar value = left_.computeCDF(x / d) - algo.integrate(cdfKernel, Interval(x / c, x / d))[0];
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if ((x >= bd) && (x < bc))
    {
      LOGDEBUG(OSS() << bd << " <= " << x << " < " << bc);
      const Scalar value = 1.0 - algo.integrate(cdfKernel, Interval(x / c, b))[0];
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    LOGDEBUG(OSS() << x << " not in [" << ad << ", " << bc << "]");
    return 0.0;
  }
  LOGDEBUG("ac > bd");
  if ((x >= ad) && (x < bd))
  {
    LOGDEBUG(OSS() << ad << " <= " << x << " < " << bd);
    const Scalar value = left_.computeCDF(x / d) - algo.integrate(cdfKernel, Interval(a, x / d))[0];
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if ((x >= bd) && (x < ac))
  {
    LOGDEBUG(OSS() << bd << " <= " << x << " < " << ac);
    const Scalar value = 1.0 - algo.integrate(cdfKernel, Interval(a, b))[0];
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if ((x >= ac) && (x < bc))
  {
    LOGDEBUG(OSS() << ac << " <= " << x << " < " << bc);
    const Scalar value = 1.0 - algo.integrate(cdfKernel, Interval(x / c, b))[0];
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  LOGDEBUG(OSS() << x << " not in [" << ad << ", " << bc << "]");
  return 0.0;
}

/* Get the CDF of the distribution: CDF(x) = 1 - \int_Q3 PDF_left(u) * CDF_right(x / u) * du when left <= 0, right <= 0 */
Scalar ProductDistribution::computeCDFQ3(const Scalar x,
    const Scalar a,
    const Scalar b,
    const Scalar c,
    const Scalar d) const
{
  LOGDEBUG(OSS() << "In ProductDistribution::computePDFQ3, x=" << x << ", a=" << a << ", b=" << b << ", c=" << c << ", d=" << d);
  const Scalar ac = a * c;
  const Scalar ad = a * d;
  const Scalar bc = b * c;
  const Scalar bd = b * d;
  LOGDEBUG(OSS() << "ac=" << ac << ", ad=" << ad << ", bc=" << bc << ", bd=" << bd);
  GaussKronrod algo;
  const CDFKernelWrapper cdfKernelWrapper(left_, right_, x);
  const Function cdfKernel(bindMethod<CDFKernelWrapper, Point, Point>(cdfKernelWrapper, &CDFKernelWrapper::eval, 1, 1));
  if (d == 0.0)
  {
    LOGDEBUG("d == 0.0");
    if ((x >= 0.0) && (x < bc))
    {
      LOGDEBUG(OSS() << 0.0 << " <= " << x << " < " << bc);
      const Scalar value = 1.0 - algo.integrate(cdfKernel, Interval(a, b))[0];
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if ((x >= bc) && (x < ac))
    {
      LOGDEBUG(OSS() << bc << " <= " << x << " < " << ac);
      const Scalar value = 1.0 - algo.integrate(cdfKernel, Interval(a, x / c))[0];
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    LOGDEBUG(OSS() << x << " not in [" << 0.0 << ", " << ac << "]");
    return 0.0;
  }
  if (ad <= bc)
  {
    LOGDEBUG("ad <= bc");
    if ((x >= bd) && (x < bc)) // BUG
    {
      LOGDEBUG(OSS() << bd << " <= " << x << " < " << ad);
      const Scalar value = left_.computeComplementaryCDF(x / d) - algo.integrate(cdfKernel, Interval(x / d, b))[0];
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if ((x >= bc) && (x < ad)) // BUG
    {
      LOGDEBUG(OSS() << ad << " <= " << x << " < " << bc);
      const Scalar value = left_.computeComplementaryCDF(x / d) - algo.integrate(cdfKernel, Interval(x / d, x / c))[0];
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if ((x >= ad) && (x < ac)) // BUG
    {
      LOGDEBUG(OSS() << bc << " <= " << x << " < " << ac);
      const Scalar value = 1.0 - algo.integrate(cdfKernel, Interval(a, x / c))[0];
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    LOGDEBUG(OSS() << x << " not in [" << bd << ", " << ac << "]");
    return 0.0;
  }
  LOGDEBUG("ad > bc");
  if ((x >= bd) && (x < ad)) // BUG
  {
    LOGDEBUG(OSS() << bd << " <= " << x << " < " << bc);
    const Scalar value = left_.computeComplementaryCDF(x / d) - algo.integrate(cdfKernel, Interval(x / d, b))[0];
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if ((x >= ad) && (x < bc)) // BUG
  {
    LOGDEBUG(OSS() << bc << " <= " << x << " < " << ad);
    const Scalar value = 1.0 - algo.integrate(cdfKernel, Interval(a, b))[0];
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if ((x >= bc) && (x < ac)) // BUG
  {
    LOGDEBUG(OSS() << ad << " <= " << x << " < " << ac);
    const Scalar value = 1.0 - algo.integrate(cdfKernel, Interval(a, x / c))[0];
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  LOGDEBUG(OSS() << x << " not in [" << bd << ", " << ac << "]");
  return 0.0;
}

/* Get the CDF of the distribution: CDF(x) = \int_Q4 PDF_left(u) * CDF_right(x / u) * du when left >= 0, right <= 0 */
Scalar ProductDistribution::computeCDFQ4(const Scalar x,
    const Scalar a,
    const Scalar b,
    const Scalar c,
    const Scalar d) const
{
  LOGDEBUG(OSS() << "In ProductDistribution::computeCDFQ4, x=" << x << ", a=" << a << ", b=" << b << ", c=" << c << ", d=" << d);
  const Scalar ac = a * c;
  const Scalar ad = a * d;
  const Scalar bc = b * c;
  const Scalar bd = b * d;
  LOGDEBUG(OSS() << "ac=" << ac << ", ad=" << ad << ", bc=" << bc << ", bd=" << bd);
  GaussKronrod algo;
  const CDFKernelWrapper cdfKernelWrapper(left_, right_, x);
  const Function cdfKernel(bindMethod<CDFKernelWrapper, Point, Point>(cdfKernelWrapper, &CDFKernelWrapper::eval, 1, 1));
  if (d == 0.0)
  {
    LOGDEBUG("d == 0.0");
    if ((x >= bc) && (x < 0.0))
    {
      LOGDEBUG(OSS() << ac << " <= " << x << " < " << 0.0);
      const Scalar value = algo.integrate(cdfKernel, Interval(x / c, b))[0];
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if ((x >= ac) && (x < 0.0))
    {
      LOGDEBUG(OSS() << bc << " <= " << x << " < " << ac);
      const Scalar value = algo.integrate(cdfKernel, Interval(a, b))[0];
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    LOGDEBUG(OSS() << x << " not in [" << bc << ", " << 0.0 << "]");
    return 0.0;
  }
  if (bd <= ac)
  {
    LOGDEBUG("bd <= ac");
    if ((x >= bc) && (x < bd))
    {
      LOGDEBUG(OSS() << bc << " <= " << x << " < " << bd);
      const Scalar value = algo.integrate(cdfKernel, Interval(x / c, b))[0];
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if ((x >= bd) && (x < ac))
    {
      LOGDEBUG(OSS() << bd << " <= " << x << " < " << ac);
      const Scalar value = left_.computeComplementaryCDF(x / d) + algo.integrate(cdfKernel, Interval(x / c, x / d))[0];
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if ((x >= ac) && (x < ad))
    {
      LOGDEBUG(OSS() << ac << " <= " << x << " < " << ad);
      const Scalar value = left_.computeComplementaryCDF(x / d) + algo.integrate(cdfKernel, Interval(a, x / d))[0];
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    LOGDEBUG(OSS() << x << " not in [" << bc << ", " << ad << "]");
    return 0.0;
  }
  LOGDEBUG("bd > ac");
  if ((x >= bc) && (x < ac))
  {
    LOGDEBUG(OSS() << bc << " <= " << x << " < " << ac);
    const Scalar value = algo.integrate(cdfKernel, Interval(x / c, b))[0];
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if ((x >= ac) && (x < bd))
  {
    LOGDEBUG(OSS() << ac << " <= " << x << " < " << bd);
    const Scalar value = algo.integrate(cdfKernel, Interval(a, b))[0];
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if ((x >= bd) && (x < ad))
  {
    LOGDEBUG(OSS() << bd << " <= " << x << " < " << ad);
    const Scalar value = left_.computeComplementaryCDF(x / d) + algo.integrate(cdfKernel, Interval(a, x / d))[0];
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  LOGDEBUG(OSS() << x << " not in [" << bc << ", " << ad << "]");
  return 0.0;
}

/* Compute the probability content of an interval */
Scalar ProductDistribution::computeProbability(const Interval & interval) const
{
  return computeProbabilityContinuous(interval);
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
Complex ProductDistribution::computeCharacteristicFunction(const Scalar x) const
{
  const Scalar muLeft = left_.getMean()[0];
  const Scalar muRight = right_.getMean()[0];
  const Scalar varLeft = left_.getCovariance()(0, 0);
  const Scalar varRight = right_.getCovariance()(0, 0);
  if (x * x * (varLeft + muLeft * muLeft + varRight + muRight * muRight) < 2.0 * SpecFunc::ScalarEpsilon) return Complex(1.0, -x * muLeft * muRight);
  if (std::abs(x) > ResourceMap::GetAsScalar("ProductDistribution-LargeCharacteristicFunctionArgument")) return ContinuousDistribution::computeCharacteristicFunction(x);
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
void ProductDistribution::computeMean() const
{
  mean_ = Point(1, left_.getMean()[0] * right_.getMean()[0]);
  isAlreadyComputedMean_ = true;
}

/* Compute the covariance of the distribution */
void ProductDistribution::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  const Scalar meanLeft = left_.getMean()[0];
  const Scalar meanRight = right_.getMean()[0];
  const Scalar varLeft = left_.getCovariance()(0, 0);
  const Scalar varRight = right_.getCovariance()(0, 0);
  covariance_(0, 0) = meanLeft * meanLeft * varRight + meanRight * meanRight * varLeft + varLeft * varRight;
  isAlreadyComputedCovariance_ = true;
}

/* Get the skewness of the distribution */
Point ProductDistribution::getSkewness() const
{
  const Scalar meanLeft = left_.getMean()[0];
  const Scalar meanRight = right_.getMean()[0];
  const Scalar varLeft = left_.getCovariance()(0, 0);
  const Scalar varRight = right_.getCovariance()(0, 0);
  const Scalar mu3Left = left_.getSkewness()[0] * std::pow(varLeft, 1.5);
  const Scalar mu3Right = right_.getSkewness()[0] * std::pow(varRight, 1.5);
  const Scalar variance = meanLeft * meanLeft * varRight + meanRight * meanRight * varLeft + varLeft * varRight;
  return Point(1, (mu3Left * mu3Right + mu3Left * std::pow(meanRight, 3.0) + mu3Right * std::pow(meanLeft, 3.0) + 3.0 * (mu3Left * varRight * meanRight + mu3Right * varLeft * meanLeft) + 6.0 * varLeft * varRight * meanLeft * meanRight) / std::pow(variance, 1.5));
}

/* Get the kurtosis of the distribution */
Point ProductDistribution::getKurtosis() const
{
  const Scalar meanLeft = left_.getMean()[0];
  const Scalar meanLeft2 = meanLeft * meanLeft;
  const Scalar meanLeft4 = meanLeft2 * meanLeft2;
  const Scalar meanRight = right_.getMean()[0];
  const Scalar meanRight2 = meanRight * meanRight;
  const Scalar meanRight4 = meanRight2 * meanRight2;
  const Scalar varLeft = left_.getCovariance()(0, 0);
  const Scalar varRight = right_.getCovariance()(0, 0);
  const Scalar mu3Left = left_.getSkewness()[0] * std::pow(varLeft, 1.5);
  const Scalar mu3Right = right_.getSkewness()[0] * std::pow(varRight, 1.5);
  const Scalar mu4Left = left_.getKurtosis()[0] * varLeft * varLeft;
  const Scalar mu4Right = right_.getKurtosis()[0] * varRight * varRight;
  const Scalar variance = meanLeft * meanLeft * varRight + meanRight * meanRight * varLeft + varLeft * varRight;
  return Point(1, (mu4Left * mu4Right + mu4Left * meanRight4 + mu4Right * meanLeft4 + 4.0 * (mu4Left * mu3Right * meanRight + mu4Right * mu3Left * meanLeft) + 6.0 * (varLeft * meanLeft2 * varRight * meanRight2 + mu4Left * varRight * meanRight2 + mu4Right * varLeft * meanLeft2) + 12.0 * (mu3Left * meanLeft * mu3Right * meanRight + mu3Left * meanLeft * varRight * meanRight2 + mu3Right * meanRight * varLeft * meanLeft2)) / (variance * variance));
}

/* Get the raw moments of the distribution */
Point ProductDistribution::getMoment(const UnsignedInteger n) const
{
  return Point(1, left_.getMoment(n)[0] * right_.getMoment(n)[0]);
}


/* Parameters value accessor */
Point ProductDistribution::getParameter() const
{
  Point point(left_.getParameter());
  point.add(right_.getParameter());
  return point;
}

void ProductDistribution::setParameter(const Point & parameter)
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

/* Check if the distribution is elliptical */
Bool ProductDistribution::isElliptical() const
{
  return (left_.isElliptical() && (std::abs(left_.getRange().getLowerBound()[0] + left_.getRange().getUpperBound()[0]) < ResourceMap::GetAsScalar("Distribution-DefaultQuantileEpsilon"))) || (right_.isElliptical() && (std::abs(right_.getRange().getLowerBound()[0] + right_.getRange().getUpperBound()[0]) < ResourceMap::GetAsScalar("Distribution-DefaultQuantileEpsilon")));
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

/* Get the PDF singularities inside of the range - 1D only */
Point ProductDistribution::getSingularities() const
{
  if (getRange().getLowerBound()[0] >= 0.0) return Point(0);
  if (getRange().getUpperBound()[0] <= 0.0) return Point(0);
  return Point(1);
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
