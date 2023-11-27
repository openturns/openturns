//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all distributions
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
#include <cstdlib>
#include <algorithm>

#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/DistributionImplementation.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Log.hxx"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/Collection.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/CompositeDistribution.hxx"
#include "openturns/Chi.hxx"
#include "openturns/ChiSquare.hxx"
#include "openturns/ComposedDistribution.hxx"
#include "openturns/Dirac.hxx"
#include "openturns/LogNormal.hxx"
#include "openturns/LogUniform.hxx"
#include "openturns/Mixture.hxx"
#include "openturns/Normal.hxx"
#include "openturns/RandomMixture.hxx"
#include "openturns/MaximumDistribution.hxx"
#include "openturns/ProductDistribution.hxx"
#include "openturns/SquaredNormal.hxx"
#include "openturns/TruncatedDistribution.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/IndependentCopula.hxx"
#include "openturns/MarginalDistribution.hxx"
#include "openturns/MarginalTransformationEvaluation.hxx"
#include "openturns/MarginalTransformationGradient.hxx"
#include "openturns/MarginalTransformationHessian.hxx"
#include "openturns/RosenblattEvaluation.hxx"
#include "openturns/InverseRosenblattEvaluation.hxx"
#include "openturns/Function.hxx"
#include "openturns/SklarCopula.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PlatformInfo.hxx"
#include "openturns/Cloud.hxx"
#include "openturns/Contour.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Polygon.hxx"
#include "openturns/Staircase.hxx"
#include "openturns/Drawable.hxx"
#include "openturns/Graph.hxx"
#include "openturns/Brent.hxx"
#include "openturns/Box.hxx"
#include "openturns/Tuples.hxx"
#include "openturns/Combinations.hxx"
#include "openturns/TBBImplementation.hxx"
#include "openturns/GaussKronrod.hxx"
#include "openturns/GaussLegendre.hxx"
#include "openturns/IteratedQuadrature.hxx"
#include "openturns/OptimizationProblem.hxx"
#include "openturns/Cobyla.hxx"
#include "openturns/TriangularMatrix.hxx"
#include "openturns/MethodBoundEvaluation.hxx"
#include "openturns/SobolSequence.hxx"
#include "openturns/SymbolicFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DistributionImplementation)

typedef Collection<Distribution>                                      DistributionCollection;

static const Factory<DistributionImplementation> Factory_DistributionImplementation;

/* Default constructor */
DistributionImplementation::DistributionImplementation()
  : PersistentObject()
  , mean_(Point(1, 0.0))
  , covariance_(CovarianceMatrix(1))
  , gaussNodes_()
  , gaussWeights_()
  , integrationNodesNumber_(ResourceMap::GetAsUnsignedInteger("Distribution-DefaultIntegrationNodesNumber"))
  , isAlreadyComputedMean_(false)
  , isAlreadyComputedCovariance_(false)
  , isAlreadyComputedGaussNodesAndWeights_(false)
  , pdfEpsilon_(ResourceMap::GetAsScalar("Distribution-DefaultPDFEpsilon"))
  , cdfEpsilon_(ResourceMap::GetAsScalar("Distribution-DefaultCDFEpsilon"))
  , quantileEpsilon_(ResourceMap::GetAsScalar("Distribution-DefaultQuantileEpsilon"))
  , quantileIterations_(ResourceMap::GetAsUnsignedInteger("Distribution-DefaultQuantileIteration"))
  , isAlreadyComputedStandardDistribution_(false)
  , p_standardDistribution_()
  , isAlreadyCreatedGeneratingFunction_(false)
  , generatingFunction_(0)
  , dimension_(1)
  , weight_(1.0)
    // The range is empty by default
  , range_(Interval(1.0, -1.0))
  , description_(1)
  , isParallel_(ResourceMap::GetAsBool("Distribution-Parallel"))
  , isCopula_(false)
{
  description_[0] = "X0";
}

/* Virtual constructor */
DistributionImplementation * DistributionImplementation::clone() const
{
  return new DistributionImplementation(*this);
}

/* Comparison operator */
Bool DistributionImplementation::operator ==(const DistributionImplementation & other) const
{
  if (this == &other) return true;
  // Compare both this to other and other to this to ensure symmetry
  return equals(other) && other.equals(*this);
}

Bool DistributionImplementation::equals(const DistributionImplementation & ) const
{
  throw NotYetImplementedException(HERE) << "In DistributionImplementation::equals";
}

/* Comparison operator */
Bool DistributionImplementation::operator !=(const DistributionImplementation & other) const
{
  return !operator==(other);
}

/* Addition operator */
Distribution DistributionImplementation::operator + (const Distribution & other) const
{
  return operator + (*(other.getImplementation()));
}

Distribution DistributionImplementation::operator + (const DistributionImplementation & other) const
{
  if (dimension_ != other.dimension_)
    throw InvalidDimensionException(HERE) << "Can only sum distributions with the same dimension:" << dimension_ <<  " vs " << other.dimension_;

  if (dimension_ == 1)
  {
    if (getClassName() == "Dirac") return other + getRealization()[0];
    Collection< Distribution > coll(2);
    coll[0] = *this;
    coll[1] = other.clone();
    RandomMixture res(coll);
    // Check if a simplification has occurred
    if ((res.getDistributionCollection().getSize() == 1) && (res.getWeights()(0, 0) == 1.0) && (res.getConstant()[0] == 0.0))
      return res.getDistributionCollection()[0];
    // No simplification
    return res.clone();
  }

  if (!hasIndependentCopula() || !other.hasIndependentCopula())
    throw NotYetImplementedException(HERE) << "Can only sum distributions with independent copulas";

  Collection< Distribution > marginals(dimension_);
  for (UnsignedInteger j = 0; j < dimension_; ++ j)
    marginals[j] = getMarginal(j) + other.getMarginal(j);
  return new ComposedDistribution(marginals);
}

Distribution DistributionImplementation::operator + (const Scalar value) const
{
  if (value == 0.0) return clone();

  if (dimension_ == 1)
  {
    if (getClassName() == "Dirac") return new Dirac(getRealization()[0] + value);
    Collection< Distribution > coll(2);
    coll[0] = *this;
    coll[1] = Dirac(Point(1, value));
    RandomMixture res(coll);
    // Check if a simplification has occurred
    if ((res.getDistributionCollection().getSize() == 1) && (res.getWeights()(0, 0) == 1.0) && (res.getConstant()[0] == 0.0))
      return res.getDistributionCollection()[0];
    // No simplification
    return res.clone();
  }

  Collection< Distribution > marginals(dimension_);
  for (UnsignedInteger j = 0; j < dimension_; ++ j)
    marginals[j] = getMarginal(j) + value;
  return new ComposedDistribution(marginals, getCopula());
}

/* Subtraction operator */
Distribution DistributionImplementation::operator - (const Distribution & other) const
{
  return operator - (*(other.getImplementation()));
}

Distribution DistributionImplementation::operator - (const DistributionImplementation & other) const
{
  if (dimension_ != other.dimension_)
    throw InvalidDimensionException(HERE) << "Can only subtract distributions with the same dimension:" << dimension_ <<  " vs " << other.dimension_;

  Point weights(2);
  weights[0] = 1.0;
  weights[1] = -1.0;

  if (dimension_ == 1)
  {
    if (other.getClassName() == "Dirac") return *this + (-other.getRealization()[0]);
    Collection< Distribution > coll(2);
    coll[0] = *this;
    coll[1] = other.clone();
    RandomMixture res(coll, weights);
    // Check if a simplification has occurred
    if ((res.getDistributionCollection().getSize() == 1) && (res.getWeights()(0, 0) == 1.0) && (res.getConstant()[0] == 0.0))
      return res.getDistributionCollection()[0];
    // No simplification
    return res.clone();
  }

  if (!hasIndependentCopula() || !other.hasIndependentCopula())
    throw NotYetImplementedException(HERE) << "Can only subtract distributions with independent copulas";

  Collection< Distribution > marginals(dimension_);
  for (UnsignedInteger j = 0; j < dimension_; ++ j)
    marginals[j] = getMarginal(j) - other.getMarginal(j);
  return new ComposedDistribution(marginals);
}

Distribution DistributionImplementation::operator - (const Scalar value) const
{
  if (value == 0.0) return clone();
  return *this + (-value);
}

/* Multiplication operator */
Distribution DistributionImplementation::operator * (const Distribution & other) const
{
  return operator * (*(other.getImplementation()));
}

Distribution DistributionImplementation::operator * (const DistributionImplementation & other) const
{
  // Special case: Dirac distribution
  if ((getDimension() == 1) && (other.getDimension() == 1))
  {
    if (getClassName() == "Dirac")
      return other * getRealization()[0];
    if (other.getClassName() == "Dirac")
      return *this * other.getRealization()[0];
  }
  // Special case: LogNormal distributions
  if ((getClassName() == "LogNormal") && (other.getClassName() == "LogNormal"))
  {
    const Point parameters(getParameter());
    const Point otherParameters(other.getParameter());
    return new LogNormal(parameters[0] + otherParameters[0], std::sqrt(parameters[1] * parameters[1] + otherParameters[1] * otherParameters[1]));
  }
  if ((getClassName() == "LogUniform") && (other.getClassName() == "LogUniform"))
  {
    const Point parameters(getParameter());
    const Point otherParameters(other.getParameter());
    return (Uniform(parameters[0], parameters[1]) + Uniform(otherParameters[0], otherParameters[1])).exp();
  }
  if ((getClassName() == "LogUniform") && (other.getClassName() == "LogNormal"))
  {
    const Point parameters(getParameter());
    const Point otherParameters(other.getParameter());
    return (Uniform(parameters[0], parameters[1]) + Normal(otherParameters[0], otherParameters[1])).exp();
  }
  if ((getClassName() == "LogNormal") && (other.getClassName() == "LogUniform"))
  {
    const Point parameters(getParameter());
    const Point otherParameters(other.getParameter());
    return (Normal(parameters[0], parameters[1]) + Uniform(otherParameters[0], otherParameters[1])).exp();
  }
  return new ProductDistribution(*this, other.clone());
}

Distribution DistributionImplementation::operator * (const Scalar value) const
{
  if (dimension_ != 1) throw NotYetImplementedException(HERE) << "In DistributionImplementation::operator * (const Scalar value) const: can multiply by a constant 1D distributions only.";
  if (value == 0.0) return new Dirac(Point(1, 0.0));
  if (value == 1.0) return clone();
  if (getClassName() == "Dirac") return new Dirac(getRealization()[0] * value);
  const Collection< Distribution > coll(1, *this);
  const Point weight(1, value);
  RandomMixture res(coll, weight);
  // If the weight has been integrated into the unique atom and there is no constant
  if ((res.getWeights()(0, 0) == 1.0) && (res.getConstant()[0] == 0.0))
    return res.getDistributionCollection()[0];
  return res.clone();
}

/* Division operator */
Distribution DistributionImplementation::operator / (const Distribution & other) const
{
  return operator / (*(other.getImplementation()));
}

Distribution DistributionImplementation::operator / (const DistributionImplementation & other) const
{
  if ((dimension_ != 1) || (other.dimension_ != 1)) throw NotYetImplementedException(HERE) << "In DistributionImplementation::operator / (const DistributionImplementation & other) const: can multiply 1D distributions only.";
  if (other.getClassName() == "Dirac")
  {
    const Scalar otherValue = other.getRealization()[0];
    if (otherValue == 0.0) throw InvalidArgumentException(HERE) << "Error: cannot divide by a Dirac distribution located in 0";
    return *this * (1.0 / otherValue);
  }
  return operator * (other.inverse());
}

Distribution DistributionImplementation::operator / (const Scalar value) const
{
  if (dimension_ != 1) throw NotYetImplementedException(HERE) << "In DistributionImplementation::operator / (const Scalar value) const: can divide multiply by a constant 1D distributions only.";
  if (value == 0.0) throw InvalidArgumentException(HERE) << "Error: cannot divide by 0.";
  if (value == 1.0) return clone();
  return (*this) * (1.0 / value);
}

/* Product operator */
Distribution operator * (const Scalar scalar,
                         const DistributionImplementation & distribution)
{
  return distribution * scalar;
}

Distribution operator * (const Scalar scalar,
                         const DistributionImplementation::Implementation & p_distribution)
{
  return (*p_distribution) * scalar;
}

/* Division operator */
Distribution operator / (const Scalar scalar,
                         const DistributionImplementation & distribution)
{
  return (distribution.inverse()) * scalar;
}

Distribution operator / (const Scalar scalar,
                         const DistributionImplementation::Implementation & p_distribution)
{
  return (*p_distribution).inverse() * scalar;
}

/* Addition operator */
Distribution operator + (const Scalar scalar,
                         const DistributionImplementation & distribution)
{
  return distribution + scalar;
}

Distribution operator + (const Scalar scalar,
                         const DistributionImplementation::Implementation & p_distribution)
{
  return (*p_distribution) + scalar;
}

/* Subtraction operator */
Distribution operator - (const Scalar scalar,
                         const DistributionImplementation & distribution)
{
  return (distribution * (-1.0)) + scalar;
}

Distribution operator - (const Scalar scalar,
                         const DistributionImplementation::Implementation & p_distribution)
{
  return ((*p_distribution) * (-1.0)) + scalar;
}

Distribution operator - (const DistributionImplementation & distribution)
{
  return distribution * (-1.0);
}

Distribution operator - (const DistributionImplementation::Implementation & p_distribution)
{
  return (*p_distribution) * (-1.0);
}


Distribution maximum(const DistributionImplementation::Implementation & p_left,
                     const DistributionImplementation::Implementation & p_right)
{
  MaximumDistribution::DistributionCollection coll(2);
  coll[0] = p_left;
  coll[1] = p_right;
  return new MaximumDistribution(coll);
}

Distribution maximum(const DistributionImplementation & left,
                     const DistributionImplementation::Implementation & p_right)
{
  MaximumDistribution::DistributionCollection coll(2);
  coll[0] = left;
  coll[1] = p_right;
  return new MaximumDistribution(coll);
}

Distribution maximum(const DistributionImplementation::Implementation & p_left,
                     const DistributionImplementation & right)
{
  MaximumDistribution::DistributionCollection coll(2);
  coll[0] = p_left;
  coll[1] = right;
  return new MaximumDistribution(coll);
}

Distribution maximum(const DistributionImplementation & left,
                     const DistributionImplementation & right)
{
  MaximumDistribution::DistributionCollection coll(2);
  coll[0] = left;
  coll[1] = right;
  return new MaximumDistribution(coll);
}

/* String converter */
String DistributionImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << DistributionImplementation::GetClassName()
      << " description=" << description_;
  return oss;
}

/* String converter */
String DistributionImplementation::__str__(const String & ) const
{
  return __repr__();
}

String DistributionImplementation::_repr_html_() const
{
  OSS oss(false);
  oss << getClassName() << "\n";
  oss << "<ul>\n";
  oss << "  <li>name=" << getName() << "</li>\n";
  oss << "  <li>dimension=" << dimension_ << "</li>\n";
  oss << "  <li>weight=" << weight_ << "</li>\n";
  oss << "  <li>range=" << range_ << "</li>\n";
  oss << "  <li>description=" << description_ << "</li>\n";
  oss << "  <li>isParallel=" << isParallel_ << "</li>\n";
  oss << "  <li>isCopula=" << isCopula_ << "</li>\n";
  oss << "</ul>\n";
  return oss;
}

String DistributionImplementation::__repr_markdown__() const
{
  OSS oss(false);
  oss << getClassName() << "\n";
  oss << "- name=" << getName() << "\n";
  oss << "- dimension=" << dimension_ << "\n";
  oss << "- weight=" << weight_ << "\n";
  oss << "- range=" << range_ << "\n";
  oss << "- description=" << description_ << "\n";
  oss << "- isParallel=" << isParallel_ << "\n";
  oss << "- isCopula=" << isCopula_ << "\n";
  return oss;
}

/* Weight accessor */
void DistributionImplementation::setWeight(const Scalar w)
{
  weight_ = w;
}

/* Weight accessor */
Scalar DistributionImplementation::getWeight() const
{
  return weight_;
}


/* Dimension accessor */
UnsignedInteger DistributionImplementation::getDimension() const
{
  return dimension_;
}

/* Get the roughness, i.e. the L2-norm of the PDF */
Scalar DistributionImplementation::getRoughness() const
{
  // Use adaptive multidimensional integration of the PDF on the reduced interval
  const PDFSquaredWrapper pdfSquaredWrapper(this);
  Scalar roughness = 0.0;
  if (dimension_ == 1)
  {
    Scalar error = -1.0;
    const Point singularities(getSingularities());
    // If no singularity inside of the given reduced interval
    const UnsignedInteger singularitiesNumber = singularities.getSize();
    const Scalar lower = range_.getLowerBound()[0];
    const Scalar upper = range_.getUpperBound()[0];
    if (singularitiesNumber == 0 || singularities[0] >= upper || singularities[singularitiesNumber - 1] <= lower)
      roughness = GaussKronrod().integrate(pdfSquaredWrapper, range_, error)[0];
    else
    {
      Scalar a = lower;
      for (UnsignedInteger i = 0; i < singularitiesNumber; ++i)
      {
        const Scalar b = singularities[i];
        if (b > lower && b < upper)
        {
          roughness += GaussKronrod().integrate(pdfSquaredWrapper, Interval(a, b), error)[0];
          a = b;
        }
        // Exit the loop if no more singularities inside of the reduced interval
        if (b >= upper)
          break;
      } // for
      // Last contribution
      roughness += GaussKronrod().integrate(pdfSquaredWrapper, Interval(a, upper), error)[0];
    } // else
    return std::max(0.0, roughness);
  } // dimension_ == 1

  // Dimension > 1
  if (hasIndependentCopula())
  {
    roughness = 1.0;
    for (UnsignedInteger i = 0; i < dimension_; ++i)
      roughness *= getMarginal(i).getRoughness();
  }
  else
  {
    // Small dimension
    if (dimension_ <= ResourceMap::GetAsUnsignedInteger("Distribution-SmallDimensionRoughness"))
    {
      roughness = IteratedQuadrature().integrate(pdfSquaredWrapper, range_)[0];
    } // Small dimension
    else
    {
      const UnsignedInteger size = ResourceMap::GetAsUnsignedInteger("Distribution-RoughnessSamplingSize");
      const String samplingMethod(ResourceMap::GetAsString("Distribution-RoughnessSamplingMethod"));
      Sample sample;
      if (samplingMethod == "MonteCarlo")
        sample = getSample(size);
      else if (samplingMethod == "QuasiMonteCarlo")
        sample = getSampleByQMC(size);
      else
      {
        LOGWARN(OSS() << "Unknown sampling method=" << samplingMethod << " to compute roughness. Resort to MonteCarlo");
        sample = getSample(size);
      }
      roughness = computePDF(sample).computeMean()[0];
    }

  }
  // Roughness is a L2-norm, so must be positive
  return std::max(0.0, roughness);
}

/* Dimension accessor */
void DistributionImplementation::setDimension(const UnsignedInteger dim)
{
  if (dim == 0) throw InvalidArgumentException(HERE) << "Dimension argument must be an integer >= 1, here dim = " << dim;
  if (dim != dimension_)
  {
    dimension_ = dim;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    isAlreadyComputedGaussNodesAndWeights_ = false;
    // Check if the current description is compatible with the new dimension
    if (description_.getSize() != dim) description_ = Description::BuildDefault(dim, "X");
  }
}

/* Get one realization of the distribution */
Point DistributionImplementation::getRealization() const
{
  return getRealizationByInversion();
}

/* Get a numerical sample whose elements follow the distributionImplementation */
Sample DistributionImplementation::getSample(const UnsignedInteger size) const
{
  SampleImplementation returnSample(size, dimension_);
  UnsignedInteger shift = 0;
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    const Point point(getRealization());
    std::copy(point.begin(), point.end(), returnSample.data_begin() + shift);
    shift += dimension_;
  }
  returnSample.setName(getName());
  returnSample.setDescription(getDescription());
  return returnSample;
}

/* Get one realization of the distribution */
Point DistributionImplementation::getRealizationByInversion() const
{
  return getSampleByInversion(1)[0];
}

/* Get a numerical sample whose elements follow the distributionImplementation */
Sample DistributionImplementation::getSampleByInversion(const UnsignedInteger size) const
{
  SampleImplementation returnSample(size, dimension_);
  UnsignedInteger shift = 0;
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    const Point point(computeSequentialConditionalQuantile(RandomGenerator::Generate(dimension_)));
    std::copy(point.begin(), point.end(), returnSample.data_begin() + shift);
    shift += dimension_;
  }
  returnSample.setName(getName());
  returnSample.setDescription(getDescription());
  return returnSample;
}

Sample DistributionImplementation::getSampleByQMC(const UnsignedInteger size) const
{
  static SobolSequence sequence(dimension_);
  SampleImplementation returnSample(size, dimension_);
  const Sample u(sequence.generate(size));
  if (getDimension() == 1)
  {
    for (UnsignedInteger i = 0; i < size; ++ i)
      returnSample(i, 0) = computeScalarQuantile(u(i, 0));
  }
  else
  {
    UnsignedInteger shift = 0;
    for (UnsignedInteger i = 0; i < size; ++ i)
    {
      const Point point(computeSequentialConditionalQuantile(u[i]));
      std::copy(point.begin(), point.end(), returnSample.data_begin() + shift);
      shift += dimension_;
    }
  }
  returnSample.setName(getName());
  returnSample.setDescription(getDescription());
  return returnSample;
}

/* Get the DDF of the distribution */
Point DistributionImplementation::computeDDF(const Point & point) const
{
  if (isContinuous())
  {
    const UnsignedInteger dimension = getDimension();
    Point ddf(dimension);
    const Scalar h = std::pow(pdfEpsilon_, 1.0 / 3.0);
    LOGINFO(OSS() << "h=" << h);
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      Point left(point);
      left[i] += h;
      Point right(point);
      right[i] -= h;
      const Scalar denom = left[i] - right[i];
      const Scalar pdfLeft = computePDF(left);
      const Scalar pdfRight = computePDF(right);
      ddf[i] = (pdfLeft - pdfRight) / denom;
      LOGINFO(OSS() << "left=" << left << ", right=" << right << ", pdfLeft=" << pdfLeft << ", pdfRight=" << pdfRight);
    }
    return ddf;
  }
  if (dimension_ == 1)
  {
    Point ddf(dimension_);
    const Scalar cdfPoint = computeCDF(point);
    const Scalar h = std::pow(cdfEpsilon_, 0.25);
    const Scalar idenom = 1.0 / std::sqrt(cdfEpsilon_);
    for (UnsignedInteger i = 0; i < dimension_; ++ i)
    {
      Point epsilon(dimension_, 0.0);
      epsilon[i] = h;
      ddf[i] = (computeCDF(point + epsilon) - 2.0 * cdfPoint + computeCDF(point - epsilon)) * idenom;
    }
    return ddf;
  }
  throw NotDefinedException(HERE) << "In DistributionImplementation::computeDDF()";
}

/* Get the PDF of the distribution */
Scalar DistributionImplementation::computePDF(const Point & point) const
{
  const Scalar epsilon = 2.0 * std::pow(cdfEpsilon_, 1.0 / 3.0);
  const Sample xSample(((Box(Indices(dimension_, 0)).generate() - Point(dimension_, 0.5)) * Point(dimension_, epsilon)) + point);
  const Sample cdfSample(computeCDF(xSample));
  Scalar pdf = 0.0;
  const UnsignedInteger iMax = cdfSample.getSize();
  for (UnsignedInteger i = 0; i < iMax; ++ i)
  {
    // The points with an even number of positive shifts are counted positively
    // The others are counted negatively
    const UnsignedInteger numNullBits = dimension_ - SpecFunc::BitCount(i);
    pdf += (1.0 - 2.0 * (numNullBits % 2)) * cdfSample(i, 0);
  }
  return pdf / std::pow(epsilon, 1.0 * dimension_);
}

Scalar DistributionImplementation::computeLogPDF(const Point & point) const
{
  const Scalar pdf = computePDF(point);
  Scalar logPdf = SpecFunc::LowestScalar;
  if ( pdf > 0.0 ) logPdf = std::log(pdf);
  return logPdf;
}

/* Get the CDF, complementary CDF and survival function of the distribution */
/* On a Point */
Scalar DistributionImplementation::computeCDF(const Point & ) const
{
  throw NotYetImplementedException(HERE) << "In DistributionImplementation::computeCDF(const Point & point) const";
}

Scalar DistributionImplementation::computeComplementaryCDF(const Point & point) const
{
  const Scalar cdf = computeCDF(point);
  return 0.5 + (0.5 - cdf);
}

/* Computation of the survival function:
   A_k = \{X_k > x_k\}, k=0..n-1
   P(\cap A_k) = 1 - \bar{P}(\cap A_k)
   = 1 + \sum_{j=0}^{n-1}(-1)^j\sum_{\{k_0,\dots,k_{j-1}\}\subset\{0,\dots,n-1\}P(\bar{A}_{k_0},\dots,\bar{A}_{k_{j-1}})
   so
   n=1:
   P(X_1>x_1) = 1 - P(X_1\leq x_1)

   n=2:
   P(X_1>x_1, X_2>x_2) = 1 - P(X_1\leq x_1) - P(X_2\leq x_2) + P(X_1\leq x_1, X_2\leq x_2)

   n=3:
   P(X_1>x_1, X_2>x_2, X_3>x_3) = 1 - P(X_1\leq x_1) - P(X_2\leq x_2) - P(X_3\leq x_3) + P(X_1\leq x_1, X_2\leq x_2) + P(X_1\leq x_1, X_3\leq x_3) + P(X_2\leq x_2, X_3\leq x_3) - P(X_1\leq x_1, X_2\leq x_2, X_3\leq x_3)
*/
Scalar DistributionImplementation::computeSurvivalFunction(const Point & point) const
{
  if (dimension_ == 1) return computeComplementaryCDF(point);
  // Special case for independent copula
  if (hasIndependentCopula())
  {
    Scalar value = 1.0;
    for (UnsignedInteger i = 0; i < dimension_; ++i) value *= getMarginal(i).computeComplementaryCDF(point[i]);
    return value;
  }
  // For elliptical distributions,
  // P(X_1-mu_1<=x_1,...,X_d-mu_d<=x_d)=P(X_1-mu_1>=-x_1,...,X_d-mu_d>=-x_d)
  // So
  // P(X_1>=x_1,...,X_d>=x_d)=P(X_1<=2mu_1-x_1,...,X_d<=2mu_d-x_d)
  if (isElliptical()) return computeCDF(getMean() * 2.0 - point);
  const Point lowerBounds(range_.getLowerBound());
  const Point upperBounds(range_.getUpperBound());
  Bool allOutside = true;
  for (UnsignedInteger i = 0; i < dimension_; ++ i)
  {
    if (point[i] >= upperBounds[i]) return 0.0;
    allOutside &= (point[i] <= lowerBounds[i]);
  }
  if (allOutside) return 1.0;

  // Use Poincaré's formula
  const Scalar cdf = computeCDF(point);
  Scalar value = 1.0 + (dimension_ % 2 == 0 ? cdf : -cdf);
  Scalar sign = -1.0;
  for (UnsignedInteger i = 1; i < dimension_; ++ i)
  {
    Scalar contribution = 0.0;
    IndicesCollection indices(Combinations(i, dimension_).generate());
    Point subPoint(i);
    for (UnsignedInteger j = 0; j < indices.getSize(); ++j)
    {
      const Indices marginalJ(indices.cbegin_at(j), indices.cend_at(j));
      for (UnsignedInteger k = 0; k < i; ++k) subPoint[k] = point[marginalJ[k]];
      contribution += getMarginal(marginalJ).computeCDF(subPoint);
    }
    value += sign * contribution;
    sign = -sign;
  }
  // Due to roundoff, the value can be slightly outside of [0,1]
  return SpecFunc::Clip01(value);
}

Point DistributionImplementation::computeInverseSurvivalFunction(const Scalar prob) const
{
  Scalar marginalProb = 0.0;
  return computeInverseSurvivalFunction(prob, marginalProb);
}

Point DistributionImplementation::computeInverseSurvivalFunction(const Scalar prob,
    Scalar & marginalProb) const
{
  // Special case for bording values
  marginalProb = prob;
  if (prob < 0.0) return range_.getUpperBound();
  if (prob >= 1.0) return range_.getLowerBound();
  // Special case for dimension 1
  if (dimension_ == 1) return Point(1, computeScalarQuantile(prob, true));
  // Special case for independent copula
  if (hasIndependentCopula())
  {
    Point result(dimension_);
    marginalProb = std::pow(prob, 1.0 / dimension_);
    for (UnsignedInteger i = 0; i < dimension_; ++i) result[i] = getMarginal(i).computeScalarQuantile(marginalProb, true);
    return result;
  }
  // For elliptical distributions,
  // P(X_1-mu_1<=x_1,...,X_d-mu_d<=x_d)=P(X_1-mu_1>=-x_1,...,X_d-mu_d>=-x_d)
  // So
  // P(X_1>=x_1,...,X_d>=x_d)=P(X_1<=2mu_1-x_1,...,X_d<=2mu_d-x_d)
  // So
  // InverseSurvivalFunction(q) = 2mu-Quantile(q)
  if (isElliptical()) return getMean() * 2.0 - computeQuantile(prob, false, marginalProb);
  // Extract the marginal distributions
  Collection<Implementation> marginals(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; i++) marginals[i] = getMarginal(i).getImplementation();
  // The n-D inverse survival function is defined as X(\tau) = (S_1^{-1}(\tau), ..., S_n^{-1}(\tau)),
  // with tau such as S(X(\tau)) = q.
  // As F(x) = C(F_1(x_1),...,F_n(x_n)), the constraint F(X(\tau)) = q reads:
  // C(\tau,...,\tau) = q
  // Bracketing of \tau using the Frechet Hoeffding bounds:
  // max(n\tau - n + 1, 0) <= C(\tau,...,\tau) <= \tau
  // from which we deduce that q <= \tau and \tau <= 1 - (1 - q) / n
  // Lower bound of the bracketing interval
  const SurvivalFunctionWrapper wrapper(marginals, this);
  const Function f(bindMethod<SurvivalFunctionWrapper, Point, Point>(wrapper, &SurvivalFunctionWrapper::computeDiagonal, 1, 1));
  Scalar leftTau = prob;
  Scalar leftSurvival = f(Point(1, leftTau))[0];
  // Due to numerical precision issues, the theoretical bound can be slightly violated
  if (leftSurvival > prob)
  {
    leftTau = 0.0;
    leftSurvival = 1.0;
  }
  // Upper bound of the bracketing interval
  Scalar rightTau = 1.0 - (1.0 - prob) / dimension_;
  Scalar rightSurvival = f(Point(1, rightTau))[0];
  // Due to numerical precision issues, the theoretical bound can be slightly violated
  if (rightSurvival < prob)
  {
    rightTau = 1.0;
    rightSurvival = 0.0;
  }
  // Use Brent's method to compute the quantile efficiently for continuous distributions
  const Brent solver(quantileEpsilon_, cdfEpsilon_, cdfEpsilon_, quantileIterations_);
  marginalProb = solver.solve(f, prob, leftTau, rightTau, leftSurvival, rightSurvival);
  LOGINFO(OSS(false) << "tau=" << marginalProb);
  return wrapper.diagonalToSpace(marginalProb);
}

/* On a Sample */
Sample DistributionImplementation::computeCDFSequential(const Sample & inSample) const
{
  const UnsignedInteger size = inSample.getSize();
  Sample outSample(size, 1);
  for (UnsignedInteger i = 0; i < size; ++ i) outSample(i, 0) = computeCDF(inSample[i]);
  return outSample;
}

struct ComputeCDFPolicy
{
  const Sample & input_;
  Sample & output_;
  const DistributionImplementation & distribution_;

  ComputeCDFPolicy( const Sample & input,
                    Sample & output,
                    const DistributionImplementation & distribution)
    : input_(input)
    , output_(output)
    , distribution_(distribution)
  {}

  inline void operator()( const TBBImplementation::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++ i) output_(i, 0) = distribution_.computeCDF(input_[i]);
  }

}; /* end struct ComputeCDFPolicy */

Sample DistributionImplementation::computeCDFParallel(const Sample & inSample) const
{
  if (inSample.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << dimension_ << ", got " << inSample.getDimension();
  const UnsignedInteger size = inSample.getSize();
  Sample result(size, 1);
  const ComputeCDFPolicy policy( inSample, result, *this );
  TBBImplementation::ParallelFor( 0, size, policy );
  return result;
}

Sample DistributionImplementation::computeCDF(const Sample & inSample) const
{
  if (inSample.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << dimension_ << ", got " << inSample.getDimension();
  if (isParallel_) return computeCDFParallel(inSample);
  else return computeCDFSequential(inSample);
}

Sample DistributionImplementation::computeComplementaryCDFSequential(const Sample & inSample) const
{
  const UnsignedInteger size = inSample.getSize();
  Sample outSample(size, 1);
  for (UnsignedInteger i = 0; i < size; ++ i) outSample(i, 0) = computeComplementaryCDF(inSample[i]);
  return outSample;
}

struct ComputeComplementaryCDFPolicy
{
  const Sample & input_;
  Sample & output_;
  const DistributionImplementation & distribution_;

  ComputeComplementaryCDFPolicy( const Sample & input,
                                 Sample & output,
                                 const DistributionImplementation & distribution)
    : input_(input)
    , output_(output)
    , distribution_(distribution)
  {}

  inline void operator()( const TBBImplementation::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++ i) output_(i, 0) = distribution_.computeComplementaryCDF(input_[i]);
  }

}; /* end struct ComputeComplementaryCDFPolicy */

Sample DistributionImplementation::computeComplementaryCDFParallel(const Sample & inSample) const
{
  if (inSample.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << dimension_ << ", got " << inSample.getDimension();
  const UnsignedInteger size = inSample.getSize();
  Sample result(size, 1);
  const ComputeComplementaryCDFPolicy policy( inSample, result, *this );
  TBBImplementation::ParallelFor( 0, size, policy );
  return result;
}

Sample DistributionImplementation::computeComplementaryCDF(const Sample & inSample) const
{
  if (inSample.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << dimension_ << ", got " << inSample.getDimension();
  if (isParallel_) return computeComplementaryCDFParallel(inSample);
  else return computeComplementaryCDFSequential(inSample);
}

Sample DistributionImplementation::computeSurvivalFunctionSequential(const Sample & inSample) const
{
  const UnsignedInteger size = inSample.getSize();
  Sample outSample(size, 1);
  for (UnsignedInteger i = 0; i < size; ++ i) outSample(i, 0) = computeSurvivalFunction(inSample[i]);
  return outSample;
}

struct ComputeSurvivalFunctionPolicy
{
  const Sample & input_;
  Sample & output_;
  const DistributionImplementation & distribution_;

  ComputeSurvivalFunctionPolicy( const Sample & input,
                                 Sample & output,
                                 const DistributionImplementation & distribution)
    : input_(input)
    , output_(output)
    , distribution_(distribution)
  {}

  inline void operator()( const TBBImplementation::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i) output_(i, 0) = distribution_.computeSurvivalFunction(input_[i]);
  }

}; /* end struct ComputeSurvivalFunctionPolicy */

Sample DistributionImplementation::computeSurvivalFunctionParallel(const Sample & inSample) const
{
  if (inSample.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << dimension_ << ", got " << inSample.getDimension();
  const UnsignedInteger size = inSample.getSize();
  Sample result(size, 1);
  const ComputeSurvivalFunctionPolicy policy( inSample, result, *this );
  TBBImplementation::ParallelFor( 0, size, policy );
  return result;
}

Sample DistributionImplementation::computeSurvivalFunction(const Sample & inSample) const
{
  if (inSample.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << dimension_ << ", got " << inSample.getDimension();
  if (isParallel_) return computeSurvivalFunctionParallel(inSample);
  else return computeSurvivalFunctionSequential(inSample);
}

/* Compute the probability content of an interval */
Scalar DistributionImplementation::computeProbability(const Interval & interval) const
{
  if (interval.getDimension() != dimension_)
    throw InvalidArgumentException(HERE) << "Error: expected an interval of dimension=" << dimension_ << ", got dimension=" << interval.getDimension();

  // Generic implementations for continuous & discrete distributions
  if (isContinuous())
    return computeProbabilityContinuous(interval);
  if (isDiscrete())
    return computeProbabilityDiscrete(interval);

  if (dimension_ == 1)
    return computeProbabilityGeneral1D(interval.getLowerBound()[0], interval.getUpperBound()[0]);

  return computeProbabilityGeneral(interval);
}

/* Generic implementation for continuous distributions */
Scalar DistributionImplementation::computeProbabilityGeneral(const Interval & interval) const
{
  const Interval reducedInterval(interval.intersect(range_));
  if (reducedInterval.isEmpty()) return 0.0;
  if (reducedInterval == range_) return 1.0;

  const Point a(reducedInterval.getLowerBound());
  const Point b(reducedInterval.getUpperBound());
  Scalar probability = 1.0;
  if (hasIndependentCopula())
  {
    for (UnsignedInteger i = 0; i < dimension_; ++i) probability *= getMarginal(i).getImplementation()->computeProbabilityGeneral1D(a[i], b[i]);
  } // independent
  else
  {
    // P(\bigcap_i ai < Xi \leq bi) = \sum_c (−1)^n(c) F(c_1,c_2,...,c_n)
    // with c = (c_i, i =1, ..., n), c_i \in [a_i, b_i]
    // and n(c) = Card({c_i == a_i, i = 1, ..., n})
    probability = 0.0;
    const UnsignedInteger iMax = 1 << dimension_;
    Point probabilities(iMax);
    for( UnsignedInteger i = 0; i < iMax; ++i )
    {
      Bool evenLower = true;
      Point c(b);
      for( UnsignedInteger j = 0; j < dimension_; ++j )
      {
        const UnsignedInteger mask = 1 << j;
        if (i & mask)
        {
          c[j] = a[j];
          evenLower = (!evenLower);
        }
      } // j
      const Scalar cdf = computeCDF(c);
      probabilities[i] = (evenLower ? cdf : -cdf);
    } // i
    probability = SpecFunc::AccurateSum(probabilities);
  } // not independent

  return SpecFunc::Clip01(probability);
}

/* Generic implementation for 1D continuous distributions */
Scalar DistributionImplementation::computeProbabilityGeneral1D(const Scalar a,
    const Scalar b) const
{
  const Scalar ccdfA = computeComplementaryCDF(a);
  if (ccdfA <= 0.5)
  {
    const Scalar ccdfB = computeComplementaryCDF(b);
    return ccdfA - ccdfB;
  }
  const Scalar cdfA = computeCDF(a);
  const Scalar cdfB = computeCDF(b);
  return cdfB - cdfA;
}

/* Generic implementation for continuous distribution by integration of the PDF */
Scalar DistributionImplementation::computeProbabilityContinuous(const Interval & interval) const
{

  const Interval reducedInterval(interval.intersect(range_));
  if (reducedInterval.isEmpty()) return 0.0;
  if (reducedInterval == range_) return 1.0;
  // Use adaptive multidimensional integration of the PDF on the reduced interval
  const PDFWrapper pdfWrapper(this);
  Scalar probability = 1.0;
  if (hasIndependentCopula())
  {
    const Point lower(interval.getLowerBound());
    const Point upper(interval.getUpperBound());
    for (UnsignedInteger i = 0; i < dimension_; ++i) probability *= getMarginal(i).getImplementation()->computeProbabilityContinuous1D(lower[i], upper[i]);
  }
  else
    probability = IteratedQuadrature().integrate(pdfWrapper, reducedInterval)[0];

  return SpecFunc::Clip01(probability);
}

/* Generic implementation for 1D continuous distribution by integration of the PDF */
Scalar DistributionImplementation::computeProbabilityContinuous1D(const Scalar aa, const Scalar bb) const
{
  // Use adaptive multidimensional integration of the PDF on the reduced interval
  const PDFWrapper pdfWrapper(this);
  Scalar probability = 0.0;
  Scalar error = -1.0;
  Point ai;
  Point bi;
  Sample fi;
  Point ei;
  const Point singularities(getSingularities());
  // Consider only the intersection with the range
  const Scalar a = std::max(aa, range_.getLowerBound()[0]);
  const Scalar b = std::min(bb, range_.getUpperBound()[0]);
  // If no singularity inside of the given reduced interval
  const UnsignedInteger singularitiesNumber = singularities.getSize();
  if (singularitiesNumber == 0 || singularities[0] >= b || singularities[singularitiesNumber - 1] <= a) probability = GaussKronrod().integrate(pdfWrapper, a, b, error, ai, bi, fi, ei)[0];
  else
  {
    Scalar lower = a;
    for (UnsignedInteger i = 0; i < singularitiesNumber; ++i)
    {
      const Scalar upper = singularities[i];
      if (upper > a && upper < b)
      {
        probability += GaussKronrod().integrate(pdfWrapper, lower, upper, error, ai, bi, fi, ei)[0];
        lower = upper;
      }
      // Exit the loop if no more singularities inside of the reduced interval
      if (upper >= b) break;
    } // for
    // Last contribution
    probability += GaussKronrod().integrate(pdfWrapper, lower, b, error, ai, bi, fi, ei)[0];
  } // else
  return SpecFunc::Clip01(probability);
}

/* Generic implementation for discrete distributions */
Scalar DistributionImplementation::computeProbabilityDiscrete(const Interval & interval) const
{
  const Sample support(getSupport(interval));
  Scalar value = 0.0;
  for (UnsignedInteger i = 0; i < support.getSize(); ++i) value += computePDF(support[i]);
  return value;
}


/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
Complex DistributionImplementation::computeCharacteristicFunction(const Scalar x) const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "Error:  cannot use the computeCharacteristicFunction method with distributions of dimension > 1";
  if (x == 0.0) return 1.0;
  Complex value(0.0);
  // In the continuous case, we use simple gauss integration with a fixed number of integration points. We divide the interval in order to have a sufficient number of integration points by interval. It is good for low to moderate value of x, but is prohibitive for large x. In this case, we use Filon's method with linear interpolation, it means the modified trapezoidal rule as in E. O. Tuck, 'A simple "Filon-Trapezoidal" Rule'
  if (isContinuous())
  {
    const UnsignedInteger N = ResourceMap::GetAsUnsignedInteger("Distribution-CharacteristicFunctionNMax");
    // The circular function will have x(b-a)/2\pi arches over [a, b], so we need a number of points of this order, we decide to take 8 points per arch
    Point legendreWeights;
    const Point legendreNodes(getGaussNodesAndWeights(legendreWeights));
    // How many sub-intervals?
    // nPts = 8*x(b-a)/2\pi => (b-a)/2 = nPts * \pi / (8*x)
    const Scalar xMin = range_.getLowerBound()[0];
    const Scalar xMax = range_.getUpperBound()[0];
    const Scalar delta = xMax - xMin;
    const UnsignedInteger intervalsNumber = std::max(1, static_cast<int>(round(2 * x * delta / integrationNodesNumber_)));
    if (intervalsNumber * integrationNodesNumber_ < N)
    {
      const Scalar halfLength = 0.5 * delta / intervalsNumber;
      for (UnsignedInteger n = 0; n < intervalsNumber; ++n)
      {
        const Scalar a = xMin + 2.0 * n * halfLength;
        for (UnsignedInteger i = 0; i < integrationNodesNumber_; ++i)
        {
          const Scalar xi = a + (1.0 + legendreNodes[i]) * halfLength;
          value += legendreWeights[i] * computePDF(xi) * std::exp(Complex(0.0, x * xi));
        }
      }
      // We factor out the scaling as all the sub intervals have the same length
      value *= halfLength;
    }
    else
    {
      const Scalar a = range_.getLowerBound()[0];
      const Scalar b = range_.getUpperBound()[0];
      const Scalar T = 0.5 * (b - a);
      const Scalar c = 0.5 * (a + b);
      const Scalar dt = T / N;
      static Point pdfGrid;
      if (!pdfGrid.getSize())
      {
        Sample locations(Box(Indices(1, 2 * N - 1)).generate());
        locations *= Point(1, b - a);
        locations += Point(1, a);
        pdfGrid = computePDF(locations).getImplementation()->getData();
      }
      const Scalar omegaDt = x * dt;
      const Scalar omegaDt2 = omegaDt * omegaDt;
      const Scalar cosOmegaDt = std::cos(omegaDt);
      const Scalar sinOmegaDt = std::sin(omegaDt);
      // The bound 4.3556e-4 is such that we get full double precision
      const Complex wM(std::abs(omegaDt) < 4.3556e-4 ? Complex(0.5 - omegaDt2 / 24.0, omegaDt / 6.0 * (1.0 - omegaDt2 / 40.0)) : Complex((1.0 - cosOmegaDt) / omegaDt2, (omegaDt - sinOmegaDt) / omegaDt2));
      const Complex wP(std::abs(omegaDt) < 4.3556e-4 ? Complex(0.5 - omegaDt2 / 24.0, -omegaDt / 6.0 * (1.0 - omegaDt2 / 40.0)) : Complex((1.0 - cosOmegaDt) / omegaDt2, (-omegaDt + sinOmegaDt) / omegaDt2));
      const Scalar cosNOmegaDt = std::cos(N * omegaDt);
      const Scalar sinNOmegaDt = std::sin(N * omegaDt);
      // The bound 4.3556e-4 is such that we get full double precision
      const Scalar w = std::abs(omegaDt) < 4.3556e-4 ? std::pow(std::sin(0.5 * omegaDt) / (0.5 * omegaDt), 2) : 1.0 - omegaDt2 / 12.0;
      //      value = pdfGrid[N] * w + pdfGrid[0] * wM * Complex(cosNOmegaDt, -sinNOmegaDt) + pdfGrid[2 * N] * wP * Complex(cosNOmegaDt, sinNOmegaDt);
      value = pdfGrid[0] * wM * Complex(cosNOmegaDt, -sinNOmegaDt) + pdfGrid[2 * N - 1] * wP * Complex(cosNOmegaDt, sinNOmegaDt);
      for (UnsignedInteger n = 1; n < N; ++n)
      {
        const Scalar cosN = std::cos(n * omegaDt);
        const Scalar sinN = std::sin(n * omegaDt);
        value += Complex(w * cosN * (pdfGrid[N + n - 1] + pdfGrid[N - n]), w * sinN * (pdfGrid[N + n - 1] - pdfGrid[N - n]));
      }
      return dt * value * Complex(std::cos(x * c), std::sin(x * c));
    }
  } // Continuous
  else
  {
    // Discrete
    // In the discrete case, we have a reasonably efficient algorithm both in term of speed and precision.
    if (isDiscrete())
    {
      const Sample support(getSupport());
      const Point probabilities(getProbabilities());
      const UnsignedInteger size = support.getSize();
      for (UnsignedInteger i = 0; i < size; ++i)
      {
        const Scalar pt = support(i, 0);
        value += probabilities[i] * std::exp(Complex(0.0, x * pt));
      }
    }
    // In the composite case, no default algorithm
    else
    {
      throw NotYetImplementedException(HERE) << "In DistributionImplementation::computeCharacteristicFunction(const Scalar x) const: no default algorithm to compute the characteristic function in the composite case.";
    }
  } // !Continuous
  return value;
}

Complex DistributionImplementation::computeCharacteristicFunction(const Point & x) const
{
  if (dimension_ == 1) return computeCharacteristicFunction(x[0]);
  throw NotYetImplementedException(HERE) << "In DistributionImplementation::computeCharacteristicFunction(const Point & x) const";
}

Complex DistributionImplementation::computeLogCharacteristicFunction(const Scalar x) const
{
  const Complex value(computeCharacteristicFunction(x));
  const Complex result(std::log(value));
  return result;
}

Complex DistributionImplementation::computeLogCharacteristicFunction(const Point & x) const
{
  if (dimension_ == 1) return computeLogCharacteristicFunction(x[0]);
  throw NotYetImplementedException(HERE) << "In DistributionImplementation::computeLogCharacteristicFunction(const Point & x) const";
}

Complex DistributionImplementation::computeCharacteristicFunction(const UnsignedInteger index,
    const Scalar step) const
{
  return computeCharacteristicFunction(index * step);
}

Complex DistributionImplementation::computeLogCharacteristicFunction(const UnsignedInteger index,
    const Scalar step) const
{
  return computeLogCharacteristicFunction(index * step);
}

Complex DistributionImplementation::computeCharacteristicFunction(const Indices & indices,
    const Point & step) const
{
  Point point(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i) point[i] = indices[i] * step[i];
  return computeCharacteristicFunction(point);
}

Complex DistributionImplementation::computeLogCharacteristicFunction(const Indices & indices,
    const Point & step) const
{
  Point point(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i) point[i] = indices[i] * step[i];
  return computeLogCharacteristicFunction(point);
}

/* Get the generating function of the distribution, i.e. psi(z) = E(z^X) */
Scalar DistributionImplementation::computeGeneratingFunction(const Scalar z) const
{
  return computeGeneratingFunction(Complex(z, 0.0)).real();
}

Scalar DistributionImplementation::computeLogGeneratingFunction(const Scalar z) const
{
  return computeLogGeneratingFunction(Complex(z, 0.0)).real();
}

Complex DistributionImplementation::computeGeneratingFunction(const Complex & z) const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "Error:  cannot use the computeCharacteristicFunction method with distributions of dimension > 1";
  if (!isDiscrete()) throw NotDefinedException(HERE) << "Error: cannot compute the generating function for non discrete distributions.";
  Complex value(0.0);
  // Create the generating function as a univariate polynomial. It will be used as such if the distribution is integral, or as a container for the individual probabilities if the distribution is not integral
  if (!isAlreadyCreatedGeneratingFunction_)
  {
    generatingFunction_ = UniVariatePolynomial(getProbabilities());
    isAlreadyCreatedGeneratingFunction_ = true;
  }
  // If the distribution is integral, the generating function is either a polynomial if the support is finite, or can be well approximated by such a polynomial
  if (isIntegral())
  {
    value = generatingFunction_(z);
  }
  // The distribution is discrete but not integral
  else
  {
    const Sample support(getSupport());
    const UnsignedInteger size = support.getSize();
    const Point probabilities(generatingFunction_.getCoefficients());
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const Scalar pt = support(i, 0);
      value += probabilities[i] * std::pow(z, pt);
    }
  }
  return value;
}

Complex DistributionImplementation::computeLogGeneratingFunction(const Complex & z) const
{
  Complex value = computeGeneratingFunction(z);
  return std::log(value);
}

/* Compute the entropy of the distribution */
Scalar DistributionImplementation::computeEntropy() const
{
  if (isDiscrete())
  {
    const Point probabilities(getProbabilities());
    Scalar entropy = 0.0;
    for (UnsignedInteger i = 0; i < probabilities.getSize(); ++i)
    {
      const Scalar pI = probabilities[i];
      if (pI > 0.0)
        entropy += -pI * std::log(pI);
    }
    return entropy;
  }
  if (isContinuous())
  {
    // If the components are independent the entropy is the sum of the marginal entropies
    // We factor the construction of the 1D quadrature rules over the marginals
    const Point lowerBound_(range_.getLowerBound());
    const Point upperBound_(range_.getUpperBound());
    if (hasIndependentCopula())
    {
      const GaussLegendre integrator(Indices(1, integrationNodesNumber_));
      const Sample nodes(integrator.getNodes());
      const Point weights(integrator.getWeights());
      Scalar entropy = 0.0;
      for (UnsignedInteger marginal = 0; marginal < dimension_; ++marginal)
      {
        const Implementation marginalDistribution(getMarginal(marginal).getImplementation());
        Point integrationBounds(1, lowerBound_[marginal]);
        integrationBounds.add(getSingularities());
        integrationBounds.add(upperBound_[marginal]);
        for (UnsignedInteger j = 0; j < integrationBounds.getSize() - 1; ++j)
        {
          const Scalar a = integrationBounds[j];
          const Scalar b = integrationBounds[j + 1];
          const Scalar delta = b - a;
          const Point logPDFAtNodes(marginalDistribution->computeLogPDF(nodes * Point(1, delta) + Point(1, a)).asPoint());
          for (UnsignedInteger i = 0; i < integrationNodesNumber_; ++i)
          {
            const Scalar logPI = logPDFAtNodes[i];
            entropy += -logPI * std::exp(logPI) * weights[i] * delta;
          } // integration nodes
        } // Singularities
      } // marginal
      return entropy;
    } // hasIndependentCopula()
    // In low dimension, use an adaptive quadrature
    if (dimension_ <= ResourceMap::GetAsUnsignedInteger("Distribution-SmallDimensionEntropy"))
    {
      const EntropyKernel entropyKernel(this);
      return IteratedQuadrature().integrate(entropyKernel, range_)[0];
    } // Low dimension
  } // isContinuous()
  const UnsignedInteger size = ResourceMap::GetAsUnsignedInteger("Distribution-EntropySamplingSize");
  const String samplingMethod(ResourceMap::GetAsString("Distribution-EntropySamplingMethod"));
  Sample sample;
  if (samplingMethod == "MonteCarlo")
    sample = getSample(size);
  else if (samplingMethod == "QuasiMonteCarlo")
    sample = getSampleByQMC(size);
  else
  {
    LOGWARN(OSS() << "Unknown sampling method=" << samplingMethod << " to compute entropy. Resort to MonteCarlo");
    sample = getSample(size);
  }
  return -computeLogPDF(sample).computeMean()[0];
}

/* Get the DDF of the distribution */
Sample DistributionImplementation::computeDDFSequential(const Sample & inSample) const
{
  const UnsignedInteger size = inSample.getSize();
  SampleImplementation outSample(size, dimension_);
  UnsignedInteger shift = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Point point(computeDDF(inSample[i]));
    std::copy(point.begin(), point.end(), outSample.data_begin() + shift);
    shift += dimension_;
  }
  return outSample;
}

/* Get the DDF of the distribution */
struct ComputeDDFPolicy
{
  const Sample & input_;
  Sample & output_;
  const DistributionImplementation & distribution_;

  ComputeDDFPolicy( const Sample & input,
                    Sample & output,
                    const DistributionImplementation & distribution)
    : input_(input)
    , output_(output)
    , distribution_(distribution)
  {}

  inline void operator()( const TBBImplementation::BlockedRange<UnsignedInteger> & r ) const
  {
    const UnsignedInteger dimension = input_.getDimension();
    UnsignedInteger shift = dimension * r.begin();
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      const Point point(distribution_.computeDDF(input_[i]));
      std::copy(point.begin(), point.end(), output_.getImplementation()->data_begin() + shift);
      shift += dimension;
    }
  }
}; /* end struct ComputeDDFPolicy */

Sample DistributionImplementation::computeDDFParallel(const Sample & inSample) const
{
  if (inSample.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << dimension_ << ", got " << inSample.getDimension();
  const UnsignedInteger size = inSample.getSize();
  Sample result(size, 1);
  const ComputeDDFPolicy policy( inSample, result, *this );
  TBBImplementation::ParallelFor( 0, size, policy );
  return result;
}

Sample DistributionImplementation::computeDDF(const Sample & inSample) const
{
  if (inSample.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << dimension_ << ", got " << inSample.getDimension();
  if (isParallel_) return computeDDFParallel(inSample);
  else return computeDDFSequential(inSample);
}

/* Get the PDF of the distribution */
Sample DistributionImplementation::computePDFSequential(const Sample & inSample) const
{
  const UnsignedInteger size = inSample.getSize();
  SampleImplementation outSample(size, 1);
  for (UnsignedInteger i = 0; i < size; ++i) outSample(i, 0) = computePDF(inSample[i]);
  return outSample;
}

/* Get the PDF of the distribution */
struct ComputePDFPolicy
{
  const Sample & input_;
  Sample & output_;
  const DistributionImplementation & distribution_;

  ComputePDFPolicy( const Sample & input,
                    Sample & output,
                    const DistributionImplementation & distribution)
    : input_(input)
    , output_(output)
    , distribution_(distribution)
  {}

  inline void operator()( const TBBImplementation::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i) output_(i, 0) = distribution_.computePDF(input_[i]);
  }

}; /* end struct ComputePDFPolicy */

Sample DistributionImplementation::computePDFParallel(const Sample & inSample) const
{
  if (inSample.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << dimension_ << ", got " << inSample.getDimension();
  const UnsignedInteger size = inSample.getSize();
  Sample result(size, 1);
  const ComputePDFPolicy policy( inSample, result, *this );
  TBBImplementation::ParallelFor( 0, size, policy );
  return result;
}

Sample DistributionImplementation::computePDF(const Sample & inSample) const
{
  if (inSample.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << dimension_ << ", got " << inSample.getDimension();
  if (isParallel_) return computePDFParallel(inSample);
  else return computePDFSequential(inSample);
}

/* Get the Log PDF of the distribution */
Sample DistributionImplementation::computeLogPDFSequential(const Sample & inSample) const
{
  const UnsignedInteger size = inSample.getSize();
  Sample outSample(size, 1);
  for (UnsignedInteger i = 0; i < size; ++i) outSample(i, 0) = computeLogPDF(inSample[i]);
  return outSample;
}

/* Get the LogPDF of the distribution */
struct ComputeLogPDFPolicy
{
  const Sample & input_;
  Sample & output_;
  const DistributionImplementation & distribution_;

  ComputeLogPDFPolicy( const Sample & input,
                       Sample & output,
                       const DistributionImplementation & distribution)
    : input_(input)
    , output_(output)
    , distribution_(distribution)
  {}

  inline void operator()( const TBBImplementation::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i) output_(i, 0) = distribution_.computeLogPDF(input_[i]);
  }

}; /* end struct ComputeLogPDFPolicy */

Sample DistributionImplementation::computeLogPDFParallel(const Sample & inSample) const
{
  if (inSample.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << dimension_ << ", got " << inSample.getDimension();
  const UnsignedInteger size = inSample.getSize();
  Sample result(size, 1);
  const ComputeLogPDFPolicy policy( inSample, result, *this );
  TBBImplementation::ParallelFor( 0, size, policy );
  return result;
}

Sample DistributionImplementation::computeLogPDF(const Sample & inSample) const
{
  if (inSample.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << dimension_ << ", got " << inSample.getDimension();
  if (isParallel_) return computeLogPDFParallel(inSample);
  else return computeLogPDFSequential(inSample);
}

/* Get the DDF of the distribution */
Scalar DistributionImplementation::computeDDF(const Scalar scalar) const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "ERROR: cannot use the simplified interface of computeDDF with distributions of dimension > 1";
  return computeDDF(Point(1, scalar))[0];
}

/* Get the PDF of the distribution */
Scalar DistributionImplementation::computePDF(const Scalar scalar) const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "ERROR: cannot use the simplified interface of computePDF with distributions of dimension > 1";
  return computePDF(Point(1, scalar));
}

Scalar DistributionImplementation::computeLogPDF(const Scalar scalar) const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "ERROR: cannot use the simplified interface of computeLogPDF with distributions of dimension > 1";
  return computeLogPDF(Point(1, scalar));
}

/* Get the CDF of the distribution */
Scalar DistributionImplementation::computeCDF(const Scalar scalar) const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "ERROR: cannot use the simplified interface of computeCDF with distributions of dimension > 1";
  return computeCDF(Point(1, scalar));
}

Scalar DistributionImplementation::computeComplementaryCDF(const Scalar scalar) const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "ERROR: cannot use the simplified interface of computeComplementaryCDF with distributions of dimension > 1";
  return computeComplementaryCDF(Point(1, scalar));
}

Scalar DistributionImplementation::computeSurvivalFunction(const Scalar scalar) const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "ERROR: cannot use the simplified interface of computeSurvivalFunction with distributions of dimension > 1";
  return computeSurvivalFunction(Point(1, scalar));
}

/* Compute the PDF of 1D distributions over a regular grid */
Sample DistributionImplementation::computePDF(const Scalar xMin,
    const Scalar xMax,
    const UnsignedInteger pointNumber,
    Sample & grid) const
{
  return computePDF(Point(1, xMin), Point(1, xMax), Indices(1, pointNumber), grid);
}

/* Compute the PDF of nD distributions over a regular grid */
Sample DistributionImplementation::computePDF(const Point & xMin,
    const Point & xMax,
    const Indices & pointNumber,
    Sample & grid) const
{
  if (xMin.getDimension() != xMax.getDimension()) throw InvalidArgumentException(HERE) << "Error: the two corner points must have the same dimension. Here, dim(xMin)=" << xMin.getDimension() << " and dim(xMax)=" << xMax.getDimension();
  if (xMin.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the corner points must have the same dimension as the distribution. Here, dim(xMin)=" << xMin.getDimension() << " and distribution dimension=" << dimension_;
  if (dimension_ != pointNumber.getSize()) throw InvalidArgumentException(HERE) << "Error: the discretization must match the distribution dimension. Here, dim(discretization)=" << pointNumber.getSize() << " and distribution dimension=" << dimension_;
  IndicesCollection indices(Tuples(pointNumber).generate());
  const UnsignedInteger size = indices.getSize();
  Sample inputSample(indices.getSize(), dimension_);
  for (UnsignedInteger i = 0; i < size; ++i)
    for (UnsignedInteger j = 0; j < dimension_; ++j) inputSample(i, j) = xMin[j] + indices(i, j) * (xMax[j] - xMin[j]) / (pointNumber[j] - 1.0);
  grid = inputSample;
  return computePDF(inputSample);
}

/* Compute the log-PDF of 1D distributions over a regular grid */
Sample DistributionImplementation::computeLogPDF(const Scalar xMin,
    const Scalar xMax,
    const UnsignedInteger pointNumber,
    Sample & grid) const
{
  return computeLogPDF(Point(1, xMin), Point(1, xMax), Indices(1, pointNumber), grid);
}

/* Compute the log-PDF of nD distributions over a regular grid */
Sample DistributionImplementation::computeLogPDF(const Point & xMin,
    const Point & xMax,
    const Indices & pointNumber,
    Sample & grid) const
{
  if (xMin.getDimension() != xMax.getDimension()) throw InvalidArgumentException(HERE) << "Error: the two corner points must have the same dimension. Here, dim(xMin)=" << xMin.getDimension() << " and dim(xMax)=" << xMax.getDimension();
  if (xMin.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the corner points must have the same dimension as the distribution. Here, dim(xMin)=" << xMin.getDimension() << " and distribution dimension=" << dimension_;
  if (dimension_ != pointNumber.getSize()) throw InvalidArgumentException(HERE) << "Error: the discretization must match the distribution dimension. Here, dim(discretization)=" << pointNumber.getSize() << " and distribution dimension=" << dimension_;
  IndicesCollection indices(Tuples(pointNumber).generate());
  const UnsignedInteger size = indices.getSize();
  Sample inputSample(indices.getSize(), dimension_);
  for (UnsignedInteger i = 0; i < size; ++i)
    for (UnsignedInteger j = 0; j < dimension_; ++j) inputSample(i, j) = xMin[j] + indices(i, j) * (xMax[j] - xMin[j]) / (pointNumber[j] - 1.0);
  grid = inputSample;
  return computeLogPDF(inputSample);
}

/* Compute the CDF of 1D distributions over a regular grid */
Sample DistributionImplementation::computeCDF(const Scalar xMin,
    const Scalar xMax,
    const UnsignedInteger pointNumber,
    Sample & grid) const
{
  return computeCDF(Point(1, xMin), Point(1, xMax), Indices(1, pointNumber), grid);
}

/* Compute the CDF of nD distributions over a regular grid */
Sample DistributionImplementation::computeCDF(const Point & xMin,
    const Point & xMax,
    const Indices & pointNumber,
    Sample & grid) const
{
  if (xMin.getDimension() != xMax.getDimension()) throw InvalidArgumentException(HERE) << "Error: the two corner points must have the same dimension. Here, dim(xMin)=" << xMin.getDimension() << " and dim(xMax)=" << xMax.getDimension();
  if (xMin.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the corner points must have the same dimension as the distribution. Here, dim(xMin)=" << xMin.getDimension() << " and distribution dimension=" << dimension_;
  if (dimension_ != pointNumber.getSize()) throw InvalidArgumentException(HERE) << "Error: the discretization must match the distribution dimension. Here, dim(discretization)=" << pointNumber.getSize() << " and distribution dimension=" << dimension_;
  IndicesCollection indices(Tuples(pointNumber).generate());
  const UnsignedInteger size = indices.getSize();
  Sample inputSample(indices.getSize(), dimension_);
  for (UnsignedInteger i = 0; i < size; ++i)
    for (UnsignedInteger j = 0; j < dimension_; ++j) inputSample(i, j) = xMin[j] + indices(i, j) * (xMax[j] - xMin[j]) / (pointNumber[j] - 1.0);
  grid = inputSample;
  return computeCDF(inputSample);
}

Sample DistributionImplementation::computeComplementaryCDF(const Scalar xMin,
    const Scalar xMax,
    const UnsignedInteger pointNumber,
    Sample & grid) const
{
  if (dimension_ != 1) throw InvalidArgumentException(HERE) << "Error: cannot compute the CDF over a regular 1D grid if the dimension is > 1";
  Sample result(pointNumber, 2);
  Scalar x = xMin;
  Scalar step = (xMax - xMin) / Scalar(pointNumber - 1.0);
  grid = Sample(pointNumber, 1);
  for (UnsignedInteger i = 0; i < pointNumber; ++i)
  {
    grid(i, 0) = x;
    result(i, 0) = x;
    result(i, 1) = computeComplementaryCDF(x);
    x += step;
  }
  return result;
}

/*  Compute the quantile over a regular grid */
Sample DistributionImplementation::computeQuantile(const Scalar qMin,
    const Scalar qMax,
    const UnsignedInteger pointNumber,
    const Bool tail) const
{
  Sample grid;
  return computeQuantile(qMin, qMax, pointNumber, grid, tail);
}

Sample DistributionImplementation::computeQuantile(const Scalar qMin,
    const Scalar qMax,
    const UnsignedInteger pointNumber,
    Sample & grid,
    const Bool tail) const
{
  // First, build the regular grid for the quantile levels
  grid = Sample(pointNumber, 1);
  for (UnsignedInteger i = 0; i < pointNumber; ++i) grid(i, 0) = qMin + i * (qMax - qMin) / (pointNumber - 1.0);
  // Use possible parallelization
  return computeQuantile(grid.getImplementation()->getData(), tail);
}

/* Compute the quantile over a provided grid */
Sample DistributionImplementation::computeQuantileSequential(const Point & prob,
    const Bool tail) const
{
  const UnsignedInteger size = prob.getSize();
  SampleImplementation result(size, dimension_);
  UnsignedInteger shift = 0;
  for ( UnsignedInteger i = 0; i < size; ++ i )
  {
    const Point point(computeQuantile(prob[i], tail));
    std::copy(point.begin(), point.end(), result.data_begin() + shift);
    shift += dimension_;
  }
  return result;
}

struct ComputeQuantilePolicy
{
  const Point & prob_;
  Sample & output_;
  Bool tail_;
  const DistributionImplementation & distribution_;

  ComputeQuantilePolicy( const Point & prob,
                         Sample & output,
                         const Bool tail,
                         const DistributionImplementation & distribution)
    : prob_(prob)
    , output_(output)
    , tail_(tail)
    , distribution_(distribution)
  {}

  inline void operator()( const TBBImplementation::BlockedRange<UnsignedInteger> & r ) const
  {
    const UnsignedInteger dimension = distribution_.getDimension();
    UnsignedInteger shift = dimension * r.begin();
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      const Point point(distribution_.computeQuantile(prob_[i], tail_));
      std::copy(point.begin(), point.end(), output_.getImplementation()->data_begin() + shift);
      shift += dimension;
    }
  }

}; /* end struct ComputeQuantilePolicy */

Sample DistributionImplementation::computeQuantileParallel(const Point & prob,
    const Bool tail) const
{
  const UnsignedInteger size = prob.getSize();
  Sample result(size, dimension_);
  const ComputeQuantilePolicy policy( prob, result, tail, *this );
  TBBImplementation::ParallelFor( 0, size, policy );
  return result;
}

Sample DistributionImplementation::computeQuantile(const Point & prob,
    const Bool tail) const
{
  if (isParallel_) return computeQuantileParallel(prob, tail);
  else return computeQuantileSequential(prob, tail);
}

/* Get the PDF gradient of the distribution */
Point DistributionImplementation::computePDFGradient(const Point & point) const
{
  const Point initialParameters(getParameter());
  const UnsignedInteger parametersDimension = initialParameters.getDimension();
  Point PDFGradient(parametersDimension);
  // Clone the distribution
  Implementation cloneDistribution(clone());
  // Increment for centered differences
  const Scalar eps = std::pow(ResourceMap::GetAsScalar("DistFunc-Precision"), 1.0 / 3.0);
  // Increment for noncentered differences
  const Scalar eps2 = std::pow(ResourceMap::GetAsScalar("DistFunc-Precision"), 1.0 / 2.0);
  Point newParameters(initialParameters);
  for (UnsignedInteger i = 0; i < parametersDimension; ++i)
  {
    Scalar delta = 0.0;
    Scalar rightPDF = 0.0;
    // We will try a centered finite difference approximation
    try
    {
      newParameters[i] = initialParameters[i] + eps;
      cloneDistribution->setParameter(newParameters);
      rightPDF = cloneDistribution->computePDF(point);
      delta += eps;
    }
    catch (...)
    {
      // If something went wrong with the right point, stay at the center point
      newParameters[i] = initialParameters[i];
      cloneDistribution->setParameter(newParameters);
      rightPDF = cloneDistribution->computePDF(point);
    }
    Scalar leftPDF = 0.0;
    try
    {
      // If something is wrong with the right point, use non-centered finite differences
      const Scalar leftEpsilon = delta == 0.0 ? eps2 : eps;
      newParameters[i] = initialParameters[i] - leftEpsilon;
      cloneDistribution->setParameter(newParameters);
      leftPDF = cloneDistribution->computePDF(point);
      delta += leftEpsilon;
    }
    catch (...)
    {
      // If something is wrong with the left point, it is either because the gradient is not computable or because we must use non-centered finite differences, in which case the right point has to be recomputed
      if (delta == 0.0) throw InvalidArgumentException(HERE) << "Error: cannot compute the PDF gradient at x=" << point << " for the current values of the parameters=" << initialParameters;
      newParameters[i] = initialParameters[i] + eps2;
      cloneDistribution->setParameter(newParameters);
      rightPDF = cloneDistribution->computePDF(point);
      delta += eps2;
      // And the left point will be the center point
      newParameters[i] = initialParameters[i];
      cloneDistribution->setParameter(newParameters);
      leftPDF = cloneDistribution->computePDF(point);
    }
    PDFGradient[i] = (rightPDF - leftPDF) / delta;
    newParameters[i] = initialParameters[i];
  }
  return PDFGradient;
}

/* ComputePDFGradient On a Sample */
Sample DistributionImplementation::computePDFGradient(const Sample & inSample) const
{
  const UnsignedInteger size = inSample.getSize();
  Sample outSample(size, 0);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    Point grad(computePDFGradient(inSample[i]));
    if (i == 0)
      outSample = Sample(size, grad.getDimension());
    outSample[i] = grad;
  }
  return outSample;
}

/* Get the logPDF gradient of the distribution */
Point DistributionImplementation::computeLogPDFGradient(const Point & point) const
{
  // log(u)' = u' / u for any u function
  // With u(theta|point) = PDF(theta|point), du(theta|point)/dtheta = PDFGradient(theta|point)
  const Scalar pdf = computePDF(point);
  if (pdf > 0)
  {
    const Point logPDFGradient(computePDFGradient(point) / pdf);
    return logPDFGradient;
  }
  else
    // LogPDFGradient is used to maximize the log-likelihood for exponential models
    // if pdf is zero the u'/u has undetermined form (for exponential models we could extract
    // the correct determined form)
    return Point(getParameterDimension(), 0.0);
}

/* Get the LogPDFGradient of the distributionImplementation */
struct ComputeLogPDFGradientPolicy
{
  const Sample & input_;
  Sample & output_;
  const DistributionImplementation & distribution_;

  ComputeLogPDFGradientPolicy( const Sample & input,
                               Sample & output,
                               const DistributionImplementation & distribution)
    : input_(input)
    , output_(output)
    , distribution_(distribution)
  {}

  inline void operator()( const TBBImplementation::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      const Point out = distribution_.computeLogPDFGradient(input_[i]);
      for (UnsignedInteger j = 0; j < output_.getDimension(); ++j)
        output_(i, j) = out[j];
    }
  }
}; /* end struct ComputeLogPDFGradientPolicy */

/* Get the logPDF gradient of the distribution (sequential implementation) */
Sample DistributionImplementation::computeLogPDFGradientSequential(const Sample & sample) const
{
  const UnsignedInteger size = sample.getSize();
  Sample outSample(size, getParameterDimension());
  // Sequential implementation
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Point result = computeLogPDFGradient(sample[i]);
    for (UnsignedInteger j = 0; j < getParameterDimension(); ++j)
      outSample(i, j) = result[j];
  }
  return outSample;
}

/* Get the logPDF gradient of the distribution (parallel implementation) */
Sample DistributionImplementation::computeLogPDFGradientParallel(const Sample & sample) const
{
  const UnsignedInteger size = sample.getSize();
  Sample outSample(size, getParameterDimension());
  const ComputeLogPDFGradientPolicy policy( sample, outSample, *this );
  TBBImplementation::ParallelFor( 0, size, policy );
  return outSample;
}

/* Get the logPDF gradient of the distribution */
Sample DistributionImplementation::computeLogPDFGradient(const Sample & inSample) const
{
  if (isParallel_) return computeLogPDFGradientParallel(inSample);
  else return computeLogPDFGradientSequential(inSample);
}

/* ComputeCDFGradient On a Sample */
Sample DistributionImplementation::computeCDFGradient(const Sample & inSample) const
{
  const UnsignedInteger size = inSample.getSize();
  SampleImplementation outSample(size, getParameterDimension());
  UnsignedInteger shift = 0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Point point(computeCDFGradient(inSample[i]));
    std::copy(point.begin(), point.end(), outSample.data_begin() + shift);
    shift += dimension_;
  }
  return outSample;
}

/* Get the CDF gradient of the distribution */
Point DistributionImplementation::computeCDFGradient(const Point & point) const
{
  const Point initialParameters(getParameter());
  const UnsignedInteger parametersDimension = initialParameters.getDimension();
  Point CDFGradient(parametersDimension);
  // Clone the distribution
  Implementation cloneDistribution(clone());
  // We will use centered differences
  const Scalar eps = std::pow(ResourceMap::GetAsScalar("DistFunc-Precision"), 1.0 / 3.0);
  // Increment for noncentered differences
  const Scalar eps2 = std::pow(ResourceMap::GetAsScalar("DistFunc-Precision"), 1.0 / 2.0);
  Point newParameters(initialParameters);
  for (UnsignedInteger i = 0; i < parametersDimension; ++i)
  {
    Scalar delta = 0.0;
    Scalar rightCDF = 0.0;
    // We will try a centered finite difference approximation
    try
    {
      newParameters[i] = initialParameters[i] + eps;
      cloneDistribution->setParameter(newParameters);
      rightCDF = cloneDistribution->computeCDF(point);
      delta += eps;
    }
    catch (...)
    {
      // If something went wrong with the right point, stay at the center point
      newParameters[i] = initialParameters[i];
      cloneDistribution->setParameter(newParameters);
      rightCDF = cloneDistribution->computeCDF(point);
    }
    Scalar leftCDF = 0.0;
    try
    {
      // If something is wrong with the right point, use non-centered finite differences
      const Scalar leftEpsilon = delta == 0.0 ? eps2 : eps;
      newParameters[i] = initialParameters[i] - leftEpsilon;
      cloneDistribution->setParameter(newParameters);
      leftCDF = cloneDistribution->computeCDF(point);
      delta += leftEpsilon;
    }
    catch (...)
    {
      // If something is wrong with the left point, it is either because the gradient is not computable or because we must use non-centered finite differences, in which case the right point has to be recomputed
      if (delta == 0.0) throw InvalidArgumentException(HERE) << "Error: cannot compute the CDF gradient at x=" << point << " for the current values of the parameters=" << initialParameters;
      newParameters[i] = initialParameters[i] + eps2;
      cloneDistribution->setParameter(newParameters);
      rightCDF = cloneDistribution->computeCDF(point);
      delta += eps2;
      // And the left point will be the center point
      newParameters[i] = initialParameters[i];
      cloneDistribution->setParameter(newParameters);
      leftCDF = cloneDistribution->computeCDF(point);
    }
    CDFGradient[i] = (rightCDF - leftCDF) / delta;
    newParameters[i] = initialParameters[i];
  }
  return CDFGradient;
}

/* Build a C1 interpolation of the CDF function for 1D continuous distributions */
Collection<PiecewiseHermiteEvaluation> DistributionImplementation::interpolatePDFCDF(const UnsignedInteger n)
{
  if (!isContinuous()) throw InternalException(HERE) << "Error: cannot interpolate the PDF and CDF of noncontinuous distributions.";
  if (dimension_ != 1) throw NotYetImplementedException(HERE) << "In DistributionImplementation::interpolatePDFCDF(const UnsignedInteger n): cannot interpolate CDF for multidimensional distributions.";
  const Scalar xMin = range_.getLowerBound()[0];
  const Scalar xMax = range_.getUpperBound()[0];
  const Scalar mu = getMean()[0];
  // Here we use an absolute precision of 0.0 in order to force the algorithm to use all the available discretization points
  GaussKronrod algorithm( n - 1, cdfEpsilon_ * cdfEpsilon_, GaussKronrodRule::G3K7);
  const PDFWrapper pdfWrapper(this);
  Scalar error = -1.0;
  Point ai;
  Point bi;
  Sample fi;
  Point ei;
  algorithm.integrate(pdfWrapper, xMin, mu, error, ai, bi, fi, ei);
  ai.add(mu);
  Sample locationsCDF(ai.getSize(), 1);
  locationsCDF.getImplementation()->setData(ai);
  locationsCDF = locationsCDF.sort(0);
  algorithm.integrate(pdfWrapper, mu, xMax, error, ai, bi, fi, ei);
  ai.add(xMax);
  Sample locationsCCDF(ai.getSize(), 1);
  locationsCCDF.getImplementation()->setData(ai);
  locationsCCDF = locationsCCDF.sort(0);
  Collection<PiecewiseHermiteEvaluation> coll(4);
  const Sample valuesCDF(computeCDF(locationsCDF));
  const Sample valuesPDF(computePDF(locationsCDF));
  const Sample valuesDDF(computeDDF(locationsCDF));
  coll[0] = PiecewiseHermiteEvaluation(locationsCDF.getImplementation()->getData(), valuesPDF, valuesDDF);
  coll[1] = PiecewiseHermiteEvaluation(locationsCDF.getImplementation()->getData(), valuesCDF, valuesPDF);
  const Sample valuesCCDF(computeComplementaryCDF(locationsCCDF));
  const Sample valuesCPDF(computePDF(locationsCCDF));
  Sample derivativesCCDF(valuesCPDF);
  derivativesCCDF *= Point(1, -1.0);
  const Sample valuesCDDF(computeDDF(locationsCCDF));
  coll[2] = PiecewiseHermiteEvaluation(locationsCCDF.getImplementation()->getData(), valuesCPDF, valuesCDDF);
  coll[3] = PiecewiseHermiteEvaluation(locationsCCDF.getImplementation()->getData(), valuesCCDF, derivativesCCDF);
  return coll;
}

/* Compute the DDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar DistributionImplementation::computeConditionalDDF(const Scalar,
    const Point & ) const
{
  throw NotYetImplementedException(HERE) << "In DistributionImplementation::computeConditionalDDF(const Scalar x, const Point & y) const";
}

/* Compute the DDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Point DistributionImplementation::computeSequentialConditionalDDF(const Point & ) const
{
  throw NotYetImplementedException(HERE) << "In DistributionImplementation::computeConditionalDDF(const Point & x) const";
}

/* Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar DistributionImplementation::computeConditionalPDF(const Scalar x,
    const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= dimension_) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional PDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula())) return getMarginal(conditioningDimension).computePDF(x);
  // General case
  Indices conditioning(conditioningDimension);
  conditioning.fill();
  Indices conditioned(conditioning);
  conditioned.add(conditioningDimension);
  const Implementation conditioningDistribution(getMarginal(conditioning).getImplementation());
  const Scalar pdfConditioning = conditioningDistribution->computePDF(y);
  if (pdfConditioning <= 0.0) return 0.0;
  Point z(y);
  z.add(x);
  const Implementation conditionedDistribution(getMarginal(conditioned).getImplementation());
  const Scalar pdfConditioned = conditionedDistribution->computePDF(z);
  return pdfConditioned / pdfConditioning;
}

/* Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Point DistributionImplementation::computeSequentialConditionalPDF(const Point & x) const
{
  if (x.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << dimension_ << ", got dimension=" << x.getDimension();
  Point result(dimension_);
  Indices conditioning(1, 0);
  Implementation conditioningDistribution(getMarginal(conditioning).getImplementation());
  Point currentX(1, x[0]);
  Scalar pdfConditioning = conditioningDistribution->computePDF(currentX);
  result[0] = pdfConditioning;
  for (UnsignedInteger conditioningDimension = 1; conditioningDimension < dimension_; ++conditioningDimension)
  {
    // Return the result as soon as a conditional pdf is zero
    if (pdfConditioning == 0) return result;
    conditioning.add(conditioningDimension);
    conditioningDistribution = getMarginal(conditioning).getImplementation();
    currentX.add(x[conditioningDimension]);
    const Scalar pdfConditioned = conditioningDistribution->computePDF(currentX);
    result[conditioningDimension] = pdfConditioned / pdfConditioning;
    pdfConditioning = pdfConditioned;
  } // conditioningDimension
  return result;
}

/* Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Point DistributionImplementation::computeConditionalPDF(const Point & x,
    const Sample & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= dimension_) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional PDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Convert the values in x into a Sample
  const UnsignedInteger size = x.getDimension();
  SampleImplementation xAsSample(size, 1);
  xAsSample.setData(x);
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula()))
    return getMarginal(conditioningDimension).computePDF(xAsSample).getImplementation()->getData();
  // General case
  Indices conditioning(conditioningDimension);
  conditioning.fill();
  Indices conditioned(conditioning);
  conditioned.add(conditioningDimension);
  const Implementation conditioningDistribution(getMarginal(conditioning).getImplementation());
  const Sample pdfConditioning(conditioningDistribution->computePDF(y));
  Sample z(y);
  z.stack(xAsSample);
  const Implementation conditionedDistribution(getMarginal(conditioned).getImplementation());
  const Sample pdfConditioned(conditionedDistribution->computePDF(z));
  Point result(size);
  for (UnsignedInteger i = 0; i < size; ++i)
    if (pdfConditioning(i, 0) > 0.0) result[i] = pdfConditioned(i, 0) / pdfConditioning(i, 0);
  return result;
}

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar DistributionImplementation::computeConditionalCDF(const Scalar x,
    const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= dimension_) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional CDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula())) return getMarginal(conditioningDimension).computeCDF(x);
  // General case
  Indices conditioning(conditioningDimension);
  conditioning.fill();
  Indices conditioned(conditioning);
  conditioned.add(conditioningDimension);
  const Implementation conditioningDistribution(getMarginal(conditioning).getImplementation());
  const Scalar pdfConditioning = conditioningDistribution->computePDF(y);
  if (pdfConditioning <= 0.0) return 0.0;
  const Implementation conditionedDistribution(getMarginal(conditioned).getImplementation());
  const Scalar xMin = conditionedDistribution->range_.getLowerBound()[conditioningDimension];
  if (x <= xMin) return 0.0;
  const Scalar xMax = conditionedDistribution->range_.getUpperBound()[conditioningDimension];
  if (x >= xMax) return 1.0;
  // Numerical integration with respect to x
  Pointer<ConditionalPDFWrapper> p_conditionalPDFWrapper = new ConditionalPDFWrapper(conditionedDistribution);
  p_conditionalPDFWrapper->setParameter(y);
  GaussKronrod algo;
  const Scalar value = algo.integrate(UniVariateFunction(p_conditionalPDFWrapper), xMin, x);
  return SpecFunc::Clip01(value / pdfConditioning);
}

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Point DistributionImplementation::computeSequentialConditionalCDF(const Point & x) const
{
  if (x.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << dimension_ << ", got dimension=" << x.getDimension();
  Point result(dimension_);
  Indices conditioning(1, 0);
  Implementation conditioningDistribution(getMarginal(conditioning).getImplementation());
  Point currentX(1, x[0]);
  result[0] = conditioningDistribution->computeCDF(currentX);
  Scalar pdfConditioning = conditioningDistribution->computePDF(currentX);
  GaussKronrod algo;
  for (UnsignedInteger conditioningDimension = 1; conditioningDimension < dimension_; ++conditioningDimension)
  {
    // Return the result as soon as a conditional pdf is zero
    if (pdfConditioning == 0) return result;
    // If the current component is at the left of the marginal range, the conditional CDF is zero as well as the PDF
    const Scalar xMin = range_.getLowerBound()[conditioningDimension];
    if (x[conditioningDimension] <= xMin) return result;
    conditioning.add(conditioningDimension);
    conditioningDistribution = getMarginal(conditioning).getImplementation();
    // If the current component is at the left of the marginal range, the conditional CDF is one and the conditional PDF is zero
    const Scalar xMax = range_.getUpperBound()[conditioningDimension];
    if (x[conditioningDimension] >= xMax)
    {
      result[conditioningDimension] = 1.0;
    }
    else
    {
      // Here we have to integrate something...
      Pointer<ConditionalPDFWrapper> p_conditionalPDFWrapper = new ConditionalPDFWrapper(conditioningDistribution);
      p_conditionalPDFWrapper->setParameter(currentX);
      const Scalar cdfConditioned = algo.integrate(UniVariateFunction(p_conditionalPDFWrapper), xMin, std::min(x[conditioningDimension], xMax));
      result[conditioningDimension] = cdfConditioned / pdfConditioning;
    }
    currentX.add(x[conditioningDimension]);
    pdfConditioning = conditioningDistribution->computePDF(currentX);
  } // conditioningDimension
  return result;
}

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Point DistributionImplementation::computeConditionalCDF(const Point & x,
    const Sample & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= dimension_) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional CDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Convert the values in x into a Sample
  const UnsignedInteger size = x.getDimension();
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula()))
  {
    SampleImplementation xAsSample(size, 1);
    xAsSample.setData(x);
    return getMarginal(conditioningDimension).computeCDF(xAsSample).getImplementation()->getData();
  }
  // General case
  Indices conditioning(conditioningDimension);
  conditioning.fill();
  Indices conditioned(conditioning);
  conditioned.add(conditioningDimension);
  const Implementation conditioningDistribution(getMarginal(conditioning).getImplementation());
  const Sample pdfConditioning(conditioningDistribution->computePDF(y));
  const Implementation conditionedDistribution(getMarginal(conditioned).getImplementation());
  const Scalar xMin = conditionedDistribution->range_.getLowerBound()[conditioningDimension];
  const Scalar xMax = conditionedDistribution->range_.getUpperBound()[conditioningDimension];
  Point result(size);
  Pointer<ConditionalPDFWrapper> p_conditionalPDFWrapper = new ConditionalPDFWrapper(conditionedDistribution);
  GaussKronrod algo;
  for (UnsignedInteger i = 0; i < size; ++i)
    if (pdfConditioning(i, 0) > 0.0)
    {
      if (x[i] >= xMax) result[i] = 1.0;
      else if (x[i] > xMin)
      {
        // Numerical integration with respect to x
        p_conditionalPDFWrapper->setParameter(y[i]);
        const Scalar value(algo.integrate(UniVariateFunction(p_conditionalPDFWrapper), xMin, x[i]));
        result[i] = SpecFunc::Clip01(value / pdfConditioning(i, 0));
      } // xMin < x < xMax
    } // pdfConditioning(i, 0) > 0
  return result;
}

/* Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
Scalar DistributionImplementation::computeConditionalQuantile(const Scalar q,
    const Point & y) const
{
  if (y.getDimension() == 0 || hasIndependentCopula()) return getMarginal(y.getDimension()).computeScalarQuantile(q);
  return computeConditionalQuantile(Point(1, q), Sample(1, y))[0];
}

/* Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
Point DistributionImplementation::computeSequentialConditionalQuantile(const Point & q) const
{
  Point result(0);
  Point y(0);
  for (UnsignedInteger i = 0; i < dimension_; ++i)
    result.add(computeConditionalQuantile(q[i], result));
  return result;
}

/* Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
Point DistributionImplementation::computeConditionalQuantile(const Point & q,
    const Sample & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= dimension_) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile with a conditioning point of dimension greater or equal to the distribution dimension.";
  const UnsignedInteger size = q.getDimension();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if ((q[i] < 0.0) || (q[i] > 1.0)) throw InvalidArgumentException(HERE) << "Error: point=" << i << ", cannot compute a conditional quantile for a probability level q[" << i << "]=" << q[i] << " outside of [0, 1]";
  }
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula()))
    return getMarginal(conditioningDimension).computeQuantile(q).getImplementation()->getData();
  // General case
  const Scalar xMin = range_.getLowerBound()[conditioningDimension];
  const Scalar xMax = range_.getUpperBound()[conditioningDimension];
  Point result(size);
  // Here we recreate a ConditionalCDFWrapper only if none has been created or if the parameter dimension has changed
  Pointer<ConditionalCDFWrapper> p_conditionalCDFWrapper = new ConditionalCDFWrapper(this);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    p_conditionalCDFWrapper->setParameter(y[i]);
    Brent solver(quantileEpsilon_, cdfEpsilon_, cdfEpsilon_, quantileIterations_);
    result[i] = solver.solve(UniVariateFunction(p_conditionalCDFWrapper), q[i], xMin, xMax, 0.0, 1.0);
  }
  return result;
}

/* Quantile computation for dimension=1 */
Scalar DistributionImplementation::computeScalarQuantile(const Scalar prob,
    const Bool tail) const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "Error: the method computeScalarQuantile is only defined for 1D distributions";
  // This test allows one to check if one can trust the current range. If not, it means that we are here to compute the range and then we cannot rely on it!
  Scalar lower = range_.getLowerBound()[0];
  Scalar upper = range_.getUpperBound()[0];
  // This test allows one to know if the range has already been computed. If not, it is the role of the computeScalarQuantile() to do it.
  if (lower > upper)
  {
    LOGDEBUG("DistributionImplementation::computeScalarQuantile: look for a bracketing of the bounds of the range");
    // Find a rough estimate of the lower bound and the upper bound
    Scalar step = 1.0;
    Scalar cdf = computeCDF(lower);
    if (cdf >= cdfEpsilon_)
    {
      // negative lower bound
      lower -= step;
      cdf = computeCDF(lower);
      while (cdf >= cdfEpsilon_)
      {
        step *= 2.0;
        lower -= step;
        cdf = computeCDF(lower);
      }
    }
    else
    {
      // positive lower bound
      lower += step;
      cdf = computeCDF(lower);
      while (computeCDF(lower) <= cdfEpsilon_)
      {
        step *= 2.0;
        lower += step;
        cdf = computeCDF(lower);
      }
    }
    // Here, lower is a rough estimate of the lower bound
    // Go to the upper bound
    upper = lower;
    step = 1.0;
    Scalar ccdf = computeComplementaryCDF(upper);
    while (ccdf >= cdfEpsilon_)
    {
      upper += step;
      step *= 2.0;
      ccdf = computeComplementaryCDF(upper);
    }
  }
  LOGDEBUG(OSS() << "DistributionImplementation::computeScalarQuantile: lower=" << lower << ", upper=" << upper);
  if (prob < 0.0) return (tail ? upper : lower);
  if (prob >= 1.0) return (tail ? lower : upper);
  const Scalar q = tail ? 1.0 - prob : prob;
  const CDFWrapper wrapper(this);
  const Function f(bindMethod<CDFWrapper, Point, Point>(wrapper, &CDFWrapper::computeCDF, 1, 1));
  const Scalar leftTau = lower;
  const Scalar leftCDF = 0.0;
  const Scalar rightTau = upper;
  const Scalar rightCDF = 1.0;
  Brent solver(quantileEpsilon_, cdfEpsilon_, cdfEpsilon_, quantileIterations_);
  const Scalar root = solver.solve(f, q, leftTau, rightTau, leftCDF, rightCDF);
  LOGDEBUG(OSS() << "root=" << root);
  return root;
} // computeScalarQuantile


// Structure used to implement the computeQuantile() method efficiently
struct CopulaQuantileWrapper
{
  CopulaQuantileWrapper(const DistributionImplementation * p_distribution)
    : p_distribution_(p_distribution)
    , dimension_(p_distribution->getDimension())
  {
    // Nothing to do
  }

  Point computeDiagonal(const Point & u) const
  {
    const Point point(dimension_, u[0]);
    const Scalar cdf = p_distribution_->computeCDF(point);
    const Point value(1, cdf);
    return value;
  }

  const DistributionImplementation * p_distribution_;
  const UnsignedInteger dimension_;
}; // struct CopulaQuantileWrapper

/* Generic implementation of the quantile computation for copulas */
Point DistributionImplementation::computeQuantileCopula(const Scalar prob,
    const Bool tail) const
{
  const UnsignedInteger dimension = getDimension();
  // Special case for bording values
  const Scalar q = tail ? 1.0 - prob : prob;
  if (q <= 0.0) return Point(dimension, 0.0);
  if (q >= 1.0) return Point(dimension, 1.0);
  // Special case for dimension 1
  if (dimension == 1) return Point(1, q);
  CopulaQuantileWrapper wrapper(this);
  const Function f(bindMethod<CopulaQuantileWrapper, Point, Point>(wrapper, &CopulaQuantileWrapper::computeDiagonal, 1, 1));
  Scalar leftTau = q;
  const Point leftPoint(1, leftTau);
  const Point leftValue(f(leftPoint));
  Scalar leftCDF = leftValue[0];
  // Upper bound of the bracketing interval
  Scalar rightTau = 1.0 - (1.0 - q) / dimension;
  Point rightPoint(1, rightTau);
  const Point rightValue(f(rightPoint));
  Scalar rightCDF = rightValue[0];
  // Use Brent's method to compute the quantile efficiently
  Brent solver(cdfEpsilon_, cdfEpsilon_, cdfEpsilon_, quantileIterations_);
  return Point(dimension, solver.solve(f, q, leftTau, rightTau, leftCDF, rightCDF));
}

/* Generic implementation of the quantile computation */
Point DistributionImplementation::computeQuantile(const Scalar prob,
    const Bool tail) const
{
  if (isCopula())
    return computeQuantileCopula(prob, tail);
  Scalar marginalProb = 0.0;
  return computeQuantile(prob, tail, marginalProb);
}

Point DistributionImplementation::computeQuantile(const Scalar prob,
    const Bool tail,
    Scalar & marginalProb) const
{
  const Scalar q = tail ? 1.0 - prob : prob;
  marginalProb = q;
  // Special case for bording values
  if (prob < quantileEpsilon_) return (tail ? range_.getUpperBound() : range_.getLowerBound());
  if (prob >= 1.0 - quantileEpsilon_) return (tail ? range_.getLowerBound() : range_.getUpperBound());
  // Special case for dimension 1
  if (dimension_ == 1) return Point(1, computeScalarQuantile(prob, tail));
  // Special case for independent copula
  if (hasIndependentCopula())
  {
    Point result(dimension_);
    marginalProb = std::pow(q, 1.0 / dimension_);
    for (UnsignedInteger i = 0; i < dimension_; ++i) result[i] = getMarginal(i).computeScalarQuantile(marginalProb);
    return result;
  }
  // Extract the marginal distributions
  Collection<Implementation> marginals(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; i++) marginals[i] = getMarginal(i).getImplementation();
  // The n-D quantile is defined as X(\tau) = (F_1^{-1}(\tau), ..., F_n^{-1}(\tau)),
  // with tau such as F(X(\tau)) = q.
  // As F(x) = C(F_1(x_1),...,F_n(x_n)), the constraint F(X(\tau)) = q reads:
  // C(\tau,...,\tau) = q
  // Bracketing of \tau using the Frechet Hoeffding bounds:
  // max(n\tau - n + 1, 0) <= C(\tau,...,\tau) <= \tau
  // from which we deduce that q <= \tau and \tau <= 1 - (1 - q) / n
  // Lower bound of the bracketing interval
  const QuantileWrapper wrapper(marginals, this);
  const Function f(bindMethod<QuantileWrapper, Point, Point>(wrapper, &QuantileWrapper::computeDiagonal, 1, 1));
  Scalar leftTau = q;
  Scalar leftCDF = f(Point(1, leftTau))[0];
  // Due to numerical precision issues, the theoretical bound can be slightly violated
  if (leftCDF > prob)
  {
    leftTau = 0.0;
    leftCDF = 0.0;
  }
  // Upper bound of the bracketing interval
  Scalar rightTau = 1.0 - (1.0 - q) / dimension_;
  Scalar rightCDF = f(Point(1, rightTau))[0];
  // Due to numerical precision issues, the theoretical bound can be slightly violated
  if (rightCDF < prob)
  {
    rightTau = 1.0;
    rightCDF = 1.0;
  }
  LOGDEBUG(OSS() << "DistributionImplementation::computeQuantile: dimension=" << dimension_ << ", q=" << q << ", leftTau=" << leftTau << ", leftCDF=" << leftCDF << ", rightTau=" << rightTau << ", rightCDF=" << rightCDF);
  // Use Brent's method to compute the quantile efficiently for continuous distributions
  const Brent solver(quantileEpsilon_, cdfEpsilon_, cdfEpsilon_, quantileIterations_);
  marginalProb = solver.solve(f, q, leftTau, rightTau, leftCDF, rightCDF);
  LOGINFO(OSS(false) << "tau=" << marginalProb);
  return wrapper.diagonalToSpace(marginalProb);
}

/* Get the minimum volume interval containing at least a given probability of the distribution.
   The minimum volume interval [a, b] is such that:
   a\in[lowerBound, F^{-1}(1-p)]
   b = F^{-1}(p+F(a))
   f(a) = f(b) = f(F^{-1}(p+F(a)))
   so we look for the root of f(F^{-1}(p+F(a))) - f(a)
*/
struct MinimumVolumeIntervalWrapper
{
  MinimumVolumeIntervalWrapper(const DistributionImplementation * p_distribution,
                               const Collection<Distribution> & marginals,
                               const Scalar prob)
    : p_distribution_(p_distribution)
    , marginals_(marginals)
    , prob_(prob)
  {
    // Nothing to do
  }

  MinimumVolumeIntervalWrapper(const DistributionImplementation * p_distribution,
                               const Scalar prob)
    : p_distribution_(p_distribution)
    , marginals_(0)
    , prob_(prob)
  {
    // Nothing to do
  }

  // The minimum volume interval [a, b] is such that:
  // a\in[lowerBound, F^{-1}(1-p)]
  // b = F^{-1}(p+F(a))
  // f(a) = f(b) = f(F^{-1}(p+F(a)))
  // Here we compute f(F^{-1}(p+F(a))) - f(a)
  Point operator() (const Point & A) const
  {
    const Scalar B = computeB(A[0]);
    const Scalar pdfB = p_distribution_->computePDF(B);
    const Scalar pdfA = p_distribution_->computePDF(A);
    return Point(1, pdfB - pdfA);
  }

  Point objective(const Point & A) const
  {
    const Scalar B = computeB(A[0]);
    return Point(1, B - A[0]);
  }

  Scalar computeB(const Scalar A) const
  {
    const Scalar alphaB = prob_ + p_distribution_->computeCDF(A);
    const Scalar B = p_distribution_->computeQuantile(alphaB)[0];
    return B;
  }

  Interval buildBilateralInterval(const Scalar beta) const
  {
    const UnsignedInteger size(marginals_.getSize());
    Point lower(size);
    Point upper(size);
    const Scalar alpha(0.5 * (1.0 - beta));
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      lower[i] = marginals_[i].computeQuantile(alpha, false)[0];
      upper[i] = marginals_[i].computeQuantile(alpha, true)[0];
    }
    return Interval(lower, upper);
  }

  Interval buildMinimumVolumeInterval(const Scalar beta) const
  {
    const UnsignedInteger size(marginals_.getSize());
    Point lower(size);
    Point upper(size);
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const Interval marginalIC(marginals_[i].computeMinimumVolumeInterval(beta));
      lower[i] = marginalIC.getLowerBound()[0];
      upper[i] = marginalIC.getUpperBound()[0];
    }
    return Interval(lower, upper);
  }

  Point computeBilateralProbability(const Point & beta) const
  {
    const Interval IC(buildBilateralInterval(beta[0]));
    const Scalar probability = p_distribution_->computeProbability(IC);
    return Point(1, probability);
  }

  Point computeMinimumVolumeProbability(const Point & beta) const
  {
    const Interval IC(buildMinimumVolumeInterval(beta[0]));
    const Scalar probability = p_distribution_->computeProbability(IC);
    return Point(1, probability);
  }

  const DistributionImplementation * p_distribution_;
  Collection<Distribution> marginals_;
  const Scalar prob_;
}; // struct MinimumVolumeIntervalWrapper

Interval DistributionImplementation::computeMinimumVolumeInterval(const Scalar prob) const
{
  Scalar marginalProb = -1.0;
  return computeMinimumVolumeIntervalWithMarginalProbability(prob, marginalProb);
}

Interval DistributionImplementation::computeMinimumVolumeIntervalWithMarginalProbability(const Scalar prob,
    Scalar & marginalProb) const
{
  if (!isContinuous()) throw NotYetImplementedException(HERE) << "In DistributionImplementation::computeMinimumVolumeInterval()";
  // If the distribution is elliptical, the minimum volume interval is equal to the bilateral confidence interval which is much cheaper to compute
  if (isElliptical())
  {
    LOGINFO("Compute the minimum volume interval using the bilateral confidence interval (elliptical case)");
    const Interval result(computeBilateralConfidenceIntervalWithMarginalProbability(prob, marginalProb));
    return result;
  }
  if (prob <= 0.0)
  {
    const Point median(computeQuantile(0.5));
    marginalProb = 0.0;
    return Interval(median, median);
  }
  if (prob >= 1.0)
  {
    marginalProb = 1.0;
    return range_;
  }
  if (dimension_ == 1)
  {
    // First, the most accurate method, which assumes a continuous PDF
    try
    {
      const Interval result(computeUnivariateMinimumVolumeIntervalByRootFinding(prob, marginalProb));
      LOGINFO("Compute the minimum volume interval by root finding (continuous case)");
      return result;
    }
    // Second, the general purpose method
    catch(...)
    {
      const Interval result(computeUnivariateMinimumVolumeIntervalByOptimization(prob, marginalProb));
      LOGINFO("Compute the minimum volume interval by optimization (general case)");
      return result;
    }
  }
  Collection<Distribution> marginals(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i) marginals[i] = getMarginal(i);
  const MinimumVolumeIntervalWrapper minimumVolumeIntervalWrapper(this, marginals, prob);
  const Function function(bindMethod<MinimumVolumeIntervalWrapper, Point, Point>(minimumVolumeIntervalWrapper, &MinimumVolumeIntervalWrapper::computeMinimumVolumeProbability, 1, 1));
  Brent solver(quantileEpsilon_, pdfEpsilon_, pdfEpsilon_, quantileIterations_);
  // Here the equation we have to solve is P(X\in IC(\beta))=prob
  marginalProb = solver.solve(function, prob, 0.0, 1.0, 0.0, 1.0);
  const Interval IC(minimumVolumeIntervalWrapper.buildMinimumVolumeInterval(marginalProb));
  return IC;
}

/* If the density is continuous, we have to solve PDF(b) - PDF(a) == 0 with F(b)-F(a)=prob, b>=a
   ie b=F^{-1}(prob+F(a))
*/
Interval DistributionImplementation::computeUnivariateMinimumVolumeIntervalByRootFinding(const Scalar prob,
    Scalar & marginalProb) const
{
  const MinimumVolumeIntervalWrapper minimumVolumeIntervalWrapper(this, prob);
  const Function function(bindMethod<MinimumVolumeIntervalWrapper, Point, Point>(minimumVolumeIntervalWrapper, &MinimumVolumeIntervalWrapper::operator(), 1, 1));
  Brent solver(quantileEpsilon_, pdfEpsilon_, pdfEpsilon_, quantileIterations_);
  const Scalar xMin = range_.getLowerBound()[0];
  const Scalar xMax = computeScalarQuantile(prob, true);
  const Scalar a = solver.solve(function, 0.0, xMin, xMax);
  const Scalar b = minimumVolumeIntervalWrapper.computeB(a);
  marginalProb = prob;
  return Interval(a, b);
}

/* We minimize b-a with the constraint F(b)-F(a)=prob, b>=a
   ie b=F^{-1}(prob+F(a))
*/
Interval DistributionImplementation::computeUnivariateMinimumVolumeIntervalByOptimization(const Scalar prob,
    Scalar & marginalProb) const
{
  const MinimumVolumeIntervalWrapper minimumVolumeIntervalWrapper(this, prob);
  const Function objective(bindMethod<MinimumVolumeIntervalWrapper, Point, Point>(minimumVolumeIntervalWrapper, &MinimumVolumeIntervalWrapper::objective, 1, 1));
  OptimizationProblem problem(objective);
  problem.setBounds(Interval(range_.getLowerBound(), computeQuantile(prob, true)));
  Cobyla solver(problem);

  // if the tail is on the right side, the result is more likely close to the lower bound
  if (getSkewness()[0] > 0.0)
    solver.setStartingPoint(range_.getLowerBound());
  else
    solver.setStartingPoint(computeQuantile(prob, true));

  solver.run();
  const Scalar a = solver.getResult().getOptimalPoint()[0];
  const Scalar b = minimumVolumeIntervalWrapper.computeB(a);
  marginalProb = prob;
  return Interval(a, b);
}

/* Get the product bilateral confidence interval containing a given probability of the distribution */
Interval DistributionImplementation::computeBilateralConfidenceInterval(const Scalar prob) const
{
  Scalar marginalProb = -1.0;
  return computeBilateralConfidenceIntervalWithMarginalProbability(prob, marginalProb);
}

Interval DistributionImplementation::computeBilateralConfidenceIntervalWithMarginalProbability(const Scalar prob,
    Scalar & marginalProb) const
{
  if (prob <= 0.0)
  {
    const Point median(computeQuantile(0.5));
    marginalProb = 0.0;
    return Interval(median, median);
  }
  if (prob >= 1.0)
  {
    marginalProb = 1.0;
    return range_;
  }
  if (dimension_ == 1)
  {
    marginalProb = prob;
    const Interval IC(computeQuantile(0.5 * (1.0 - prob), false), computeQuantile(0.5 * (1.0 - prob), true));
    return IC;
  }
  if (!isContinuous()) throw NotYetImplementedException(HERE) << "In DistributionImplementation::computeBilateralConfidenceIntervalWithMarginalProbability() for non continuous multivariate distributions";
  Collection<Distribution> marginals(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i) marginals[i] = getMarginal(i);
  const MinimumVolumeIntervalWrapper minimumVolumeIntervalWrapper(this, marginals, prob);
  const Function function(bindMethod<MinimumVolumeIntervalWrapper, Point, Point>(minimumVolumeIntervalWrapper, &MinimumVolumeIntervalWrapper::computeBilateralProbability, 1, 1));
  Brent solver(quantileEpsilon_, pdfEpsilon_, pdfEpsilon_, quantileIterations_);
  marginalProb = solver.solve(function, prob, 0.0, 1.0, 0.0, 1.0);
  const Interval IC(minimumVolumeIntervalWrapper.buildBilateralInterval(marginalProb));
  return IC;
}

/* Get the product unilateral confidence interval containing a given probability of the distribution */
Interval DistributionImplementation::computeUnilateralConfidenceInterval(const Scalar prob,
    const Bool tail) const
{
  Scalar marginalProb = -1.0;
  return computeUnilateralConfidenceIntervalWithMarginalProbability(prob, tail, marginalProb);
}

Interval DistributionImplementation::computeUnilateralConfidenceIntervalWithMarginalProbability(const Scalar prob,
    const Bool tail,
    Scalar & marginalProb) const
{
  marginalProb = -1.0;
  if (tail)
  {
    const Point lowerBound(computeInverseSurvivalFunction(prob, marginalProb));
    return Interval(lowerBound, range_.getUpperBound());
  }
  const Point upperBound(computeQuantile(prob, false, marginalProb));
  return Interval(range_.getLowerBound(), upperBound);
}

/* Get the minimum volume level set containing at least a given probability of the distribution.
   The minimum volume level A(p) set is such that A(p)={x\in R^n | y(x) <= y_p}
   where y(x)=-\log X and y_p is the p-quantile of Y=pdf(X)
*/
LevelSet DistributionImplementation::computeMinimumVolumeLevelSet(const Scalar prob) const
{
  Scalar threshold = -1.0;
  return computeMinimumVolumeLevelSetWithThreshold(prob, threshold);
}

LevelSet DistributionImplementation::computeMinimumVolumeLevelSetWithThreshold(const Scalar prob,
    Scalar & threshold) const
{
  if (!isContinuous()) throw NotYetImplementedException(HERE) << "In DistributionImplementation::computeMinimumVolumeLevelSet()";
  // 1D special case here to avoid a double construction of minimumVolumeLevelSetFunction
  if ((dimension_ == 1) && (ResourceMap::GetAsBool("Distribution-MinimumVolumeLevelSetBySampling")))
  {
    LOGINFO("Compute the minimum volume level set by sampling (QMC)");
    const LevelSet result(computeUnivariateMinimumVolumeLevelSetByQMC(prob, threshold));
    return result;
  }
  if (!(prob >= 0.0) || !(prob <= 1.0))
    throw InvalidArgumentException(HERE) << "The probability must be in [0, 1] here prob=" << prob;
  Function minimumVolumeLevelSetFunction(MinimumVolumeLevelSetEvaluation(clone()).clone());
  minimumVolumeLevelSetFunction.setGradient(MinimumVolumeLevelSetGradient(clone()).clone());
  // If dimension_ == 1 the threshold can be computed analyticaly
  Scalar minusLogPDFThreshold;
  if (dimension_ == 1)
  {
    const CompositeDistribution composite(minimumVolumeLevelSetFunction, *this);
    minusLogPDFThreshold = composite.computeScalarQuantile(prob);
    LOGINFO("Compute the minimum volume level set by using a composite distribution quantile (univariate general case)");
  } // dimension == 1
  else
  {
    LOGINFO("Compute the minimum volume level set by sampling (Monte Carlo)");
    const UnsignedInteger size = ResourceMap::GetAsUnsignedInteger("Distribution-MinimumVolumeLevelSetSamplingSize");
    const Sample xSample(getSample(size));
    const Sample logPDFSample(computeLogPDF(xSample));
    minusLogPDFThreshold = -logPDFSample.computeQuantile(1.0 - prob)[0];
  } // dimension > 1
  threshold = std::exp(-minusLogPDFThreshold);

  return LevelSet(minimumVolumeLevelSetFunction, LessOrEqual(), minusLogPDFThreshold);
}

LevelSet DistributionImplementation::computeUnivariateMinimumVolumeLevelSetByQMC(const Scalar prob,
    Scalar & threshold) const
{
  Function minimumVolumeLevelSetFunction(MinimumVolumeLevelSetEvaluation(clone()).clone());
  minimumVolumeLevelSetFunction.setGradient(MinimumVolumeLevelSetGradient(clone()).clone());
  // As we are in 1D and as the function defining the composite distribution can have complex variations,
  // we use an improved sampling method to compute the quantile of the -logPDF(X) distribution
  const UnsignedInteger size = SpecFunc::NextPowerOfTwo(ResourceMap::GetAsUnsignedInteger("Distribution-MinimumVolumeLevelSetSamplingSize"));
  const Sample xQMC(getSampleByQMC(size));
  const Sample logPDFSample(computeLogPDF(xQMC));
  const Scalar minusLogPDFThreshold = -logPDFSample.computeQuantile(1.0 - prob)[0];
  threshold = std::exp(-minusLogPDFThreshold);

  return LevelSet(minimumVolumeLevelSetFunction, LessOrEqual(), minusLogPDFThreshold);
}

/* Get the mathematical and numerical range of the distribution.
   Its mathematical range is the smallest closed interval outside
   of which the PDF is zero, and the numerical range is the interval
   outside of which the PDF is rounded to zero in double precision */
Interval DistributionImplementation::getRange() const
{
  return range_;
}

void DistributionImplementation::setRange(const Interval & range)
{
  if (range.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given range has a dimension incompatible with the dimension of the distribution.";
  range_ = range;
}

/* Compute the numerical range of the distribution given the parameters values */
void DistributionImplementation::computeRange()
{
  // Quick return for copulas
  if (isCopula())
  {
    range_ = Interval(dimension_);
    return;
  }
  const Interval::BoolCollection finiteLowerBound(dimension_, false);
  const Interval::BoolCollection finiteUpperBound(dimension_, false);
  // Initialize the range with inverted bounds in order to inform the generic implementation of the
  // computeScalarQuantile() method that it cannot trust the current range
  range_ = Interval(Point(dimension_, 1.0), Point(dimension_, -1.0), finiteLowerBound, finiteUpperBound);
  setRange(Interval(computeLowerBound(), computeUpperBound(), finiteLowerBound, finiteUpperBound));
}

/* Compute the lower bound of the range */
Point DistributionImplementation::computeLowerBound() const
{
  // For a multivariate distribution, the range is the axes aligned box that fits to the marginal ranges
  Point lowerBound(dimension_);
  // Here, we must separate the 1D case from the nD case as the getMarginal() method is generic for 1D case and
  // would involve a circular call to computeRange()
  if (dimension_ == 1) lowerBound[0] = computeScalarQuantile(cdfEpsilon_);
  else for (UnsignedInteger i = 0; i < dimension_; ++i) lowerBound[i] = getMarginal(i).computeScalarQuantile(cdfEpsilon_);
  return lowerBound;
}

/* Compute the upper bound of the range */
Point DistributionImplementation::computeUpperBound() const
{
  // For a multivariate distribution, the range is the axes aligned box that fits to the marginal ranges
  Point upperBound(dimension_);
  if (dimension_ == 1) upperBound[0] = computeScalarQuantile(cdfEpsilon_, true);
  else for (UnsignedInteger i = 0; i < dimension_; ++i) upperBound[i] = getMarginal(i).computeScalarQuantile(cdfEpsilon_, true);
  return upperBound;
}

/* Compute the mean of the distribution */
void DistributionImplementation::computeMean() const
{
  mean_ = getShiftedMoment(1, Point(getDimension(), 0.0));
  isAlreadyComputedMean_ = true;
}

/* Get the mean of the distribution */
Point DistributionImplementation::getMean() const
{
  if (isCopula())
    return Point(getDimension(), 0.5);
  if (!isAlreadyComputedMean_) computeMean();
  return mean_;
}

/* Get the standard deviation of the distribution */
Point DistributionImplementation::getStandardDeviation() const
{
  // In the case of dimension==1, use the covariance matrix. The call
  // to getCovariance() reuse the covariance if it has already been computed.
  if (dimension_ == 1) return Point(1, std::sqrt(getCovariance()(0, 0)));
  // In higher dimension, either use the covariance if it has already been
  // computed...

  if (isCopula())
    // 0.2886751345948128822545744 = 1 / sqrt(12)
    return Point(getDimension(), 0.2886751345948128822545744);

  if (isAlreadyComputedCovariance_)
  {
    Point result(dimension_);
    for (UnsignedInteger i = 0; i < dimension_; ++i) result[i] = std::sqrt(covariance_(i, i));
    return result;
  }
  // ... or compute only the marginal variances.
  const Point variance(getCentralMoment(2));
  Point result(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i) result[i] = std::sqrt(variance[i]);
  return result;
}

/* Get the skewness of the distribution */
Point DistributionImplementation::getSkewness() const
{
  if (isCopula())
    return Point(getDimension(), 0.0);
  const Point variance(getCentralMoment(2));
  const Point thirdMoment(getCentralMoment(3));
  Point result(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i) result[i] = thirdMoment[i] / std::pow(variance[i], 1.5);
  return result;
}

/* Get the kurtosis of the distribution */
Point DistributionImplementation::getKurtosis() const
{
  if (isCopula())
    // 1.8 = 9/5
    return Point(getDimension(), 1.8);

  const Point variance(getCentralMoment(2));
  const Point fourthMoment(getCentralMoment(4));
  Point result(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i) result[i] = fourthMoment[i] / std::pow(variance[i], 2.0);
  return result;
}

/* Get the moments of the distribution */
Point DistributionImplementation::getMoment(const UnsignedInteger n) const
{
  if (n == 0) return Point(dimension_, 1.0);
  return getShiftedMoment(n, Point(dimension_, 0.0));
}

/* Get the central moments of the distribution */
Point DistributionImplementation::getCentralMoment(const UnsignedInteger n) const
{
  if (n == 0) return Point(dimension_, 1.0);
  if (n == 1) return Point(dimension_, 0.0);
  return getShiftedMoment(n, getMean());
}

/* Compute the covariance of the distribution */
void DistributionImplementation::computeCovariance() const
{
  if (isCopula()) computeCovarianceCopula();
  else if (isContinuous()) computeCovarianceContinuous();
  else if (isDiscrete()) computeCovarianceDiscrete();
  else computeCovarianceGeneral();
}



struct CopulaCovarianceWrapper
{
  CopulaCovarianceWrapper(const Distribution & distribution)
    : distribution_(distribution)
  {
    // Nothing to do
  }

  Point kernel(const Point & point) const
  {
    return Point(1, distribution_.computeCDF(point) - point[0] * point[1]);
  }

  const Distribution & distribution_;
};

/* Compute the covariance of the copula */
void DistributionImplementation::computeCovarianceCopula() const
{
  const UnsignedInteger dimension = getDimension();
  // We need this to initialize the covariance matrix in two cases:
  // + this is the first call to this routine (which could be checked by testing the dimension of the copula and the dimension of the matrix
  // + the copula has changed from a non-independent one to the independent copula
  covariance_ = CovarianceMatrix(dimension);
  // First the diagonal terms, which are the marginal covariances
  // Uniform marginals, the diagonal is 1/12
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    // 0.08333333333333333333333333 = 1 / 12
    covariance_(i, i) = 0.08333333333333333333333333;
  }
  // Off-diagonal terms if the copula is not the independent copula
  if (!hasIndependentCopula())
  {
    const IteratedQuadrature integrator;
    const Interval unitSquare(Point(2, 0.0), Point(2, 1.0));
    // Performs the integration for each covariance in the strictly lower triangle of the covariance matrix
    // We start with the loop over the coefficients because the most expensive task is to get the 2D marginal copulas
    Indices indices(2);
    for(UnsignedInteger rowIndex = 0; rowIndex < dimension; ++rowIndex)
    {
      indices[0] = rowIndex;
      for(UnsignedInteger columnIndex = rowIndex + 1; columnIndex < dimension; ++columnIndex)
      {
        indices[1] = columnIndex;
        // For the usual case of a bidimensional copula, no need to extract marginal distributions
        Distribution marginalDistribution(*this);
        if (dimension > 2) marginalDistribution = getMarginal(indices);
        if (!marginalDistribution.getImplementation()->hasIndependentCopula())
        {
          // Build the integrand
          CopulaCovarianceWrapper functionWrapper(marginalDistribution);
          Function function(bindMethod<CopulaCovarianceWrapper, Point, Point>(functionWrapper, &CopulaCovarianceWrapper::kernel, 2, 1));
          // Compute the covariance element
          covariance_(rowIndex, columnIndex) = integrator.integrate(function, unitSquare)[0];
        }
      } // loop over column indices
    } // loop over row indices
  } // if !hasIndependentCopula
  isAlreadyComputedCovariance_ = true;
} // computeCovariance


void DistributionImplementation::computeCovarianceContinuous() const
{
  // We need this to initialize the covariance matrix in two cases:
  // + this is the first call to this routine (which could be checked by testing the dimension of the distribution and the dimension of the matrix
  // + the copula has changed from a non-independent one to the independent copula
  mean_ = getMean();
  covariance_ = CovarianceMatrix(dimension_);
  // First the diagonal terms, which are the marginal covariances
  // Marginal covariances
  const Point variance(getCentralMoment(2));
  for (UnsignedInteger component = 0; component < dimension_; ++component) covariance_(component, component) = variance[component];
  // Off-diagonal terms if the copula is not the independent copula
  if (!hasIndependentCopula())
  {
    // Here we use the following expression of the covariance \Sigma_{i,j}:
    // \Sigma_{i,j}=\int_{\R^2}(x_i-\mu_i)(x_j-\mu_j)p_{i,j}(x_i,x_j)dx_idx_j
    // Do we use the adaptive quadrature algorithm?
    const Bool useAdaptiveAlgorithm = ResourceMap::GetAsBool("Distribution-UseCovarianceAdaptiveAlgorithm");
    IntegrationAlgorithm integrator;
    if (useAdaptiveAlgorithm) integrator = IteratedQuadrature(GaussKronrod());
    else integrator = GaussLegendre(Indices(2, static_cast<UnsignedInteger>(std::ceil(std::sqrt(1.0 * integrationNodesNumber_)))));
    // Performs the integration for each covariance in the strictly lower triangle of the covariance matrix
    // We loop over the coefficients in the outer loop because the most expensive task is to get the 2D marginal distributions
    Indices indices(2);
    for(UnsignedInteger rowIndex = 0; rowIndex < dimension_; ++rowIndex)
    {
      indices[0] = rowIndex;
      const Scalar muI = mean_[rowIndex];
      for (UnsignedInteger columnIndex = rowIndex + 1; columnIndex < dimension_; ++columnIndex)
      {
        indices[1] = columnIndex;
        const Scalar muJ = mean_[columnIndex];
        const Implementation marginalDistribution(getMarginal(indices).getImplementation());
        if (!marginalDistribution->hasIndependentCopula())
        {
          // Compute the covariance element
          const CovarianceWrapper kernel(marginalDistribution, muI, muJ);
          const Interval interval(marginalDistribution->range_);
          LOGINFO(OSS() << "Compute covariance(" << rowIndex << ", " << columnIndex << ")");
          const Point value(integrator.integrate(kernel, interval));
          LOGINFO(OSS() << "covariance(" << rowIndex << ", " << columnIndex << ")=" << value[0]);
          covariance_(rowIndex, columnIndex) = value[0];
        }
      } // loop over column indices
    } // loop over row indices
  } // if !hasIndependentCopula
  isAlreadyComputedCovariance_ = true;
} // computeCovarianceContinuous

void DistributionImplementation::computeCovarianceDiscrete() const
{
  // We need this to initialize the covariance matrix in two cases:
  // + this is the first call to this routine (which could be checked by testing the dimension of the distribution and the dimension of the matrix
  // + the copula has changed from a non-independent one to the independent copula
  covariance_ = CovarianceMatrix(dimension_);
  // First the diagonal terms, which are the marginal covariances
  // Marginal covariances
  const Point variance(getCentralMoment(2));
  for(UnsignedInteger component = 0; component < dimension_; ++component) covariance_(component, component) = variance[component];
  // Off-diagonal terms if the copula is not the independent copula
  if (!hasIndependentCopula())
  {
    // To ensure that the mean is up to date
    mean_ = getMean();
    // Performs the integration for each covariance in the strictly lower triangle of the covariance matrix
    // We first loop over the coefficients because the most expensive task is to get the 2D marginal distributions
    Indices indices(2);
    for(UnsignedInteger rowIndex = 0; rowIndex < dimension_; ++rowIndex)
    {
      indices[0] = rowIndex;
      const Scalar muI = mean_[rowIndex];
      for(UnsignedInteger columnIndex = rowIndex + 1; columnIndex < dimension_; ++columnIndex)
      {
        indices[1] = columnIndex;
        const Scalar muJ = mean_[columnIndex];
        const Implementation marginalDistribution(getMarginal(indices).getImplementation());
        if (!marginalDistribution->hasIndependentCopula())
        {
          const Sample support(marginalDistribution->getSupport());
          const Point probabilities(marginalDistribution->getProbabilities());
          Scalar value = 0.0;
          const UnsignedInteger size = support.getSize();
          for (UnsignedInteger i = 0; i < size; ++i) value += (support(i, 0) - muI) * (support(i, 1) - muJ) * probabilities[i];
          covariance_(rowIndex, columnIndex) = value;
        }
      } // loop over column indices
    } // loop over row indices
  } // if !hasIndependentCopula
  isAlreadyComputedCovariance_ = true;
}



void DistributionImplementation::computeCovarianceGeneral() const
{
  // We need this to initialize the covariance matrix in two cases:
  // + this is the first call to this routine (which could be checked by testing the dimension of the distribution and the dimension of the matrix
  // + the copula has changed from a non-independent one to the independent copula
  covariance_ = CovarianceMatrix(dimension_);
  // First the diagonal terms, which are the marginal covariances
  // To ensure that the mean is up to date
  mean_ = getMean();
  // Get the standard deviation
  const Point standardDeviation(getStandardDeviation());
  for(UnsignedInteger component = 0; component < dimension_; ++component)
    covariance_(component, component) = standardDeviation[component] * standardDeviation[component];
  // Off-diagonal terms if the copula is not the independent copula
  if (!hasIndependentCopula())
  {
    Collection<Implementation> marginals(dimension_);
    for(UnsignedInteger component = 0; component < dimension_; ++component)
      marginals[component] = getMarginal(component).getImplementation();
    const Scalar delta = 2.0;
    Indices indices(2);
    const int N(8 * 2 * 2 * 2 * 2 * 2);
    const Scalar h = 0.5 / 2 / 2 / 2 / 2 / 2;
    for(UnsignedInteger rowIndex = 0; rowIndex < dimension_; ++rowIndex)
    {
      indices[0] = rowIndex;
      const Scalar mi = marginals[rowIndex]->computeScalarQuantile(0.5);
      const Scalar di = marginals[rowIndex]->computeScalarQuantile(0.75) - marginals[rowIndex]->computeScalarQuantile(0.25);
      // We compute the upper triangle in order to avoid indices swap in marginal
      // extraction
      for(UnsignedInteger columnIndex = rowIndex + 1; columnIndex < dimension_; ++columnIndex)
      {
        indices[1] = columnIndex;
        const Implementation marginalDistribution(getMarginal(indices).getImplementation());
        if (!marginalDistribution->hasIndependentCopula())
        {
          const Scalar mj = marginals[columnIndex]->computeScalarQuantile(0.5);
          const Scalar dj = marginals[columnIndex]->computeScalarQuantile(0.75) - marginals[columnIndex]->computeScalarQuantile(0.25);
          Point xij(2);
          xij[0] = mi;
          xij[1] = mj;
          Scalar covarianceIJ = 0.0;
          // Then we loop over the integration points
          for(int rowNodeIndex = -N; rowNodeIndex < N + 1; ++rowNodeIndex)
          {
            const Scalar hi = h * rowNodeIndex;
            const Scalar expHi = std::exp(hi);
            const Scalar iexpHi = 1.0 / expHi;
            const Scalar sinhHi = 0.5 * (expHi - iexpHi);
            const Scalar expSinhHi = std::exp(sinhHi);
            const Scalar iexpSinhHi = 1.0 / expSinhHi;
            const Scalar iTwoCoshSinhHi = 1.0 / (expSinhHi + iexpSinhHi);
            const Scalar xip = mi + expSinhHi * iTwoCoshSinhHi * di * delta;
            const Scalar wi = (expHi + iexpHi) * iTwoCoshSinhHi * iTwoCoshSinhHi;
            const Scalar cdfip = marginals[rowIndex]->computeCDF(xip);
            for(int columnNodeIndex = -N; columnNodeIndex < N + 1; ++columnNodeIndex)
            {
              const Scalar hj = h * columnNodeIndex;
              const Scalar expHj = std::exp(hj);
              const Scalar iexpHj = 1.0 / expHj;
              const Scalar sinhHj = 0.5 * (expHj - iexpHj);
              const Scalar expSinhHj = std::exp(sinhHj);
              const Scalar iexpSinhHj = 1.0 / expSinhHj;
              const Scalar iTwoCoshSinhHj = 1.0 / (expSinhHj + iexpSinhHj);
              const Scalar xjp = mj + expSinhHj * iTwoCoshSinhHj * dj * delta;
              const Scalar wj = (expHj + iexpHj) * iTwoCoshSinhHj * iTwoCoshSinhHj;
              const Scalar cdfjp = marginals[columnIndex]->computeCDF(xjp);
              Point inpp(2);
              inpp[0] = xip;
              inpp[1] = xjp;
              covarianceIJ += delta * delta * di * dj * h * h * wi * wj * (marginalDistribution->computeCDF(inpp) - cdfip * cdfjp);
            } // loop over J integration nodes
          } // loop over I integration nodes
          covariance_(rowIndex, columnIndex) = covarianceIJ;
        } // if !hasIndependentCopula
      } // loop over column indices
    } // loop over row indices
  }
  isAlreadyComputedCovariance_ = true;
} // computeCovarianceGeneral

/* Get the covariance of the distribution */
CovarianceMatrix DistributionImplementation::getCovariance() const
{
  if (!isAlreadyComputedCovariance_) computeCovariance();
  return covariance_;
}

/* Correlation matrix accessor */
CorrelationMatrix DistributionImplementation::getCorrelation() const
{
  // To make sure the covariance is up to date
  (void) getCovariance();
  CorrelationMatrix R(dimension_);
  Point sigma(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    const Scalar sigmaI = std::sqrt(covariance_(i, i));
    sigma[i] = sigmaI;
    if (sigmaI > 0.0)
      for (UnsignedInteger j = 0; j < i; ++j)
        if (sigma[j] > 0)
          R(i, j) = covariance_(i, j) / (sigmaI * sigma[j]);
  }
  return R;
}

CorrelationMatrix DistributionImplementation::getPearsonCorrelation() const
{
  return getCorrelation();
}

/* Get the Spearman correlation of the distribution */
CorrelationMatrix DistributionImplementation::getSpearmanCorrelation() const
{
  if (isCopula()) return getCorrelation();
  return getCopula().getSpearmanCorrelation();
}

struct DistributionImplementationKendallTauWrapper
{
  DistributionImplementationKendallTauWrapper(const Distribution & distribution)
    : distribution_(distribution)
  {
    if (!distribution.isCopula())
    {
      const UnsignedInteger dimension = distribution.getDimension();
      marginalCollection_ = Collection<Distribution>(dimension);
      for (UnsignedInteger i = 0; i < dimension; ++i)
        marginalCollection_[i] = distribution.getMarginal(i);
    }
  }

  Point kernelForCopula(const Point & point) const
  {
    return Point(1, distribution_.computeCDF(point) * distribution_.computePDF(point));
  }

  Point kernelForDistribution(const Point & point) const
  {
    const UnsignedInteger dimension = distribution_.getDimension();
    Point x(dimension);
    Scalar factor = 1.0;
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      const Point xi(marginalCollection_[i].computeQuantile(point[i]));
      x[i] = xi[0];
      factor *= marginalCollection_[i].computePDF(xi);
      if (std::abs(factor) < SpecFunc::Precision) return Point(1, 0.0);
    }
    return Point(1, distribution_.computeCDF(point) * distribution_.computePDF(x) / factor);
  }

  const Distribution & distribution_;
  Collection<Distribution> marginalCollection_;
}; // DistributionImplementationKendallTauWrapperx

/* Get the Kendall concordance of the distribution */
CorrelationMatrix DistributionImplementation::getKendallTau() const
{
  CorrelationMatrix tau(dimension_);
  // First special case: independent marginals
  if (hasIndependentCopula()) return tau;
  // Second special case: elliptical distribution
  if (hasEllipticalCopula())
  {
    const CorrelationMatrix shape(getShapeMatrix());
    for (UnsignedInteger i = 0; i < dimension_; ++i)
      for(UnsignedInteger j = 0; j < i; ++j)
        tau(i, j) = std::asin(shape(i, j)) * (2.0 / M_PI);
    return tau;
  }
  // General case
  const IteratedQuadrature integrator;
  const Interval square(2);
  // Performs the integration in the strictly lower triangle of the tau matrix
  Indices indices(2);
  for(UnsignedInteger rowIndex = 0; rowIndex < dimension_; ++rowIndex)
  {
    indices[0] = rowIndex;
    for (UnsignedInteger columnIndex = rowIndex + 1; columnIndex < dimension_; ++columnIndex)
    {
      indices[1] = columnIndex;
      const Distribution marginalDistribution(getMarginal(indices).getImplementation());
      if (!marginalDistribution.hasIndependentCopula())
      {
        // Build the integrand
        const DistributionImplementationKendallTauWrapper functionWrapper(marginalDistribution);
        Function function;
        if (isCopula())
          function = (bindMethod<DistributionImplementationKendallTauWrapper, Point, Point>(functionWrapper, &DistributionImplementationKendallTauWrapper::kernelForCopula, 2, 1));
        else
          function = (bindMethod<DistributionImplementationKendallTauWrapper, Point, Point>(functionWrapper, &DistributionImplementationKendallTauWrapper::kernelForDistribution, 2, 1));
        tau(rowIndex, columnIndex) = integrator.integrate(function, square)[0];
      }
    } // loop over column indices
  } // loop over row indices
  return tau;
}

/* Get the shape matrix of the distribution, ie the correlation matrix
   of its copula if it is elliptical */
CorrelationMatrix DistributionImplementation::getShapeMatrix() const
{
  if (!hasEllipticalCopula()) throw NotDefinedException(HERE) << "Error: the shape matrix is defined only for distributions with elliptical copulas.";
  // Easy case: elliptical distribution
  if (isElliptical()) return getCorrelation();
  // Difficult case: elliptical distribution with nonelliptical marginals
  const Collection<Distribution> ellipticalMarginals(dimension_, getStandardDistribution().getMarginal(0));
  return ComposedDistribution(ellipticalMarginals, getCopula()).getCorrelation();
}

/* Cholesky factor of the correlation matrix accessor */
TriangularMatrix DistributionImplementation::getCholesky() const
{
  return getCovariance().computeCholesky();
}

/* Inverse of the Cholesky factor of the correlation matrix accessor */
TriangularMatrix DistributionImplementation::getInverseCholesky() const
{
  // Compute its Cholesky factor
  const TriangularMatrix inverseCholesky(getCholesky().solveLinearSystem(IdentityMatrix(dimension_)).getImplementation());
  return inverseCholesky;
}

/* Compute the nodes and weights for a 1D gauss quadrature over [-1, 1] with respect to the Lebesgue measure */
void DistributionImplementation::computeGaussNodesAndWeights() const
{
  const GaussLegendre integrator(Indices(1, integrationNodesNumber_));
  // Nodes
  gaussNodes_ = integrator.getNodes().getImplementation()->getData() * 2.0 - Point(integrationNodesNumber_, 1.0);
  // Weights
  gaussWeights_ = integrator.getWeights() * 2.0;
  isAlreadyComputedGaussNodesAndWeights_ = true;
}

/* integrationNodesNumber accessors */
UnsignedInteger DistributionImplementation::getIntegrationNodesNumber() const
{
  return integrationNodesNumber_;
}

void DistributionImplementation::setIntegrationNodesNumber(const UnsignedInteger integrationNodesNumber) const
{
  if (integrationNodesNumber != integrationNodesNumber_)
  {
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    isAlreadyComputedGaussNodesAndWeights_ = false;
    integrationNodesNumber_ = integrationNodesNumber;
  }
}


/* Gauss nodes and weights accessor */
Point DistributionImplementation::getGaussNodesAndWeights(Point & weights) const
{
  if (!isAlreadyComputedGaussNodesAndWeights_) computeGaussNodesAndWeights();
  weights = gaussWeights_;
  return gaussNodes_;
}


/* Get the shifted moments of the distribution */
Point DistributionImplementation::getShiftedMoment(const UnsignedInteger n,
    const Point & shift) const
{
  if (isContinuous()) return computeShiftedMomentContinuous(n, shift);
  if (isDiscrete()) return computeShiftedMomentDiscrete(n, shift);
  return computeShiftedMomentGeneral(n, shift);
}

Point DistributionImplementation::computeShiftedMomentContinuous(const UnsignedInteger n,
    const Point & shift) const
{
  if (shift.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the shift dimension must match the distribution dimension.";
  if (n == 0) return Point(dimension_, 1.0);
  Point moment(dimension_);
  // For each component
  GaussKronrod algo;
  for(UnsignedInteger component = 0; component < dimension_; ++component)
  {
    const Implementation marginalDistribution(getMarginal(component).getImplementation());
    const ShiftedMomentWrapper integrand(n, shift[component], marginalDistribution);
    const Scalar a = marginalDistribution->range_.getLowerBound()[0];
    const Scalar b = marginalDistribution->range_.getUpperBound()[0];
    moment[component] = algo.integrate(integrand, Interval(a, b))[0];
  } // End of each component
  return moment;
}

Point DistributionImplementation::computeShiftedMomentDiscrete(const UnsignedInteger n,
    const Point & shift) const
{
  if (n == 0) throw InvalidArgumentException(HERE) << "Error: the centered moments of order 0 are undefined.";
  if (shift.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the shift dimension must match the distribution dimension.";
  Point moment(dimension_);
  const Sample support(getSupport());
  const Point probabilities(getProbabilities());
  for (UnsignedInteger i = 0; i < support.getSize(); ++i)
    for (UnsignedInteger j = 0; j < dimension_; ++j)
      moment[j] += std::pow(support(i, j) - shift[j], static_cast<int>(n)) * probabilities[i];
  return moment;
}

Point DistributionImplementation::computeShiftedMomentGeneral(const UnsignedInteger n,
    const Point & shift) const
{
  if (n == 0) throw InvalidArgumentException(HERE) << "Error: the centered moments of order 0 are undefined.";
  if (shift.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the shift dimension must match the distribution dimension.";
  Point moment(dimension_);
  const Scalar epsilon = std::sqrt(quantileEpsilon_);
  const UnsignedInteger MaximumLevel = ResourceMap::GetAsUnsignedInteger("Distribution-DefaultLevelNumber") + 3;
  // For each component
  for(UnsignedInteger component = 0; component < dimension_; ++component)
  {
    Scalar h = 0.5;
    UnsignedInteger N = 6;
    const Implementation marginalDistribution(getMarginal(component).getImplementation());
    const Scalar shiftComponent = shift[component];
    // Central term
    moment[component] = h * 0.5 * std::pow(marginalDistribution->computeScalarQuantile(0.5), static_cast<int>(n));
    // First block
    for (UnsignedInteger j = 1; j <= N; ++j)
    {
      const Scalar hj = h * j;
      const Scalar expHj = std::exp(hj);
      const Scalar iexpHj = 1.0 / expHj;
      const Scalar sinhHj = 0.5 * (expHj - iexpHj);
      const Scalar expSinhHj = std::exp(sinhHj);
      const Scalar iexpSinhHj = 1.0 / expSinhHj;
      const Scalar iTwoCoshSinhHj = 1.0 / (expSinhHj + iexpSinhHj);
      const Scalar xjm = iexpSinhHj * iTwoCoshSinhHj;
      const Scalar xjp = expSinhHj * iTwoCoshSinhHj;
      const Scalar wj = (expHj + iexpHj) * iTwoCoshSinhHj * iTwoCoshSinhHj;
      moment[component] += h * wj * (std::pow(marginalDistribution->computeQuantile(xjm)[0] - shiftComponent, static_cast<int>(n)) + std::pow(marginalDistribution->computeQuantile(xjp)[0] - shiftComponent, static_cast<int>(n)));
    } // End of first block
    //values[0] = moment[component];
    // Sequential addition of half-blocks
    Scalar error = 1.0;
    UnsignedInteger level = 0;
    while( (error > epsilon) && (level < MaximumLevel))
    {
      ++level;
      h *= 0.5;
      moment[component] *= 0.5;
      Scalar delta = 0.0;
      for (UnsignedInteger j = 0; j <= N; ++j)
      {
        const Scalar hj = h * (2 * j + 1);
        const Scalar expHj = std::exp(hj);
        const Scalar iexpHj = 1.0 / expHj;
        const Scalar sinhHj = 0.5 * (expHj - iexpHj);
        const Scalar expSinhHj = std::exp(sinhHj);
        const Scalar iexpSinhHj = 1.0 / expSinhHj;
        const Scalar iTwoCoshSinhHj = 1.0 / (expSinhHj + iexpSinhHj);
        const Scalar xjm = iexpSinhHj * iTwoCoshSinhHj;
        const Scalar xjp = expSinhHj * iTwoCoshSinhHj;
        Scalar wj = (expHj + iexpHj) * iTwoCoshSinhHj * iTwoCoshSinhHj;
        delta += h * wj * (std::pow(marginalDistribution->computeQuantile(xjm)[0] - shiftComponent, static_cast<int>(n)) + std::pow(marginalDistribution->computeQuantile(xjp)[0] - shiftComponent, static_cast<int>(n)));
      }
      error = std::abs((delta - moment[component]) / (1.0 + std::abs(delta)));
      moment[component] += delta;
      N *= 2;
    } // End of half-block
  } // End of each component
  return moment;
}

/* Check if the distribution is elliptical */
Bool DistributionImplementation::isElliptical() const
{
  return false;
}

/* Check if the distribution is a copula */
Bool DistributionImplementation::isCopula() const
{
  return isCopula_;
}

/* Check if the distribution is continuous */
Bool DistributionImplementation::isContinuous() const
{
  return true;
}

/* Check if the distribution is discrete */
Bool DistributionImplementation::isDiscrete() const
{
  return false;
}

/* Tell if the distribution is integer valued */
Bool DistributionImplementation::isIntegral() const
{
  return false;
}

/* Tell if the distribution has elliptical copula */
Bool DistributionImplementation::hasEllipticalCopula() const
{
  return dimension_ == 1;
}

/* Tell if the distribution has independent copula */
Bool DistributionImplementation::hasIndependentCopula() const
{
  return dimension_ == 1;
}

/* Get the support of a distribution that intersect a given interval */
Sample DistributionImplementation::getSupport(const Interval & ) const
{
  throw NotYetImplementedException(HERE) << "In DistributionImplementation::getSupport(const Interval & interval) const";
}

/* Get the support on the whole range */
Sample DistributionImplementation::getSupport() const
{
  return getSupport(range_);
}

/* Get the discrete probability levels */
Point DistributionImplementation::getProbabilities() const
{
  if (!isDiscrete())
    throw InternalException(HERE) << "Error: cannot return probability levels of a non discrete distribution.";

  return computePDF(getSupport()).getImplementation()->getData();
}

/* Get the PDF singularities inside of the range - 1D only */
Point DistributionImplementation::getSingularities() const
{
  if (dimension_ != 1) throw NotDefinedException(HERE) << "Error: cannot ask for PDF singularities for multivariate distributions.";
  return Point(0);
}

/* Get the i-th marginal distribution */
Distribution DistributionImplementation::getMarginal(const UnsignedInteger i) const
{
  if (isCopula())
    return new IndependentCopula(1);
  return getMarginal(Indices(1, i));
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution DistributionImplementation::getMarginal(const Indices & indices) const
{
  if (!indices.check(dimension_)) throw InvalidArgumentException(HERE) << "Marginal indices cannot exceed dimension";
  Indices full(dimension_);
  full.fill();
  if (indices == full) return clone();
  if (isCopula() && (indices.getSize() == 1)) return new Uniform(0.0, 1.0);
  return new MarginalDistribution(*this, indices);
}

/* Get the copula of a distribution */
Distribution DistributionImplementation::getCopula() const
{
  if (dimension_ == 1) return new IndependentCopula(1);
  if (isCopula()) return clone();
  return new SklarCopula(*this);
}

/* Get the isoprobabilist transformation */
DistributionImplementation::IsoProbabilisticTransformation DistributionImplementation::getIsoProbabilisticTransformation() const
{
  // Special case for dimension 1
  if (dimension_ == 1)
  {
    DistributionCollection collection(1, *this);
    // Get the marginal transformation evaluation implementation
    const MarginalTransformationEvaluation evaluation(collection, MarginalTransformationEvaluation::FROM, Normal());
    // Get the marginal transformation gradient implementation
    const Gradient gradient(new MarginalTransformationGradient(evaluation));
    // Get the marginal transformation hessian implementation
    const Hessian hessian(new MarginalTransformationHessian(evaluation));
    InverseIsoProbabilisticTransformation inverseTransformation(evaluation, gradient, hessian);
    PointWithDescription parameters(getParameter());
    const UnsignedInteger parametersDimension = parameters.getDimension();
    Description parametersDescription(parameters.getDescription());
    const String name(parameters.getName());
    for (UnsignedInteger i = 0; i < parametersDimension; i++) parametersDescription[i] = OSS() << name << "_" << parametersDescription[i];
    parameters.setDescription(parametersDescription);
    inverseTransformation.setParameter(parameters);
    return inverseTransformation;
  }
  // General case, Rosenblatt transformation
  return FunctionImplementation(new RosenblattEvaluation(clone()));
}

/* Get the inverse isoprobabilist transformation */
DistributionImplementation::InverseIsoProbabilisticTransformation DistributionImplementation::getInverseIsoProbabilisticTransformation() const
{
  // Special case for dimension 1
  if (dimension_ == 1)
  {
    DistributionCollection collection(1, *this);
    // Get the marginal transformation evaluation implementation
    MarginalTransformationEvaluation evaluation(collection, MarginalTransformationEvaluation::TO, Normal());
    // Get the marginal transformation gradient implementation
    const Gradient gradient(new MarginalTransformationGradient(evaluation));
    // Get the marginal transformation hessian implementation
    const Hessian hessian(new MarginalTransformationHessian(evaluation));
    InverseIsoProbabilisticTransformation inverseTransformation(evaluation, gradient, hessian);
    PointWithDescription parameters(getParameter());
    const UnsignedInteger parametersDimension = parameters.getDimension();
    Description parametersDescription(parameters.getDescription());
    const String name(parameters.getName());
    for (UnsignedInteger i = 0; i < parametersDimension; i++) parametersDescription[i] = OSS() << name << "_" << parametersDescription[i];
    parameters.setDescription(parametersDescription);
    inverseTransformation.setParameter(parameters);
    return inverseTransformation;
  }
  // General case, inverse Rosenblatt transformation
  return FunctionImplementation(new InverseRosenblattEvaluation(clone()));
}

/* Get the standard distribution */
void DistributionImplementation::computeStandardDistribution() const
{
  Normal standardDistribution(dimension_);
  standardDistribution.setDescription(getDescription());
  p_standardDistribution_ = standardDistribution.clone();
  isAlreadyComputedStandardDistribution_ = true;
}

/* Get the standard distribution */
Distribution DistributionImplementation::getStandardDistribution() const
{
  if (!isAlreadyComputedStandardDistribution_) computeStandardDistribution();
  return p_standardDistribution_;
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Distribution DistributionImplementation::getStandardRepresentative() const
{
  return clone();
}

/* Compute the radial distribution CDF */
Scalar DistributionImplementation::computeRadialDistributionCDF(const Scalar,
    const Bool ) const
{
  throw NotYetImplementedException(HERE) << "In DistributionImplementation::computeRadialDistributionCDF(const Scalar radius, const Bool tail) const";
}


/* Draw the PDF of a discrete distribution */
Graph DistributionImplementation::drawDiscretePDF(const Scalar xMin,
    const Scalar xMax,
    const Bool logScale) const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: cannot draw the PDF of a multidimensional discrete distribution this way.";
  if (xMax < xMin - ResourceMap::GetAsScalar("DiscreteDistribution-SupportEpsilon")) throw InvalidArgumentException(HERE) << "Error: cannot draw a PDF with xMax < xMin, here xmin=" << xMin << " and xmax=" << xMax;
  const String title(OSS() << getDescription()[0] << " PDF");
  const Sample support(getSupport(Interval(xMin, xMax)));
  // First the vertical bars
  const String xName(getDescription()[0]);
  const GraphImplementation::LogScale scale = static_cast<GraphImplementation::LogScale>(logScale ? 1 : 0);
  Graph graphPDF(title, xName, "PDF", true, "topright", 1, scale);
  Point point(2);
  point[0] = xMin - ResourceMap::GetAsScalar("DiscreteDistribution-SupportEpsilon");
  const Sample gridY(computePDF(support));

  Sample data(0, 2);
  data.add(point);
  for (UnsignedInteger i = 0; i < support.getSize(); ++i)
  {
    point[0] = support(i, 0);
    data.add(point);
    point[1] = gridY(i, 0);
    data.add(point);
    point[1] = 0.0;
    data.add(point);
  }
  point[0] = xMax + ResourceMap::GetAsScalar("DiscreteDistribution-SupportEpsilon");
  point[1] = 0.0;
  data.add(point);
  graphPDF.add(Curve(data, "red", "solid", 2, title));
  return graphPDF;
}

/* Draw the PDF of the distribution when its dimension is 1 */
Graph DistributionImplementation::drawPDF(const Scalar xMin,
    const Scalar xMax,
    const UnsignedInteger pointNumber,
    const Bool logScale) const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "Error: can draw a PDF only if dimension equals 1, here dimension=" << dimension_;
  if (!(xMin < xMax)) throw InvalidArgumentException(HERE) << "Error: cannot draw a PDF with xMax <= xMin, here xmin=" << xMin << " and xmax=" << xMax;
  if (pointNumber < 2) throw InvalidArgumentException(HERE) << "Error: cannot draw a PDF with a point number < 2";
  if (isDiscrete()) return drawDiscretePDF(xMin, xMax, logScale);
  // Discretization of the x axis
  const PDFWrapper pdfWrapper(this);
  const GraphImplementation::LogScale scale = static_cast<GraphImplementation::LogScale>(logScale ? 1 : 0);
  Graph graphPDF(pdfWrapper.draw(xMin, xMax, pointNumber, scale));
  Drawable drawable(graphPDF.getDrawable(0));
  const String title(OSS() << getDescription()[0] << " PDF");
  drawable.setLegend(title);
  drawable.setLineStyle("solid");
  drawable.setLineWidth(2);
  graphPDF.setDrawable(drawable, 0);
  graphPDF.setXTitle(getDescription()[0]);
  graphPDF.setYTitle("PDF");
  graphPDF.setTitle("");
  graphPDF.setLegendPosition("topright");
  return graphPDF;
}

/* Draw the PDF of the distribution when its dimension is 1 */
Graph DistributionImplementation::drawPDF(const UnsignedInteger pointNumber,
    const Bool logScale) const
{
  if (dimension_ == 2) return drawPDF(Indices(2, pointNumber), logScale, logScale);
  if (dimension_ != 1) throw InvalidArgumentException(HERE) << "Error: this method is available only for 1D or 2D distributions";
  // For discrete distributions, use the numerical range to define the drawing range
  const Scalar xMin = computeQuantile(ResourceMap::GetAsScalar("Distribution-QMin"))[0];
  const Scalar xMax = computeQuantile(ResourceMap::GetAsScalar("Distribution-QMax"))[0];
  const Scalar delta = 2.0 * (xMax - xMin) * (1.0 - 0.5 * (ResourceMap::GetAsScalar("Distribution-QMax" ) - ResourceMap::GetAsScalar("Distribution-QMin")));
  if (isDiscrete())
  {
    Scalar a = std::max(xMin - delta, range_.getLowerBound()[0] - 1.0);
    Scalar b = std::min(xMax + delta, range_.getUpperBound()[0] + 1.0);
    if (b <= a)
    {
      a -= 1.0;
      b += 1.0;
    }
    return drawDiscretePDF(a, b, logScale);
  }
  return drawPDF(xMin - delta, xMax + delta, pointNumber, logScale);
}

/* Draw the PDF of a 1D marginal */
Graph DistributionImplementation::drawMarginal1DPDF(const UnsignedInteger marginalIndex,
    const Scalar xMin,
    const Scalar xMax,
    const UnsignedInteger pointNumber,
    const Bool logScale) const
{
  Graph marginalGraph(getMarginal(marginalIndex).drawPDF(xMin, xMax, pointNumber, logScale));
  marginalGraph.setTitle(OSS() << getDescription() << "->" << description_[marginalIndex] << " component PDF");
  return marginalGraph;
}

/* Draw the PDF of the distribution when its dimension is 2 */
Graph DistributionImplementation::drawPDF(const Point & xMin,
    const Point & xMax,
    const Indices & pointNumber,
    const Bool logScaleX,
    const Bool logScaleY) const
{
  if (dimension_ == 1) return drawPDF(xMin[0], xMax[0], pointNumber[0], logScaleX);
  if (!(pointNumber[0] >= 2 && pointNumber[1] >= 2)) throw InvalidArgumentException(HERE) << "Error: the discretization must have at least 2 points per component";
  const GraphImplementation::LogScale scale = static_cast<GraphImplementation::LogScale>((logScaleX ? 1 : 0) + (logScaleY ? 2 : 0));
  if (isContinuous())
  {
    Graph graph(PDFWrapper(this).draw(xMin, xMax, pointNumber, scale));
    graph.setXTitle(description_[0]);
    graph.setYTitle(description_[1]);
    graph.setTitle(String(OSS() << getDescription() << " iso-PDF"));
    return graph;
  }
  if (isDiscrete())
  {
    const Sample support(getSupport());
    const Point probabilities(getProbabilities());
    const UnsignedInteger size = support.getSize();
    SampleImplementation fullProba(size, 1);
    fullProba.setData(probabilities);
    fullProba.stack(*support.getImplementation());
    fullProba.sortAccordingToAComponentInPlace(0);
    const Scalar pMin = fullProba(0, 0);
    const Scalar pMax = fullProba(size - 1, 0);
    const Scalar scaling = ResourceMap::GetAsScalar("Distribution-DiscreteDrawPDFScaling") / std::sqrt(pMax);
    const String xName(description_[0]);
    const String yName(description_[1]);
    const String title(OSS() << getDescription() << " PDF");
    Graph graph(title, xName, yName, true, "topright", 1, scale);
    const Bool scaleColors = ResourceMap::GetAsBool("Distribution-ScaleColorsDiscretePDF") && (pMin < pMax);
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const Scalar x = fullProba(i, 1);
      const Scalar y = fullProba(i, 2);
      const Scalar p = fullProba(i, 0);
      const Scalar eta = std::sqrt(p) * scaling;
      Sample square(4, 2);
      if (logScaleX)
      {
        square(0, 0) = x * (1.0 - eta);
        square(1, 0) = x * (1.0 - eta);
        square(2, 0) = x * (1.0 + eta);
        square(3, 0) = x * (1.0 + eta);
      }
      else
      {
        square(0, 0) = x - eta;
        square(1, 0) = x - eta;
        square(2, 0) = x + eta;
        square(3, 0) = x + eta;
      }
      if (logScaleY)
      {
        square(0, 1) = y * (1.0 - eta);
        square(1, 1) = y * (1.0 + eta);
        square(2, 1) = y * (1.0 + eta);
        square(3, 1) = y * (1.0 - eta);
      }
      else
      {
        square(0, 1) = y - eta;
        square(1, 1) = y + eta;
        square(2, 1) = y + eta;
        square(3, 1) = y - eta;
      }
      Polygon mark(square);
      const Scalar rho = (scaleColors ? (1.0 - 1.0 / size) * (p - pMin) / (pMax - pMin) : p);
      mark.setColor(Polygon::ConvertFromHSV(360.0 * rho, 1.0, 1.0));
      mark.setEdgeColor(Polygon::ConvertFromHSV(360.0 * rho, 1.0, 0.9));
      graph.add(mark);
    }
    if (ResourceMap::GetAsBool("Distribution-ShowSupportDiscretePDF"))
    {
      Cloud cloud(support);
      graph.add(cloud);
    }
    return graph;
  }
  throw NotYetImplementedException(HERE) << "Error: the drawPDF() method is defined only for continuous or discrete distributions.";
}

/* Draw the PDF of the distribution when its dimension is 2 */
Graph DistributionImplementation::drawPDF(const Point & xMin,
    const Point & xMax,
    const Bool logScaleX,
    const Bool logScaleY) const
{
  return drawPDF(xMin, xMax, Indices(2, ResourceMap::GetAsUnsignedInteger("Distribution-DefaultPointNumber")), logScaleX, logScaleY);
}

/* Draw the PDF of the distribution when its dimension is 2 */
Graph DistributionImplementation::drawPDF(const Indices & pointNumber,
    const Bool logScaleX,
    const Bool logScaleY) const
{
  if (pointNumber.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: pointNumber must be of size 2, here size=" << pointNumber.getSize();
  Point xMin(2);
  if (isCopula()) xMin = Point(2, 0.0);
  else
  {
    xMin[0] = getMarginal(0).computeQuantile(ResourceMap::GetAsScalar("Distribution-QMin"))[0];
    xMin[1] = getMarginal(1).computeQuantile(ResourceMap::GetAsScalar("Distribution-QMin"))[0];
  }
  Point xMax(2);
  if (isCopula()) xMax = Point(2, 1.0);
  else
  {
    xMax[0] = getMarginal(0).computeQuantile(ResourceMap::GetAsScalar("Distribution-QMax"))[0];
    xMax[1] = getMarginal(1).computeQuantile(ResourceMap::GetAsScalar("Distribution-QMax"))[0];
  }
  Point delta(2, 0.0);
  if (!isCopula()) delta = (2.0 * (xMax - xMin) * (1.0 - 0.5 * (ResourceMap::GetAsScalar("Distribution-QMax" ) - ResourceMap::GetAsScalar("Distribution-QMin"))));
  const Interval intersection(range_.intersect(Interval(xMin - delta, xMax + delta)));
  Graph graph(drawPDF(intersection.getLowerBound(), intersection.getUpperBound(), pointNumber, logScaleX, logScaleY));
  // Add a border for a copula
  if (isCopula())
  {
    const Drawable drawable(graph.getDrawable(0));
    Sample data(5, 2);
    data(1, 0) = 1.0;
    data[2]    = Point(2, 1.0);
    data(3, 1) = 1.0;
    Curve square(data);
    graph.setDrawable(square, 0);
    graph.add(drawable);
  }
  return graph;
}

/* Draw the PDF of a 2D marginal */
Graph DistributionImplementation::drawMarginal2DPDF(const UnsignedInteger firstMarginal,
    const UnsignedInteger secondMarginal,
    const Point & xMin,
    const Point & xMax,
    const Indices & pointNumber,
    const Bool logScaleX,
    const Bool logScaleY) const
{
  Indices indices = {firstMarginal, secondMarginal};
  Graph marginalGraph(getMarginal(indices).drawPDF(xMin, xMax, pointNumber, logScaleX, logScaleY));
  marginalGraph.setTitle(OSS() << getDescription() << "->[" << description_[firstMarginal] << ", " << description_[secondMarginal] << "] components iso-PDF");
  return marginalGraph;
}

/* Draw the log-PDF of a discrete distribution */
Graph DistributionImplementation::drawDiscreteLogPDF(const Scalar xMin,
    const Scalar xMax,
    const Bool logScale) const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: cannot draw the PDF of a multidimensional discrete distribution this way.";
  if (xMax < xMin - ResourceMap::GetAsScalar("DiscreteDistribution-SupportEpsilon")) throw InvalidArgumentException(HERE) << "Error: cannot draw a PDF with xMax < xMin, here xmin=" << xMin << " and xmax=" << xMax;
  const String title(OSS() << getDescription()[0] << " PDF");
  const Sample support(getSupport(Interval(xMin, xMax)));
  // First the vertical bars
  const String xName(getDescription()[0]);
  const GraphImplementation::LogScale scale = static_cast<GraphImplementation::LogScale>(logScale ? 1 : 0);
  Graph graphLogPDF(title, xName, "PDF", true, "topright", 1, scale);
  Point point(2);
  point[0] = xMin - ResourceMap::GetAsScalar("DiscreteDistribution-SupportEpsilon");
  const Sample gridY(computeLogPDF(support));

  Sample data(0, 2);
  data.add(point);
  for (UnsignedInteger i = 0; i < support.getSize(); ++i)
  {
    point[0] = support(i, 0);
    data.add(point);
    point[1] = gridY(i, 0);
    data.add(point);
    point[1] = 0.0;
    data.add(point);
  }
  point[0] = xMax + ResourceMap::GetAsScalar("DiscreteDistribution-SupportEpsilon");
  point[1] = 0.0;
  data.add(point);
  graphLogPDF.add(Curve(data, "red", "solid", 2, title));
  return graphLogPDF;
}

/* Draw the log-PDF of the distribution when its dimension is 1 */
Graph DistributionImplementation::drawLogPDF(const Scalar xMin,
    const Scalar xMax,
    const UnsignedInteger pointNumber,
    const Bool logScale) const
{
  if (dimension_ == 2) return drawLogPDF(Indices(2, pointNumber), logScale, logScale);
  if (dimension_ != 1) throw InvalidArgumentException(HERE) << "Error: this method is available only for 1D or 2D distributions";
  if (!(xMin < xMax)) throw InvalidArgumentException(HERE) << "Error: cannot draw a logPDF with xMax <= xMin, here xmin=" << xMin << " and xmax=" << xMax;
  if (pointNumber < 2) throw InvalidArgumentException(HERE) << "Error: cannot draw a logPDF with a point number < 2";
  if (isDiscrete()) return drawDiscreteLogPDF(xMin, xMax, logScale);
  // Discretization of the x axis
  const LogPDFWrapper logPdfWrapper(this);
  const GraphImplementation::LogScale scale = static_cast<GraphImplementation::LogScale>(logScale ? 1 : 0);
  Graph graphLogPDF(logPdfWrapper.draw(xMin, xMax, pointNumber, scale));
  Drawable drawable(graphLogPDF.getDrawable(0));
  const String title(OSS() << getDescription()[0] << "log PDF");
  drawable.setLegend(title);
  drawable.setLineStyle("solid");
  drawable.setLineWidth(2);
  graphLogPDF.setDrawable(drawable, 0);
  graphLogPDF.setXTitle(getDescription()[0]);
  graphLogPDF.setYTitle("log PDF");
  graphLogPDF.setTitle("");
  graphLogPDF.setLegendPosition("topright");
  return graphLogPDF;
}

/* Draw the log-PDF of the distribution when its dimension is 1 */
Graph DistributionImplementation::drawLogPDF(const UnsignedInteger pointNumber,
    const Bool logScale) const
{
  if (dimension_ == 2) return drawLogPDF(Indices(2, pointNumber), logScale, logScale);
  if (dimension_ != 1) throw InvalidArgumentException(HERE) << "Error: this method is available only for 1D or 2D distributions";
  // For discrete distributions, use the numerical range to define the drawing range
  const Scalar xMin = computeQuantile(ResourceMap::GetAsScalar("Distribution-QMin"))[0];
  const Scalar xMax = computeQuantile(ResourceMap::GetAsScalar("Distribution-QMax"))[0];
  const Scalar delta = 2.0 * (xMax - xMin) * (1.0 - 0.5 * (ResourceMap::GetAsScalar("Distribution-QMax" ) - ResourceMap::GetAsScalar("Distribution-QMin")));
  if (isDiscrete())
  {
    Scalar a = std::max(xMin - delta, range_.getLowerBound()[0] - 1.0);
    Scalar b = std::min(xMax + delta, range_.getUpperBound()[0] + 1.0);
    if (b <= a)
    {
      a -= 1.0;
      b += 1.0;
    }
    return drawDiscreteLogPDF(a, b, logScale);
  }
  return drawLogPDF(xMin - delta, xMax + delta, pointNumber, logScale);
}

/* Draw the log-PDF of a 1D marginal */
Graph DistributionImplementation::drawMarginal1DLogPDF(const UnsignedInteger marginalIndex,
    const Scalar xMin,
    const Scalar xMax,
    const UnsignedInteger pointNumber,
    const Bool logScale) const
{
  Graph marginalGraph(getMarginal(marginalIndex).drawLogPDF(xMin, xMax, pointNumber, logScale));
  marginalGraph.setTitle(OSS() << getDescription() << "->" << description_[marginalIndex] << " component log PDF");
  return marginalGraph;
}

/* Draw the log-PDF of the distribution when its dimension is 2 */
Graph DistributionImplementation::drawLogPDF(const Point & xMin,
    const Point & xMax,
    const Indices & pointNumber,
    const Bool logScaleX,
    const Bool logScaleY) const
{
  if (dimension_ == 1) return drawLogPDF(xMin[0], xMax[0], pointNumber[0], logScaleX);
  if (!(pointNumber[0] >= 2 && pointNumber[1] >= 2)) throw InvalidArgumentException(HERE) << "Error: the discretization must have at least 2 points per component";
  const GraphImplementation::LogScale scale = static_cast<GraphImplementation::LogScale>((logScaleX ? 1 : 0) + (logScaleY ? 2 : 0));
  if (isContinuous())
  {
    Graph graph(LogPDFWrapper(this).draw(xMin, xMax, pointNumber, scale));
    graph.setXTitle(description_[0]);
    graph.setYTitle(description_[1]);
    graph.setTitle(String(OSS() << getDescription() << " iso-LogPDF"));
    return graph;
  }
  if (isDiscrete())
  {
    const Sample support(getSupport());
    const Point probabilities(getProbabilities());
    const UnsignedInteger size = support.getSize();
    SampleImplementation fullProba(size, 1);
    fullProba.setData(probabilities);
    fullProba.stack(*support.getImplementation());
    fullProba.sortAccordingToAComponentInPlace(0);
    const Scalar absLogPMin = -std::log(fullProba(0, 0));
    const Scalar absLogPMax = -std::log(fullProba(size - 1, 0));
    const Scalar scaling = ResourceMap::GetAsScalar("Distribution-DiscreteDrawPDFScaling") / std::sqrt(absLogPMin);
    const String xName(description_[0]);
    const String yName(description_[1]);
    const String title(OSS() << getDescription() << " PDF");
    Graph graph(title, xName, yName, true, "topright", 1, scale);
    const Bool scaleColors = ResourceMap::GetAsBool("Distribution-ScaleColorsDiscretePDF") && (absLogPMin < absLogPMax);
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const Scalar x = fullProba(i, 1);
      const Scalar y = fullProba(i, 2);
      const Scalar absLogP = -std::log(fullProba(i, 0));
      const Scalar eta = scaling * std::sqrt(absLogP);
      Sample square(4, 2);
      if (logScaleX)
      {
        square(0, 0) = x * (1.0 - eta);
        square(1, 0) = x * (1.0 - eta);
        square(2, 0) = x * (1.0 + eta);
        square(3, 0) = x * (1.0 + eta);
      }
      else
      {
        square(0, 0) = x - eta;
        square(1, 0) = x - eta;
        square(2, 0) = x + eta;
        square(3, 0) = x + eta;
      }
      if (logScaleY)
      {
        square(0, 1) = y * (1.0 - eta);
        square(1, 1) = y * (1.0 + eta);
        square(2, 1) = y * (1.0 + eta);
        square(3, 1) = y * (1.0 - eta);
      }
      else
      {
        square(0, 1) = y - eta;
        square(1, 1) = y + eta;
        square(2, 1) = y + eta;
        square(3, 1) = y - eta;
      }
      Polygon mark(square);
      const Scalar rho = (scaleColors ? (1.0 - 1.0 / size) * (absLogP - absLogPMin) / (absLogPMax - absLogPMin) : absLogP);
      mark.setColor(Polygon::ConvertFromHSV(360.0 * rho, 1.0, 1.0));
      mark.setEdgeColor(Polygon::ConvertFromHSV(360.0 * rho, 1.0, 0.9));
      graph.add(mark);
    }
    if (ResourceMap::GetAsBool("Distribution-ShowSupportDiscretePDF"))
    {
      Cloud cloud(support);
      graph.add(cloud);
    }
    return graph;
  }
  throw NotYetImplementedException(HERE) << "Error: the drawPDF() method is defined only for continuous or discrete distributions.";
}

/* Draw the log-PDF of the distribution when its dimension is 2 */
Graph DistributionImplementation::drawLogPDF(const Point & xMin,
    const Point & xMax,
    const Bool logScaleX,
    const Bool logScaleY) const
{
  return drawLogPDF(xMin, xMax, Indices(2, ResourceMap::GetAsUnsignedInteger("Distribution-DefaultPointNumber")), logScaleX, logScaleY);
}

/* Draw the log-PDF of the distribution when its dimension is 2 */
Graph DistributionImplementation::drawLogPDF(const Indices & pointNumber,
    const Bool logScaleX,
    const Bool logScaleY) const
{
  if (pointNumber.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: pointNumber must be of size 2, here size=" << pointNumber.getSize();
  Point xMin(2);
  if (isCopula()) xMin = Point(2, 0.0);
  else
  {
    xMin[0] = getMarginal(0).computeQuantile(ResourceMap::GetAsScalar("Distribution-QMin"))[0];
    xMin[1] = getMarginal(1).computeQuantile(ResourceMap::GetAsScalar("Distribution-QMin"))[0];
  }
  Point xMax(2);
  if (isCopula()) xMax = Point(2, 1.0);
  else
  {
    xMax[0] = getMarginal(0).computeQuantile(ResourceMap::GetAsScalar("Distribution-QMax"))[0];
    xMax[1] = getMarginal(1).computeQuantile(ResourceMap::GetAsScalar("Distribution-QMax"))[0];
  }
  Point delta(2, 0.0);
  if (!isCopula()) delta = (2.0 * (xMax - xMin) * (1.0 - 0.5 * (ResourceMap::GetAsScalar("Distribution-QMax" ) - ResourceMap::GetAsScalar("Distribution-QMin"))));
  const Interval intersection(range_.intersect(Interval(xMin - delta, xMax + delta)));
  Graph graph(drawLogPDF(intersection.getLowerBound(), intersection.getUpperBound(), pointNumber, logScaleX, logScaleY));
  // Add a border for a copula
  if (isCopula())
  {
    const Drawable drawable(graph.getDrawable(0));
    Sample data(5, 2);
    data(0, 0) = (logScaleX ? std::log(SpecFunc::Precision) : 0.0);
    data(0, 1) = (logScaleY ? std::log(SpecFunc::Precision) : 0.0);
    data(1, 0) = 1.0;
    data(1, 1) = (logScaleY ? std::log(SpecFunc::Precision) : 0.0);
    data(2, 0) = 1.0;
    data(2, 1) = 1.0;
    data(3, 0) = (logScaleX ? std::log(SpecFunc::Precision) : 0.0);
    data(3, 1) = 1.0;
    data[4] = data[0];
    Curve square(data);
    graph.setDrawable(square, 0);
    graph.add(drawable);
  }
  return graph;
}

/* Draw the log-PDF of a 2D marginal */
Graph DistributionImplementation::drawMarginal2DLogPDF(const UnsignedInteger firstMarginal,
    const UnsignedInteger secondMarginal,
    const Point & xMin,
    const Point & xMax,
    const Indices & pointNumber,
    const Bool logScaleX,
    const Bool logScaleY) const
{
  Indices indices = {firstMarginal, secondMarginal};
  Graph marginalGraph(getMarginal(indices).drawLogPDF(xMin, xMax, pointNumber, logScaleX, logScaleY));
  marginalGraph.setTitle(OSS() << getDescription() << "->[" << description_[firstMarginal] << ", " << description_[secondMarginal] << "] components iso-log PDF");
  return marginalGraph;
}

/* Draw the CDF of a discrete distribution */
Graph DistributionImplementation::drawDiscreteCDF(const Scalar xMin,
    const Scalar xMax,
    const Bool logScale) const
{
  // Value :    0    1/5  2/5  3/5    4/5    1
  // Data  : ------+-----+---+------+----+---------
  // Case 1: ------------------------------[----]--
  // Case 2: ------------------[---]---------------
  //         -[--]---------------------------------
  // Case 3: ----------[---]-----------------------
  //         ---[-----------------------------]----
  //         -------[-----------------]------------
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: cannot draw the CDF of a multidimensional discrete distribution this way.";
  if (!(xMin < xMax)) throw InvalidArgumentException(HERE) << "Error: cannot draw a PDF with xMax <= xMin, here xmin=" << xMin << " and xmax=" << xMax;
  // Create the graph that will store the staircase representing the empirical CDF
  const String title(OSS() << getDescription()[0] << " CDF");
  const Sample support(getSupport(Interval(xMin, xMax)));
  const Sample gridY(computeCDF(support));
  const UnsignedInteger size = support.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "empty range (" << xMin << ", " << xMax << ")" << ", support is (" << getSupport().getMin()[0] << ", " << getSupport().getMax()[0] << ")";
  const String xName(getDescription()[0]);
  const GraphImplementation::LogScale scale = static_cast<GraphImplementation::LogScale>(logScale ? 1 : 0);
  Graph graphCDF(title, xName, "CDF", true, "topleft", 1, scale);
  Sample data(size + 2, 2);
  data(0, 0) = xMin;
  data(0, 1) = computeCDF(xMin);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar x = support(i, 0);
    data(i + 1, 0) = x;
    data(i + 1, 1) = gridY(i, 0);
  }
  if (support(size - 1, 0) == xMax) data[size + 1] = data[size];
  else
  {
    data(size + 1, 0) = xMax;
    data(size + 1, 1) = computeCDF(xMax);
  }
  graphCDF.add(Staircase(data, "red", "solid", 2, "s", title));
  return graphCDF;
}

/* Draw the CDF of the distribution when its dimension is 1 */
Graph DistributionImplementation::drawCDF(const Scalar xMin,
    const Scalar xMax,
    const UnsignedInteger pointNumber,
    const Bool logScale) const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "Error: can draw a CDF only if dimension equals 1, here dimension=" << dimension_;
  if (!(xMin < xMax)) throw InvalidArgumentException(HERE) << "Error: cannot draw a CDF with xMax <= xMin, here xmin=" << xMin << " and xmax=" << xMax;
  if (pointNumber < 2) throw InvalidArgumentException(HERE) << "Error: cannot draw a CDF with a point number < 2";
  if (isDiscrete()) return drawDiscreteCDF(xMin, xMax, logScale);
  const CDFWrapper cdfWrapper(this);
  const GraphImplementation::LogScale scale = static_cast<GraphImplementation::LogScale>(logScale ? 1 : 0);
  Graph graphCDF(cdfWrapper.draw(xMin, xMax, pointNumber, scale));
  Drawable drawable(graphCDF.getDrawable(0));
  const String title(OSS() << getDescription()[0] << " CDF");
  drawable.setLegend(title);
  drawable.setLineStyle("solid");
  drawable.setLineWidth(2);
  graphCDF.setDrawable(drawable, 0);
  graphCDF.setXTitle(getDescription()[0]);
  graphCDF.setYTitle("CDF");
  graphCDF.setTitle("");
  graphCDF.setLegendPosition("topleft");
  return graphCDF;
}

/* Draw the CDF of the distribution when its dimension is 1 */
Graph DistributionImplementation::drawCDF(const UnsignedInteger pointNumber,
    const Bool logScale) const
{
  if (dimension_ == 2) return drawCDF(Indices(2, pointNumber), logScale, logScale);
  if (dimension_ != 1) throw InvalidArgumentException(HERE) << "Error: this method is available only for 1D or 2D distributions";
  // For discrete distributions, use the numerical range to define the drawing range
  const Scalar xMin = computeQuantile(ResourceMap::GetAsScalar("Distribution-QMin"))[0];
  const Scalar xMax = computeQuantile(ResourceMap::GetAsScalar("Distribution-QMax"))[0];
  const Scalar delta = 2.0 * (xMax - xMin) * (1.0 - 0.5 * (ResourceMap::GetAsScalar("Distribution-QMax" ) - ResourceMap::GetAsScalar("Distribution-QMin")));
  if (isDiscrete())
  {
    Scalar a = std::max(xMin - delta, range_.getLowerBound()[0] - 1.0);
    Scalar b = std::min(xMax + delta, range_.getUpperBound()[0] + 1.0);
    if (b <= a)
    {
      a -= 1.0;
      b += 1.0;
    }
    return drawDiscreteCDF(a, b, logScale);
  }
  return drawCDF(xMin - delta, xMax + delta, pointNumber, logScale);
}

/* Draw the CDF of a 1D marginal */
Graph DistributionImplementation::drawMarginal1DCDF(const UnsignedInteger marginalIndex,
    const Scalar xMin,
    const Scalar xMax,
    const UnsignedInteger pointNumber,
    const Bool logScale) const
{
  Graph marginalGraph(getMarginal(marginalIndex).drawCDF(xMin, xMax, pointNumber, logScale));
  marginalGraph.setTitle(OSS() << getDescription() << "->" << description_[marginalIndex] << " component CDF");
  return marginalGraph;
}

/* Draw the CDF of the distribution when its dimension is 2 */
Graph DistributionImplementation::drawCDF(const Point & xMin,
    const Point & xMax,
    const Indices & pointNumber,
    const Bool logScaleX,
    const Bool logScaleY) const
{
  if (dimension_ == 1) return drawCDF(xMin[0], xMax[0], pointNumber[0], logScaleX);
  if (xMin.getDimension() != 2) throw InvalidArgumentException(HERE) << "Error: expected xMin to be of dimension 2, here dimension=" << xMin.getDimension();
  if (xMax.getDimension() != 2) throw InvalidArgumentException(HERE) << "Error: expected xMax to be of dimension 2, here dimension=" << xMax.getDimension();
  if (pointNumber.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: expected pointNumber to be of size 2, here size=" << pointNumber.getSize();
  if (!(pointNumber[0] >= 2 && pointNumber[1] >= 2)) throw InvalidArgumentException(HERE) << "Error: the discretization must have at least 2 points per component";
  const GraphImplementation::LogScale scale = static_cast<GraphImplementation::LogScale>((logScaleX ? 1 : 0) + (logScaleY ? 2 : 0));
  Graph graph(CDFWrapper(this).draw(xMin, xMax, pointNumber, scale));
  graph.setXTitle(description_[0]);
  graph.setYTitle(description_[1]);
  graph.setTitle(String(OSS() << getDescription() << " iso-CDF"));
  return graph;
}

/* Draw the CDF of the distribution when its dimension is 2 */
Graph DistributionImplementation::drawCDF(const Point & xMin,
    const Point & xMax,
    const Bool logScaleX,
    const Bool logScaleY) const
{
  return drawCDF(xMin, xMax, Indices(2, ResourceMap::GetAsUnsignedInteger("Distribution-DefaultPointNumber")), logScaleX, logScaleY);
}

/* Draw the CDF of the distribution when its dimension is 2 */
Graph DistributionImplementation::drawCDF(const Indices & pointNumber,
    const Bool logScaleX,
    const Bool logScaleY) const
{
  if (pointNumber.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: expected pointNumber to be of size 2, here size=" << pointNumber.getSize();
  Point xMin(2);
  if (isCopula()) xMin = Point(2, 0.0);
  else
  {
    xMin[0] = getMarginal(0).computeQuantile(ResourceMap::GetAsScalar("Distribution-QMin"))[0];
    xMin[1] = getMarginal(1).computeQuantile(ResourceMap::GetAsScalar("Distribution-QMin"))[0];
  }
  Point xMax(2);
  if (isCopula()) xMax = Point(2, 1.0);
  else
  {
    xMax[0] = getMarginal(0).computeQuantile(ResourceMap::GetAsScalar("Distribution-QMax"))[0];
    xMax[1] = getMarginal(1).computeQuantile(ResourceMap::GetAsScalar("Distribution-QMax"))[0];
  }
  Point delta(2, 0.0);
  if (!isCopula()) delta = (2.0 * (xMax - xMin) * (1.0 - 0.5 * (ResourceMap::GetAsScalar("Distribution-QMax" ) - ResourceMap::GetAsScalar("Distribution-QMin"))));
  return drawCDF(xMin - delta, xMax + delta, pointNumber, logScaleX, logScaleY);
}

/* Draw the CDF of a 2D marginal */
Graph DistributionImplementation::drawMarginal2DCDF(const UnsignedInteger firstMarginal,
    const UnsignedInteger secondMarginal,
    const Point & xMin,
    const Point & xMax,
    const Indices & pointNumber,
    const Bool logScaleX,
    const Bool logScaleY) const
{
  Indices indices = {firstMarginal, secondMarginal};
  Graph marginalGraph(getMarginal(indices).drawCDF(xMin, xMax, pointNumber, logScaleX, logScaleY));
  marginalGraph.setTitle(OSS() << getDescription() << "->[" << description_[firstMarginal] << ", " << description_[secondMarginal] << "] components iso-CDF");
  return marginalGraph;
}

/* Draw the SurvivalFunction of a discrete distribution */
Graph DistributionImplementation::drawDiscreteSurvivalFunction(const Scalar xMin,
    const Scalar xMax,
    const Bool logScale) const
{
  // Value :    0    1/5  2/5  3/5    4/5    1
  // Data  : ------+-----+---+------+----+---------
  // Case 1: ------------------------------[----]--
  // Case 2: ------------------[---]---------------
  //         -[--]---------------------------------
  // Case 3: ----------[---]-----------------------
  //         ---[-----------------------------]----
  //         -------[-----------------]------------
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: cannot draw the SurvivalFunction of a multidimensional discrete distribution this way.";
  if (!(xMin < xMax)) throw InvalidArgumentException(HERE) << "Error: cannot draw a PDF with xMax <= xMin, here xmin=" << xMin << " and xmax=" << xMax;
  // Create the graph that will store the staircase representing the empirical SurvivalFunction
  const String title(OSS() << getDescription()[0] << " SurvivalFunction");
  const Sample support(getSupport(Interval(xMin, xMax)));
  const Sample gridY(computeSurvivalFunction(support));
  const UnsignedInteger size = support.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "empty range (" << xMin << ", " << xMax << ")" << ", support is (" << getSupport().getMin()[0] << ", " << getSupport().getMax()[0] << ")";
  const String xName(getDescription()[0]);
  const GraphImplementation::LogScale scale = static_cast<GraphImplementation::LogScale>(logScale ? 1 : 0);
  Graph graphSurvivalFunction(title, xName, "SurvivalFunction", true, "topleft", 1, scale);
  Sample data(size + 2, 2);
  data(0, 0) = xMin;
  data(0, 1) = computeSurvivalFunction(xMin);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar x = support(i, 0);
    data(i + 1, 0) = x;
    data(i + 1, 1) = gridY(i, 0);
  }
  if (support(size - 1, 0) == xMax) data[size + 1] = data[size];
  else
  {
    data(size + 1, 0) = xMax;
    data(size + 1, 1) = computeSurvivalFunction(xMax);
  }
  graphSurvivalFunction.add(Staircase(data, "red", "solid", 2, "s", title));
  return graphSurvivalFunction;
}

/* Draw the SurvivalFunction of the distribution when its dimension is 1 */
Graph DistributionImplementation::drawSurvivalFunction(const Scalar xMin,
    const Scalar xMax,
    const UnsignedInteger pointNumber,
    const Bool logScale) const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "Error: can draw a SurvivalFunction only if dimension equals 1, here dimension=" << dimension_;
  if (!(xMin < xMax)) throw InvalidArgumentException(HERE) << "Error: cannot draw a SurvivalFunction with xMax <= xMin, here xmin=" << xMin << " and xmax=" << xMax;
  if (pointNumber < 2) throw InvalidArgumentException(HERE) << "Error: cannot draw a SurvivalFunction with a point number < 2";
  if (isDiscrete()) return drawDiscreteSurvivalFunction(xMin, xMax, logScale);
  const SurvivalFunctionWrapper survivalWrapper(Collection<Implementation>(0), this);
  const GraphImplementation::LogScale scale = static_cast<GraphImplementation::LogScale>(logScale ? 1 : 0);
  Graph graphSurvivalFunction(survivalWrapper.draw(xMin, xMax, pointNumber, scale));
  Drawable drawable(graphSurvivalFunction.getDrawable(0));
  const String title(OSS() << getDescription()[0] << " SurvivalFunction");
  drawable.setLegend(title);
  drawable.setLineStyle("solid");
  drawable.setLineWidth(2);
  graphSurvivalFunction.setDrawable(drawable, 0);
  graphSurvivalFunction.setXTitle(getDescription()[0]);
  graphSurvivalFunction.setYTitle("SurvivalFunction");
  graphSurvivalFunction.setTitle("");
  graphSurvivalFunction.setLegendPosition("topright");
  return graphSurvivalFunction;
}

/* Draw the SurvivalFunction of the distribution when its dimension is 1 */
Graph DistributionImplementation::drawSurvivalFunction(const UnsignedInteger pointNumber,
    const Bool logScale) const
{
  if (dimension_ == 2) return drawSurvivalFunction(Indices(2, pointNumber), logScale, logScale);
  if (dimension_ != 1) throw InvalidArgumentException(HERE) << "Error: this method is available only for 1D or 2D distributions";
  // For discrete distributions, use the numerical range to define the drawing range
  const Scalar xMin = computeQuantile(ResourceMap::GetAsScalar("Distribution-QMin"))[0];
  const Scalar xMax = computeQuantile(ResourceMap::GetAsScalar("Distribution-QMax"))[0];
  const Scalar delta = 2.0 * (xMax - xMin) * (1.0 - 0.5 * (ResourceMap::GetAsScalar("Distribution-QMax" ) - ResourceMap::GetAsScalar("Distribution-QMin")));
  if (isDiscrete())
  {
    Scalar a = std::max(xMin - delta, range_.getLowerBound()[0] - 1.0);
    Scalar b = std::min(xMax + delta, range_.getUpperBound()[0] + 1.0);
    if (b <= a)
    {
      a -= 1.0;
      b += 1.0;
    }
    return drawDiscreteSurvivalFunction(a, b, logScale);
  }
  return drawSurvivalFunction(xMin - delta, xMax + delta, pointNumber, logScale);
}

/* Draw the SurvivalFunction of a 1D marginal */
Graph DistributionImplementation::drawMarginal1DSurvivalFunction(const UnsignedInteger marginalIndex,
    const Scalar xMin,
    const Scalar xMax,
    const UnsignedInteger pointNumber,
    const Bool logScale) const
{
  Graph marginalGraph(getMarginal(marginalIndex).drawSurvivalFunction(xMin, xMax, pointNumber, logScale));
  marginalGraph.setTitle(OSS() << getDescription() << "->" << description_[marginalIndex] << " component SurvivalFunction");
  return marginalGraph;
}

/* Draw the SurvivalFunction of the distribution when its dimension is 2 */
Graph DistributionImplementation::drawSurvivalFunction(const Point & xMin,
    const Point & xMax,
    const Indices & pointNumber,
    const Bool logScaleX,
    const Bool logScaleY) const
{
  if (dimension_ == 1) return drawSurvivalFunction(xMin[0], xMax[0], pointNumber[0], logScaleX);
  if (xMin.getDimension() != 2) throw InvalidArgumentException(HERE) << "Error: expected xMin to be of dimension 2, here dimension=" << xMin.getDimension();
  if (xMax.getDimension() != 2) throw InvalidArgumentException(HERE) << "Error: expected xMax to be of dimension 2, here dimension=" << xMax.getDimension();
  if (pointNumber.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: expected pointNumber to be of size 2, here size=" << pointNumber.getSize();
  if (!(pointNumber[0] >= 2 && pointNumber[1] >= 2)) throw InvalidArgumentException(HERE) << "Error: the discretization must have at least 2 points per component";
  const GraphImplementation::LogScale scale = static_cast<GraphImplementation::LogScale>((logScaleX ? 1 : 0) + (logScaleY ? 2 : 0));
  Graph graph(SurvivalFunctionWrapper(Collection<Implementation>(0), this).draw(xMin, xMax, pointNumber, scale));
  graph.setXTitle(description_[0]);
  graph.setYTitle(description_[1]);
  graph.setTitle(String(OSS() << getDescription() << " iso-SurvivalFunction"));
  return graph;
}

/* Draw the SurvivalFunction of the distribution when its dimension is 2 */
Graph DistributionImplementation::drawSurvivalFunction(const Point & xMin,
    const Point & xMax,
    const Bool logScaleX,
    const Bool logScaleY) const
{
  return drawSurvivalFunction(xMin, xMax, Indices(2, ResourceMap::GetAsUnsignedInteger("Distribution-DefaultPointNumber")), logScaleX, logScaleY);
}

/* Draw the SurvivalFunction of the distribution when its dimension is 2 */
Graph DistributionImplementation::drawSurvivalFunction(const Indices & pointNumber,
    const Bool logScaleX,
    const Bool logScaleY) const
{
  if (pointNumber.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: expected pointNumber to be of size 2, here size=" << pointNumber.getSize();
  Point xMin(2);
  if (isCopula()) xMin = Point(2, 0.0);
  else
  {
    xMin[0] = getMarginal(0).computeQuantile(ResourceMap::GetAsScalar("Distribution-QMin"))[0];
    xMin[1] = getMarginal(1).computeQuantile(ResourceMap::GetAsScalar("Distribution-QMin"))[0];
  }
  Point xMax(2);
  if (isCopula()) xMax = Point(2, 1.0);
  else
  {
    xMax[0] = getMarginal(0).computeQuantile(ResourceMap::GetAsScalar("Distribution-QMax"))[0];
    xMax[1] = getMarginal(1).computeQuantile(ResourceMap::GetAsScalar("Distribution-QMax"))[0];
  }
  Point delta(2, 0.0);
  if (!isCopula()) delta = (2.0 * (xMax - xMin) * (1.0 - 0.5 * (ResourceMap::GetAsScalar("Distribution-QMax" ) - ResourceMap::GetAsScalar("Distribution-QMin"))));
  return drawSurvivalFunction(xMin - delta, xMax + delta, pointNumber, logScaleX, logScaleY);
}

/* Draw the SurvivalFunction of a 2D marginal */
Graph DistributionImplementation::drawMarginal2DSurvivalFunction(const UnsignedInteger firstMarginal,
    const UnsignedInteger secondMarginal,
    const Point & xMin,
    const Point & xMax,
    const Indices & pointNumber,
    const Bool logScaleX,
    const Bool logScaleY) const
{
  Indices indices = {firstMarginal, secondMarginal};
  Graph marginalGraph(getMarginal(indices).drawSurvivalFunction(xMin, xMax, pointNumber, logScaleX, logScaleY));
  marginalGraph.setTitle(OSS() << getDescription() << "->[" << description_[firstMarginal] << ", " << description_[secondMarginal] << "] components iso-SurvivalFunction");
  return marginalGraph;
}

/* Draw the quantile of the distribution when its dimension is 1 or 2 */
Graph DistributionImplementation::drawQuantile(const UnsignedInteger pointNumber,
    const Bool logScale) const
{
  const Scalar qMin = SpecFunc::ScalarEpsilon;
  const Scalar qMax = 1.0 - qMin;
  return drawQuantile(qMin, qMax, pointNumber, logScale);
}

Graph DistributionImplementation::drawQuantile(const Scalar qMin,
    const Scalar qMax,
    const UnsignedInteger pointNumber,
    const Bool logScale) const
{
  // Generic interface for the 1D and 2D cases
  if (dimension_ == 1) return drawQuantile1D(qMin, qMax, pointNumber, logScale);
  if (dimension_ == 2) return drawQuantile2D(qMin, qMax, pointNumber, logScale);
  throw InvalidDimensionException(HERE) << "Error: can draw the quantiles only if dimension equals 1 or 2, here dimension=" << dimension_;
}

/* Draw the quantile of the distribution when its dimension is 1 */
Graph DistributionImplementation::drawQuantile1D(const Scalar qMin,
    const Scalar qMax,
    const UnsignedInteger pointNumber,
    const Bool logScale) const
{
  const QuantileWrapper quantileWrapper(Collection<Implementation>(0), this);
  const GraphImplementation::LogScale scale = static_cast<GraphImplementation::LogScale>(logScale ? 1 : 0);
  Graph graphQuantile(quantileWrapper.draw(qMin, qMax, pointNumber, scale));
  Drawable drawable(graphQuantile.getDrawable(0));
  const String title(OSS() << getDescription()[0] << " Quantile");
  drawable.setLegend(title);
  drawable.setLineStyle("solid");
  drawable.setLineWidth(2);
  graphQuantile.setDrawable(drawable, 0);
  graphQuantile.setXTitle(getDescription()[0]);
  graphQuantile.setYTitle("Quantile");
  graphQuantile.setTitle("");
  graphQuantile.setLegendPosition("topleft");
  return graphQuantile;
}

/* Draw the quantile of the distribution when its dimension is 2 */
Graph DistributionImplementation::drawQuantile2D(const Scalar qMin,
    const Scalar qMax,
    const UnsignedInteger pointNumber,
    const Bool logScaleX,
    const Bool logScaleY) const
{
  const String title(OSS() << getDescription() << " Quantile");
  const Sample data(computeQuantile(qMin, qMax, pointNumber));
  Curve curveQuantile(data);
  curveQuantile.setLegend(title);
  curveQuantile.setLineStyle("solid");
  curveQuantile.setLineWidth(2);
  const String xName(getDescription()[0]);
  const String yName(getDescription()[1]);
  const GraphImplementation::LogScale scale = static_cast<GraphImplementation::LogScale>((logScaleX ? 1 : 0) + (logScaleY ? 2 : 0));
  Graph graphQuantile(title, xName, yName, true, "topleft", 1, scale);
  graphQuantile.add(drawSurvivalFunction(data.getMin(), data.getMax(), logScaleX, logScaleY).getDrawable(0));
  graphQuantile.add(curveQuantile);
  Description legends(2);
  legends[0] = "iso-SurvivalFunction";
  legends[1] = "quantile";
  graphQuantile.setLegends(legends);
  return graphQuantile;
}


/* Draw dependence functions */
class DistributionImplementationDependenceEvaluation : public EvaluationImplementation
{
public:
  DistributionImplementationDependenceEvaluation(const Distribution & distribution,
      const String & linkFormula,
      const Bool survival = false)
    : EvaluationImplementation()
    , distribution_(distribution)
    , link_(SymbolicFunction(Description({"u", "cuu"}), Description(1, linkFormula)))
  , survival_(survival)
  {}

  DistributionImplementationDependenceEvaluation * clone() const override
  {
    return new DistributionImplementationDependenceEvaluation(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    return 1;
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

  Point operator()(const Point & inP) const override
  {
    const Scalar u = inP[0];
    const Point uu = {u, u};
    const Scalar cuu = survival_ ? distribution_.computeSurvivalFunction(uu) : distribution_.computeCDF(uu);
    Point xu(1, 0);
    if ((cuu > 0.0) && (cuu < 1.0))
      xu = link_(Point({u, cuu}));
    return xu;
  }

private:
  Distribution distribution_;
  Function link_;
  Bool survival_ = false;
};

Function DistributionImplementation::getUpperTailDependenceFunction() const
{
  return DistributionImplementationDependenceEvaluation(*this, "2-log(cuu)/log(u)");
}

Function DistributionImplementation::getUpperExtremalDependenceFunction() const
{
  return DistributionImplementationDependenceEvaluation(*this, "2*log1p(-u)/log(cuu)-1", true);
}

Function DistributionImplementation::getLowerTailDependenceFunction() const
{
  return DistributionImplementationDependenceEvaluation(*this, "log1p(-cuu)/log1p(-u)");
}

Function DistributionImplementation::getLowerExtremalDependenceFunction() const
{
  return DistributionImplementationDependenceEvaluation(*this, "2*log(u)/log(cuu)-1");
}

CorrelationMatrix DistributionImplementation::computeDependenceMatrix(const Function & dependenceFunction, const Bool lower) const
{
  if (getDimension() < 2)
    throw InvalidArgumentException(HERE) << "Can only compute dependence matrix of a multivariate distribution";
  CorrelationMatrix dependence(getDimension());
  const Scalar epsilon = ResourceMap::GetAsScalar("Distribution-DependenceEpsilon");
  const Scalar u = lower ? epsilon : 1.0 - epsilon;
  for (UnsignedInteger i = 0; i < getDimension(); ++ i)
    for (UnsignedInteger j = 0; j < i; ++ j)
      dependence(i, j) = dependenceFunction(Point({u}))[0];
  return dependence;
}

CorrelationMatrix DistributionImplementation::computeUpperTailDependenceMatrix() const
{
  return computeDependenceMatrix(getUpperTailDependenceFunction());
}

CorrelationMatrix DistributionImplementation::computeUpperExtremalDependenceMatrix() const
{
  return computeDependenceMatrix(getUpperExtremalDependenceFunction());
}

CorrelationMatrix DistributionImplementation::computeLowerTailDependenceMatrix() const
{
  return computeDependenceMatrix(getLowerTailDependenceFunction(), true);
}

CorrelationMatrix DistributionImplementation::computeLowerExtremalDependenceMatrix() const
{
  return computeDependenceMatrix(getLowerExtremalDependenceFunction(), true);
}

Graph DistributionImplementation::drawDependenceFunction(const Function & dependenceFunction, const String & legend) const
{
  if (getDimension() != 2)
    throw InvalidArgumentException(HERE) << "Can only draw dependence function of a bivariate distribution";

  const Scalar epsilon = 1e-3;
  Graph graph(dependenceFunction.draw(epsilon, 1.0 - epsilon));
  graph.setLegendPosition("bottom");
  graph.setLegends(Description({legend}));
  graph.setXTitle("u");
  graph.setYTitle(legend);
  return graph;
}

Graph DistributionImplementation::drawUpperTailDependenceFunction() const
{
  Graph graph(drawDependenceFunction(getUpperTailDependenceFunction(), "$\\chi(u)$"));
  graph.setTitle("Upper tail dependence function");
  return graph;
}

Graph DistributionImplementation::drawUpperExtremalDependenceFunction() const
{
  Graph result(drawDependenceFunction(getUpperExtremalDependenceFunction(), "$\\bar{\\chi}(u)$"));
  result.setTitle("Upper extremal dependence function");
  return result;
}

Graph DistributionImplementation::drawLowerTailDependenceFunction() const
{
  Graph result(drawDependenceFunction(getLowerTailDependenceFunction(), "$\\chi_L(u)$"));
  result.setTitle("Lower tail dependence function");
  return result;
}

Graph DistributionImplementation::drawLowerExtremalDependenceFunction() const
{
  Graph result(drawDependenceFunction(getLowerExtremalDependenceFunction(), "$\\bar{\\chi}_L(u)$"));
  result.setTitle("Lower extremal dependence function");
  return result;
}

/* Parameters value and description accessor */
DistributionImplementation::PointWithDescriptionCollection DistributionImplementation::getParametersCollection() const
{
  // Use compact accessor
  PointWithDescription parameters(getParameter());
  parameters.setDescription(getParameterDescription());
  parameters.setName(getDescription()[0]);
  return PointWithDescriptionCollection(1, parameters);
}

void DistributionImplementation::setParametersCollection(const PointWithDescriptionCollection & parametersCollection)
{
  if (getDimension() == 1)
  {
    if (parametersCollection.getSize() != 1) throw InvalidArgumentException(HERE) << "Expected collection of size 1, got " << parametersCollection.getSize();
    setParameter(parametersCollection[0]);
  }

  // Get the actual collection of parameters to check the description and the size
  const PointWithDescriptionCollection actualParameters(getParametersCollection());
  const UnsignedInteger size = actualParameters.getSize();
  if (parametersCollection.getSize() != size) throw InvalidArgumentException(HERE) << "Error: the given parameters collection has an invalid size (" << parametersCollection.getSize() << "), it should be " << size;
  PointCollection coll(0);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger dimension = actualParameters[i].getDimension();
    if (parametersCollection[i].getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given parameters collection has an invalid dimension at index " << i;
    coll.add(parametersCollection[i]);
  }
  setParametersCollection(coll);
}

void DistributionImplementation::setParametersCollection(const PointCollection & parametersCollection)
{
  const UnsignedInteger size = parametersCollection.getSize();
  Point newParameters;
  for (UnsignedInteger i = 0; i < size; ++ i) newParameters.add(parametersCollection[i]);
  setParameter(newParameters);
}

/* Parameters value accessor */
Point DistributionImplementation::getParameter() const
{
  throw NotYetImplementedException(HERE) << "In DistributionImplementation::getParameter()";
}

void DistributionImplementation::setParameter(const Point & parameters)
{
  if (parameters.getSize() != 0) throw InvalidArgumentException(HERE) << "Error: expected 0 parameters, got " << parameters.getSize();
}

/* Parameters description accessor */
Description DistributionImplementation::getParameterDescription() const
{
  throw NotYetImplementedException(HERE) << "In DistributionImplementation::getParameterDescription()";
}


/* Parameters number */
UnsignedInteger DistributionImplementation::getParameterDimension() const
{
  return getParameter().getSize();
}

/* Description accessor */
void DistributionImplementation::setDescription(const Description & description)
{
  const UnsignedInteger size = description.getSize();
  if (size != getDimension()) throw InvalidArgumentException(HERE) << "Error: the description must have the same size than the distribution dimension, here size=" << size << " and dimension=" << getDimension();
  // Check if the description is valid
  // First, copy the description
  Description test(description);
  // Second, sort the copy
  std::sort(test.begin(), test.end());
  // Third, move the duplicates at the end
  Description::const_iterator it = std::unique(test.begin(), test.end());
  // Fourth, check if there was any duplicate
  if (it != test.end())
  {
    LOGWARN(OSS() << "Warning! The description of the distribution " << getName() << " is " << description << " and cannot identify uniquely the marginal distribution. Use default description instead.");
    description_ = Description::BuildDefault(dimension_, "X");
  }
  else description_ = description;
}

/* Description accessot */
Description DistributionImplementation::getDescription() const
{
  return description_;
}

/* Accessor to PDF computation precision */
Scalar DistributionImplementation::getPDFEpsilon() const
{
  return pdfEpsilon_;
}

/* Accessor to CDF computation precision */
Scalar DistributionImplementation::getCDFEpsilon() const
{
  return cdfEpsilon_;
}

/* Get a position indicator for a 1D distribution */
Scalar DistributionImplementation::getPositionIndicator() const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "Error: cannot get the position indicator of a distribution with dimension > 1";
  // First, try to return the mean of the distribution
  try
  {
    return getMean()[0];
  }
  catch (...)
  {
    // Second, return the median of the distribution
    return computeScalarQuantile(0.5);
  }
}

/* Get a dispersion indicator for a 1D distribution */
Scalar DistributionImplementation::getDispersionIndicator() const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "Error: cannot get the dispersion indicator of a distribution with dimension > 1";
  // First, try to return the standard deviation of the distribution
  try
  {
    return getStandardDeviation()[0];
  }
  catch (...)
  {
    // Second, return the interquartile of the distribution
    return computeScalarQuantile(0.75) - computeScalarQuantile(0.25);
  }
}

/* Is it safe to compute PDF/CDF etc in parallel? */
Bool DistributionImplementation::isParallel() const
{
  return isParallel_;
}

void DistributionImplementation::setParallel(const Bool flag)
{
  isParallel_ = flag;
}

/* Method save() stores the object through the StorageManager */
void DistributionImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "mean_", mean_ );
  adv.saveAttribute( "covariance_", covariance_ );
  adv.saveAttribute( "gaussNodes_", gaussNodes_ );
  adv.saveAttribute( "gaussWeights_", gaussWeights_ );
  adv.saveAttribute( "integrationNodesNumber_", integrationNodesNumber_ );
  adv.saveAttribute( "isAlreadyComputedMean_", isAlreadyComputedMean_ );
  adv.saveAttribute( "isAlreadyComputedCovariance_", isAlreadyComputedCovariance_ );
  adv.saveAttribute( "isAlreadyComputedGaussNodesAndWeights_", isAlreadyComputedGaussNodesAndWeights_ );
  adv.saveAttribute( "dimension_", dimension_ );
  adv.saveAttribute( "weight_", weight_ );
  adv.saveAttribute( "range_", range_ );
  adv.saveAttribute( "description_", description_ );
  adv.saveAttribute( "isCopula_", isCopula_ );
  adv.saveAttribute( "isParallel_", isParallel_ );
}

/* Method load() reloads the object from the StorageManager */
void DistributionImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "mean_", mean_ );
  adv.loadAttribute( "covariance_", covariance_ );
  adv.loadAttribute( "gaussNodes_", gaussNodes_ );
  adv.loadAttribute( "gaussWeights_", gaussWeights_ );
  adv.loadAttribute( "integrationNodesNumber_", integrationNodesNumber_ );
  adv.loadAttribute( "isAlreadyComputedMean_", isAlreadyComputedMean_ );
  adv.loadAttribute( "isAlreadyComputedCovariance_", isAlreadyComputedCovariance_ );
  adv.loadAttribute( "isAlreadyComputedGaussNodesAndWeights_", isAlreadyComputedGaussNodesAndWeights_ );
  adv.loadAttribute( "dimension_", dimension_ );
  adv.loadAttribute( "weight_", weight_ );
  adv.loadAttribute( "range_", range_ );
  adv.loadAttribute( "description_", description_ );
  adv.loadAttribute( "isCopula_", isCopula_ );
  if (adv.hasAttribute("isParallel_"))
    adv.loadAttribute( "isParallel_", isParallel_ );
}

/* Transformation of distributions by usual functions */
Distribution DistributionImplementation::cos() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  const Scalar a = range_.getLowerBound()[0];
  const Scalar b = range_.getUpperBound()[0];
  const SignedInteger nMin = static_cast< SignedInteger > (ceil(a / M_PI));
  const SignedInteger nMax = static_cast< SignedInteger > (floor(b / M_PI));
  Point bounds(1, a);
  Point values(1, std::cos(a));
  for (SignedInteger n = nMin; n <= nMax; ++n)
  {
    bounds.add(n * M_PI);
    values.add(n % 2 == 0 ? 1.0 : -1.0);
  }
  bounds.add(b);
  values.add(std::cos(b));
  return new CompositeDistribution(SymbolicFunction("x", "cos(x)"), clone(), bounds, values);
}

Distribution DistributionImplementation::sin() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  const Scalar a = range_.getLowerBound()[0];
  const Scalar b = range_.getUpperBound()[0];
  const SignedInteger nMin = static_cast< SignedInteger > (ceil(a / M_PI - 0.5));
  const SignedInteger nMax = static_cast< SignedInteger > (floor(b / M_PI - 0.5));
  Point bounds(1, a);
  Point values(1, std::sin(a));
  for (SignedInteger n = nMin; n <= nMax; ++n)
  {
    bounds.add((n + 0.5) * M_PI);
    values.add(n % 2 == 0 ? 1.0 : -1.0);
  }
  bounds.add(b);
  values.add(std::sin(b));
  return new CompositeDistribution(SymbolicFunction("x", "sin(x)"), clone(), bounds, values);
}

Distribution DistributionImplementation::tan() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  const Scalar a = range_.getLowerBound()[0];
  const Scalar b = range_.getUpperBound()[0];
  const SignedInteger nMin = static_cast< SignedInteger > (ceil(a / M_PI - 0.5));
  const SignedInteger nMax = static_cast< SignedInteger > (floor(b / M_PI - 0.5));
  // Compute the lower bound and upper bound of the support of tan(X)
  Scalar sumPDF = 0.0;
  for (SignedInteger n = nMin; n <= nMax; ++n) sumPDF += computePDF((n + 0.5) * M_PI);
  const Scalar bound = std::tan(M_PI_2 - quantileEpsilon_ / sumPDF);
  Point bounds(1, a);
  Point values(1, std::tan(a));
  for (SignedInteger n = nMin; n <= nMax; ++n)
  {
    bounds.add((n + 0.5) * M_PI);
    values.add(bound);
    bounds.add((n + 0.5) * M_PI);
    values.add(-bound);
  }
  bounds.add(b);
  values.add(std::tan(b));
  return new CompositeDistribution(SymbolicFunction("x", "tan(x)"), clone(), bounds, values);
}

Distribution DistributionImplementation::acos() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  const Scalar a = range_.getLowerBound()[0];
  if (a < -1.0) throw InvalidArgumentException(HERE) << "Error: cannot take the arc cos of a random variable that takes values less than -1 with positive probability.";
  const Scalar b = range_.getUpperBound()[0];
  if (!(b <= 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot take the arc cos of a random variable that takes values greater than 1 with positive probability.";
  const Point bounds = {a, b};
  const Point values = {std::acos(a), std::acos(b)};
  return new CompositeDistribution(SymbolicFunction("x", "acos(x)"), clone(), bounds, values);
}

Distribution DistributionImplementation::asin() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  const Scalar a = range_.getLowerBound()[0];
  if (a < -1.0) throw InvalidArgumentException(HERE) << "Error: cannot take the arc sin of a random variable that takes values less than -1 with positive probability.";
  const Scalar b = range_.getUpperBound()[0];
  if (!(b <= 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot take the arc sin of a random variable that takes values greater than 1 with positive probability.";
  const Point bounds = {a, b};
  const Point values = {std::asin(a), std::asin(b)};
  return new CompositeDistribution(SymbolicFunction("x", "asin(x)"), clone(), bounds, values);
}

Distribution DistributionImplementation::atan() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  const Scalar a = range_.getLowerBound()[0];
  const Scalar b = range_.getUpperBound()[0];
  const Point bounds = {a, b};
  const Point values = {std::atan(a), std::atan(b)};
  return new CompositeDistribution(SymbolicFunction("x", "atan(x)"), clone(), bounds, values);
}

Distribution DistributionImplementation::cosh() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  const Scalar a = range_.getLowerBound()[0];
  Point bounds(1, a);
  Point values(1, std::cosh(a));
  const Scalar b = range_.getUpperBound()[0];
  if ((a < 0.0) && (b > 0.0))
  {
    bounds.add(0.0);
    values.add(1.0);
  }
  bounds.add(b);
  values.add(std::cosh(b));
  return new CompositeDistribution(SymbolicFunction("x", "cosh(x)"), clone(), bounds, values);
}

Distribution DistributionImplementation::sinh() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  const Scalar a = range_.getLowerBound()[0];
  const Scalar b = range_.getUpperBound()[0];
  const Point bounds = {a, b};
  const Point values = {std::sinh(a), std::sinh(b)};
  return new CompositeDistribution(SymbolicFunction("x", "sinh(x)"), clone(), bounds, values);
}

Distribution DistributionImplementation::tanh() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  const Scalar a = range_.getLowerBound()[0];
  const Scalar b = range_.getUpperBound()[0];
  const Point bounds = {a, b};
  const Point values = {std::tanh(a), std::tanh(b)};
  return new CompositeDistribution(SymbolicFunction("x", "tanh(x)"), clone(), bounds, values);
}

Distribution DistributionImplementation::acosh() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  const Scalar a = range_.getLowerBound()[0];
  if (!(a >= 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot take the arc cosh of a random variable that takes values less than 1 with positive probability.";
  const Scalar b = range_.getUpperBound()[0];
  const Point bounds = {a, b};
  const Point values = {SpecFunc::Acosh(a), SpecFunc::Acosh(b)};
  return new CompositeDistribution(SymbolicFunction("x", "acosh(x)"), clone(), bounds, values);
}

Distribution DistributionImplementation::asinh() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  const Scalar a = range_.getLowerBound()[0];
  const Scalar b = range_.getUpperBound()[0];
  const Point bounds = {a, b};
  const Point values = {SpecFunc::Asinh(a), SpecFunc::Asinh(b)};
  return new CompositeDistribution(SymbolicFunction("x", "asinh(x)"), clone(), bounds, values);
}

Distribution DistributionImplementation::atanh() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  const Scalar a = range_.getLowerBound()[0];
  if (a < -1.0) throw InvalidArgumentException(HERE) << "Error: cannot take the arc tanh of a random variable that takes values less than -1 with positive probability.";
  const Scalar b = range_.getUpperBound()[0];
  if (!(b <= 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot take the arc tanh of a random variable that takes values greater than 1 with positive probability.";
  Point bounds(1, a);
  // F_Y(y)=P(atanh(X)<y)<->P(X<tanh(y))=F_X(tanh(y))
  // y s.t. F_Y(y)=epsilon<->y=atanh(F_X^{-1}(epsilon))

  Point values(1, a == -1.0 ? SpecFunc::Atanh(computeScalarQuantile(quantileEpsilon_)) : SpecFunc::Atanh(a));
  bounds.add(b);
  values.add(b == 1.0 ? SpecFunc::Atanh(computeScalarQuantile(quantileEpsilon_, true)) : SpecFunc::Atanh(b));
  return new CompositeDistribution(SymbolicFunction("x", "atanh(x)"), clone(), bounds, values);
}

Distribution DistributionImplementation::exp() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  // Check if we can reuse an existing class
  if (getClassName() == "Normal")
  {
    const Point parameters(getParameter());
    return new LogNormal(parameters[0], parameters[1]);
  }
  if (getClassName() == "Uniform")
  {
    const Point parameters(getParameter());
    return new LogUniform(parameters[0], parameters[1]);
  }
  const Scalar a = range_.getLowerBound()[0];
  const Scalar b = range_.getUpperBound()[0];
  const Point bounds = {a, b};
  const Point values = {std::exp(a), std::exp(b)};
  return new CompositeDistribution(SymbolicFunction("x", "exp(x)"), clone(), bounds, values);
}

Distribution DistributionImplementation::log() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  // Check if we can reuse an existing class
  if (getClassName() == "LogNormal")
  {
    const Point parameters(getParameter());
    if (parameters[2] == 0.0) return new Normal(parameters[0], parameters[1]);
  }
  if (getClassName() == "LogUniform")
  {
    const Point parameters(getParameter());
    return new Uniform(parameters[0], parameters[1]);
  }
  const Scalar a = range_.getLowerBound()[0];
  if (!(a >= 0.0)) throw NotDefinedException(HERE) << "Error: cannot take the logarithm of a random variable that takes negative values with positive probability.";
  const Scalar b = range_.getUpperBound()[0];
  const Point bounds = {a, b};
  Point values(1, (a == 0.0 ? std::log(computeScalarQuantile(quantileEpsilon_)) : std::log(a)));
  values.add(std::log(b));
  return new CompositeDistribution(SymbolicFunction("x", "log(x)"), clone(), bounds, values);
}

Distribution DistributionImplementation::ln() const
{
  return log();
}

Distribution DistributionImplementation::pow(const Scalar exponent) const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  // First, the case where the exponent is integer
  if (trunc(exponent) == exponent) return pow(static_cast< SignedInteger >(trunc(exponent)));
  if (exponent == 0.5) return (*this).sqrt();
  if (exponent == 1.0 / 3.0) return (*this).cbrt();
  const Scalar a = range_.getLowerBound()[0];
  if (!(a >= 0.0)) throw NotDefinedException(HERE) << "Error: cannot take a fractional power of a random variable that takes negative values with positive probability.";

  SymbolicFunction toPower("x", String(OSS() << (exponent < 0.0 ? "x^(" : "x^") << exponent << (exponent < 0.0 ? ")" : "")));
  Point bounds(1, a);
  Point values(1, (a == 0.0 ? (exponent < 0.0 ? std::pow(computeScalarQuantile(quantileEpsilon_), exponent) : 0.0) : std::pow(a, exponent)));
  const Scalar b = range_.getUpperBound()[0];
  bounds.add(b);
  values.add(std::pow(b, exponent));
  return new CompositeDistribution(toPower, clone(), bounds, values);
}

Distribution DistributionImplementation::pow(const SignedInteger exponent) const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  if (exponent == 0) return new Dirac(Point(1, 1.0));
  if (exponent == 1) return *this;
  if (exponent == 2) return (*this).sqr();
  if (exponent == -1) return (*this).inverse();
  const Scalar a = range_.getLowerBound()[0];
  SymbolicFunction toPower("x", String(OSS() << (exponent < 0.0 ? "x^(" : "x^") << exponent << (exponent < 0.0 ? ")" : "")));
  // Easy case: a >= 0
  if (a >= 0.0)
  {
    Point bounds(1, a);
    Point values(1, (a == 0.0 ? (exponent < 0.0 ? std::pow(computeScalarQuantile(quantileEpsilon_), 1.0 * exponent) : 0.0) : std::pow(a, 1.0 * exponent)));
    const Scalar b = range_.getUpperBound()[0];
    bounds.add(b);
    values.add(std::pow(b, 1.0 * exponent));
    return new CompositeDistribution(toPower, clone(), bounds, values);
  }
  // Easy case: b <= 0
  Point bounds(1, a);
  Point values(1, std::pow(a, 1.0 * exponent));
  const Scalar b = range_.getUpperBound()[0];
  if (b <= 0.0)
  {
    bounds.add(b);
    values.add(b == 0.0 ? (exponent < 0.0 ? std::pow(computeScalarQuantile(quantileEpsilon_, true), 1.0 * exponent) : 0.0) : std::pow(b, 1.0 * exponent));
    return new CompositeDistribution(toPower, clone(), bounds, values);
  }
  // Difficult case: a < 0 < b
  // For odd exponents, the function is bijective
  if (exponent % 2 != 0)
  {
    // No singularity at 0 for positive exponent
    if (exponent > 0)
    {
      bounds.add(b);
      values.add(std::pow(b, 1.0 * exponent));
      return new CompositeDistribution(toPower, clone(), bounds, values);
    }
    // A singularity at 0 for negative exponent
    bounds.add(0.0);
    values.add(SpecFunc::LowestScalar);
    bounds.add(0.0);
    values.add(SpecFunc::MaxScalar);
    bounds.add(b);
    values.add(std::pow(b, 1.0 * exponent));
    return new CompositeDistribution(toPower, clone(), bounds, values);
  }
  // For even exponent, the behaviour changes at 0
  bounds.add(0.0);
  values.add(exponent > 0 ? 0.0 : SpecFunc::MaxScalar);
  bounds.add(b);
  values.add(std::pow(b, 1.0 * exponent));
  return new CompositeDistribution(toPower, clone(), bounds, values);
}

Distribution DistributionImplementation::sqr() const
{
  // Check if we can reuse an existing class
  if (getClassName() == "Chi")
  {
    const Point parameters(getParameter());
    return new ChiSquare(parameters[0]);
  }
  if (getClassName() == "Normal")
  {
    const Point parameters(getParameter());
    return new SquaredNormal(parameters[0], parameters[1]);
  }
  return new CompositeDistribution(SymbolicFunction("x", "x^2"), clone());
}

Distribution DistributionImplementation::inverse() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  const Scalar a = range_.getLowerBound()[0];
  Point bounds(1, a);
  const SymbolicFunction inverseFunction("x", "1.0 / x");
  // Easy case: a >= 0
  if (a >= 0.0)
  {
    Point values(1, (a == 0.0 ? 1.0 / computeScalarQuantile(quantileEpsilon_) : 1.0 / a));
    const Scalar b = range_.getUpperBound()[0];
    bounds.add(b);
    if (range_.getFiniteUpperBound()[0])
      values.add(1.0 / b);
    else
      values.add(0.0);
    return new CompositeDistribution(inverseFunction, clone(), bounds, values);
  }
  // Here, a < 0
  Point values(1);
  if (range_.getFiniteLowerBound()[0])
    values[0] = 1.0 / a;
  else
    values[0] = 0.0;
  const Scalar b = range_.getUpperBound()[0];
  // Easy case: b <= 0
  if (b <= 0.0)
  {
    bounds.add(b);
    values.add(b == 0.0 ? 1.0 / computeScalarQuantile(quantileEpsilon_, true) : 1.0 / b);
    return new CompositeDistribution(inverseFunction, clone(), bounds, values);
  }
  // Difficult case: a < 0 < b
  // A singularity at 0
  bounds.add(0.0);
  // The CDF of Y=1/X is
  // F_Y(y)=[F_X(0) - F_X(1 / y)]1_{y < 0} +
  //        [F_X(0) + 1 - F_X(1 / y)]1_{y > 0} +
  //        F_X(0)1_{y = 0}
  // so the bounds for Y are obtained when X->0^- and X->0^+
  values.add(1.0 / computeScalarQuantile(computeCDF(0.0) - quantileEpsilon_));
  bounds.add(0.0);
  values.add(1.0 / computeScalarQuantile(computeCDF(0.0) + quantileEpsilon_));
  bounds.add(b);
  if (range_.getFiniteUpperBound()[0])
    values.add(1.0 / b);
  else
    values.add(0.0);
  return new CompositeDistribution(inverseFunction, clone(), bounds, values);
}

Distribution DistributionImplementation::sqrt() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  // Check if we can reuse an existing class
  if (getClassName() == "ChiSquare")
  {
    const Point parameters(getParameter());
    return new Chi(parameters[0]);
  }
  const Scalar a = range_.getLowerBound()[0];
  if (!(a >= 0.0)) throw NotDefinedException(HERE) << "Error: cannot take the square root of a random variable that takes negative values with positive probability.";
  const Scalar b = range_.getUpperBound()[0];
  const Point bounds = {a, b};
  const Point values = {std::sqrt(a), std::sqrt(b)};
  return new CompositeDistribution(SymbolicFunction("x", "sqrt(x)"), clone(), bounds, values);
}

Distribution DistributionImplementation::cbrt() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  const Scalar a = range_.getLowerBound()[0];
  const Scalar b = range_.getUpperBound()[0];
  const Point bounds = {a, b};
  const Point values = {SpecFunc::Cbrt(a), SpecFunc::Cbrt(b)};
  return new CompositeDistribution(SymbolicFunction("x", "cbrt(x)"), clone(), bounds, values);
}

Distribution DistributionImplementation::abs() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  // First the easy cases
  const Scalar a = range_.getLowerBound()[0];
  if (a >= 0.0) return *this;
  const Scalar b = range_.getUpperBound()[0];
  if (b <= 0.0) return (*this) * (-1.0);
  // Now the difficult case
  const Point bounds = {a, 0.0, b};
  const Point values = {std::abs(a), 0.0, b};
  return new CompositeDistribution(SymbolicFunction("x", "abs(x)"), clone(), bounds, values);
}

/* Quantile epsilon accessor */
Scalar DistributionImplementation::getQuantileEpsilon() const
{
  return quantileEpsilon_;
}

void DistributionImplementation::setQuantileEpsilon(const Scalar quantileEpsilon)
{
  if (!(quantileEpsilon >= 0.0) || !(quantileEpsilon <= 1.0))
    throw InvalidArgumentException(HERE) << "Quantile epsilon must be in [0, 1] here epsilon=" << quantileEpsilon;
  quantileEpsilon_ = quantileEpsilon;
}

END_NAMESPACE_OPENTURNS
