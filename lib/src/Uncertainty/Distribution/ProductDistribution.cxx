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

#include "openturns/ProductDistribution.hxx"
#include "openturns/GaussKronrod.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/MethodBoundNumericalMathEvaluationImplementation.hxx"
#include "openturns/Uniform.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ProductDistribution);

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
  if ((aLeft >= 0.0) && (aRight >= 0.0))
  {
    LOGDEBUG("In ProductDistribution::computePDF, Q1");
    const NumericalScalar value(computePDFQ1(x, aLeft, bLeft, aRight, bRight));
    LOGDEBUG(OSS() << "pdf=" << value);
    return value;
  }
  if ((bLeft <= 0.0) && (aRight >= 0.0))
  {
    LOGDEBUG("In ProductDistribution::computePDF, Q2");
    const NumericalScalar value(computePDFQ2(x, aLeft, bLeft, aRight, bRight));
    LOGDEBUG(OSS() << "pdf=" << value);
    return value;
  }
  if ((bLeft <= 0.0) && (bRight <= 0.0))
  {
    LOGDEBUG("In ProductDistribution::computePDF, Q3");
    const NumericalScalar value(computePDFQ3(x, aLeft, bLeft, aRight, bRight));
    LOGDEBUG(OSS() << "pdf=" << value);
    return value;
  }
  if ((aLeft >= 0.0) && (bRight <= 0.0))
  {
    LOGDEBUG("In ProductDistribution::computePDF, Q3");
    const NumericalScalar value(computePDFQ4(x, aLeft, bLeft, aRight, bRight));
    LOGDEBUG(OSS() << "pdf=" << value);
    return value;
  }
  // Second, the case where the support is in Q1 U Q2
  if (aRight > 0.0)
  {
    LOGDEBUG("In ProductDistribution::computePDF, Q1 U Q2");
    const NumericalScalar q1(computePDFQ1(x, 0.0, bLeft, aRight, bRight));
    const NumericalScalar q2(computePDFQ2(x, aLeft, 0.0, aRight, bRight));
    LOGDEBUG(OSS() << "value Q1=" << q1 << ", value Q2=" << q2 << ", pdf=" << q1 + q2);
    return q1 + q2;
  }
  // Third, the case where the support is in Q3 U Q4
  if (bRight <= 0.0)
  {
    LOGDEBUG("In ProductDistribution::computePDF, Q3 U Q4");
    const NumericalScalar q3(computePDFQ3(x, aLeft, 0.0, aRight, bRight));
    const NumericalScalar q4(computePDFQ4(x, 0.0, bLeft, aRight, bRight));
    LOGDEBUG(OSS() << "value Q3=" << q3 << ", value Q4=" << q4 << ", pdf=" << q3 + q4);
    return q3 + q4;
  }
  // Fourth, the case where the support is in Q1 U Q4
  if (aLeft >= 0.0)
  {
    LOGDEBUG("In ProductDistribution::computePDF, Q1 U Q4");
    const NumericalScalar q1(computePDFQ1(x, aLeft, bLeft, 0.0, bRight));
    const NumericalScalar q4(computePDFQ4(x, aLeft, bLeft, aRight, 0.0));
    LOGDEBUG(OSS() << "value Q1=" << q1 << ", value Q4=" << q4 << ", pdf=" << q1 + q4);
    return q1 + q4;
  }
  // Fifth, the case where the support is in Q2 U Q3
  if (bLeft <= 0.0)
  {
    LOGDEBUG("In ProductDistribution::computePDF, Q2 U Q3");
    const NumericalScalar q2(computePDFQ2(x, aLeft, bLeft, 0.0, bRight));
    const NumericalScalar q3(computePDFQ3(x, aLeft, bLeft, aRight, 0.0));
    LOGDEBUG(OSS() << "value Q2=" << q2 << ", value Q3=" << q3 << ", pdf=" << q2 + q3);
    return q2 + q3;
  }
  // Sixth, the case where the support is in Q1 U Q2 U Q3 U Q4
  LOGDEBUG("In ProductDistribution::computePDF, Q1 U Q2 U Q3 U Q4");
  const NumericalScalar q1(computePDFQ1(x, 0.0, bLeft, 0.0, bRight));
  const NumericalScalar q2(computePDFQ2(x, aLeft, 0.0, 0.0, bRight));
  const NumericalScalar q3(computePDFQ3(x, aLeft, 0.0, aRight, 0.0));
  const NumericalScalar q4(computePDFQ4(x, 0.0, bLeft, aRight, 0.0));
  LOGDEBUG(OSS() << "value Q1=" << q1 << "value Q2=" << q2 << ", value Q3=" << q3 << "value Q4=" << q4 << ", pdf=" << q1 + q2 + q3 + q4);
  return q1 + q2 + q3 + q4;
}

/* Get the PDF of the distribution: PDF(x) = \int_Q1 PDF_left(u) * PDF_right(x / u) * du / |u| when left >= 0, right >= 0 */
NumericalScalar ProductDistribution::computePDFQ1(const NumericalScalar x,
    const NumericalScalar a,
    const NumericalScalar b,
    const NumericalScalar c,
    const NumericalScalar d) const
{
  LOGDEBUG(OSS() << "In ProductDistribution::computePDFQ1, x=" << x << ", a=" << a << ", b=" << b << ", c=" << c << ", d=" << d);
  const NumericalScalar ac(a * c);
  const NumericalScalar ad(a * d);
  const NumericalScalar bc(b * c);
  const NumericalScalar bd(b * d);
  LOGDEBUG(OSS() << "ac=" << ac << ", ad=" << ad << ", bc=" << bc << ", bd=" << bd);
  // Here the support is included into [ac, bd]
  if ((x < ac) || (x >= bd)) return 0.0;
  GaussKronrod algo;
  const PDFKernelWrapper pdfKernelWrapper(left_, right_, x);
  const NumericalMathFunction pdfKernel(bindMethod<PDFKernelWrapper, NumericalPoint, NumericalPoint>(pdfKernelWrapper, &PDFKernelWrapper::eval, 1, 1));
  if (c == 0.0)
  {
    LOGDEBUG("c == 0.0");
    if (x < ad)
    {
      LOGDEBUG(OSS() << x << " < " << ad);
      const NumericalScalar value(algo.integrate(pdfKernel, Interval(a, b), pdfEpsilon_)[0]);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    LOGDEBUG(OSS() << ad << " <= " << x);
    const NumericalScalar value(algo.integrate(pdfKernel, Interval(x / d, b), pdfEpsilon_)[0]);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if (ad <= bc)
  {
    LOGDEBUG("ad <= bc");
    if (x < ad)
    {
      LOGDEBUG(OSS() << x << " < " << ad);
      const NumericalScalar value(algo.integrate(pdfKernel, Interval(a, x / c), pdfEpsilon_)[0]);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if (x < bc)
    {
      LOGDEBUG(OSS() << x << " < " << bc);
      const NumericalScalar value(algo.integrate(pdfKernel, Interval(x / d, x / c), pdfEpsilon_)[0]);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    LOGDEBUG(OSS() << x << " < " << bd);
    const NumericalScalar value(algo.integrate(pdfKernel, Interval(x / d, b), pdfEpsilon_)[0]);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  LOGDEBUG("ad > bc");
  if (x < bc)
  {
    LOGDEBUG(OSS() << x << " < " << bc);
    const NumericalScalar value(algo.integrate(pdfKernel, Interval(a, x / c), pdfEpsilon_)[0]);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if (x < ad)
  {
    LOGDEBUG(OSS() << x << " < " << ad);
    const NumericalScalar value(algo.integrate(pdfKernel, Interval(a, b), pdfEpsilon_)[0]);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  LOGDEBUG(OSS() << x << " < " << bd);
  const NumericalScalar value(algo.integrate(pdfKernel, Interval(x / d, b), pdfEpsilon_)[0]);
  LOGDEBUG(OSS() << "value=" << value);
  return value;
}

/* Get the PDF of the distribution: PDF(x) = \int_Q2 PDF_left(u) * PDF_right(x / u) * du / |u| when left <= 0, right >= 0 */
NumericalScalar ProductDistribution::computePDFQ2(const NumericalScalar x,
    const NumericalScalar a,
    const NumericalScalar b,
    const NumericalScalar c,
    const NumericalScalar d) const
{
  LOGDEBUG(OSS() << "In ProductDistribution::computePDFQ2, x=" << x << ", a=" << a << ", b=" << b << ", c=" << c << ", d=" << d);
  const NumericalScalar ac(a * c);
  const NumericalScalar ad(a * d);
  const NumericalScalar bc(b * c);
  const NumericalScalar bd(b * d);
  LOGDEBUG(OSS() << "ac=" << ac << ", ad=" << ad << ", bc=" << bc << ", bd=" << bd);
  // Here the support is included into [ad, bc]
  if ((x < ad) || (x >= bc)) return 0.0;
  GaussKronrod algo;
  const PDFKernelWrapper pdfKernelWrapper(left_, right_, x);
  const NumericalMathFunction pdfKernel(bindMethod<PDFKernelWrapper, NumericalPoint, NumericalPoint>(pdfKernelWrapper, &PDFKernelWrapper::eval, 1, 1));
  if (c == 0.0)
  {
    LOGDEBUG("c == 0.0");
    if (x < bd)
    {
      LOGDEBUG(OSS() << x << " < " << bd);
      const NumericalScalar value(algo.integrate(pdfKernel, Interval(a, x / d), pdfEpsilon_)[0]);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    LOGDEBUG(OSS() << x << " < " << 0.0);
    const NumericalScalar value(algo.integrate(pdfKernel, Interval(a, b), pdfEpsilon_)[0]);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if (ac <= bd)
  {
    LOGDEBUG("ac <= bd");
    if (x < ac)
    {
      LOGDEBUG(OSS() << x << " < " << ac);
      const NumericalScalar value(algo.integrate(pdfKernel, Interval(a, x / d), pdfEpsilon_)[0]);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if (x < bd)
    {
      LOGDEBUG(OSS() << x << " < " << bd);
      const NumericalScalar value(algo.integrate(pdfKernel, Interval(x / c, x / d), pdfEpsilon_)[0]);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    LOGDEBUG(OSS() << x << " < " << bc);
    const NumericalScalar value(algo.integrate(pdfKernel, Interval(x / c, b), pdfEpsilon_)[0]);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  LOGDEBUG("ac > bd");
  if (x < bd)
  {
    LOGDEBUG(OSS() << x << " < " << bd);
    const NumericalScalar value(algo.integrate(pdfKernel, Interval(a, x / d), pdfEpsilon_)[0]);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if (x < ac)
  {
    LOGDEBUG(OSS() << x << " < " << ac);
    const NumericalScalar value(algo.integrate(pdfKernel, Interval(a, b), pdfEpsilon_)[0]);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  LOGDEBUG(OSS() << x << " < " << bc);
  const NumericalScalar value(algo.integrate(pdfKernel, Interval(x / c, b), pdfEpsilon_)[0]);
  LOGDEBUG(OSS() << "value=" << value);
  return value;
}

/* Get the PDF of the distribution: PDF(x) = \int_Q3 PDF_left(u) * PDF_right(x / u) * du / |u| when left <= 0, right <= 0 */
NumericalScalar ProductDistribution::computePDFQ3(const NumericalScalar x,
    const NumericalScalar a,
    const NumericalScalar b,
    const NumericalScalar c,
    const NumericalScalar d) const
{
  LOGDEBUG(OSS() << "In ProductDistribution::computePDFQ3, x=" << x << ", a=" << a << ", b=" << b << ", c=" << c << ", d=" << d);
  const NumericalScalar ac(a * c);
  const NumericalScalar ad(a * d);
  const NumericalScalar bc(b * c);
  const NumericalScalar bd(b * d);
  LOGDEBUG(OSS() << "ac=" << ac << ", ad=" << ad << ", bc=" << bc << ", bd=" << bd);
  // Here the support is included into [bd, ac]
  if ((x < bd) || (x >= ac)) return 0.0;
  GaussKronrod algo;
  const PDFKernelWrapper pdfKernelWrapper(left_, right_, x);
  const NumericalMathFunction pdfKernel(bindMethod<PDFKernelWrapper, NumericalPoint, NumericalPoint>(pdfKernelWrapper, &PDFKernelWrapper::eval, 1, 1));
  if (d == 0.0)
  {
    LOGDEBUG("d == 0.0");
    if (x < bc)
    {
      LOGDEBUG(OSS() << x << " < " << bc);
      const NumericalScalar value(algo.integrate(pdfKernel, Interval(a, b), pdfEpsilon_)[0]);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    LOGDEBUG(OSS() << x << " < " << ac);
    const NumericalScalar value(algo.integrate(pdfKernel, Interval(a, x / c), pdfEpsilon_)[0]);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if (ad <= bc)
  {
    LOGDEBUG("ad <= bc");
    if (x < ad)
    {
      LOGDEBUG(OSS() << x << " < " << ad);
      const NumericalScalar value(algo.integrate(pdfKernel, Interval(x / d, b), pdfEpsilon_)[0]);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if (x < bc)
    {
      LOGDEBUG(OSS() << x << " < " << bc);
      const NumericalScalar value(algo.integrate(pdfKernel, Interval(a, b), pdfEpsilon_)[0]);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    LOGDEBUG(OSS() << x << " < " << ac);
    const NumericalScalar value(algo.integrate(pdfKernel, Interval(a, x / c), pdfEpsilon_)[0]);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  LOGDEBUG("ad > bc");
  if (x < bc)
  {
    LOGDEBUG(OSS() << x << " < " << bc);
    const NumericalScalar value(algo.integrate(pdfKernel, Interval(x / d, b), pdfEpsilon_)[0]);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if (x < ad)
  {
    LOGDEBUG(OSS() << x << " < " << ad);
    const NumericalScalar value(algo.integrate(pdfKernel, Interval(x / d, x / c), pdfEpsilon_)[0]);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  LOGDEBUG(OSS() << x << " < " << ac);
  const NumericalScalar value(algo.integrate(pdfKernel, Interval(a, x / c), pdfEpsilon_)[0]);
  LOGDEBUG(OSS() << "value=" << value);
  return value;
}

/* Get the PDF of the distribution: PDF(x) = \int_Q4 PDF_left(u) * PDF_right(x / u) * du / |u| when left >= 0, right <= 0 */
NumericalScalar ProductDistribution::computePDFQ4(const NumericalScalar x,
    const NumericalScalar a,
    const NumericalScalar b,
    const NumericalScalar c,
    const NumericalScalar d) const
{
  LOGDEBUG(OSS() << "In ProductDistribution::computePDFQ4, x=" << x << ", a=" << a << ", b=" << b << ", c=" << c << ", d=" << d);
  const NumericalScalar ac(a * c);
  const NumericalScalar ad(a * d);
  const NumericalScalar bc(b * c);
  const NumericalScalar bd(b * d);
  LOGDEBUG(OSS() << "ac=" << ac << ", ad=" << ad << ", bc=" << bc << ", bd=" << bd);
  // Here the support is included into [bc, ad]
  if ((x < bc) || (x >= ad)) return 0.0;
  GaussKronrod algo;
  const PDFKernelWrapper pdfKernelWrapper(left_, right_, x);
  const NumericalMathFunction pdfKernel(bindMethod<PDFKernelWrapper, NumericalPoint, NumericalPoint>(pdfKernelWrapper, &PDFKernelWrapper::eval, 1, 1));
  if (d == 0.0)
  {
    LOGDEBUG("d == 0.0");
    if (x < ac)
    {
      LOGDEBUG(OSS() << x << " < " << ac);
      const NumericalScalar value(algo.integrate(pdfKernel, Interval(x / c, b), pdfEpsilon_)[0]);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    LOGDEBUG(OSS() << x << " < " << 0.0);
    const NumericalScalar value(algo.integrate(pdfKernel, Interval(a, b), pdfEpsilon_)[0]);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if (bd <= ac)
  {
    LOGDEBUG("bd <= ac");
    if (x < bd)
    {
      LOGDEBUG(OSS() << x << " < " << bd);
      const NumericalScalar value(algo.integrate(pdfKernel, Interval(x / c, b), pdfEpsilon_)[0]);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if (x < ac)
    {
      LOGDEBUG(OSS() << x << " < " << ac);
      const NumericalScalar value(algo.integrate(pdfKernel, Interval(x / c, x / d), pdfEpsilon_)[0]);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    LOGDEBUG(OSS() << x << " < " << ad);
    const NumericalScalar value(algo.integrate(pdfKernel, Interval(a, x / d), pdfEpsilon_)[0]);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  LOGDEBUG("In ProductDistribution::computePDFQ4, bd > ac");
  LOGDEBUG("bd > ac");
  if (x < ac)
  {
    LOGDEBUG(OSS() << x << " < " << ac);
    const NumericalScalar value(algo.integrate(pdfKernel, Interval(x / c, b), pdfEpsilon_)[0]);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if (x < bd)
  {
    LOGDEBUG(OSS() << x << " < " << bd);
    const NumericalScalar value(algo.integrate(pdfKernel, Interval(a, b), pdfEpsilon_)[0]);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  LOGDEBUG(OSS() << bd << " <= " << x << " < " << ad);
  const NumericalScalar value(algo.integrate(pdfKernel, Interval(a, x / d), pdfEpsilon_)[0]);
  LOGDEBUG(OSS() << "value=" << value);
  return value;
}

/* Get the CDF of the distribution */
NumericalScalar ProductDistribution::computeCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0]);
  const NumericalScalar a(getRange().getLowerBound()[0]);
  const NumericalScalar b(getRange().getUpperBound()[0]);
  if (x <= a) return 0.0;
  if (x >= b) return 1.0;
  const NumericalScalar aLeft(left_.getRange().getLowerBound()[0]);
  const NumericalScalar bLeft(left_.getRange().getUpperBound()[0]);
  NumericalScalar value(0.0);
  // First, compute the negative part
  GaussKronrod algo;
  const CDFKernelWrapper cdfKernelWrapper(left_, right_, x);
  if (aLeft < 0)
  {
      const NumericalMathFunction cdfKernel(bindMethod<CDFKernelWrapper, NumericalPoint, NumericalPoint>(cdfKernelWrapper, &CDFKernelWrapper::evalComplementary, 1, 1));
      value += algo.integrate(cdfKernel, Interval(aLeft, std::min(bLeft, 0.0)), cdfEpsilon_)[0];
  }
  if (bLeft >= 0)
  {
      const NumericalMathFunction cdfKernel(bindMethod<CDFKernelWrapper, NumericalPoint, NumericalPoint>(cdfKernelWrapper, &CDFKernelWrapper::eval, 1, 1));
      value += algo.integrate(cdfKernel, Interval(std::max(0.0, aLeft), bLeft), cdfEpsilon_)[0];
  }
    return value;
  }

/* Get the CDF of the distribution: CDF(x) = \int_Q1 PDF_left(u) * CDF_right(x / u) * du when left >= 0, right >= 0 */
NumericalScalar ProductDistribution::computeCDFQ1(const NumericalScalar x,
    const NumericalScalar a,
    const NumericalScalar b,
    const NumericalScalar c,
    const NumericalScalar d) const
{
  LOGDEBUG(OSS() << "In ProductDistribution::computeCDFQ1, x=" << x << ", a=" << a << ", b=" << b << ", c=" << c << ", d=" << d);
  const NumericalScalar ac(a * c);
  const NumericalScalar ad(a * d);
  const NumericalScalar bc(b * c);
  const NumericalScalar bd(b * d);
  LOGDEBUG(OSS() << "ac=" << ac << ", ad=" << ad << ", bc=" << bc << ", bd=" << bd);
  GaussKronrod algo;
  const CDFKernelWrapper cdfKernelWrapper(left_, right_, x);
  const NumericalMathFunction cdfKernel(bindMethod<CDFKernelWrapper, NumericalPoint, NumericalPoint>(cdfKernelWrapper, &CDFKernelWrapper::eval, 1, 1));
  if (c == 0.0)
  {
    LOGDEBUG("c == 0.0");
    if ((x >= 0.0) && (x < ad))
    {
      LOGDEBUG(OSS() << 0.0 << " <= " << x << " < " << ad);
      const NumericalScalar value(algo.integrate(cdfKernel, Interval(a, b), cdfEpsilon_)[0]);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if ((x >= ad) && (x < bd))
    {
      LOGDEBUG(OSS() << ad << " <= " << x << " < " << bd);
      const NumericalScalar value(left_.computeCDF(x / d) + algo.integrate(cdfKernel, Interval(x / d, b), cdfEpsilon_)[0]);
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
      const NumericalScalar value(algo.integrate(cdfKernel, Interval(a, x / c), cdfEpsilon_)[0]);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if ((x >= ad) && (x < bc))
    {
      LOGDEBUG(OSS() << ad << " <= " << x << " < " << bc);
      const NumericalScalar value(left_.computeCDF(x / d) + algo.integrate(cdfKernel, Interval(x / d, x / c), cdfEpsilon_)[0]);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if ((x >= bc) && (x < bd))
    {
      LOGDEBUG(OSS() << bc << " <= " << x << " < " << bd);
      const NumericalScalar value(left_.computeCDF(x / d) + algo.integrate(cdfKernel, Interval(x / d, b), cdfEpsilon_)[0]);
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
    const NumericalScalar value(algo.integrate(cdfKernel, Interval(a, x / c), cdfEpsilon_)[0]);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if ((x >= bc) && (x < ad))
  {
    LOGDEBUG(OSS() << bc << " <= " << x << " < " << ad);
    const NumericalScalar value(algo.integrate(cdfKernel, Interval(a, b), cdfEpsilon_)[0]);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if ((x >= ad) && (x < bd))
  {
    LOGDEBUG(OSS() << ad << " <= " << x << " < " << bd);
    const NumericalScalar value(left_.computeCDF(x / d) + algo.integrate(cdfKernel, Interval(x / d, b), cdfEpsilon_)[0]);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  LOGDEBUG(OSS() << x << " not in [" << ac << ", " << bd << "]");
  return 0.0;
}

/* Get the CDF of the distribution: CDF(x) = 1 - \int_Q2 PDF_left(u) * CDF_right(x / u) * du when left <= 0, right >= 0 */
NumericalScalar ProductDistribution::computeCDFQ2(const NumericalScalar x,
    const NumericalScalar a,
    const NumericalScalar b,
    const NumericalScalar c,
    const NumericalScalar d) const
{
  LOGDEBUG(OSS() << "In ProductDistribution::computeCDFQ2, x=" << x << ", a=" << a << ", b=" << b << ", c=" << c << ", d=" << d);
  const NumericalScalar ac(a * c);
  const NumericalScalar ad(a * d);
  const NumericalScalar bc(b * c);
  const NumericalScalar bd(b * d);
  LOGDEBUG(OSS() << "ac=" << ac << ", ad=" << ad << ", bc=" << bc << ", bd=" << bd);
  GaussKronrod algo;
  const CDFKernelWrapper cdfKernelWrapper(left_, right_, x);
  const NumericalMathFunction cdfKernel(bindMethod<CDFKernelWrapper, NumericalPoint, NumericalPoint>(cdfKernelWrapper, &CDFKernelWrapper::eval, 1, 1));
  if (c == 0.0)
  {
    LOGDEBUG("c == 0.0");
    if ((x >= ad) && (x < bd))
    {
      LOGDEBUG(OSS() << ad << " <= " << x << " < " << bd);
      const NumericalScalar value(left_.computeCDF(x / d) - algo.integrate(cdfKernel, Interval(a, x / d), cdfEpsilon_)[0]);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if ((x >= bd) && (x < 0.0))
    {
      LOGDEBUG(OSS() << ad << " <= " << x << " < " << bd);
      const NumericalScalar value(1.0 - algo.integrate(cdfKernel, Interval(a, b), cdfEpsilon_)[0]);
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
      const NumericalScalar value(left_.computeCDF(x / d) - algo.integrate(cdfKernel, Interval(a, x / d), cdfEpsilon_)[0]);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if ((x >= ac) && (x < bd))
    {
      LOGDEBUG(OSS() << ac << " <= " << x << " < " << bd);
      const NumericalScalar value(left_.computeCDF(x / d) - algo.integrate(cdfKernel, Interval(x / c, x / d), cdfEpsilon_)[0]);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if ((x >= bd) && (x < bc))
    {
      LOGDEBUG(OSS() << bd << " <= " << x << " < " << bc);
      const NumericalScalar value(1.0 - algo.integrate(cdfKernel, Interval(x / c, b), cdfEpsilon_)[0]);
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
    const NumericalScalar value(left_.computeCDF(x / d) - algo.integrate(cdfKernel, Interval(a, x / d), cdfEpsilon_)[0]);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if ((x >= bd) && (x < ac))
  {
    LOGDEBUG(OSS() << bd << " <= " << x << " < " << ac);
    const NumericalScalar value(1.0 - algo.integrate(cdfKernel, Interval(a, b), cdfEpsilon_)[0]);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if ((x >= ac) && (x < bc))
  {
    LOGDEBUG(OSS() << ac << " <= " << x << " < " << bc);
    const NumericalScalar value(1.0 - algo.integrate(cdfKernel, Interval(x / c, b), cdfEpsilon_)[0]);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  LOGDEBUG(OSS() << x << " not in [" << ad << ", " << bc << "]");
  return 0.0;
}

/* Get the CDF of the distribution: CDF(x) = 1 - \int_Q3 PDF_left(u) * CDF_right(x / u) * du when left <= 0, right <= 0 */
NumericalScalar ProductDistribution::computeCDFQ3(const NumericalScalar x,
    const NumericalScalar a,
    const NumericalScalar b,
    const NumericalScalar c,
    const NumericalScalar d) const
{
  LOGDEBUG(OSS() << "In ProductDistribution::computePDFQ3, x=" << x << ", a=" << a << ", b=" << b << ", c=" << c << ", d=" << d);
  const NumericalScalar ac(a * c);
  const NumericalScalar ad(a * d);
  const NumericalScalar bc(b * c);
  const NumericalScalar bd(b * d);
  LOGDEBUG(OSS() << "ac=" << ac << ", ad=" << ad << ", bc=" << bc << ", bd=" << bd);
  GaussKronrod algo;
  const CDFKernelWrapper cdfKernelWrapper(left_, right_, x);
  const NumericalMathFunction cdfKernel(bindMethod<CDFKernelWrapper, NumericalPoint, NumericalPoint>(cdfKernelWrapper, &CDFKernelWrapper::eval, 1, 1));
  if (d == 0.0)
  {
    LOGDEBUG("d == 0.0");
    if ((x >= 0.0) && (x < bc))
    {
      LOGDEBUG(OSS() << 0.0 << " <= " << x << " < " << bc);
      const NumericalScalar value(1.0 - algo.integrate(cdfKernel, Interval(a, b), cdfEpsilon_)[0]);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if ((x >= bc) && (x < ac))
    {
      LOGDEBUG(OSS() << bc << " <= " << x << " < " << ac);
      const NumericalScalar value(1.0 - algo.integrate(cdfKernel, Interval(a, x / c), cdfEpsilon_)[0]);
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
      const NumericalScalar value(left_.computeComplementaryCDF(x / d) - algo.integrate(cdfKernel, Interval(x / d, b), cdfEpsilon_)[0]);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if ((x >= bc) && (x < ad)) // BUG
    {
      LOGDEBUG(OSS() << ad << " <= " << x << " < " << bc);
      const NumericalScalar value(left_.computeComplementaryCDF(x / d) - algo.integrate(cdfKernel, Interval(x / d, x / c), cdfEpsilon_)[0]);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if ((x >= ad) && (x < ac)) // BUG
    {
      LOGDEBUG(OSS() << bc << " <= " << x << " < " << ac);
      const NumericalScalar value(1.0 - algo.integrate(cdfKernel, Interval(a, x / c), cdfEpsilon_)[0]);
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
    const NumericalScalar value(left_.computeComplementaryCDF(x / d) - algo.integrate(cdfKernel, Interval(x / d, b), cdfEpsilon_)[0]);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if ((x >= ad) && (x < bc)) // BUG
  {
    LOGDEBUG(OSS() << bc << " <= " << x << " < " << ad);
    const NumericalScalar value(1.0 - algo.integrate(cdfKernel, Interval(a, b), cdfEpsilon_)[0]);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if ((x >= bc) && (x < ac)) // BUG
  {
    LOGDEBUG(OSS() << ad << " <= " << x << " < " << ac);
    const NumericalScalar value(1.0 - algo.integrate(cdfKernel, Interval(a, x / c), cdfEpsilon_)[0]);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  LOGDEBUG(OSS() << x << " not in [" << bd << ", " << ac << "]");
  return 0.0;
}

/* Get the CDF of the distribution: CDF(x) = \int_Q4 PDF_left(u) * CDF_right(x / u) * du when left >= 0, right <= 0 */
NumericalScalar ProductDistribution::computeCDFQ4(const NumericalScalar x,
    const NumericalScalar a,
    const NumericalScalar b,
    const NumericalScalar c,
    const NumericalScalar d) const
{
  LOGDEBUG(OSS() << "In ProductDistribution::computeCDFQ4, x=" << x << ", a=" << a << ", b=" << b << ", c=" << c << ", d=" << d);
  const NumericalScalar ac(a * c);
  const NumericalScalar ad(a * d);
  const NumericalScalar bc(b * c);
  const NumericalScalar bd(b * d);
  LOGDEBUG(OSS() << "ac=" << ac << ", ad=" << ad << ", bc=" << bc << ", bd=" << bd);
  GaussKronrod algo;
  const CDFKernelWrapper cdfKernelWrapper(left_, right_, x);
  const NumericalMathFunction cdfKernel(bindMethod<CDFKernelWrapper, NumericalPoint, NumericalPoint>(cdfKernelWrapper, &CDFKernelWrapper::eval, 1, 1));
  if (d == 0.0)
  {
    LOGDEBUG("d == 0.0");
    if ((x >= bc) && (x < 0.0))
    {
      LOGDEBUG(OSS() << ac << " <= " << x << " < " << 0.0);
      const NumericalScalar value(algo.integrate(cdfKernel, Interval(x / c, b), cdfEpsilon_)[0]);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if ((x >= ac) && (x < 0.0))
    {
      LOGDEBUG(OSS() << bc << " <= " << x << " < " << ac);
      const NumericalScalar value(algo.integrate(cdfKernel, Interval(a, b), cdfEpsilon_)[0]);
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
      const NumericalScalar value(algo.integrate(cdfKernel, Interval(x / c, b), cdfEpsilon_)[0]);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if ((x >= bd) && (x < ac))
    {
      LOGDEBUG(OSS() << bd << " <= " << x << " < " << ac);
      const NumericalScalar value(left_.computeComplementaryCDF(x / d) + algo.integrate(cdfKernel, Interval(x / c, x / d), cdfEpsilon_)[0]);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if ((x >= ac) && (x < ad))
    {
      LOGDEBUG(OSS() << ac << " <= " << x << " < " << ad);
      const NumericalScalar value(left_.computeComplementaryCDF(x / d) + algo.integrate(cdfKernel, Interval(a, x / d), cdfEpsilon_)[0]);
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
    const NumericalScalar value(algo.integrate(cdfKernel, Interval(x / c, b), cdfEpsilon_)[0]);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if ((x >= ac) && (x < bd))
  {
    LOGDEBUG(OSS() << ac << " <= " << x << " < " << bd);
    const NumericalScalar value(algo.integrate(cdfKernel, Interval(a, b), cdfEpsilon_)[0]);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if ((x >= bd) && (x < ad))
  {
    LOGDEBUG(OSS() << bd << " <= " << x << " < " << ad);
    const NumericalScalar value(left_.computeComplementaryCDF(x / d) + algo.integrate(cdfKernel, Interval(a, x / d), cdfEpsilon_)[0]);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  LOGDEBUG(OSS() << x << " not in [" << bc << ", " << ad << "]");
  return 0.0;
}

/* Compute the probability content of an interval */
NumericalScalar ProductDistribution::computeProbability(const Interval & interval) const
{
  return computeProbabilityContinuous(interval);
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
    throw InvalidArgumentException(HERE) << "Error: expected " << leftSize + rightSize << " values, got " << parameter.getSize();
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

/* Check if the distribution is elliptical */
Bool ProductDistribution::isElliptical() const
{
  return (left_.isElliptical() && (std::abs(left_.getRange().getLowerBound()[0] + left_.getRange().getUpperBound()[0]) < ResourceMap::GetAsNumericalScalar("Distribution-DefaultQuantileEpsilon"))) || (right_.isElliptical() && (std::abs(right_.getRange().getLowerBound()[0] + right_.getRange().getUpperBound()[0]) < ResourceMap::GetAsNumericalScalar("Distribution-DefaultQuantileEpsilon")));
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
