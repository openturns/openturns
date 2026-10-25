//                                               -*- C++ -*-
/**
 *  @brief The ProductDistribution distribution
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
#include <algorithm>
#include <cmath>
#include <vector>

#include "openturns/ProductDistribution.hxx"
#include "openturns/GaussKronrod.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/UniVariateFunctionImplementation.hxx"
#include "openturns/Uniform.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ProductDistribution)

static const Factory<ProductDistribution> Factory_ProductDistribution;

namespace
{
Scalar productBound(const Scalar left, const Scalar right)
{
  // The numerical support can be infinite for custom distributions.  The
  // product of a zero bound with an infinite one is defined as zero by
  // convention, instead of producing a NaN.
  if ((left == 0.0) || (right == 0.0)) return 0.0;
  return left * right;
}

Bool isCenteredAtZero(const DistributionImplementation & distribution)
{
  const Scalar epsilon = ResourceMap::GetAsScalar("Distribution-DefaultQuantileEpsilon");
  try
  {
    const Scalar mean = distribution.getMean()[0];
    return std::abs(mean) <= epsilon * std::max(1.0, std::abs(mean));
  }
  catch (...)
  {
    // Some symmetric distributions (e.g. Student with one degree of freedom)
    // have no finite mean.  Their numerical range is still a reliable
    // centeredness indicator when it is symmetric.
    const Scalar lower = distribution.getRange().getLowerBound()[0];
    const Scalar upper = distribution.getRange().getUpperBound()[0];
    return std::isfinite(lower) && std::isfinite(upper)
           && (std::abs(lower + upper) <= epsilon * std::max(1.0, std::max(std::abs(lower), std::abs(upper))));
  }
}
}

/* Default constructor */
ProductDistribution::ProductDistribution()
  : DistributionImplementation()
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
  : DistributionImplementation()
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
  return hasEqualBase(other) && (p_left_ == other.getLeft().getImplementation()) && (p_right_ == other.getRight().getImplementation());
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
  const Scalar ac = productBound(a, c);
  const Scalar ad = productBound(a, d);
  const Scalar bc = productBound(b, c);
  const Scalar bd = productBound(b, d);
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
  {
    // Nothing to do
  }

  PDFKernelProductDistribution * clone() const override
  {
    return new PDFKernelProductDistribution(*this);
  }

  Scalar operator() (const Scalar u) const override
  {
    const Scalar value = p_left_->computePDF(u);
    if (value == 0.0) return 0.0;
    const Scalar absU = std::abs(u);
    // Only the exact value zero is a numerical singularity here.  A small but
    // non-zero value must still use the actual right density: a vanishing
    // right density at zero does not imply a vanishing product density.
    if (x_ == 0.0)
    {
      const Scalar pdf0 = p_right_->computePDF(0.0);
      if ((pdf0 == 0.0) || (absU == 0.0)) return 0.0;
      return value * pdf0 / absU;
    }
    if (absU == 0.0) return 0.0;
    return value * p_right_->computePDF(x_ / u) / absU;
  }

private:
  const Pointer<DistributionImplementation> p_left_;
  const Pointer<DistributionImplementation> p_right_;
  const Scalar x_;

}; // class PDFKernelProductDistribution

// Class used to integrate the density of one factor divided by |u|.  It gives
// the finite limiting density at zero when the other factor has a vanishing
// density at zero.
class InverseAbsoluteDensityKernelProductDistribution: public UniVariateFunctionImplementation
{
public:
  explicit InverseAbsoluteDensityKernelProductDistribution(const Pointer<DistributionImplementation> & p_distribution)
    : UniVariateFunctionImplementation()
    , p_distribution_(p_distribution)
  {
    // Nothing to do
  }

  InverseAbsoluteDensityKernelProductDistribution * clone() const override
  {
    return new InverseAbsoluteDensityKernelProductDistribution(*this);
  }

  Scalar operator() (const Scalar u) const override
  {
    if (u == 0.0) return 0.0;
    return p_distribution_->computePDF(u) / std::abs(u);
  }

private:
  const Pointer<DistributionImplementation> p_distribution_;

}; // class InverseAbsoluteDensityKernelProductDistribution

// Class used to wrap the kernel of the integral defining the CDF of the product
// after the change of variable t = x / u.  This removes the narrow interval
// that appears near the support endpoints when integrating directly over u.
class CDFKernelProductDistribution: public UniVariateFunctionImplementation
{
public:
  CDFKernelProductDistribution(const Pointer<DistributionImplementation> & p_left,
                               const Pointer<DistributionImplementation> & p_right,
                               const Scalar x,
                               const Bool complementary,
                               const Scalar aLeft,
                               const Scalar bLeft)
    : UniVariateFunctionImplementation()
    , p_left_(p_left)
    , p_right_(p_right)
    , x_(x)
    , complementary_(complementary)
    , aLeft_(aLeft)
    , bLeft_(bLeft)
  {
    // Nothing to do
  }

  CDFKernelProductDistribution * clone() const override
  {
    return new CDFKernelProductDistribution(*this);
  }

  Scalar operator() (const Scalar t) const override
  {
    if (t == 0.0) return 0.0;
    const Scalar u = x_ / t;
    if ((u < aLeft_) || (u > bLeft_)) return 0.0;
    const Scalar value = p_left_->computePDF(u);
    if (value == 0.0) return 0.0;
    const Scalar right = (u > 0.0)
                          ? (complementary_ ? p_right_->computeComplementaryCDF(t) : p_right_->computeCDF(t))
                          : (complementary_ ? p_right_->computeCDF(t) : p_right_->computeComplementaryCDF(t));
    return value * std::abs(x_) / (t * t) * right;
  }

private:
  const Pointer<DistributionImplementation> p_left_;
  const Pointer<DistributionImplementation> p_right_;
  const Scalar x_;
  const Bool complementary_;
  const Scalar aLeft_;
  const Scalar bLeft_;
}; // class CDFKernelProductDistribution

// Logarithmic change of variable used when a CDF integration interval spans
// several orders of magnitude.  Without it, a tail contribution concentrated
// near a very small t is missed by the Gauss-Kronrod nodes.
class LogCDFKernelProductDistribution: public UniVariateFunctionImplementation
{
public:
  LogCDFKernelProductDistribution(const CDFKernelProductDistribution & kernel,
                                  const Scalar sign)
    : UniVariateFunctionImplementation()
    , kernel_(kernel)
    , sign_(sign)
  {
    // Nothing to do
  }

  LogCDFKernelProductDistribution * clone() const override
  {
    return new LogCDFKernelProductDistribution(*this);
  }

  Scalar operator() (const Scalar s) const override
  {
    const Scalar t = sign_ * std::exp(s);
    return kernel_(t) * std::abs(t);
  }

private:
  const CDFKernelProductDistribution kernel_;
  const Scalar sign_;
}; // class LogCDFKernelProductDistribution

// Class used to wrap one component of the kernel of the integral defining the
// characteristic function of the product.  The real and imaginary parts are
// integrated by two scalar Gauss-Kronrod passes instead of a two-dimensional
// vector evaluator, which avoids a Point allocation at every quadrature node.
class CFKernelProductDistribution: public UniVariateFunctionImplementation
{
public:
  CFKernelProductDistribution(const Pointer<DistributionImplementation> & p_left,
                              const Pointer<DistributionImplementation> & p_right,
                              const Scalar x,
                              const Bool imaginary)
    : UniVariateFunctionImplementation()
    , p_left_(p_left)
    , p_right_(p_right)
    , x_(x)
    , imaginary_(imaginary)
  {
    // Nothing to do
  };

  CFKernelProductDistribution * clone() const override
  {
    return new CFKernelProductDistribution(*this);
  }

  Scalar operator() (const Scalar u) const override
  {
    const Complex phi(p_right_->computeCharacteristicFunction(u * x_));
    const Scalar pdf = p_left_->computePDF(u);
    return pdf * (imaginary_ ? phi.imag() : phi.real());
  };

private:
  const Pointer<DistributionImplementation> p_left_;
  const Pointer<DistributionImplementation> p_right_;
  const Scalar x_;
  const Bool imaginary_;
}; // class CFKernelProductDistribution
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
  if (x == 0.0)
  {
    if (hasSingularityAtZero()) return SpecFunc::Infinity;
    return computePDFAtZero();
  }
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
    LOGDEBUG("In ProductDistribution::computePDF, Q4");
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

/* Get the limiting PDF at zero when it is finite.  Conditioning on each
   factor gives
   PDF(0) = PDF_left(0) \int PDF_right(v) / |v| dv
            + PDF_right(0) \int PDF_left(u) / |u| du.
   If both densities are positive at zero the two integrals diverge and
   hasSingularityAtZero() has already returned infinity.  If exactly one
   density vanishes at zero, the corresponding integral is finite under the
   assumptions of the class and the other term vanishes. */
Scalar ProductDistribution::computePDFAtZero() const
{
  const Scalar leftPDF0 = p_left_->computePDF(0.0);
  const Scalar rightPDF0 = p_right_->computePDF(0.0);
  if ((leftPDF0 == 0.0) && (rightPDF0 == 0.0)) return 0.0;
  const Scalar aLeft = p_left_->getRange().getLowerBound()[0];
  const Scalar bLeft = p_left_->getRange().getUpperBound()[0];
  const Scalar aRight = p_right_->getRange().getLowerBound()[0];
  const Scalar bRight = p_right_->getRange().getUpperBound()[0];
  if ((leftPDF0 > 0.0) && (rightPDF0 == 0.0))
  {
    const InverseAbsoluteDensityKernelProductDistribution kernel(p_right_);
    return leftPDF0 * algo_.integrate(kernel, aRight, bRight);
  }
  // Here rightPDF0 > 0 and leftPDF0 == 0, because the case of two positive
  // densities is the singular case handled by the caller.
  const InverseAbsoluteDensityKernelProductDistribution kernel(p_left_);
  return rightPDF0 * algo_.integrate(kernel, aLeft, bLeft);
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
  Scalar value = computeCDFOrComplementaryCDF(x, false);
  // Use the complementary integral above the exact mathematical midpoint:
  // adding a tiny tail to a CDF close to one loses most of its significant
  // digits.
  if (value > 0.5)
    value = 1.0 - computeCDFOrComplementaryCDF(x, true);
  return SpecFunc::Clip01(value);
}

Scalar ProductDistribution::computeComplementaryCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  return computeComplementaryCDF(point[0]);
}

Scalar ProductDistribution::computeComplementaryCDF(const Scalar x) const
{
  const Scalar a = getRange().getLowerBound()[0];
  const Scalar b = getRange().getUpperBound()[0];
  if (x <= a) return 1.0;
  if (x >= b) return 0.0;
  Scalar value = computeCDFOrComplementaryCDF(x, true);
  // Symmetrically, use the direct CDF below the exact mathematical midpoint.
  if (value > 0.5)
    value = 1.0 - computeCDFOrComplementaryCDF(x, false);
  return SpecFunc::Clip01(value);
}

Scalar ProductDistribution::computeScalarQuantile(const Scalar prob,
    const Bool tail) const
{
  const Scalar a = getRange().getLowerBound()[0];
  const Scalar b = getRange().getUpperBound()[0];
  if (prob < 0.0) return (tail ? b : a);
  if (prob >= 1.0) return (tail ? a : b);
  // The generic solver uses an absolute quantile tolerance and stops on an
  // absolute CDF residual.  For a probability many orders of magnitude below
  // the support width this gives a poor quantile.  Both tails are therefore
  // solved in the logarithmic distance to the nearest support bound.  The
  // CDF integration used here is adaptive; the iteration budget and the
  // convergence tolerance are the distribution-level ResourceMap values.
  if (!std::isfinite(a) || !std::isfinite(b))
    return DistributionImplementation::computeScalarQuantile(prob, tail);
  const Scalar supportWidth = b - a;
  if (!(supportWidth > 0.0) || !std::isfinite(supportWidth))
    return DistributionImplementation::computeScalarQuantile(prob, tail);
  // Keep the tail probability passed by the caller instead of routing the
  // inverse survival function through 1 - prob, which loses all its digits
  // when prob is close to zero.
  const Scalar lowerProbability = tail ? 1.0 - prob : prob;
  if (lowerProbability < 0.5)
  {
    // Bracket the root on the gap x - a, then bisect in log(gap).
    Scalar gapHigh = supportWidth;
    Scalar gapLow = gapHigh;
    Scalar cdf = computeCDF(a + gapLow);
    UnsignedInteger iteration = 0;
    while ((cdf >= lowerProbability) && (iteration < quantileIterations_))
    {
      gapLow *= 0.5;
      if (!(a + gapLow > a)) break;
      cdf = computeCDF(a + gapLow);
      ++iteration;
    }
    if (!(gapLow > 0.0) || !(a + gapLow > a)) return a;
    Scalar logLow = std::log(gapLow);
    Scalar logHigh = std::log(gapHigh);
    for (iteration = 0; iteration < quantileIterations_; ++iteration)
    {
      const Scalar logMiddle = 0.5 * (logLow + logHigh);
      const Scalar gap = std::exp(logMiddle);
      if (computeCDF(a + gap) < lowerProbability) logLow = logMiddle;
      else logHigh = logMiddle;
      if (logHigh - logLow <= quantileEpsilon_ * std::max(1.0, std::abs(logMiddle))) break;
    }
    return a + std::exp(0.5 * (logLow + logHigh));
  }
  // For the upper half, solve the complementary CDF for the gap b - x.
  const Scalar upperTailProbability = tail ? prob : 1.0 - prob;
  Scalar gapHigh = supportWidth;
  Scalar gapLow = gapHigh;
  Scalar ccdf = computeComplementaryCDF(b - gapLow);
  UnsignedInteger iteration = 0;
  while ((ccdf >= upperTailProbability) && (iteration < quantileIterations_))
  {
    gapLow *= 0.5;
    if (!(b - gapLow < b)) break;
    ccdf = computeComplementaryCDF(b - gapLow);
    ++iteration;
  }
  if (!(gapLow > 0.0) || !(b - gapLow < b)) return b;
  Scalar logLow = std::log(gapLow);
  Scalar logHigh = std::log(gapHigh);
  for (iteration = 0; iteration < quantileIterations_; ++iteration)
  {
    const Scalar logMiddle = 0.5 * (logLow + logHigh);
    const Scalar gap = std::exp(logMiddle);
    if (computeComplementaryCDF(b - gap) > upperTailProbability) logHigh = logMiddle;
    else logLow = logMiddle;
    if (logHigh - logLow <= quantileEpsilon_ * std::max(1.0, std::abs(logMiddle))) break;
  }
  return b - std::exp(0.5 * (logLow + logHigh));
}

Scalar ProductDistribution::computeCDFOrComplementaryCDF(const Scalar x,
    const Bool complementary) const
{
  // Conditioning on the sign of the left factor and using t = x / u avoids
  // integrating the product density itself.  The quadrant decomposition is
  // described in M. D. Springer, The Algebra of Random Variables, Wiley,
  // 1979, Chapter 4 (products of independent random variables).
  const Scalar aLeft = p_left_->getRange().getLowerBound()[0];
  const Scalar bLeft = p_left_->getRange().getUpperBound()[0];
  const Scalar aRight = p_right_->getRange().getLowerBound()[0];
  const Scalar bRight = p_right_->getRange().getUpperBound()[0];
  // The product is exactly zero with probability one conditional on a zero
  // factor, so this case has to be handled separately.
  if (x == 0.0)
  {
    const Scalar leftCDF = p_left_->computeCDF(0.0);
    const Scalar leftCCDF = p_left_->computeComplementaryCDF(0.0);
    const Scalar rightCDF = p_right_->computeCDF(0.0);
    const Scalar rightCCDF = p_right_->computeComplementaryCDF(0.0);
    return SpecFunc::Clip01(complementary
                            ? leftCDF * rightCDF + leftCCDF * rightCCDF
                            : leftCDF * rightCCDF + leftCCDF * rightCDF);
  }
  Scalar value = 0.0;
  const auto integrateSegment = [&](const Scalar lower, const Scalar upper)
  {
    if (!(upper > lower)) return;
    std::vector<Scalar> breakpoints(2);
    breakpoints[0] = lower;
    breakpoints[1] = upper;
    // The right CDF changes regime when t = x/u crosses one of its support
    // bounds.  Splitting there keeps every quadrature interval in a single
    // regime, including the constant zero/one regions.
    if (aRight != 0.0)
    {
      const Scalar point = x / aRight;
      if ((point > lower) && (point < upper)) breakpoints.push_back(point);
    }
    if (bRight != 0.0)
    {
      const Scalar point = x / bRight;
      if ((point > lower) && (point < upper)) breakpoints.push_back(point);
    }
    std::sort(breakpoints.begin(), breakpoints.end());
    breakpoints.erase(std::unique(breakpoints.begin(), breakpoints.end()), breakpoints.end());
    for (UnsignedInteger i = 0; i + 1 < breakpoints.size(); ++i)
    {
      const Scalar u0 = breakpoints[i];
      const Scalar u1 = breakpoints[i + 1];
      if (!(u1 > u0)) continue;
      const Scalar uMid = 0.5 * (u0 + u1);
      const Scalar tMid = x / uMid;
      if ((tMid >= aRight) && (tMid <= bRight))
      {
        const Scalar t0 = x / u0;
        const Scalar t1 = x / u1;
        const Scalar tLower = std::min(t0, t1);
        const Scalar tUpper = std::max(t0, t1);
        if (tUpper > tLower)
        {
          const CDFKernelProductDistribution cdfKernel(p_left_, p_right_, x, complementary, aLeft, bLeft);
          const Scalar absLower = std::abs(tLower);
          const Scalar absUpper = std::abs(tUpper);
          const Scalar minAbs = std::min(absLower, absUpper);
          const Scalar maxAbs = std::max(absLower, absUpper);
          // One decimal order of magnitude is the algorithmic threshold below
          // which a plain Gauss-Kronrod rule resolves the integrand.  Above
          // it, the interval contains a genuinely logarithmic tail and must
          // be integrated in log(t), otherwise its contribution can sit
          // entirely between two quadrature nodes.
          const Scalar logarithmicSpanRatio = 10.0;
          if ((minAbs > 0.0) && (maxAbs > logarithmicSpanRatio * minAbs))
          {
            // The interval contains many orders of magnitude, typically for a
            // product close to zero with a factor whose support starts at
            // zero.  Integrate in log(t) so that no tail contribution can sit
            // between two quadrature nodes.
            const Scalar sign = (tLower > 0.0) ? 1.0 : -1.0;
            const Scalar logMin = std::log(minAbs);
            const Scalar logMax = std::log(maxAbs);
            const LogCDFKernelProductDistribution logKernel(cdfKernel, sign);
            value += algo_.integrate(logKernel, logMin, logMax);
          }
          else value += algo_.integrate(cdfKernel, tLower, tUpper);
        }
      }
      else
      {
        // Outside the right support the right CDF is either zero or one.
        // Integrating f_X over the corresponding u interval avoids the
        // infinite t range induced by u -> 0.
        Bool constantOne = false;
        if (uMid > 0.0)
          constantOne = complementary ? (tMid < aRight) : (tMid > bRight);
        else
          constantOne = complementary ? (tMid > bRight) : (tMid < aRight);
        if (constantOne)
          value += p_left_->computeCDF(u1) - p_left_->computeCDF(u0);
      }
    }
  };
  // Integrate separately on each side of zero, where the event xY <= 0
  // changes from the CDF of Y to its complementary CDF.
  if (bLeft > 0.0) integrateSegment(std::max(aLeft, 0.0), bLeft);
  if (aLeft < 0.0) integrateSegment(aLeft, std::min(bLeft, 0.0));
  return SpecFunc::Clip01(value);
}

/* Compute the probability content of an interval */
Scalar ProductDistribution::computeProbability(const Interval & interval) const
{
  const Interval reducedInterval(interval.intersect(getRange()));
  if (reducedInterval.isEmpty()) return 0.0;
  if (reducedInterval == getRange()) return 1.0;
  const Bool finiteLower = reducedInterval.getFiniteLowerBound()[0];
  const Bool finiteUpper = reducedInterval.getFiniteUpperBound()[0];
  if (!finiteLower)
  {
    if (!finiteUpper) return 1.0;
    return computeCDF(reducedInterval.getUpperBound()[0]);
  }
  if (!finiteUpper) return computeComplementaryCDF(reducedInterval.getLowerBound()[0]);
  const Scalar lower = reducedInterval.getLowerBound()[0];
  const Scalar upper = reducedInterval.getUpperBound()[0];
  const Scalar lowerCDF = computeCDF(lower);
  const Scalar lowerCCDF = computeComplementaryCDF(lower);
  // Use the stable branch at the lower endpoint: a difference of upper
  // probabilities in the upper half, and a difference of lower probabilities
  // elsewhere.  This avoids the nested PDF quadrature of the generic
  // continuous probability implementation.
  if (lowerCCDF <= 0.5)
    return SpecFunc::Clip01(lowerCCDF - computeComplementaryCDF(upper));
  return SpecFunc::Clip01(computeCDF(upper) - lowerCDF);
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
Complex ProductDistribution::computeCharacteristicFunction(const Scalar x) const
{
  const Scalar muLeft = p_left_->getMean()[0];
  const Scalar muRight = p_right_->getMean()[0];
  const Scalar varLeft = p_left_->getCovariance()(0, 0);
  const Scalar varRight = p_right_->getCovariance()(0, 0);
  if (x * x * (varLeft + muLeft * muLeft + varRight + muRight * muRight) < 2.0 * SpecFunc::ScalarEpsilon) return Complex(1.0, x * muLeft * muRight);
  const Scalar aLeft = p_left_->getRange().getLowerBound()[0];
  const Scalar bLeft = p_left_->getRange().getUpperBound()[0];
  const CFKernelProductDistribution realKernel(p_left_, p_right_, x, false);
  const CFKernelProductDistribution imaginaryKernel(p_left_, p_right_, x, true);
  // Integrate the factor characteristic function directly.  This avoids the
  // nested product-PDF quadrature used by the generic implementation; the
  // Gauss-Kronrod rule adapts to the oscillations of the factor kernel.  Above
  // the historical threshold, allow twice as many sub-intervals so that the
  // larger number of oscillations is still resolved.
  GaussKronrod cfAlgorithm(algo_);
  if (std::abs(x) > ResourceMap::GetAsScalar("ProductDistribution-LargeCharacteristicFunctionArgument"))
    cfAlgorithm.setMaximumSubIntervals(2 * cfAlgorithm.getMaximumSubIntervals());
  const Scalar realPart = cfAlgorithm.integrate(realKernel, aLeft, muLeft)
                        + cfAlgorithm.integrate(realKernel, muLeft, bLeft);
  const Scalar imaginaryPart = cfAlgorithm.integrate(imaginaryKernel, aLeft, muLeft)
                             + cfAlgorithm.integrate(imaginaryKernel, muLeft, bLeft);
  return Complex(realPart, imaginaryPart);
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
  *this = ProductDistribution(newLeft, newRight);
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
  // In dimension one an elliptical distribution is just a distribution
  // symmetric about its mean.  If one factor is symmetric about zero, the
  // product with any independent factor is symmetric about zero: conditionally
  // on the other factor, X and -X have the same distribution.
  return (p_left_->isElliptical() && isCenteredAtZero(*p_left_))
         || (p_right_->isElliptical() && isCenteredAtZero(*p_right_));
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
  setParallel(p_left_->isParallel() && p_right_->isParallel());
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
  setParallel(p_left_->isParallel() && p_right_->isParallel());
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

Bool ProductDistribution::hasSingularityAtZero() const
{
  const Scalar aLeft = p_left_->getRange().getLowerBound()[0];
  const Scalar bLeft = p_left_->getRange().getUpperBound()[0];
  const Scalar aRight = p_right_->getRange().getLowerBound()[0];
  const Scalar bRight = p_right_->getRange().getUpperBound()[0];
  // A singularity at zero requires both factors to be able to vanish, with a
  // non-vanishing density at the origin.  This also covers densities that
  // diverge at zero, for which computePDF returns infinity.
  if (!((aLeft <= 0.0) && (bLeft >= 0.0) && (aRight <= 0.0) && (bRight >= 0.0)))
    return false;
  return (p_left_->computePDF(0.0) > 0.0) && (p_right_->computePDF(0.0) > 0.0);
}

/* Get the PDF singularities inside of the range - 1D only */
Point ProductDistribution::getSingularities() const
{
  if (hasSingularityAtZero()) return Point(1, 0.0);
  return Point(0);
}

/* Method save() stores the object through the StorageManager */
void ProductDistribution::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute( "left_", *p_left_ );
  adv.saveAttribute( "right_", *p_right_ );
}

/* Method load() reloads the object from the StorageManager */
void ProductDistribution::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  Distribution left;
  adv.loadAttribute( "left_", left );
  p_left_ = left.getImplementation();
  Distribution right;
  adv.loadAttribute( "right_", right );
  p_right_ = right.getImplementation();
  computeRange();
}

END_NAMESPACE_OPENTURNS
