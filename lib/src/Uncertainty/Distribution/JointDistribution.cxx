//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all ComposedDistributions
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/JointDistribution.hxx"
#include "openturns/MarginalTransformationEvaluation.hxx"
#include "openturns/MarginalTransformationGradient.hxx"
#include "openturns/MarginalTransformationHessian.hxx"
#include "openturns/NatafEllipticalDistributionEvaluation.hxx"
#include "openturns/NatafEllipticalDistributionGradient.hxx"
#include "openturns/NatafEllipticalDistributionHessian.hxx"
#include "openturns/InverseNatafEllipticalDistributionEvaluation.hxx"
#include "openturns/InverseNatafEllipticalDistributionGradient.hxx"
#include "openturns/InverseNatafEllipticalDistributionHessian.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/Indices.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/Normal.hxx"
#include "openturns/IndependentCopula.hxx"
#include "openturns/NormalCopula.hxx"
#include "openturns/Log.hxx"
#include "openturns/TBBImplementation.hxx"
#include "openturns/ComposedFunction.hxx"
#include "openturns/Os.hxx"
#include "openturns/OSS.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(JointDistribution)

static const Factory<JointDistribution> Factory_JointDistribution;

/* Default constructor */
JointDistribution::JointDistribution()
  : DistributionImplementation()
  , distributionCollection_()
  , core_(IndependentCopula(1))
{
  setName("JointDistribution");
  setDimension(1);
  DistributionCollection coll(1);
  coll[0] = Uniform();
  // This call set also the range.
  setDistributionCollection( coll );
}

/* Default constructor */
JointDistribution::JointDistribution(const DistributionCollection & coll)
  : DistributionImplementation()
  , distributionCollection_()
  , core_(IndependentCopula(coll.getSize()))
{
  setName("JointDistribution");
  setDimension(coll.getSize());
  // We can NOT set distributionCollection_ in the member area of the constructor
  // because we must check before if the collection is valid (ie, if all the
  // distributions of the collection have the dimension 1). We do this by calling
  // the setDistributionCollection() method that do it for us.
  // This call set also the range.
  setDistributionCollection( coll );
}

/* Default constructor */
JointDistribution::JointDistribution(const DistributionCollection & coll,
                                     const Distribution & core)
  : DistributionImplementation()
  , distributionCollection_()
  , core_(core)
{
  setName("JointDistribution");
  setDimension(core.getDimension());
  // We can NOT set distributionCollection_ in the member area of the constructor
  // because we must check before if the collection is valid (ie, if all the
  // distributions of the collection have the dimension 1). We do this by calling
  // the setDistributionCollection() method that do it for us.
  // This call set also the range.
  setDistributionCollection(coll);
  setCore(core);
}

/* Comparison operator */
Bool JointDistribution::operator ==(const JointDistribution & other) const
{
  if (this == &other) return true;
  // The core...
  // Store the result of hasIndependentCopula() as it may be costly.
  const Bool hasIndependent = hasIndependentCopula();
  // One of the copulas is the independent one, not the other
  if (!hasIndependent == other.hasIndependentCopula()) return false;
  // None of the copulas is the independent one, and the two copulas are different
  if (!hasIndependent && !(core_ == other.getCore())) return false;
  // Then the marginals
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    const Distribution left(distributionCollection_[i]);
    const Distribution right(other.distributionCollection_[i]);
    if (!(left == right)) return false;
  }
  return true;
}

Bool JointDistribution::equals(const DistributionImplementation & other) const
{
  // First, test the dimension
  if (!(dimension_ == other.getDimension())) return false;
  // Second, check with cast
  const JointDistribution* p_other = dynamic_cast<const JointDistribution*>(&other);
  if (p_other != 0) return (*this == *p_other);
  // Third, check by properties
  // We coud go there eg. when comparing a JointDistribution([Normal()]*2) with a Normal(2)
  // The copula...
  // Store the result of hasIndependentCopula() as it may be costly.
  const Bool hasIndependent = hasIndependentCopula();
  // One of the copulas is the independent one, not the other
  if (!hasIndependent == other.hasIndependentCopula()) return false;
  // None of the copulas is the independent one, and the two copulas are different
  if (!hasIndependent && !(getCopula() == other.getCopula())) return false;
  // Then the marginals
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    const Distribution left(distributionCollection_[i]);
    const Distribution right(other.getMarginal(i));
    if (!(left == right)) return false;
  }
  return true;
}

/* String converter */
String JointDistribution::__repr__() const
{
  OSS oss;
  oss << "class=" << JointDistribution::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension();
  if (core_.isCopula())
    oss << " copula=" << core_;
  else
    oss << " core=" << core_;
  for (UnsignedInteger i = 0; i < getDimension(); ++i) oss << " marginal[" << i << "]=" << distributionCollection_[i];
  return oss;
}

String JointDistribution::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "(";
  String separator("");
  for (UnsignedInteger i = 0; i < distributionCollection_.getSize(); ++i)
  {
    oss << separator << distributionCollection_[i].__str__();
    separator = ", ";
  }
  if (getDimension() > 1) oss << ", " << core_.__str__();
  oss << ")";
  return oss;
}

String JointDistribution::_repr_html_() const
{
  OSS oss(false);
  oss << getClassName() << "\n";
  oss << "<ul>\n";
  oss << "  <li>name=" << getName() << "</li>\n";
  oss << "  <li>dimension: " << getDimension() << "</li>\n";
  oss << "  <li>description=" << description_ << "\n";
  if (getDimension() > 1)
  {
    if (core_.isCopula())
      oss << "  <li>copula: " << core_.__str__() << "</li>\n";
    else
      oss << "  <li>core: " << core_.__str__() << "</li>\n";
  }
  oss << "</ul>\n";
  oss << "\n";
  // Table of marginals
  oss << "<table>\n";
  // Header
  oss << "  <tr>\n";
  oss << "    <th>Index</th>\n";
  oss << "    <th>Variable</th>\n";
  oss << "    <th>Distribution</th>\n";
  oss << "  </tr>\n";
  // Content
  for (UnsignedInteger i = 0; i < distributionCollection_.getSize(); ++i)
  {
    oss << "  <tr>\n";
    oss << "    <td>" << i << "</td>\n";
    oss << "    <td>" << description_[i] << "</td>\n";
    oss << "    <td>" << distributionCollection_[i].__str__() << "</td>\n";
    oss << "  </tr>\n";
  }
  oss << "</table>\n";
  return oss;
}

String JointDistribution::__repr_markdown__() const
{
  OSS oss(false);
  oss << getClassName() << "\n";
  oss << "- name=" << getName() << "\n";
  oss << "- dimension=" << getDimension() << "\n";
  oss << "- description=" << description_ << "\n";
  if (getDimension() > 1)
  {
    if (core_.isCopula())
      oss << "- copula=" << core_.__str__() << "\n";
    else
      oss << "- core=" << core_.__str__() << "\n";
  }
  // Compute maximum distribution's column width
  String intermediateString;
  UnsignedInteger maximumColumnWidth = 0;
  for (UnsignedInteger i = 0; i < distributionCollection_.getSize(); ++i)
  {
    intermediateString = OSS() << " " << distributionCollection_[i].__str__() << " ";
    if (intermediateString.size() > maximumColumnWidth)
      maximumColumnWidth = intermediateString.size();
  }
  intermediateString = OSS() << " Distribution ";
  if (intermediateString.size() > maximumColumnWidth)
    maximumColumnWidth = intermediateString.size();
  // Format the table
  oss << "\n";
  oss << "| Index | Variable |"
      << OSS::PadString(" Distribution ", maximumColumnWidth) << "|" << "\n";
  oss << "|-------|----------|" << String(maximumColumnWidth, '-') << "|" << "\n";
  for (UnsignedInteger i = 0; i < distributionCollection_.getSize(); ++i)
  {
    oss << "| " << std::setw(5) << i << " |"
        << " " << std::setw(8) << description_[i] << " |";
    intermediateString = OSS() << " " << distributionCollection_[i].__str__() << " ";
    oss << OSS::PadString(intermediateString, maximumColumnWidth) << "|" << "\n";
  }
  return oss;
}


/* Distribution collection accessor */
void JointDistribution::setDistributionCollection(const DistributionCollection & coll)
{
  // Check if the collection is not empty
  const UnsignedInteger size = coll.getSize();
  if ((getDimension() != 0) && (size != getDimension())) throw InvalidArgumentException(HERE) << "The distribution collection must have a size equal to the core dimension";
  Description description(size);
  Point lowerBound(size);
  Point upperBound(size);
  Interval::BoolCollection finiteLowerBound(size);
  Interval::BoolCollection finiteUpperBound(size);
  if (size == 0) throw InvalidArgumentException(HERE) << "Collection of distributions is empty";
  // First, check that all the marginal distributions are of dimension 1
  Bool parallel = core_.getImplementation()->isParallel();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if (coll[i].getDimension() != 1) throw InvalidArgumentException(HERE) << "The marginal distribution " << i << " is of dimension " << coll[i].getDimension() << ", which is different from 1.";
    parallel = parallel && coll[i].getImplementation()->isParallel();
    const Interval marginalRange(coll[i].getRange());
    lowerBound[i] = marginalRange.getLowerBound()[0];
    upperBound[i] = marginalRange.getUpperBound()[0];
    finiteLowerBound[i] = marginalRange.getFiniteLowerBound()[0];
    finiteUpperBound[i] = marginalRange.getFiniteUpperBound()[0];
    // The description of the JointDistribution is built first by using the marginal description
    // then by using the marginal name if the description is empty, which should never occur
    const String marginalDescription(coll[i].getDescription()[0]);
    if (marginalDescription.size() > 0) description[i] = marginalDescription;
    else
    {
      LOGWARN(OSS() << "Warning: using the name of the marginal " << i << " instead of its description for building the description of the JointDistribution, because the marginal description is empty.");
      const String marginalName(coll[i].getName());
      description[i] = marginalName;
    }
  }
  setParallel(parallel);
  // Everything is ok, store the collection
  distributionCollection_ = coll;
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;

  // avoid description warning with identical entries
  std::map<String, UnsignedInteger> occurrence;
  UnsignedInteger idx = 0;
  for (UnsignedInteger i = 0; i < description.getSize(); ++ i)
  {
    const String currentName(description[i]);
    ++ occurrence[currentName];
    if (occurrence[currentName] > 1)
    {
      while (occurrence.find(OSS() << "X" << idx) != occurrence.end())
        ++ idx;
      const String newName(OSS() << "X" << idx);
      ++ occurrence[newName]; // avoid duplicates with new ones too
      description[i] = newName;
    }
  }
  setDescription(description);

  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}


/* Distribution collection accessor */
JointDistribution::DistributionCollection JointDistribution::getDistributionCollection() const
{
  return distributionCollection_;
}


/* Core accessor */
void JointDistribution::setCore(const Distribution & core)
{
  // We check if the core has a dimension compatible with the one of the object,
  // especially if the object has already been created and has a collection of distribution
  if (getDimension() != core.getDimension())
    throw InvalidArgumentException(HERE) << "Core has a dimension different from the JointDistribution's";

  const Interval unitCube(core.getDimension());
  const Interval coreRange(core.getRange());
  if ((!unitCube.contains(coreRange.getLowerBound())) || (!unitCube.contains(coreRange.getUpperBound()))) throw InvalidArgumentException(HERE) << "Error: the given core=" << core << " has a range=" << core.getRange() << " not included in the unit hypercube.";
  core_ = core;
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  // We ensure that the core has the same description than the JointDistribution
  core_.setDescription(getDescription());
}


/* Core accessor */
Distribution JointDistribution::getCore() const
{
  return core_;
}

/* Copula accessor */
void JointDistribution::setCopula(const Distribution & copula)
{
  // We check if the copula has a dimension compatible with the one of the object,
  // especially if the object has already been created and has a collection of distribution
  if (getDimension() != 0)
  {
    if (getDimension() != copula.getDimension())
      throw InvalidArgumentException(HERE) << "Copula has a dimension different from the JointDistribution's";
  }
  else
    setDimension(copula.getDimension());

  core_ = copula;
  isAlreadyComputedCovariance_ = false;
  // We ensure that the copula has the same description than the JointDistribution
  core_.setDescription(getDescription());
}


/* Copula accessor */
Distribution JointDistribution::getCopula() const
{
  if (core_.isCopula())
    return core_;
  return core_.getCopula();
}

/* Virtual constructor */
JointDistribution * JointDistribution::clone() const
{
  return new JointDistribution(*this);
}

/* Get one realization of the JointDistribution */
Point JointDistribution::getRealization() const
{
  const UnsignedInteger dimension = getDimension();
  // Complex case: use 1D CDF inversion
  if (!core_.isCopula() || !hasIndependentCopula())
  {
    Point realization(core_.getRealization());
    for (UnsignedInteger i = 0; i < dimension; ++i) realization[i] = distributionCollection_[i].computeScalarQuantile(realization[i]);
    return realization;
  } // General case
  if (dimension == 1) return distributionCollection_[0].getRealization();
  // Special case for independent copula
  Point result(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i) result[i] = distributionCollection_[i].getRealization()[0];
  return result;
}

struct ComposedDistributionComputeSamplePolicy
{
  const Sample input_;
  Sample & output_;
  const Collection<Distribution> distributionCollection_;
  UnsignedInteger dimension_;

  ComposedDistributionComputeSamplePolicy(const Sample & input,
                                          Sample & output,
                                          const Collection<Distribution> & distributionCollection)
    : input_(input)
    , output_(output)
    , distributionCollection_(distributionCollection)
    , dimension_(distributionCollection.getSize())
  {}

  inline void operator()( const TBBImplementation::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
      for (UnsignedInteger j = 0; j < dimension_; ++j)
        output_(i, j) = distributionCollection_[j].computeScalarQuantile(input_(i, j));
  }

}; /* end struct ComposedDistributionComputeSamplePolicy */

/* Get a sample of the distribution */
Sample JointDistribution::getSampleParallel(const UnsignedInteger size) const
{
  const UnsignedInteger dimension = getDimension();
  if (!core_.isCopula() || !hasIndependentCopula())
  {
    // For dependent components, we can use some parallelism on top
    // of possible parallelism of the getSample() method of the core
    const Sample coreSample(core_.getSample(size));
    Sample result(size, dimension);
    const ComposedDistributionComputeSamplePolicy policy( coreSample, result, distributionCollection_ );
    TBBImplementation::ParallelFor( 0, size, policy );
    result.setName(getName());
    result.setDescription(getDescription());
    return result;
  } // General case
  // For 1D or independent components, we can only rely on possible parallel
  // implementation of getSample() methods of the marginal distributions
  if (dimension == 1) return distributionCollection_[0].getSample(size);
  // Special case for independent copula
  Point data(size * dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Point marginalSample(distributionCollection_[i].getSample(size).getImplementation()->getData());
    UnsignedInteger shift = i;
    for (UnsignedInteger j = 0; j < size; ++j)
    {
      data[shift] = marginalSample[j];
      shift += dimension;
    }
  }
  SampleImplementation result(size, dimension);
  result.setData(data);
  result.setName(getName());
  result.setDescription(getDescription());
  return result;
}

Sample JointDistribution::getSample(const UnsignedInteger size) const
{
  if (isParallel_) return getSampleParallel(size);
  return DistributionImplementation::getSample(size);
}

/* Get the DDF of the JointDistribution */
Point JointDistribution::computeDDF(const Point & point) const
{
  /* PDF = PDF_core(CDF_dist1(p1), ..., CDF_distn(pn))xPDF_dist1(p1)x...xPDF_distn(pn) */
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  Point uPoint(dimension);
  Point pdfMarginal(dimension);
  Point ddfMarginal(dimension);
  Scalar productPDF = 1.0;
  Point component(1);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    component[0] = point[i];
    uPoint[i] = distributionCollection_[i].computeCDF(component);
    pdfMarginal[i] = distributionCollection_[i].computePDF(component);
    ddfMarginal[i] = distributionCollection_[i].computeDDF(component)[0];
    productPDF *= pdfMarginal[i];
  }
  // Initialization with the values of an independent copula
  Scalar pdfCore = 1.0;
  Point ddfCore(dimension, 0.0);
  // If the distribution does not have an independent copula
  if (!core_.isCopula() || !hasIndependentCopula())
  {
    pdfCore = core_.computePDF(uPoint);
    ddfCore = core_.computeDDF(uPoint);
  } // General case
  // Compute the ddf
  Point ddf(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i) if (pdfMarginal[i] > 0.0) ddf[i] = productPDF * (ddfCore[i] * pdfMarginal[i] + pdfCore * ddfMarginal[i] / pdfMarginal[i]);
  return ddf;
}

/* Get the PDF of the JointDistribution */
Scalar JointDistribution::computePDF(const Point & point) const
{
  /* PDF = PDF_core(CDF_dist1(p1), ..., CDF_distn(pn))xPDF_dist1(p1)x...xPDF_distn(pn) */
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  if (!core_.isCopula() || !hasIndependentCopula())
  {
    // General case
    Point uPoint(dimension);
    Point component(1);
    Scalar productPDF = 1.0;
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      component[0] = point[i];
      uPoint[i] = distributionCollection_[i].computeCDF(component);
      productPDF *= distributionCollection_[i].computePDF(component);
    }
    return core_.computePDF(uPoint) * productPDF;
  } // General case
  // Special case for dimension 1, to boost performances
  if (dimension == 1) return distributionCollection_[0].computePDF(point);
  Scalar productPDF = 1.0;
  // Special case for the independent case, to boost performances
  for (UnsignedInteger i = 0; i < dimension; ++i) productPDF *= distributionCollection_[i].computePDF(point[i]);
  return productPDF;
}

/* Get the logarithm of the PDF of the JointDistribution */
Scalar JointDistribution::computeLogPDF(const Point & point) const
{
  /* PDF = PDF_core(CDF_dist1(p1), ..., CDF_distn(pn))xPDF_dist1(p1)x...xPDF_distn(pn) */
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  if (!core_.isCopula() || !hasIndependentCopula())
  {
    // General case
    Point uPoint(dimension);
    Point component(1);
    Scalar sumLogPDF = 0.0;
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      component[0] = point[i];
      uPoint[i] = distributionCollection_[i].computeCDF(component);
      sumLogPDF += distributionCollection_[i].computeLogPDF(component);
    }
    return core_.computeLogPDF(uPoint) + sumLogPDF;
  } // General case
  // Special case for dimension 1, to boost performances
  if ((dimension == 1) && (core_.isCopula())) return distributionCollection_[0].computeLogPDF(point);
  Scalar sumLogPDF = 0.0;
  // Special case for the independent case, to boost performances
  for (UnsignedInteger i = 0; i < dimension; ++i) sumLogPDF += distributionCollection_[i].computeLogPDF(point[i]);
  return sumLogPDF;
}

/* Get the CDF of the JointDistribution */
Scalar JointDistribution::computeCDF(const Point & point) const
{
  /* CDF = CDF_core(CDF_dist1(p1), ..., CDF_distn(pn)) */
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  if (!core_.isCopula() || !hasIndependentCopula())
  {
    // General case
    Point uPoint(dimension);
    for (UnsignedInteger i = 0; i < dimension; ++i) uPoint[i] = distributionCollection_[i].computeCDF(point[i]);
    return core_.computeCDF(uPoint);
  } // General case
  if (dimension == 1) return distributionCollection_[0].computeCDF(point);
  // Special case for the independent case, to boost performances
  Scalar productCDF = 1.0;
  for (UnsignedInteger i = 0; i < dimension; ++i) productCDF *= distributionCollection_[i].computeCDF(point[i]);
  return productCDF;
}

Scalar JointDistribution::computeSurvivalFunction(const Point & point) const
{
  /* Survival = \hat{F}(x_1, \dots, x_d)
   *          = \hat{C}(\hat{F}_1(x_1), \dots, \hat{F}_d(x_d))
   *          = \bar{C}(1-\hat{F}_1(x_1), \dots, 1-\hat{F}_d(x_d))
   *          = \bar{C}(F_1(x_1), \dots, F_d(x_d))
   *
   * With \bar{C} the survival function of the core, not to be mistaken with the survival core \hat{C}
   */
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  if (!core_.isCopula() || !hasIndependentCopula())
  {
    // General case
    Point uPoint(dimension);
    for (UnsignedInteger i = 0; i < dimension; ++i) uPoint[i] = distributionCollection_[i].computeCDF(point[i]);
    return core_.computeSurvivalFunction(uPoint);
  }
  if  ((dimension == 1) && (core_.isCopula())) return distributionCollection_[0].computeSurvivalFunction(point);
  // Special case for the independent case, to boost performances
  Scalar productSurvival = 1.0;
  for (UnsignedInteger i = 0; i < dimension; ++i) productSurvival *= distributionCollection_[i].computeSurvivalFunction(point[i]);
  return productSurvival;
}

/* Compute the probability content of an interval */
Scalar JointDistribution::computeProbability(const Interval & interval) const
{
  const UnsignedInteger dimension = getDimension();
  if (interval.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given interval must have dimension=" << dimension << ", here dimension=" << interval.getDimension();

  // If the interval is empty
  if (interval.isEmpty()) return 0.0;
  const Point lower(interval.getLowerBound());
  const Point upper(interval.getUpperBound());
  const Interval::BoolCollection finiteLower(interval.getFiniteLowerBound());
  const Interval::BoolCollection finiteUpper(interval.getFiniteUpperBound());
  Point lowerCore(dimension);
  Point upperCore(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    if (finiteLower[i]) lowerCore[i] = distributionCollection_[i].computeCDF(lower[i]);
    else lowerCore[i] = 0.0;
    if (finiteUpper[i]) upperCore[i] = distributionCollection_[i].computeCDF(upper[i]);
    else upperCore[i] = 1.0;
  }
  return core_.computeProbability(Interval(lowerCore, upperCore));
}

/* Get the PDF gradient of the distribution */
Point JointDistribution::computePDFGradient(const Point & point) const
{
  if (!core_.isCopula() || !hasIndependentCopula())
    return DistributionImplementation::computePDFGradient(point);

  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  Point gradient;
  // The marginal parameters are supposed to be independent from one marginal distribution
  // to the others
  Point marginalPDF(dimension);
  Scalar pdf = 1.0;
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    marginalPDF[i] = distributionCollection_[i].computePDF(point[i]);
    if (marginalPDF[i] == 0.0)
      return Point(getParameter().getDimension());
    pdf *= marginalPDF[i];
  }
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    const Point marginalGradient(distributionCollection_[i].computePDFGradient(Point(1, point[i])));
    gradient.add(marginalGradient / marginalPDF[i]);
  }
  return gradient * pdf;
}

/* Get the CDF gradient of the distribution */
Point JointDistribution::computeCDFGradient(const Point & point) const
{
  if (!core_.isCopula() || !hasIndependentCopula())
    return DistributionImplementation::computeCDFGradient(point);

  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  Point gradient;
  // The marginal parameters are supposed to be independent from one marginal distribution
  // to the others
  Point marginalCDF(dimension);
  Scalar cdf = 1.0;
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    marginalCDF[i] = distributionCollection_[i].computeCDF(point[i]);
    cdf *= marginalCDF[i];
  }
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    Point marginalGradient(distributionCollection_[i].getParameter().getDimension());
    if (marginalCDF[i] > 0.0)
    {
      marginalGradient = distributionCollection_[i].computeCDFGradient(Point(1, point[i]));
      marginalGradient *= cdf / marginalCDF[i];
    }
    gradient.add(marginalGradient);
  }
  return gradient;
}

/* Get the quantile of the JointDistribution */
Point JointDistribution::computeQuantile(const Scalar prob,
    const Bool tail) const
{
  if (!((prob >= 0.0) && (prob <= 1.0))) throw InvalidArgumentException(HERE) << "Error: cannot compute a quantile for a probability level outside of [0, 1]";
  const UnsignedInteger dimension = getDimension();
  if  ((dimension == 1) && (core_.isCopula())) return distributionCollection_[0].computeQuantile(prob, tail);
  // General case
  Point quantile(core_.computeQuantile(prob, tail));
  for (UnsignedInteger i = 0; i < dimension; ++i) quantile[i] = distributionCollection_[i].computeQuantile(quantile[i])[0];
  return quantile;
}

/* Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar JointDistribution::computeConditionalPDF(const Scalar x, const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional PDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if (core_.isCopula() && ((conditioningDimension == 0) || hasIndependentCopula())) return distributionCollection_[conditioningDimension].computePDF(x);
  // General case
  Point u(conditioningDimension);
  for (UnsignedInteger i = 0; i < conditioningDimension; ++i) u[i] = distributionCollection_[i].computeCDF(y[i]);
  const Scalar pdfX = distributionCollection_[conditioningDimension].computePDF(x);
  if (pdfX == 0.0)
    return 0.0;
  const Scalar corePDF = core_.computeConditionalPDF(distributionCollection_[conditioningDimension].computeCDF(x), u);
  return pdfX * corePDF;
}

Point JointDistribution::computeSequentialConditionalPDF(const Point & x) const
{
  if (x.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: cannot compute sequential conditional PDF with an argument of dimension=" << x.getDimension() << " different from distribution dimension=" << dimension_;
  Point result(dimension_);
  if (core_.isCopula() && hasIndependentCopula())
  {
    for (UnsignedInteger i = 0; i < dimension_; ++i)
      result[i] = distributionCollection_[i].computePDF(x[i]);
  }
  else
  {
    Point u(dimension_);
    for (UnsignedInteger i = 0; i < dimension_; ++i)
      u[i] = distributionCollection_[i].computeCDF(x[i]);
    const Point corePDF(core_.computeSequentialConditionalPDF(u));
    for (UnsignedInteger i = 0; i < dimension_; ++i)
    {
      result[i] = distributionCollection_[i].computePDF(x[i]) * corePDF[i];
      if (result[i] == 0.0) break;
    }
  }
  return result;
}

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar JointDistribution::computeConditionalCDF(const Scalar x, const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional CDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if (core_.isCopula() && ((conditioningDimension == 0) || hasIndependentCopula())) return distributionCollection_[conditioningDimension].computeCDF(x);
  // General case$
  Point u(conditioningDimension);
  for (UnsignedInteger i = 0; i < conditioningDimension; ++i) u[i] = distributionCollection_[i].computeCDF(y[i]);
  return core_.computeConditionalCDF(distributionCollection_[conditioningDimension].computeCDF(x), u);
}

Point JointDistribution::computeSequentialConditionalCDF(const Point & x) const
{
  if (x.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: cannot compute sequential conditional CDF with an argument of dimension=" << x.getDimension() << " different from distribution dimension=" << dimension_;
  Point u(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i)
    u[i] = distributionCollection_[i].computeCDF(x[i]);
  if (core_.isCopula() && hasIndependentCopula()) return u;
  return core_.computeSequentialConditionalCDF(u);
}

/* Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
/* Fk|1,...,k-1(x_k|x_1,...,x_{k-1})=Ck|1,...,k-1(F_k(x_k)|u_1=F_1(x_1),...,u_{k-1}=F_{k-1}(x_{k-1}))
   Fk|1,...,k-1(Qk|1,...,k-1(q)|x_1,...,x_{k-1})=Ck|1,...,k-1(u_k=F_k(x_k)|u_1=F_1(x_1),...,u_{k-1}=F_{k-1}(x_{k-1}))
*/
Scalar JointDistribution::computeConditionalQuantile(const Scalar q,
    const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional CDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if (core_.isCopula() && ((conditioningDimension == 0) || hasIndependentCopula())) return distributionCollection_[conditioningDimension].computeScalarQuantile(q);
  // General case
  Point u(conditioningDimension);
  for (UnsignedInteger i = 0; i < conditioningDimension; ++i) u[i] = distributionCollection_[i].computeCDF(y[i]);
  return distributionCollection_[conditioningDimension].computeScalarQuantile(core_.computeConditionalQuantile(q, u));
}

Point JointDistribution::computeSequentialConditionalQuantile(const Point & q) const
{
  if (q.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: cannot compute sequential conditional quantile with an argument of dimension=" << q.getDimension() << " different from distribution dimension=" << dimension_;
  Point result(dimension_);
  if (core_.isCopula() && hasIndependentCopula())
  {
    for (UnsignedInteger i = 0; i < dimension_; ++i)
      result[i] = distributionCollection_[i].computeScalarQuantile(q[i]);
  }
  else
  {
    const Point coreQuantile(core_.computeSequentialConditionalQuantile(q));
    for (UnsignedInteger i = 0; i < dimension_; ++i)
      result[i] = distributionCollection_[i].computeScalarQuantile(coreQuantile[i]);
  }
  return result;
}

/* Compute the numerical range of the distribution given the parameters values */
void JointDistribution::computeRange()
{
  const UnsignedInteger dimension = getDimension();
  Point lowerBound(dimension);
  Point upperBound(dimension);
  Interval::BoolCollection finiteLowerBound(dimension);
  Interval::BoolCollection finiteUpperBound(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Interval atomRange(distributionCollection_[i].getRange());
    lowerBound[i] = atomRange.getLowerBound()[0];
    upperBound[i] = atomRange.getUpperBound()[0];
    finiteLowerBound[i] = atomRange.getFiniteLowerBound()[0];
    finiteUpperBound[i] = atomRange.getFiniteUpperBound()[0];
  }
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}

/* Compute the mean of the distribution. It is cheap if the marginal means are cheap */
void JointDistribution::computeMean() const
{
  const UnsignedInteger dimension = getDimension();
  mean_ = Point(dimension);
  // Shortcut in the copula case
  if (core_.isCopula())
  {
    for (UnsignedInteger i = 0; i < dimension; ++i) mean_[i] = distributionCollection_[i].getMean()[0];
    isAlreadyComputedMean_ = true;
  }
  // Otherwise we have to take the margins of the core into account
  else
    DistributionImplementation::computeMean();
}

/* Compute the entropy of the distribution */
Scalar JointDistribution::computeEntropy() const
{
  Scalar entropy = core_.computeEntropy();
  for (UnsignedInteger i = 0; i < getDimension(); ++i)
    entropy += distributionCollection_[i].computeEntropy();
  return entropy;
}

/* Get the standard deviation of the distribution */
Point JointDistribution::getStandardDeviation() const
{
  // Shortcut in the copula case
  if (core_.isCopula())
  {
    const UnsignedInteger dimension = getDimension();
    Point standardDeviation(dimension);
    for (UnsignedInteger i = 0; i < dimension; ++i) standardDeviation[i] = distributionCollection_[i].getStandardDeviation()[0];
    return standardDeviation;
  }
  // Otherwise we have to take the margins of the core into account
  else
    return DistributionImplementation::getStandardDeviation();
}

/* Compute the covariance of the distribution */
void JointDistribution::computeCovariance() const
{
  const UnsignedInteger dimension = getDimension();
  // We need this to initialize the covariance matrix in two cases:
  // + this is the first call to this routine (which could be checked by testing the dimension of the distribution and the dimension of the matrix
  // + the core has changed from a non-independent one to the independent copula
  covariance_ = CovarianceMatrix(dimension);
  // If the dimension is 1, either the core is a copula and the covariance is simply the covariance of distributionCollection_[0]
  if (dimension_ == 1)
  {
    // Copula case
    if (core_.isCopula())
    {
      covariance_(0, 0) = distributionCollection_[0].getCovariance()(0, 0);
      isAlreadyComputedCovariance_ = true;
    }
    else
      // Here we must use the generic implementation to avoid recursive calls
      // and to take into account core margins
      DistributionImplementation::computeCovariance();
    return;
  }
  // First the diagonal terms, which are the marginal covariances
  for (UnsignedInteger component = 0; component < dimension; ++component) covariance_(component, component) = getMarginal(component).getCovariance()(0, 0);
  // Off-diagonal terms if the core is not the independent copula
  if (!hasIndependentCopula())
  {
    // Special case: elliptical distribution. The covariance is the shape matrix of the associated core, scaled by the marginal standard deviations
    if (isElliptical())
    {
      const CovarianceMatrix shape(core_.getShapeMatrix());
      for (UnsignedInteger rowIndex = 0; rowIndex < dimension; ++rowIndex)
        for (UnsignedInteger columnIndex = rowIndex + 1; columnIndex < dimension; ++columnIndex)
          covariance_(rowIndex, columnIndex) = shape(rowIndex, columnIndex) * std::sqrt(covariance_(rowIndex, rowIndex) * covariance_(columnIndex, columnIndex));
      return;
    }
    if (!core_.isCopula() || ResourceMap::GetAsBool("JointDistribution-UseGenericCovarianceAlgorithm"))
    {
      LOGINFO("JointDistribution: using the generic covariance algorithm");
      DistributionImplementation::computeCovariance();
      return;
    }
    LOGINFO("JointDistribution: using the specific covariance algorithm");
    // Here we use the following expression of the covariance \Sigma_{i,j}:
    // \Sigma_{i,j}=\int_{\R^2}(x_i-\mu_i)(x_j-\mu_j)p_{i,j}(x_i,x_j)dx_idx_j
    //             =\int_{\R^2}(x_i-\mu_i)(x_j-\mu_j)p_i(x_i)p_j(x_j}c_{i,j}(F_i(x_i),F_j(x_j))dx_idx_j
    // Let u_i=F_i(x_i) and u_j=F_j(u_j) so du_idu_j=p_i(x_i)p_j(x_j)dx_idx_j
    // \Sigma_{i,j}=\int_{[0,1]^2}(F_i^{-1}(u_i)-\mu_i)(F_j^{-1}(u_j)-\mu_j)c_{i,j}(u_i,u_j)du_idu_j

    // To ensure that the mean is up to date
    mean_ = getMean();
    // Compute the weights and nodes of the 1D gauss quadrature over [-1, 1]
    // Generate only the square-root of integrationNodesNumber_ 1D nodes in
    // order to have a total workload of integrationNodesNumber_
    const UnsignedInteger oldIntegrationNodesNumber = integrationNodesNumber_;
    integrationNodesNumber_ = static_cast<UnsignedInteger>(std::ceil(std::sqrt(1.0 * integrationNodesNumber_)));
    Point gaussWeights;
    Point gaussNodes(getGaussNodesAndWeights(gaussWeights));
    integrationNodesNumber_ = oldIntegrationNodesNumber;
    // Convert the nodes and weights for the interval [0, 1]
    for (UnsignedInteger i = 0; i < gaussWeights.getSize(); ++i)
    {
      gaussNodes[i] = 0.5 * (gaussNodes[i] + 1.0);
      gaussWeights[i] *= 0.5;
    }
    // Compute the marginal quantiles at the nodes
    Sample marginalQuantiles(gaussWeights.getSize(), dimension);
    Sample marginalPDF(gaussWeights.getSize(), dimension);
    for(UnsignedInteger component = 0; component < dimension; ++component)
    {
      const Distribution marginalDistribution(getMarginal(component));
      for(UnsignedInteger nodeIndex = 0; nodeIndex < gaussWeights.getSize(); ++nodeIndex)
      {
        const Scalar node = gaussNodes[nodeIndex];
        const Point q(marginalDistribution.computeQuantile(node));
        marginalQuantiles(nodeIndex, component) = q[0];
        marginalPDF(nodeIndex, component) = marginalDistribution.computePDF(q);
      }
    }
    // Performs the integration for each covariance in the strictly lower triangle of the covariance matrix
    // We simply use a product gauss quadrature
    // We first loop over the coeeficients because the most expensive task is to get the 2D marginal copulas
    Indices indices(2);
    // Prepare the 2D integration nodes and weights in order to use potential parallelism in 2D marginal pdf computation
    Sample nodes2D(gaussWeights.getSize() * gaussWeights.getSize(), 2);
    Point weights2D(gaussWeights.getSize() * gaussWeights.getSize());
    UnsignedInteger index = 0;
    for (UnsignedInteger rowNodeIndex = 0; rowNodeIndex < gaussWeights.getSize(); ++rowNodeIndex)
    {
      const Scalar nodeI = gaussNodes[rowNodeIndex];
      const Scalar weightI = gaussWeights[rowNodeIndex];
      for (UnsignedInteger columnNodeIndex = 0; columnNodeIndex < gaussWeights.getSize(); ++columnNodeIndex)
      {
        const Scalar nodeJ = gaussNodes[columnNodeIndex];
        const Scalar weightJ = gaussWeights[columnNodeIndex];
        nodes2D(index, 0) = nodeI;
        nodes2D(index, 1) = nodeJ;
        weights2D[index] = weightI * weightJ;
        ++index;
      } // loop over J integration nodes
    } // loop over I integration nodes
    // Now perform the integration for each component of the covariance matrix
    for (UnsignedInteger rowIndex = 0; rowIndex < dimension; ++rowIndex)
    {
      indices[0] = rowIndex;
      const Scalar muI = mean_[rowIndex];
      // We must fill the upper triangle of the covariance matrix in order to access the 2D marginal distributions
      // of the copula in the correct order for the BlockIndependentCopula
      for (UnsignedInteger columnIndex = rowIndex + 1; columnIndex < dimension; ++columnIndex)
      {
        indices[1] = columnIndex;
        const Scalar muJ = mean_[columnIndex];
        const Distribution marginalCopula(core_.getMarginal(indices));
        if (!marginalCopula.hasIndependentCopula())
        {
          LOGINFO(OSS() << "Compute covariance(" << rowIndex << ", " << columnIndex << ")");
          const Point pdf2D(marginalCopula.computePDF(nodes2D).getImplementation()->getData());
          Scalar covarianceIJ = 0.0;
          // Then we loop over the integration points
          index = 0;
          for (UnsignedInteger rowNodeIndex = 0; rowNodeIndex < gaussWeights.getSize(); ++rowNodeIndex)
          {
            for (UnsignedInteger columnNodeIndex = 0; columnNodeIndex < gaussWeights.getSize(); ++columnNodeIndex)
            {
              covarianceIJ += weights2D[index] * (marginalQuantiles(rowNodeIndex, rowIndex) - muI) * (marginalQuantiles(columnNodeIndex, columnIndex) - muJ) * pdf2D[index];
              ++index;
            } // loop over J integration nodes
          } // loop over I integration nodes
          LOGINFO(OSS() << "Covariance(" << rowIndex << ", " << columnIndex << ")=" << covarianceIJ);
          covariance_(rowIndex, columnIndex) = covarianceIJ;
        }
      } // loop over column indices
    } // loop over row indices
  } // if !hasIndependentCopula
  isAlreadyComputedCovariance_ = true;
} // computeCovariance

/* Get the skewness of the distribution */
Point JointDistribution::getSkewness() const
{
  // Shortcut in the copula case
  if (core_.isCopula())
  {
    const UnsignedInteger dimension = getDimension();
    Point skewness(dimension);
    for (UnsignedInteger i = 0; i < dimension; ++i) skewness[i] = distributionCollection_[i].getSkewness()[0];
    return skewness;
  }
  // Otherwise we have to take the margins of the core into account
  else
    return DistributionImplementation::getSkewness();
}

/* Get the kurtosis of the distribution */
Point JointDistribution::getKurtosis() const
{
  // Shortcut in the copula case
  if (core_.isCopula())
  {
    const UnsignedInteger dimension = getDimension();
    Point kurtosis(dimension);
    for (UnsignedInteger i = 0; i < dimension; ++i) kurtosis[i] = distributionCollection_[i].getKurtosis()[0];
    return kurtosis;
  }
  // Otherwise we have to take the margins of the core into account
  else
    return DistributionImplementation::getKurtosis();
}

/* Get the i-th marginal distribution */
Distribution JointDistribution::getMarginal(const UnsignedInteger i) const
{
  if (i >= getDimension()) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  if (core_.isCopula())
  {
    Distribution marginal(distributionCollection_[i]);
    marginal.setDescription({getDescription()[i]});
    return marginal;
  }
  JointDistribution marginal({distributionCollection_[i]}, core_.getMarginal(i));
  marginal.setDescription({getDescription()[i]});
  return marginal;
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution JointDistribution::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension = getDimension();
  if (!indices.check(dimension)) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal distribution must be in the range [0, dim-1] and must be different";
  const UnsignedInteger size = indices.getSize();
  if (size == 1)
    return getMarginal(indices[0]);
  JointDistribution marginal(distributionCollection_.select(indices), core_.getMarginal(indices));
  marginal.setDescription(getDescription().select(indices));
  return marginal;
}

/* Get the isoprobabilistic transformation */
JointDistribution::IsoProbabilisticTransformation JointDistribution::getIsoProbabilisticTransformation() const
{
  const UnsignedInteger dimension = getDimension();
  // Set the parameters values and descriptions
  PointWithDescriptionCollection parametersCollection(getParametersCollection());
  // First, compute the dimension of the marginal parameters space
  const UnsignedInteger size = parametersCollection.getSize();
  Point parameters(0);
  Description description(0);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const PointWithDescription marginalParameters(parametersCollection[i]);
    const Description marginalDescription(marginalParameters.getDescription());
    const UnsignedInteger marginalDimension = marginalParameters.getDimension();
    const String marginalName(marginalParameters.getName());
    for (UnsignedInteger j = 0; j < marginalDimension; ++j)
    {
      parameters.add(marginalParameters[j]);
      description.add(OSS() << marginalName << "_" << marginalDescription[j]);
    }
  }
  // Special case for the independent copula case: marginal transformations only to go to the spherical distribution
  if (core_.isCopula() && hasIndependentCopula())
  {
    // Get the evaluation implementations
    MarginalTransformationEvaluation evaluation(distributionCollection_, MarginalTransformationEvaluation::FROM, Normal());
    IsoProbabilisticTransformation marginalTransformation(evaluation.clone(), new MarginalTransformationGradient(evaluation), new MarginalTransformationHessian(evaluation));
    marginalTransformation.setParameter(parameters);
    marginalTransformation.setParameterDescription(description);
    return marginalTransformation;
  }
  // Special case for the elliptical distribution case: linear transformation
  if (isElliptical())
  {
    const TriangularMatrix inverseCholesky(getInverseCholesky());
    const Point mean(getMean());
    IsoProbabilisticTransformation transform;
    transform.setEvaluation(new NatafEllipticalDistributionEvaluation(mean, inverseCholesky));
    transform.setGradient(new NatafEllipticalDistributionGradient(inverseCholesky));
    transform.setHessian(new NatafEllipticalDistributionHessian(dimension));
    transform.setParameter(parameters);
    transform.setParameterDescription(description);
    return transform;
  }
  // Special case for the elliptical copula case: generalized Nataf transformation (marginal transformations plus linear transformation)
  if (core_.isCopula() && hasEllipticalCopula())
  {
    // Standard distribution
    const Distribution standardDistribution(getStandardDistribution());
    // Get the evaluation implementations
    const Distribution standardMarginal(standardDistribution.getMarginal(0));
    MarginalTransformationEvaluation evaluation(distributionCollection_, MarginalTransformationEvaluation::FROM, standardMarginal);
    IsoProbabilisticTransformation marginalTransformation(evaluation.clone(), MarginalTransformationGradient(evaluation).clone(), MarginalTransformationHessian(evaluation).clone());
    marginalTransformation.setParameter(parameters);
    marginalTransformation.setParameterDescription(description);
    // Suppress the correlation between the components.
    const TriangularMatrix inverseCholesky(core_.getShapeMatrix().computeCholesky().solveLinearSystem(IdentityMatrix(dimension)).getImplementation());
    LinearFunction linear(Point(dimension, 0.0), Point(dimension, 0.0), inverseCholesky);
    return ComposedFunction(linear, marginalTransformation);
  }
  // General case: go to uniform marginal distributions using marginal transformations, then use the isoprobabilistic transformation of the core
  // Get the IsoProbabilisticTransformation from the core
  const IsoProbabilisticTransformation coreIsoprobabilisticTransformation(core_.getIsoProbabilisticTransformation());
  // Get the right function implementations
  const MarginalTransformationEvaluation evaluation(distributionCollection_);
  IsoProbabilisticTransformation marginalTransformation(evaluation.clone(), new MarginalTransformationGradient(evaluation), new MarginalTransformationHessian(evaluation));
  marginalTransformation.setParameter(parameters);
  marginalTransformation.setParameterDescription(description);
  return ComposedFunction(coreIsoprobabilisticTransformation, marginalTransformation);
}

/* Get the inverse isoprobabilist transformation */
JointDistribution::InverseIsoProbabilisticTransformation JointDistribution::getInverseIsoProbabilisticTransformation() const
{
  const UnsignedInteger dimension = getDimension();
  // Set the parameters values and descriptions
  PointWithDescriptionCollection parametersCollection(getParametersCollection());
  // First, compute the dimension of the marginal parameters space
  const UnsignedInteger size = parametersCollection.getSize();
  Point parameters(0);
  Description description(0);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const PointWithDescription marginalParameters(parametersCollection[i]);
    const Description marginalDescription(marginalParameters.getDescription());
    const UnsignedInteger marginalDimension = marginalParameters.getDimension();
    const String marginalName(marginalParameters.getName());
    for (UnsignedInteger j = 0; j < marginalDimension; ++j)
    {
      parameters.add(marginalParameters[j]);
      description.add(OSS() << marginalName << "_" << marginalDescription[j]);
    }
  }
  // Special case for the independent copula case: marginal transformations only to go back from the spherical distribution
  if (core_.isCopula() && hasIndependentCopula())
  {
    // Get the evaluation implementations
    MarginalTransformationEvaluation evaluation(distributionCollection_, MarginalTransformationEvaluation::TO, Normal());
    IsoProbabilisticTransformation marginalTransformation(evaluation.clone(), new MarginalTransformationGradient(evaluation), new MarginalTransformationHessian(evaluation));
    marginalTransformation.setParameter(parameters);
    marginalTransformation.setParameterDescription(description);
    return marginalTransformation;
  }
  // Special case for the elliptical distribution case: linear transformation
  if (isElliptical())
  {
    const TriangularMatrix cholesky(getCholesky());
    const Point mean(getMean());
    InverseIsoProbabilisticTransformation inverseTransform;
    inverseTransform.setEvaluation(new InverseNatafEllipticalDistributionEvaluation(mean, cholesky));
    inverseTransform.setGradient(new InverseNatafEllipticalDistributionGradient(cholesky));
    inverseTransform.setHessian(new InverseNatafEllipticalDistributionHessian(dimension));
    inverseTransform.setParameter(parameters);
    inverseTransform.setParameterDescription(description);
    return inverseTransform;
  }
  // Special case for the elliptical copula case: generalized Nataf transformation (marginal transformations plus linear transformation)
  if (core_.isCopula() && hasEllipticalCopula())
  {
    // Standard distribution
    const Distribution standardDistribution(getStandardDistribution());
    // Get the evaluation implementations
    const Distribution standardMarginal(standardDistribution.getMarginal(0));
    MarginalTransformationEvaluation evaluation(distributionCollection_, MarginalTransformationEvaluation::TO, standardMarginal);
    InverseIsoProbabilisticTransformation marginalTransformation(evaluation.clone(), new MarginalTransformationGradient(evaluation), new MarginalTransformationHessian(evaluation));
    marginalTransformation.setParameter(parameters);
    marginalTransformation.setParameterDescription(description);
    // Suppress the correlation between the components.
    const TriangularMatrix cholesky(core_.getShapeMatrix().computeCholesky());
    LinearFunction linear(Point(dimension, 0.0), Point(dimension, 0.0), cholesky);
    return ComposedFunction(marginalTransformation, linear);
  }
  // General case: go to the core using its inverse isoprobabilistic transformation, then add the correct marginal distributions using marginal transformations
  // Get the InverseIsoProbabilisticTransformation from the copula
  const InverseIsoProbabilisticTransformation coreInverseIsoprobabilisticTransformation(core_.getInverseIsoProbabilisticTransformation());
  // Build the marginal transformation
  const MarginalTransformationEvaluation evaluation(distributionCollection_, MarginalTransformationEvaluation::TO);
  InverseIsoProbabilisticTransformation marginalTransformation(evaluation.clone(), new MarginalTransformationGradient(evaluation), new MarginalTransformationHessian(evaluation));
  marginalTransformation.setParameter(parameters);
  marginalTransformation.setParameterDescription(description);
  return ComposedFunction(marginalTransformation, coreInverseIsoprobabilisticTransformation);
}

/* Get the standard distribution */
Distribution JointDistribution::getStandardDistribution() const
{
  return getCopula().getStandardDistribution();
}

/* Parameters value and description accessor */
JointDistribution::PointWithDescriptionCollection JointDistribution::getParametersCollection() const
{
  const UnsignedInteger dimension = getDimension();
  PointWithDescriptionCollection parameters(dimension + (dimension > 1 ? 1 : 0));
  const Description description(getDescription());
  // First put the marginal parameters
  for (UnsignedInteger marginalIndex = 0; marginalIndex < dimension; ++marginalIndex)
  {
    // Each marginal distribution must output a collection of parameters of size 1, even if it contains an empty Point
    const PointWithDescriptionCollection marginalParameters(distributionCollection_[marginalIndex].getParametersCollection());
    PointWithDescription point(marginalParameters[0]);
    Description marginalParametersDescription(point.getDescription());
    // Here we must add a unique prefix to the marginal parameters description in order to desambiguate the parameters of different marginals sharing the same description
    for (UnsignedInteger i = 0; i < point.getDimension(); ++i) marginalParametersDescription[i] = (OSS() << marginalParametersDescription[i] << "_marginal_" << marginalIndex);
    point.setDescription(marginalParametersDescription);
    point.setName(description[marginalIndex]);
    parameters[marginalIndex] = point;
  } // marginalIndex
  if (dimension > 1)
  {
    // Second put the dependence parameters
    PointWithDescription point(core_.getParametersCollection()[0]);
    Description coreParametersDescription(point.getDescription());
    // Here we must add a unique prefix to the core parameters description in order to disambiguate the parameters of marginals sharing the same description
    for (UnsignedInteger i = 0; i < point.getDimension(); ++i) coreParametersDescription[i] = (OSS() << coreParametersDescription[i] << (core_.isCopula() ? "_copula" : "_core"));
    point.setDescription(coreParametersDescription);
    point.setName(core_.getName());
    parameters[dimension] = point;
  } // dimension > 1
  return parameters;
} // getParametersCollection


void JointDistribution::setParametersCollection(const PointCollection& parametersCollection)
{
  const UnsignedInteger dimension = getDimension();
  const UnsignedInteger parametersSize = dimension + (dimension > 1 ? 1 : 0);
  if (parametersCollection.getSize() < parametersSize) throw InvalidArgumentException(HERE) << "The collection is too small(" << parametersCollection.getSize() << "). Expected (" << parametersSize << ")";

  // set marginal parameters
  for (UnsignedInteger marginalIndex = 0; marginalIndex < dimension; ++marginalIndex) distributionCollection_[marginalIndex].setParameter(parametersCollection[marginalIndex]);
  // set core parameters
  if (dimension > 1) core_.setParameter(parametersCollection[dimension]);
}


Point JointDistribution::getParameter() const
{
  const UnsignedInteger dimension = getDimension();
  Point point;
  for (UnsignedInteger marginalIndex = 0; marginalIndex < dimension; ++ marginalIndex)
  {
    point.add(distributionCollection_[marginalIndex].getParameter());
  }
  if (dimension > 1)
  {
    point.add(core_.getParameter());
  }
  return point;
}

void JointDistribution::setParameter(const Point & parameter)
{
  const UnsignedInteger dimension = getDimension();
  UnsignedInteger globalIndex = 0;
  for (UnsignedInteger marginalIndex = 0; marginalIndex < dimension; ++ marginalIndex)
  {
    const UnsignedInteger parametersSize = distributionCollection_[marginalIndex].getParameterDimension();
    if (globalIndex + parametersSize > parameter.getSize()) throw InvalidArgumentException(HERE) << "Not enough values (" << parameter.getSize() << "), needed " << globalIndex + parametersSize << " for marginal " << marginalIndex;
    Point newParameters(parametersSize);
    std::copy(parameter.begin() + globalIndex, parameter.begin() + globalIndex + parametersSize, newParameters.begin());
    distributionCollection_[marginalIndex].setParameter(newParameters);
    globalIndex += parametersSize;
  }
  if (dimension > 1)
  {
    const UnsignedInteger parametersSize = core_.getParameterDimension();
    if (globalIndex + parametersSize > parameter.getSize()) throw InvalidArgumentException(HERE) << "Not enough values (" << parameter.getSize() << "), needed " << globalIndex + parametersSize << " for " << (core_.isCopula() ? "copula" : "core");
    Point newParameters(parametersSize);
    std::copy(parameter.begin() + globalIndex, parameter.begin() + globalIndex + parametersSize, newParameters.begin());
    core_.setParameter(newParameters);
  }
}

Description JointDistribution::getParameterDescription() const
{
  const UnsignedInteger dimension = getDimension();
  Description description;
  for (UnsignedInteger marginalIndex = 0; marginalIndex < dimension; ++ marginalIndex)
  {
    Description marginalParametersDescription(distributionCollection_[marginalIndex].getParameterDescription());
    for (UnsignedInteger i = 0; i < marginalParametersDescription.getSize(); ++ i)
      description.add(OSS() << marginalParametersDescription[i] << "_marginal_" << marginalIndex);
  }
  if (dimension > 1)
  {
    Description coreParametersDescription(core_.getParameterDescription());
    for (UnsignedInteger i = 0; i < coreParametersDescription.getSize(); ++ i)
      description.add(OSS() << coreParametersDescription[i] << (core_.isCopula() ? "_copula" : "_core"));
  }
  return description;
}

/* Tell if the distribution has independent copula */
Bool JointDistribution::hasIndependentCopula() const
{
  return core_.hasIndependentCopula();
}

/* Tell if the distribution has elliptical copula */
Bool JointDistribution::hasEllipticalCopula() const
{
  return core_.hasEllipticalCopula();
}

/* Check if the distribution is elliptical */
Bool JointDistribution::isElliptical() const
{
  const Bool ellipticalCopula = core_.isCopula() && core_.hasEllipticalCopula();
  if (!ellipticalCopula) return false;
  const String copulaKind(core_.getImplementation()->getClassName());
  // Easy case: Normal or independent copula with Normal marginals
  const Bool hasNormalCopula = (copulaKind == NormalCopula::GetClassName()) || hasIndependentCopula();
  Bool hasNormalMarginals = true;
  for (UnsignedInteger i = 0; i < getDimension(); ++i)
  {
    const String currentMarginalKind(distributionCollection_[i].getImplementation()->getClassName());
    hasNormalMarginals = hasNormalMarginals && (currentMarginalKind == Normal::GetClassName());
  }
  // For now, we are not smart enough to detect a fully general elliptical copula. The general
  // way to do it is to compare the density generator of the marginals and the copula.
  return hasNormalCopula && hasNormalMarginals;
}

/* Check if the distribution is continuous */
Bool JointDistribution::isContinuous() const
{
  const UnsignedInteger dimension = getDimension();
  if (!core_.isContinuous()) return false;
  for (UnsignedInteger i = 0; i < dimension; ++i) if (!distributionCollection_[i].isContinuous()) return false;
  return true;
}

/* Check if the distribution is discrete */
Bool JointDistribution::isDiscrete() const
{
  const UnsignedInteger dimension = getDimension();
  for (UnsignedInteger i = 0; i < dimension; ++i) if (!distributionCollection_[i].isDiscrete()) return false;
  return true;
}

/* Tell if the distribution is integer valued */
Bool JointDistribution::isIntegral() const
{
  const UnsignedInteger dimension = getDimension();
  for (UnsignedInteger i = 0; i < dimension; ++i) if (!distributionCollection_[i].isIntegral()) return false;
  return true;
}

/* Method save() stores the object through the StorageManager */
void JointDistribution::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute( "distributionCollection_", distributionCollection_ );
  adv.saveAttribute( "core_", core_ );
}

/* Method load() reloads the object from the StorageManager */
void JointDistribution::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute( "distributionCollection_", distributionCollection_ );
  if (adv.hasAttribute("copula_"))
    adv.loadAttribute( "copula_", core_ );
  else
    adv.loadAttribute( "core_", core_ );
  computeRange();
}

CLASSNAMEINIT(ComposedDistribution)

static const Factory<ComposedDistribution> Factory_ComposedDistribution;

END_NAMESPACE_OPENTURNS
