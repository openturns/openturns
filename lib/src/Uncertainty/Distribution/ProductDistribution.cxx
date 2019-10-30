//                                               -*- C++ -*-
/**
 *  @brief The ProductDistribution distribution
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
#include <cstdlib>
#include <cmath>

#include "openturns/ProductDistribution.hxx"
#include "openturns/GaussKronrod.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/UniVariateFunctionImplementation.hxx"
#include "openturns/Uniform.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ProductDistribution)

static const Factory<ProductDistribution> Factory_ProductDistribution;

/* Default constructor */
ProductDistribution::ProductDistribution()
  : ContinuousDistribution()
  , p_left_(new Uniform(0.0, 1.0))
  , p_right_(new Uniform(0.0, 1.0))
  , algo_()
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
  , p_left_(left.getImplementation())
  , p_right_(right.getImplementation())
  , algo_()
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
  return (p_left_ == other.getLeft().getImplementation()) && (p_right_ == other.getRight().getImplementation());
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
      << " left=" << p_left_->__repr__()
      << " right=" << p_right_->__repr__();
  return oss;
}

String ProductDistribution::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "(" << p_left_->__str__() << " * " << p_right_->__str__() << ")";
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
  const Scalar a = p_left_->getRange().getLowerBound()[0];
  const Scalar b = p_left_->getRange().getUpperBound()[0];
  const Scalar c = p_right_->getRange().getLowerBound()[0];
  const Scalar d = p_right_->getRange().getUpperBound()[0];
  const Scalar ac = a * c;
  const Scalar ad = a * d;
  const Scalar bc = b * c;
  const Scalar bd = b * d;
  setRange(Interval(std::min(std::min(ac, ad), std::min(bc, bd)), std::max(std::max(ac, ad), std::max(bc, bd))));
}

/* Get one realization of the distribution */
Point ProductDistribution::getRealization() const
{
  return Point(1, p_left_->getRealization()[0] * p_right_->getRealization()[0]);
}

namespace
{
// Class used to wrap the kernel of the integral defining the PDF of the product
class PDFKernelProductDistribution: public UniVariateFunctionImplementation
{
public:
  PDFKernelProductDistribution(const Pointer<DistributionImplementation> & p_left,
                               const Pointer<DistributionImplementation> & p_right,
                               const Scalar x)
    : UniVariateFunctionImplementation()
    , p_left_(p_left)
    , p_right_(p_right)
    , x_(x)
    , isZero_(std::abs(x) < ResourceMap::GetAsScalar("Distribution-DefaultQuantileEpsilon")), pdf0_(isZero_ ? p_right->computePDF(0.0) : 0.0)
  {
    // Nothing to do
  };

  PDFKernelProductDistribution * clone() const
  {
    return new PDFKernelProductDistribution(*this);
  }

  Scalar operator() (const Scalar u) const
  {
    const Scalar value = p_left_->computePDF(u);
    if (value == 0.0) return 0.0;
    const Scalar absU = std::abs(u);
    // x_ == 0
    if (isZero_)
    {
      if (pdf0_ == 0.0) return 0.0;
      if (absU == 0.0) return SpecFunc::MaxScalar;
      return value * pdf0_ / absU;
    }
    // x_ != 0
    if (absU == 0.0)
    {
      const Scalar epsilon = 1e-7;
      return value * 0.5 * (p_right_->computePDF(x_ / epsilon) + p_right_->computePDF(-x_ / epsilon)) / epsilon;
    }
    return value * p_right_->computePDF(x_ / u) / absU;
  };

private:
  const Pointer<DistributionImplementation> p_left_;
  const Pointer<DistributionImplementation> p_right_;
  const Scalar x_;
  const Bool isZero_;
  const Scalar pdf0_;

}; // class PDFKernelProductDistribution

// Class used to wrap the kernel of the integral defining the CDF of the product
class CDFKernelProductDistribution: public UniVariateFunctionImplementation
{
public:
  CDFKernelProductDistribution(const Pointer<DistributionImplementation> & p_left,
                               const Pointer<DistributionImplementation> & p_right,
                               const Scalar x)
    : UniVariateFunctionImplementation()
    , p_left_(p_left)
    , p_right_(p_right)
    , x_(x)
    , isZero_(std::abs(x) < ResourceMap::GetAsScalar("Distribution-DefaultQuantileEpsilon"))
    , cdf0_(isZero_ ? p_right->computeCDF(0.0) : 0.0)
    , ccdf0_(isZero_ ? p_right->computeComplementaryCDF(0.0) : 0.0)
  {
    // Nothing to do
  };

  CDFKernelProductDistribution * clone() const
  {
    return new CDFKernelProductDistribution(*this);
  }

  Scalar operator() (const Scalar u) const
  {
    const Scalar value = p_left_->computePDF(u);
    if (value == 0.0) return 0.0;
    // x_ == 0
    if (isZero_) return value * cdf0_;
    if (u == 0.0) return (x_ < 0.0 ? 0.0 : value);
    return value * p_right_->computeCDF(x_ / u);
  };

private:
  const Pointer<DistributionImplementation> p_left_;
  const Pointer<DistributionImplementation> p_right_;
  const Scalar x_;
  const Bool isZero_;
  const Scalar cdf0_;
  const Scalar ccdf0_;
}; // struct CDFKernelProductDistribution

// Class used to wrap the kernel of the integral defining the CDF of the product
class ComplementaryCDFKernelProductDistributionProductDistribution: public UniVariateFunctionImplementation
{
public:
  ComplementaryCDFKernelProductDistributionProductDistribution(const Pointer<DistributionImplementation> & p_left,
      const Pointer<DistributionImplementation> & p_right,
      const Scalar x)
    : UniVariateFunctionImplementation()
    , p_left_(p_left)
    , p_right_(p_right)
    , x_(x)
    , isZero_(std::abs(x) < ResourceMap::GetAsScalar("Distribution-DefaultQuantileEpsilon"))
    , cdf0_(isZero_ ? p_right->computeCDF(0.0) : 0.0)
    , ccdf0_(isZero_ ? p_right->computeComplementaryCDF(0.0) : 0.0)
  {
    // Nothing to do
  };

  ComplementaryCDFKernelProductDistributionProductDistribution * clone() const
  {
    return new ComplementaryCDFKernelProductDistributionProductDistribution(*this);
  }

  Scalar operator() (const Scalar u) const
  {
    const Scalar value = p_left_->computePDF(u);
    if (value == 0.0) return 0.0;
    // x_ == 0
    if (isZero_) return value * ccdf0_;
    if (u == 0.0) return (x_ < 0.0 ? 0.0 : value);
    return value * p_right_->computeComplementaryCDF(x_ / u);
  };

private:
  const Pointer<DistributionImplementation> p_left_;
  const Pointer<DistributionImplementation> p_right_;
  const Scalar x_;
  const Bool isZero_;
  const Scalar cdf0_;
  const Scalar ccdf0_;
}; // struct ComplementaryCDFKernelProductDistributionProductDistribution

// Class used to wrap the kernel of the integral defining the CDF of the product
class CFKernelProductDistribution: public EvaluationImplementation
{
public:
  CFKernelProductDistribution(const Pointer<DistributionImplementation> & p_left,
                              const Pointer<DistributionImplementation> & p_right,
                              const Scalar x)
    : EvaluationImplementation()
    , p_left_(p_left)
    , p_right_(p_right)
    , x_(x)
  {
    // Nothing to do
  };

  CFKernelProductDistribution * clone() const
  {
    return new CFKernelProductDistribution(*this);
  }

  Point operator() (const Point & point) const
  {
    Point value(2);
    const Scalar u = point[0];
    const Complex phi(p_right_->computeCharacteristicFunction(u * x_));
    const Scalar pdf = p_left_->computePDF(point);
    value[0] = pdf * phi.real();
    value[1] = pdf * phi.imag();
    return value;
  };

  UnsignedInteger getInputDimension() const
  {
    return 1;
  }

  UnsignedInteger getOutputDimension() const
  {
    return 2;
  }

private:
  const Pointer<DistributionImplementation> p_left_;
  const Pointer<DistributionImplementation> p_right_;
  const Scalar x_;
}; // struct CFKernelProductDistributionWrapper
} // anonymous namespace

/* Get the PDF of the distribution: PDF(x) = \int_R PDF_left(u) * PDF_right(x / u) * du / |u| */
Scalar ProductDistribution::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  return computePDF(point[0]);
}

Scalar ProductDistribution::computePDF(const Scalar x) const
{
  const Scalar a = getRange().getLowerBound()[0];
  const Scalar b = getRange().getUpperBound()[0];
  if ((x < a) || (x > b)) return 0.0;
  const Scalar aLeft = p_left_->getRange().getLowerBound()[0];
  const Scalar bLeft = p_left_->getRange().getUpperBound()[0];
  const Scalar aRight = p_right_->getRange().getLowerBound()[0];
  const Scalar bRight = p_right_->getRange().getUpperBound()[0];
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
  const PDFKernelProductDistribution pdfKernel(p_left_, p_right_, x);
  if (c == 0.0)
  {
    LOGDEBUG("c == 0.0");
    if (x < ad)
    {
      LOGDEBUG(OSS() << x << " < " << ad);
      const Scalar value = algo_.integrate(pdfKernel, a, b);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    LOGDEBUG(OSS() << ad << " <= " << x);
    const Scalar value = algo_.integrate(pdfKernel, x / d, b);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if (ad <= bc)
  {
    LOGDEBUG("ad <= bc");
    if (x < ad)
    {
      LOGDEBUG(OSS() << x << " < " << ad);
      const Scalar value = algo_.integrate(pdfKernel, a, x / c);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if (x < bc)
    {
      LOGDEBUG(OSS() << x << " < " << bc);
      const Scalar value = algo_.integrate(pdfKernel, x / d, x / c);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    LOGDEBUG(OSS() << x << " < " << bd);
    const Scalar value = algo_.integrate(pdfKernel, x / d, b);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  LOGDEBUG("ad > bc");
  if (x < bc)
  {
    LOGDEBUG(OSS() << x << " < " << bc);
    const Scalar value = algo_.integrate(pdfKernel, a, x / c);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if (x < ad)
  {
    LOGDEBUG(OSS() << x << " < " << ad);
    const Scalar value = algo_.integrate(pdfKernel, a, b);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  LOGDEBUG(OSS() << x << " < " << bd);
  const Scalar value = algo_.integrate(pdfKernel, x / d, b);
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
  const PDFKernelProductDistribution pdfKernel(p_left_, p_right_, x);
  if (c == 0.0)
  {
    LOGDEBUG("c == 0.0");
    if (x < bd)
    {
      LOGDEBUG(OSS() << x << " < " << bd);
      const Scalar value = algo_.integrate(pdfKernel, a, x / d);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    LOGDEBUG(OSS() << x << " < " << 0.0);
    const Scalar value = algo_.integrate(pdfKernel, a, b);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if (ac <= bd)
  {
    LOGDEBUG("ac <= bd");
    if (x < ac)
    {
      LOGDEBUG(OSS() << x << " < " << ac);
      const Scalar value = algo_.integrate(pdfKernel, a, x / d);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if (x < bd)
    {
      LOGDEBUG(OSS() << x << " < " << bd);
      const Scalar value = algo_.integrate(pdfKernel, x / c, x / d);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    LOGDEBUG(OSS() << x << " < " << bc);
    const Scalar value = algo_.integrate(pdfKernel, x / c, b);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  LOGDEBUG("ac > bd");
  if (x < bd)
  {
    LOGDEBUG(OSS() << x << " < " << bd);
    const Scalar value = algo_.integrate(pdfKernel, a, x / d);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if (x < ac)
  {
    LOGDEBUG(OSS() << x << " < " << ac);
    const Scalar value = algo_.integrate(pdfKernel, a, b);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  LOGDEBUG(OSS() << x << " < " << bc);
  const Scalar value = algo_.integrate(pdfKernel, x / c, b);
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
  const PDFKernelProductDistribution pdfKernel(p_left_, p_right_, x);
  if (d == 0.0)
  {
    LOGDEBUG("d == 0.0");
    if (x < bc)
    {
      LOGDEBUG(OSS() << x << " < " << bc);
      const Scalar value = algo_.integrate(pdfKernel, a, b);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    LOGDEBUG(OSS() << x << " < " << ac);
    const Scalar value = algo_.integrate(pdfKernel, a, x / c);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if (ad <= bc)
  {
    LOGDEBUG("ad <= bc");
    if (x < ad)
    {
      LOGDEBUG(OSS() << x << " < " << ad);
      const Scalar value = algo_.integrate(pdfKernel, x / d, b);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if (x < bc)
    {
      LOGDEBUG(OSS() << x << " < " << bc);
      const Scalar value = algo_.integrate(pdfKernel, a, b);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    LOGDEBUG(OSS() << x << " < " << ac);
    const Scalar value = algo_.integrate(pdfKernel, a, x / c);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  LOGDEBUG("ad > bc");
  if (x < bc)
  {
    LOGDEBUG(OSS() << x << " < " << bc);
    const Scalar value = algo_.integrate(pdfKernel, x / d, b);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if (x < ad)
  {
    LOGDEBUG(OSS() << x << " < " << ad);
    const Scalar value = algo_.integrate(pdfKernel, x / d, x / c);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  LOGDEBUG(OSS() << x << " < " << ac);
  const Scalar value = algo_.integrate(pdfKernel, a, x / c);
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
  const PDFKernelProductDistribution pdfKernel(p_left_, p_right_, x);
  if (d == 0.0)
  {
    LOGDEBUG("d == 0.0");
    if (x < ac)
    {
      LOGDEBUG(OSS() << x << " < " << ac);
      const Scalar value = algo_.integrate(pdfKernel, x / c, b);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    LOGDEBUG(OSS() << x << " < " << 0.0);
    const Scalar value = algo_.integrate(pdfKernel, a, b);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if (bd <= ac)
  {
    LOGDEBUG("bd <= ac");
    if (x < bd)
    {
      LOGDEBUG(OSS() << x << " < " << bd);
      const Scalar value = algo_.integrate(pdfKernel, x / c, b);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    if (x < ac)
    {
      LOGDEBUG(OSS() << x << " < " << ac);
      const Scalar value = algo_.integrate(pdfKernel, x / c, x / d);
      LOGDEBUG(OSS() << "value=" << value);
      return value;
    }
    LOGDEBUG(OSS() << x << " < " << ad);
    const Scalar value = algo_.integrate(pdfKernel, a, x / d);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  LOGDEBUG("In ProductDistribution::computePDFQ4, bd > ac");
  LOGDEBUG("bd > ac");
  if (x < ac)
  {
    LOGDEBUG(OSS() << x << " < " << ac);
    const Scalar value = algo_.integrate(pdfKernel, x / c, b);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  if (x < bd)
  {
    LOGDEBUG(OSS() << x << " < " << bd);
    const Scalar value = algo_.integrate(pdfKernel, a, b);
    LOGDEBUG(OSS() << "value=" << value);
    return value;
  }
  LOGDEBUG(OSS() << bd << " <= " << x << " < " << ad);
  const Scalar value = algo_.integrate(pdfKernel, a, x / d);
  LOGDEBUG(OSS() << "value=" << value);
  return value;
}

/* Get the CDF of the distribution */
Scalar ProductDistribution::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  return computeCDF(point[0]);
}

Scalar ProductDistribution::computeCDF(const Scalar x) const
{
  const Scalar a = getRange().getLowerBound()[0];
  const Scalar b = getRange().getUpperBound()[0];
  if (x <= a) return 0.0;
  if (x >= b) return 1.0;
  const Scalar aLeft = p_left_->getRange().getLowerBound()[0];
  const Scalar bLeft = p_left_->getRange().getUpperBound()[0];
  Scalar value = 0.0;
  // First, compute the negative part
  if (aLeft < 0)
  {
    const ComplementaryCDFKernelProductDistributionProductDistribution cdfKernel(p_left_, p_right_, x);
    value += algo_.integrate(cdfKernel, aLeft, std::min(bLeft, 0.0));
  }
  if (bLeft >= 0)
  {
    const CDFKernelProductDistribution cdfKernel(p_left_, p_right_, x);
    value += algo_.integrate(cdfKernel, std::max(0.0, aLeft), bLeft);
  }
  return value;
}

/* Compute the probability content of an interval */
Scalar ProductDistribution::computeProbability(const Interval & interval) const
{
  return computeProbabilityContinuous(interval);
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
Complex ProductDistribution::computeCharacteristicFunction(const Scalar x) const
{
  const Scalar muLeft = p_left_->getMean()[0];
  const Scalar muRight = p_right_->getMean()[0];
  const Scalar varLeft = p_left_->getCovariance()(0, 0);
  const Scalar varRight = p_right_->getCovariance()(0, 0);
  if (x * x * (varLeft + muLeft * muLeft + varRight + muRight * muRight) < 2.0 * SpecFunc::ScalarEpsilon) return Complex(1.0, -x * muLeft * muRight);
  if (std::abs(x) > ResourceMap::GetAsScalar("ProductDistribution-LargeCharacteristicFunctionArgument")) return ContinuousDistribution::computeCharacteristicFunction(x);
  const Scalar aLeft = p_left_->getRange().getLowerBound()[0];
  const Scalar bLeft = p_left_->getRange().getUpperBound()[0];
  const CFKernelProductDistribution cfKernel(p_left_, p_right_, x);
  Scalar negativeError = 0.0;
  const Point negativePart(algo_.integrate(cfKernel, Interval(aLeft, muLeft), negativeError));
  Scalar positiveError = 0.0;
  const Point positivePart(algo_.integrate(cfKernel, Interval(muLeft, bLeft), positiveError));
  Complex value(negativePart[0] + positivePart[0], negativePart[1] + positivePart[1]);
  return value;
}

/* Compute the mean of the distribution */
void ProductDistribution::computeMean() const
{
  mean_ = Point(1, p_left_->getMean()[0] * p_right_->getMean()[0]);
  isAlreadyComputedMean_ = true;
}

/* Compute the covariance of the distribution */
void ProductDistribution::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  const Scalar meanLeft = p_left_->getMean()[0];
  const Scalar meanRight = p_right_->getMean()[0];
  const Scalar varLeft = p_left_->getCovariance()(0, 0);
  const Scalar varRight = p_right_->getCovariance()(0, 0);
  covariance_(0, 0) = meanLeft * meanLeft * varRight + meanRight * meanRight * varLeft + varLeft * varRight;
  isAlreadyComputedCovariance_ = true;
}

/* Get the skewness of the distribution */
Point ProductDistribution::getSkewness() const
{
  const Scalar meanLeft = p_left_->getMean()[0];
  const Scalar meanRight = p_right_->getMean()[0];
  const Scalar varLeft = p_left_->getCovariance()(0, 0);
  const Scalar varRight = p_right_->getCovariance()(0, 0);
  const Scalar mu3Left = p_left_->getSkewness()[0] * std::pow(varLeft, 1.5);
  const Scalar mu3Right = p_right_->getSkewness()[0] * std::pow(varRight, 1.5);
  const Scalar variance = meanLeft * meanLeft * varRight + meanRight * meanRight * varLeft + varLeft * varRight;
  return Point(1, (mu3Left * mu3Right + mu3Left * std::pow(meanRight, 3.0) + mu3Right * std::pow(meanLeft, 3.0) + 3.0 * (mu3Left * varRight * meanRight + mu3Right * varLeft * meanLeft) + 6.0 * varLeft * varRight * meanLeft * meanRight) / std::pow(variance, 1.5));
}

/* Get the kurtosis of the distribution */
Point ProductDistribution::getKurtosis() const
{
  const Scalar meanLeft = p_left_->getMean()[0];
  const Scalar meanLeft2 = meanLeft * meanLeft;
  const Scalar meanLeft4 = meanLeft2 * meanLeft2;
  const Scalar meanRight = p_right_->getMean()[0];
  const Scalar meanRight2 = meanRight * meanRight;
  const Scalar meanRight4 = meanRight2 * meanRight2;
  const Scalar varLeft = p_left_->getCovariance()(0, 0);
  const Scalar varRight = p_right_->getCovariance()(0, 0);
  const Scalar mu3Left = p_left_->getSkewness()[0] * std::pow(varLeft, 1.5);
  const Scalar mu3Right = p_right_->getSkewness()[0] * std::pow(varRight, 1.5);
  const Scalar mu4Left = p_left_->getKurtosis()[0] * varLeft * varLeft;
  const Scalar mu4Right = p_right_->getKurtosis()[0] * varRight * varRight;
  const Scalar variance = meanLeft * meanLeft * varRight + meanRight * meanRight * varLeft + varLeft * varRight;
  return Point(1, (mu4Left * mu4Right + mu4Left * meanRight4 + mu4Right * meanLeft4 + 4.0 * (mu4Left * mu3Right * meanRight + mu4Right * mu3Left * meanLeft) + 6.0 * (varLeft * meanLeft2 * varRight * meanRight2 + mu4Left * varRight * meanRight2 + mu4Right * varLeft * meanLeft2) + 12.0 * (mu3Left * meanLeft * mu3Right * meanRight + mu3Left * meanLeft * varRight * meanRight2 + mu3Right * meanRight * varLeft * meanLeft2)) / (variance * variance));
}

/* Get the raw moments of the distribution */
Point ProductDistribution::getMoment(const UnsignedInteger n) const
{
  return Point(1, p_left_->getMoment(n)[0] * p_right_->getMoment(n)[0]);
}


/* Parameters value accessor */
Point ProductDistribution::getParameter() const
{
  Point point(p_left_->getParameter());
  point.add(p_right_->getParameter());
  return point;
}

void ProductDistribution::setParameter(const Point & parameter)
{
  const UnsignedInteger leftSize = p_left_->getParameterDimension();
  const UnsignedInteger rightSize = p_right_->getParameterDimension();
  if (parameter.getSize() != leftSize + rightSize)
    throw InvalidArgumentException(HERE) << "Error: expected " << leftSize + rightSize << " values, got " << parameter.getSize();
  Point newLeftParameters(leftSize);
  Point newRightParameters(rightSize);
  std::copy(parameter.begin(), parameter.begin() + leftSize, newLeftParameters.begin());
  std::copy(parameter.begin() + leftSize, parameter.end(), newRightParameters.begin());
  Distribution newLeft(p_left_);
  Distribution newRight(p_right_);
  newLeft.setParameter(newLeftParameters);
  newRight.setParameter(newRightParameters);
  const Scalar w = getWeight();
  *this = ProductDistribution(newLeft, newRight);
  setWeight(w);
}

/* Parameters description accessor */
Description ProductDistribution::getParameterDescription() const
{
  Description description(p_left_->getParameterDescription());
  description.add(p_right_->getParameterDescription());
  return description;
}

/* Check if the distribution is elliptical */
Bool ProductDistribution::isElliptical() const
{
  return (p_left_->isElliptical() && (std::abs(p_left_->getRange().getLowerBound()[0] + p_left_->getRange().getUpperBound()[0]) < ResourceMap::GetAsScalar("Distribution-DefaultQuantileEpsilon"))) || (p_right_->isElliptical() && (std::abs(p_right_->getRange().getLowerBound()[0] + p_right_->getRange().getUpperBound()[0]) < ResourceMap::GetAsScalar("Distribution-DefaultQuantileEpsilon")));
}

/* Left accessor */
void ProductDistribution::setLeft(const Distribution & left)
{
  if (left.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can multiply only distribution with dimension=1, here dimension=" << left.getDimension();
  if (!left.isContinuous()) throw InvalidArgumentException(HERE) << "Error: can multiply only continuous distributions";
  p_left_ = left.getImplementation();
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  isAlreadyCreatedGeneratingFunction_ = false;
  isParallel_ = p_left_->isParallel();
  computeRange();
}

Distribution ProductDistribution::getLeft() const
{
  return p_left_;
}

/* Right accessor */
void ProductDistribution::setRight(const Distribution & right)
{
  if (right.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can multiply only distribution with dimension=1, here dimension=" << right.getDimension();
  if (!right.isContinuous()) throw InvalidArgumentException(HERE) << "Error: can multiply only continuous distributions";
  p_right_ = right.getImplementation();
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  isAlreadyCreatedGeneratingFunction_ = false;
  isParallel_ = p_right_->isParallel();
  computeRange();
}

Distribution ProductDistribution::getRight() const
{
  return p_right_;
}

Bool ProductDistribution::isContinuous() const
{
  return p_left_->isContinuous() && p_right_->isContinuous();
}

/* Tell if the distribution is discrete */
Bool ProductDistribution::isDiscrete() const
{
  return p_left_->isDiscrete() && p_right_->isDiscrete();
}

/* Tell if the distribution is integer valued */
Bool ProductDistribution::isIntegral() const
{
  return p_left_->isIntegral() && p_right_->isIntegral();
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
  adv.saveAttribute( "left_", *p_left_ );
  adv.saveAttribute( "right_", *p_right_ );
}

/* Method load() reloads the object from the StorageManager */
void ProductDistribution::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  Distribution left;
  adv.loadAttribute( "left_", left );
  p_left_ = left.getImplementation();
  Distribution right;
  adv.loadAttribute( "right_", right );
  p_right_ = right.getImplementation();
  computeRange();
}

END_NAMESPACE_OPENTURNS
