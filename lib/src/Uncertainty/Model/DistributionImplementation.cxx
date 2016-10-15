//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all distributions
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
#include <cmath>
#include <cstdlib>
#include <algorithm>

#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/DistributionImplementation.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Log.hxx"
#include "openturns/Lapack.hxx"
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
#include "openturns/TruncatedDistribution.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/IndependentCopula.hxx"
#include "openturns/MarginalDistribution.hxx"
#include "openturns/MarginalTransformationEvaluation.hxx"
#include "openturns/MarginalTransformationGradient.hxx"
#include "openturns/MarginalTransformationHessian.hxx"
#include "openturns/RosenblattEvaluation.hxx"
#include "openturns/InverseRosenblattEvaluation.hxx"
#include "openturns/NumericalMathFunction.hxx"
#include "openturns/SklarCopula.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PlatformInfo.hxx"
#include "openturns/Contour.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Staircase.hxx"
#include "openturns/Drawable.hxx"
#include "openturns/Graph.hxx"
#include "openturns/Brent.hxx"
#include "openturns/Box.hxx"
#include "openturns/Tuples.hxx"
#include "openturns/Combinations.hxx"
#include "openturns/TBB.hxx"
#include "openturns/GaussKronrod.hxx"
#include "openturns/IteratedQuadrature.hxx"
#include "openturns/OptimizationProblem.hxx"
#include "openturns/TNC.hxx"
#include "openturns/TriangularMatrix.hxx"
#include "openturns/MethodBoundNumericalMathEvaluationImplementation.hxx"
#include "openturns/SobolSequence.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DistributionImplementation);

typedef NumericalMathFunctionImplementation::EvaluationImplementation EvaluationImplementation;
typedef NumericalMathFunctionImplementation::GradientImplementation   GradientImplementation;
typedef NumericalMathFunctionImplementation::HessianImplementation    HessianImplementation;
typedef Collection<Distribution>                                      DistributionCollection;

static const Factory<DistributionImplementation> Factory_DistributionImplementation;

/* Default constructor */
DistributionImplementation::DistributionImplementation()
  : PersistentObject()
  , mean_(NumericalPoint(0))
  , covariance_(CovarianceMatrix(0))
  , gaussNodes_()
  , gaussWeights_()
  , integrationNodesNumber_(ResourceMap::GetAsUnsignedInteger("Distribution-DefaultIntegrationNodesNumber"))
  , isAlreadyComputedMean_(false)
  , isAlreadyComputedCovariance_(false)
  , isAlreadyComputedGaussNodesAndWeights_(false)
  , pdfEpsilon_(ResourceMap::GetAsNumericalScalar("Distribution-DefaultPDFEpsilon"))
  , cdfEpsilon_(ResourceMap::GetAsNumericalScalar("Distribution-DefaultCDFEpsilon"))
  , quantileEpsilon_(ResourceMap::GetAsNumericalScalar("Distribution-DefaultQuantileEpsilon"))
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
  , isInitializedCF_(false)
  , pdfGrid_(0)
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

Bool DistributionImplementation::equals(const DistributionImplementation & other) const
{
  throw NotYetImplementedException(HERE) << "In DistributionImplementation::equals";
}

/* Comparison operator */
Bool DistributionImplementation::operator !=(const DistributionImplementation & other) const
{
  return !operator==(other);
}

/* Addition operator */
DistributionImplementation::Implementation DistributionImplementation::operator + (const DistributionImplementation & other) const
{
  return operator + (other.clone());
}

DistributionImplementation::Implementation DistributionImplementation::operator + (const Implementation & other) const
{
  if ((dimension_ != 1) || (other->dimension_ != 1)) throw NotYetImplementedException(HERE) << "In DistributionImplementation::operator + (const Implementation & other) const: can add 1D distributions only.";
  Collection< Distribution > coll(2);
  coll[0] = *this;
  coll[1] = *other;
  return RandomMixture(coll).clone();
}

DistributionImplementation::Implementation DistributionImplementation::operator + (const NumericalScalar value) const
{
  if (dimension_ != 1) throw NotYetImplementedException(HERE) << "In DistributionImplementation::operator + (const NumericalScalar value) const: can add a constant to 1D distributions only.";
  if (value == 0.0) return clone();
  Collection< Distribution > coll(2);
  coll[0] = *this;
  coll[1] = Dirac(NumericalPoint(1, value));
  return RandomMixture(coll).clone();
}

/* Substraction operator */
DistributionImplementation::Implementation DistributionImplementation::operator - (const DistributionImplementation & other) const
{
  return operator - (other.clone());
}

DistributionImplementation::Implementation DistributionImplementation::operator - (const Implementation & other) const
{
  if ((dimension_ != 1) || (other->dimension_ != 1)) throw NotYetImplementedException(HERE) << "In DistributionImplementation::operator - (const Implementation & other) const: can substract 1D distributions only.";
  Collection< Distribution > coll(2);
  coll[0] = *this;
  coll[1] = *other;
  NumericalPoint weights(2);
  weights[0] = 1.0;
  weights[1] = -1.0;
  return RandomMixture(coll, weights).clone();
}

DistributionImplementation::Implementation DistributionImplementation::operator - (const NumericalScalar value) const
{
  if (dimension_ != 1) throw NotYetImplementedException(HERE) << "In DistributionImplementation::operator - (const NumericalScalar value) const: can substract a constant to 1D distributions only.";
  if (value == 0.0) return clone();
  Collection< Distribution > coll(2);
  coll[0] = *this;
  coll[1] = Dirac(NumericalPoint(1, -value));
  return RandomMixture(coll).clone();
}

/* Multiplication operator */
DistributionImplementation::Implementation DistributionImplementation::operator * (const DistributionImplementation & other) const
{
  return operator * (other.clone());
}

DistributionImplementation::Implementation DistributionImplementation::operator * (const Implementation & other) const
{
  // Special case: LogNormal distributions
  if ((getClassName() == "LogNormal") && (other->getClassName() == "LogNormal"))
  {
    const NumericalPoint parameters(getParameter());
    const NumericalPoint otherParameters(other->getParameter());
    return LogNormal(parameters[0] + otherParameters[0], std::sqrt(parameters[1] * parameters[1] + otherParameters[1] * otherParameters[1])).clone();
  }
  if ((getClassName() == "LogUniform") && (other->getClassName() == "LogUniform"))
  {
    const NumericalPoint parameters(getParameter());
    const NumericalPoint otherParameters(other->getParameter());
    return (Uniform(parameters[0], parameters[1]) + Uniform(otherParameters[0], otherParameters[1]))->exp();
  }
  if ((getClassName() == "LogUniform") && (other->getClassName() == "LogNormal"))
  {
    const NumericalPoint parameters(getParameter());
    const NumericalPoint otherParameters(other->getParameter());
    return (Uniform(parameters[0], parameters[1]) + Normal(otherParameters[0], otherParameters[1]))->exp();
  }
  if ((getClassName() == "LogNormal") && (other->getClassName() == "LogUniform"))
  {
    const NumericalPoint parameters(getParameter());
    const NumericalPoint otherParameters(other->getParameter());
    return (Normal(parameters[0], parameters[1]) + Uniform(otherParameters[0], otherParameters[1]))->exp();
  }
  return ProductDistribution(*this, *other).clone();
}

DistributionImplementation::Implementation DistributionImplementation::operator * (const NumericalScalar value) const
{
  if (dimension_ != 1) throw NotYetImplementedException(HERE) << "In DistributionImplementation::operator * (const NumericalScalar value) const: can multiply by a constant 1D distributions only.";
  if (value == 0.0) return Dirac(NumericalPoint(1, 0.0)).clone();
  if (value == 1.0) return clone();
  const Collection< Distribution > coll(1, *this);
  const NumericalPoint weight(1, value);
  return RandomMixture(coll, weight).clone();
}

/* Division operator */
DistributionImplementation::Implementation DistributionImplementation::operator / (const DistributionImplementation & other) const
{
  return operator / (other.clone());
}

DistributionImplementation::Implementation DistributionImplementation::operator / (const Implementation & other) const
{
  if ((dimension_ != 1) || (other->dimension_ != 1)) throw NotYetImplementedException(HERE) << "In DistributionImplementation::operator / (const Implementation & other) const: can multiply 1D distributions only.";
  return operator * (*other->inverse());
}

DistributionImplementation::Implementation DistributionImplementation::operator / (const NumericalScalar value) const
{
  if (dimension_ != 1) throw NotYetImplementedException(HERE) << "In DistributionImplementation::operator / (const NumericalScalar value) const: can divide multiply by a constant 1D distributions only.";
  if (value == 0.0) throw InvalidArgumentException(HERE) << "Error: cannot divide by 0.";
  if (value == 1.0) return clone();
  return (*this) * (1.0 / value);
}

/* Product operator */
DistributionImplementation::Implementation operator * (const NumericalScalar scalar,
    const DistributionImplementation & distribution)
{
  return distribution * scalar;
}

DistributionImplementation::Implementation operator * (const NumericalScalar scalar,
    const DistributionImplementation::Implementation & p_distribution)
{
  return (*p_distribution) * scalar;
}

/* Division operator */
DistributionImplementation::Implementation operator / (const NumericalScalar scalar,
    const DistributionImplementation & distribution)
{
  return (*distribution.inverse()) * scalar;
}

DistributionImplementation::Implementation operator / (const NumericalScalar scalar,
    const DistributionImplementation::Implementation & p_distribution)
{
  return (*(*p_distribution).inverse()) * scalar;
}

/* Addition operator */
DistributionImplementation::Implementation operator + (const NumericalScalar scalar,
    const DistributionImplementation & distribution)
{
  return distribution + scalar;
}

DistributionImplementation::Implementation operator + (const NumericalScalar scalar,
    const DistributionImplementation::Implementation & p_distribution)
{
  return (*p_distribution) + scalar;
}

/* Substraction operator */
DistributionImplementation::Implementation operator - (const NumericalScalar scalar,
    const DistributionImplementation & distribution)
{
  return (*(distribution * (-1.0))) + scalar;
}

DistributionImplementation::Implementation operator - (const NumericalScalar scalar,
    const DistributionImplementation::Implementation & p_distribution)
{
  return (*((*p_distribution) * (-1.0))) + scalar;
}

DistributionImplementation::Implementation operator - (const DistributionImplementation & distribution)
{
  return distribution * (-1.0);
}

DistributionImplementation::Implementation operator - (const DistributionImplementation::Implementation & p_distribution)
{
  return (*p_distribution) * (-1.0);
}


DistributionImplementation::Implementation maximum(const DistributionImplementation::Implementation & p_left,
    const DistributionImplementation::Implementation & p_right)
{
  MaximumDistribution::DistributionCollection coll(2);
  coll[0] = p_left;
  coll[1] = p_right;
  return MaximumDistribution(coll).clone();
}

DistributionImplementation::Implementation maximum(const DistributionImplementation & left,
    const DistributionImplementation::Implementation & p_right)
{
  MaximumDistribution::DistributionCollection coll(2);
  coll[0] = left;
  coll[1] = p_right;
  return MaximumDistribution(coll).clone();
}

DistributionImplementation::Implementation maximum(const DistributionImplementation::Implementation & p_left,
    const DistributionImplementation & right)
{
  MaximumDistribution::DistributionCollection coll(2);
  coll[0] = p_left;
  coll[1] = right;
  return MaximumDistribution(coll).clone();
}

DistributionImplementation::Implementation maximum(const DistributionImplementation & left,
    const DistributionImplementation & right)
{
  MaximumDistribution::DistributionCollection coll(2);
  coll[0] = left;
  coll[1] = right;
  return MaximumDistribution(coll).clone();
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
String DistributionImplementation::__str__(const String & offset) const
{
  return __repr__();
}


/* Weight accessor */
void DistributionImplementation::setWeight(const NumericalScalar w)
{
  weight_ = w;
}

/* Weight accessor */
NumericalScalar DistributionImplementation::getWeight() const
{
  return weight_;
}


/* Dimension accessor */
UnsignedInteger DistributionImplementation::getDimension() const
{
  return dimension_;
}

/* Get the roughness, i.e. the L2-norm of the PDF */
NumericalScalar DistributionImplementation::getRoughness() const
{
  throw NotYetImplementedException(HERE) << "In DistributionImplementation::getRoughness() const";
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

/* Get one realization of the distributionImplementation */
NumericalPoint DistributionImplementation::getRealization() const
{
  return getRealizationByInversion();
}

/* Get a numerical sample whose elements follow the distributionImplementation */
NumericalSample DistributionImplementation::getSample(const UnsignedInteger size) const
{
  NumericalSample returnSample(size, dimension_);
  for (UnsignedInteger i = 0; i < size; ++ i)
    returnSample[i] = getRealization();
  returnSample.setName(getName());
  returnSample.setDescription(getDescription());
  return returnSample;
}

/* Get one realization of the distributionImplementation */
NumericalPoint DistributionImplementation::getRealizationByInversion() const
{
  // Use CDF inversion in the 1D case
  if (dimension_ == 1) return computeQuantile(RandomGenerator::Generate());
  // Use conditional CDF inversion in the 1D case
  NumericalPoint point(0);
  for (UnsignedInteger i = 0; i < dimension_; ++ i)
  {
    const NumericalScalar u = RandomGenerator::Generate();
    LOGINFO(OSS(false) << "i=" << i << ", u=" << u);
    point.add(computeConditionalQuantile(u, point));
    LOGINFO(OSS(false) << "i=" << i << ", u=" << u << ", point=" << point);
  }
  return point;
}

/* Get a numerical sample whose elements follow the distributionImplementation */
NumericalSample DistributionImplementation::getSampleByInversion(const UnsignedInteger size) const
{
  // Use CDF inversion in the 1D case
  if (dimension_ == 1) return computeQuantile(RandomGenerator::Generate(size));
  // Use conditional CDF inversion in the 1D case
  NumericalSample result(size, 0);
  for (UnsignedInteger i = 0; i < dimension_; ++ i)
  {
    const NumericalPoint u(RandomGenerator::Generate(size));
    NumericalSampleImplementation q(size, 1);
    q.setData(computeConditionalQuantile(u, result));
    result.stack(q);
  }
  return result;
}

NumericalSample DistributionImplementation::getSampleByQMC(const UnsignedInteger size) const
{
  const SobolSequence sequence(1);
  // Use CDF inversion in the 1D case
  if (dimension_ == 1) return computeQuantile(sequence.generate(size).getImplementation()->getData());
  // Use conditional CDF inversion in the 1D case
  NumericalSample result(size, 0);
  for (UnsignedInteger i = 0; i < dimension_; ++ i)
  {
    const NumericalPoint u(sequence.generate(size).getImplementation()->getData());
    NumericalSampleImplementation q(size, 1);
    q.setData(computeConditionalQuantile(u, result));
    result.stack(q);
  }
  return result;
}

/* Get the DDF of the distributionImplementation */
NumericalPoint DistributionImplementation::computeDDF(const NumericalPoint & point) const
{
  if (dimension_ == 1)
  {
    NumericalPoint ddf(dimension_);
    const NumericalScalar cdfPoint = computeCDF(point);
    const NumericalScalar h = std::pow(cdfEpsilon_, 0.25);
    const NumericalScalar idenom = 1.0 / std::sqrt(cdfEpsilon_);
    for (UnsignedInteger i = 0; i < dimension_; ++ i)
    {
      NumericalPoint epsilon(dimension_, 0.0);
      epsilon[i] = h;
      ddf[i] = (computeCDF(point + epsilon) - 2.0 * cdfPoint + computeCDF(point - epsilon)) * idenom;
    }
    return ddf;
  }
  throw NotDefinedException(HERE) << "In DistributionImplementation::computeDDF()";
}

/* Get the PDF of the distributionImplementation */
NumericalScalar DistributionImplementation::computePDF(const NumericalPoint & point) const
{
  const NumericalScalar epsilon = 2.0 * std::pow(cdfEpsilon_, 1.0 / 3.0);
  const NumericalSample xSample(((Box(Indices(dimension_, 0)).generate() - NumericalPoint(dimension_, 0.5)) * NumericalPoint(dimension_, epsilon)) + point);
  const NumericalSample cdfSample(computeCDF(xSample));
  NumericalScalar pdf = 0.0;
  const UnsignedInteger iMax = cdfSample.getSize();
  for (UnsignedInteger i = 0; i < iMax; ++ i)
  {
    // The points with an even number of positive shifts are counted positively
    // The others are counted negatively
    const UnsignedInteger numNullBits = dimension_ - SpecFunc::BitCount(i);
    pdf += (1.0 - 2.0 * (numNullBits % 2)) * cdfSample[i][0];
  }
  return pdf / std::pow(epsilon, 1.0 * dimension_);
}

NumericalScalar DistributionImplementation::computeLogPDF(const NumericalPoint & point) const
{
  const NumericalScalar pdf = computePDF(point);
  NumericalScalar logPdf = SpecFunc::LogMinNumericalScalar;
  if ( pdf > 0.0 ) logPdf = std::log(pdf);
  return logPdf;
}

/* Get the CDF, complementary CDF and survival function of the distributionImplementation */
/* On a NumericalPoint */
NumericalScalar DistributionImplementation::computeCDF(const NumericalPoint & point) const
{
  throw NotYetImplementedException(HERE) << "In DistributionImplementation::computeCDF(const NumericalPoint & point) const";
}

NumericalScalar DistributionImplementation::computeComplementaryCDF(const NumericalPoint & point) const
{
  const NumericalScalar cdf = computeCDF(point);
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
NumericalScalar DistributionImplementation::computeSurvivalFunction(const NumericalPoint & point) const
{
  if (dimension_ == 1) return computeComplementaryCDF(point);
  // Special case for independent copula
  if (hasIndependentCopula())
    {
      NumericalScalar value = 1.0;
      for (UnsignedInteger i = 0; i < dimension_; ++i) value *= getMarginal(i)->computeComplementaryCDF(point[i]);
      return value;
    }
  // For elliptical distributions, 
  // P(X_1-mu_1<=x_1,...,X_d-mu_d<=x_d)=P(X_1-mu_1>=-x_1,...,X_d-mu_d>=-x_d)
  // So
  // P(X_1>=x_1,...,X_d>=x_d)=P(X_1<=2mu_1-x_1,...,X_d<=2mu_d-x_d)
  if (isElliptical()) return computeCDF(getMean() * 2.0 - point);
  const NumericalPoint lowerBounds(getRange().getLowerBound());
  const NumericalPoint upperBounds(getRange().getUpperBound());
  Bool allOutside = true;
  for (UnsignedInteger i = 0; i < dimension_; ++ i)
  {
    if (point[i] >= upperBounds[i]) return 0.0;
    allOutside &= (point[i] <= lowerBounds[i]);
  }
  if (allOutside) return 1.0;

  // Use Poincaré's formula
  const NumericalScalar cdf = computeCDF(point);
  NumericalScalar value = 1.0 + (dimension_ % 2 == 0 ? cdf : -cdf);
  NumericalScalar sign = -1.0;
  for (UnsignedInteger i = 1; i < dimension_; ++ i)
  {
    NumericalScalar contribution = 0.0;
    Combinations::IndicesCollection indices(Combinations(i, dimension_).generate());
    NumericalPoint subPoint(i);
    for (UnsignedInteger j = 0; j < indices.getSize(); ++j)
    {
      for (UnsignedInteger k = 0; k < i; ++k) subPoint[k] = point[indices[j][k]];
      contribution += getMarginal(indices[j])->computeCDF(subPoint);
    }
    value += sign * contribution;
    sign = -sign;
  }
  return value;
}

NumericalPoint DistributionImplementation::computeInverseSurvivalFunction(const NumericalScalar prob) const
{
  NumericalScalar marginalProb = 0.0;
  return computeInverseSurvivalFunction(prob, marginalProb);
}

NumericalPoint DistributionImplementation::computeInverseSurvivalFunction(const NumericalScalar prob,
									  NumericalScalar & marginalProb) const
{
  // Special case for bording values
  marginalProb = prob;
  if (prob < 0.0) return range_.getUpperBound();
  if (prob >= 1.0) return range_.getLowerBound();
  // Special case for dimension 1
  if (dimension_ == 1) return NumericalPoint(1, computeScalarQuantile(prob, true));
  // Special case for independent copula
  if (hasIndependentCopula())
    {
      NumericalPoint result(dimension_);
      marginalProb = std::pow(prob, 1.0 / dimension_);
      for (UnsignedInteger i = 0; i < dimension_; ++i) result[i] = getMarginal(i)->computeScalarQuantile(marginalProb, true);
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
  for (UnsignedInteger i = 0; i < dimension_; i++) marginals[i] = getMarginal(i);
  // The n-D inverse survival function is defined as X(\tau) = (S_1^{-1}(\tau), ..., S_n^{-1}(\tau)),
  // with tau such as S(X(\tau)) = q.
  // As F(x) = C(F_1(x_1),...,F_n(x_n)), the constraint F(X(\tau)) = q reads:
  // C(\tau,...,\tau) = q
  // Bracketing of \tau using the Frechet Hoeffding bounds:
  // max(n\tau - n + 1, 0) <= C(\tau,...,\tau) <= \tau
  // from which we deduce that q <= \tau and \tau <= 1 - (1 - q) / n
  // Lower bound of the bracketing interval
  const SurvivalFunctionWrapper wrapper(marginals, this);
  const NumericalMathFunction f(bindMethod<SurvivalFunctionWrapper, NumericalPoint, NumericalPoint>(wrapper, &SurvivalFunctionWrapper::computeDiagonal, 1, 1));
  NumericalScalar leftTau = prob;
  NumericalScalar leftSurvival = f(NumericalPoint(1, leftTau))[0];
  // Due to numerical precision issues, the theoretical bound can be slightly violated
  if (leftSurvival > prob)
  {
    leftTau = 0.0;
    leftSurvival = 1.0;
  }
  // Upper bound of the bracketing interval
  NumericalScalar rightTau = 1.0 - (1.0 - prob) / dimension_;
  NumericalScalar rightSurvival = f(NumericalPoint(1, rightTau))[0];
  // Due to numerical precision issues, the theoretical bound can be slightly violated
  if (rightSurvival < prob)
  {
    rightTau = 1.0;
    rightSurvival = 0.0;
  }
  LOGDEBUG(OSS() << "DistributionImplementation::computeInverseSurvivalFunction: dimension=" << dimension_ << ", prob=" << prob << ", leftTau=" << leftTau << ", leftSurvival=" << leftSurvival << ", rightTau=" << rightTau << ", rightSurvival=" << rightSurvival);
  // Use Brent's method to compute the quantile efficiently for continuous distributions
  const Brent solver(quantileEpsilon_, cdfEpsilon_, cdfEpsilon_, quantileIterations_);
  marginalProb = solver.solve(f, prob, leftTau, rightTau, leftSurvival, rightSurvival);
  LOGINFO(OSS(false) << "tau=" << marginalProb);
  return wrapper.diagonalToSpace(marginalProb);
}

/* On a NumericalSample */
NumericalSample DistributionImplementation::computeCDFSequential(const NumericalSample & inSample) const
{
  const UnsignedInteger size = inSample.getSize();
  NumericalSample outSample(size, 1);
  for (UnsignedInteger i = 0; i < size; ++ i) outSample[i][0] = computeCDF(inSample[i]);
  return outSample;
}

struct ComputeCDFPolicy
{
  const NumericalSample & input_;
  NumericalSample & output_;
  const DistributionImplementation & distribution_;

  ComputeCDFPolicy( const NumericalSample & input,
                    NumericalSample & output,
                    const DistributionImplementation & distribution)
    : input_(input)
    , output_(output)
    , distribution_(distribution)
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++ i) output_[i][0] = distribution_.computeCDF(input_[i]);
  }

}; /* end struct ComputeCDFPolicy */

NumericalSample DistributionImplementation::computeCDFParallel(const NumericalSample & inSample) const
{
  if (inSample.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << dimension_ << ", got " << inSample.getDimension();
  const UnsignedInteger size = inSample.getSize();
  NumericalSample result(size, 1);
  const ComputeCDFPolicy policy( inSample, result, *this );
  TBB::ParallelFor( 0, size, policy );
  return result;
}

NumericalSample DistributionImplementation::computeCDF(const NumericalSample & inSample) const
{
  if (inSample.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << dimension_ << ", got " << inSample.getDimension();
  if (isParallel_) return computeCDFParallel(inSample);
  else return computeCDFSequential(inSample);
}

NumericalSample DistributionImplementation::computeComplementaryCDFSequential(const NumericalSample & inSample) const
{
  const UnsignedInteger size = inSample.getSize();
  NumericalSample outSample(size, 1);
  for (UnsignedInteger i = 0; i < size; ++ i) outSample[i][0] = computeComplementaryCDF(inSample[i]);
  return outSample;
}

struct ComputeComplementaryCDFPolicy
{
  const NumericalSample & input_;
  NumericalSample & output_;
  const DistributionImplementation & distribution_;

  ComputeComplementaryCDFPolicy( const NumericalSample & input,
                                 NumericalSample & output,
                                 const DistributionImplementation & distribution)
    : input_(input)
    , output_(output)
    , distribution_(distribution)
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++ i) output_[i][0] = distribution_.computeComplementaryCDF(input_[i]);
  }

}; /* end struct ComputeComplementaryCDFPolicy */

NumericalSample DistributionImplementation::computeComplementaryCDFParallel(const NumericalSample & inSample) const
{
  if (inSample.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << dimension_ << ", got " << inSample.getDimension();
  const UnsignedInteger size = inSample.getSize();
  NumericalSample result(size, 1);
  const ComputeComplementaryCDFPolicy policy( inSample, result, *this );
  TBB::ParallelFor( 0, size, policy );
  return result;
}

NumericalSample DistributionImplementation::computeComplementaryCDF(const NumericalSample & inSample) const
{
  if (inSample.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << dimension_ << ", got " << inSample.getDimension();
  if (isParallel_) return computeComplementaryCDFParallel(inSample);
  else return computeComplementaryCDFSequential(inSample);
}

NumericalSample DistributionImplementation::computeSurvivalFunctionSequential(const NumericalSample & inSample) const
{
  const UnsignedInteger size = inSample.getSize();
  NumericalSample outSample(size, 1);
  for (UnsignedInteger i = 0; i < size; ++ i) outSample[i][0] = computeSurvivalFunction(inSample[i]);
  return outSample;
}

struct ComputeSurvivalFunctionPolicy
{
  const NumericalSample & input_;
  NumericalSample & output_;
  const DistributionImplementation & distribution_;

  ComputeSurvivalFunctionPolicy( const NumericalSample & input,
                                 NumericalSample & output,
                                 const DistributionImplementation & distribution)
    : input_(input)
    , output_(output)
    , distribution_(distribution)
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i) output_[i][0] = distribution_.computeSurvivalFunction(input_[i]);
  }

}; /* end struct ComputeSurvivalFunctionPolicy */

NumericalSample DistributionImplementation::computeSurvivalFunctionParallel(const NumericalSample & inSample) const
{
  if (inSample.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << dimension_ << ", got " << inSample.getDimension();
  const UnsignedInteger size = inSample.getSize();
  NumericalSample result(size, 1);
  const ComputeSurvivalFunctionPolicy policy( inSample, result, *this );
  TBB::ParallelFor( 0, size, policy );
  return result;
}

NumericalSample DistributionImplementation::computeSurvivalFunction(const NumericalSample & inSample) const
{
  if (inSample.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << dimension_ << ", got " << inSample.getDimension();
  if (isParallel_) return computeSurvivalFunctionParallel(inSample);
  else return computeSurvivalFunctionSequential(inSample);
}

/* Compute the probability content of an interval */
NumericalScalar DistributionImplementation::computeProbability(const Interval & interval) const
{
  if (interval.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: expected an interval of dimension=" << dimension_ << ", got dimension=" << interval.getDimension();
  // Empty interval, quick check. More checks will be done in the refined algorithms
  if (interval.isNumericallyEmpty()) return 0.0;
  // Generic implementation for univariate distributions
  if (dimension_ == 1)
  {
    const Bool finiteA = interval.getFiniteLowerBound()[0];
    const Bool finiteB = interval.getFiniteUpperBound()[0];
    if (finiteA)
    {
      const NumericalScalar a = interval.getLowerBound()[0];
      const NumericalScalar ccdfA = computeComplementaryCDF(a);
      if (finiteB)
      {
        // [a, b]
        const NumericalScalar b = interval.getUpperBound()[0];
        if (ccdfA <= 0.5)
        {
          const NumericalScalar ccdfB = computeComplementaryCDF(b);
          return ccdfA - ccdfB;
        }
        const NumericalScalar cdfA = computeCDF(a);
        const NumericalScalar cdfB = computeCDF(b);
        return cdfB - cdfA;
      }
      // [a,+inf)
      return ccdfA;
    }
    // (-inf, b]
    if (finiteB) return computeCDF(interval.getUpperBound()[0]);
    // (-inf, +inf)
    return 1.0;
  }
  // Generic implementation for continuous distributions
  if (isContinuous()) return computeProbabilityContinuous(interval);
  // Generic implementation for discrete distributions
  if (isDiscrete())   return computeProbabilityDiscrete(interval);
  // Generic implementation for general distributions
  return computeProbabilityGeneral(interval);
}

/* Get the probability content of an interval, continuous case */
NumericalScalar DistributionImplementation::computeProbabilityContinuous(const Interval & interval) const
{
  const Interval reducedInterval(interval.intersect(getRange()));
  if (reducedInterval.isNumericallyEmpty()) return 0.0;
  if (reducedInterval == getRange()) return 1.0;
  // Use adaptive multidimensional integration of the PDF on the reduced interval
  const PDFWrapper pdfWrapper(this);
  NumericalScalar probability = 0;
  if (dimension_ == 1)
  {
    NumericalScalar error;
    probability = GaussKronrod().integrate(pdfWrapper, reducedInterval, error)[0];
  }
  else probability = IteratedQuadrature().integrate(pdfWrapper, reducedInterval)[0];
  return std::min(1.0, std::max(0.0, probability));
}

/* Get the probability content of an interval, discrete case */
NumericalScalar DistributionImplementation::computeProbabilityDiscrete(const Interval & interval) const
{
  const NumericalSample support(getSupport(interval));
  NumericalScalar value = 0.0;
  for (UnsignedInteger i = 0; i < support.getSize(); ++i) value += computePDF(support[i]);
  return value;
}

/* Get the probability content of an interval, general case */
NumericalScalar DistributionImplementation::computeProbabilityGeneral(const Interval & interval) const
{
  const Interval reducedInterval(interval.intersect(getRange()));
  if (reducedInterval.isNumericallyEmpty()) return 0.0;
  if (reducedInterval == getRange()) return 1.0;
  // P(\bigcap_i ai < Xi \leq bi) = \sum_c (−1)^n(c) F(c_1,c_2,...,c_n)
  // with c = (c_i, i =1, ..., n), c_i \in [a_i, b_i]
  // and n(c) = Card({c_i == a_i, i = 1, ..., n})
  NumericalScalar probability = 0.0;
  const NumericalPoint a(reducedInterval.getLowerBound());
  const NumericalPoint b(reducedInterval.getUpperBound());
  const UnsignedInteger iMax = 1 << dimension_;
  for( UnsignedInteger i = 0; i < iMax; ++i )
  {
    Bool evenLower = true;
    NumericalPoint c(b);
    for( UnsignedInteger j = 0; j < dimension_; ++j )
    {
      const UnsignedInteger mask = 1 << j;
      if (i & mask)
      {
        c[j] = a[j];
        evenLower = (!evenLower);
      }
    } // j
    const NumericalScalar cdf = computeCDF(c);
    probability += (evenLower ? cdf : -cdf);
  } // i
  return probability;
}


/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
NumericalComplex DistributionImplementation::computeCharacteristicFunction(const NumericalScalar x) const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "Error:  cannot use the computeCharacteristicFunction method with distributions of dimension > 1";
  if (x == 0.0) return 1.0;
  NumericalComplex value(0.0);
  // In the continuous case, we use simple gauss integration with a fixed number of integration points. We divide the interval in order to have a sufficient number of integration points by interval. It is good for low to moderate value of x, but is prohibitive for large x. In this case, we use Filon's method with linear interpolation, it means the modified trapezoidal rule as in E. O. Tuck, 'A simple "Filon-Trapezoidal" Rule'
  if (isContinuous())
  {
    const UnsignedInteger N = ResourceMap::GetAsUnsignedInteger("Distribution-CharacteristicFunctionNMax");
    // The circular function will have x(b-a)/2\pi arches over [a, b], so we need a number of points of this order, we decide to take 8 points per arch
    NumericalPoint legendreWeights;
    const NumericalPoint legendreNodes(getGaussNodesAndWeights(legendreWeights));
    // How many sub-intervals?
    // nPts = 8*x(b-a)/2\pi => (b-a)/2 = nPts * \pi / (8*x)
    const NumericalScalar xMin = range_.getLowerBound()[0];
    const NumericalScalar xMax = range_.getUpperBound()[0];
    const NumericalScalar delta = xMax - xMin;
    const UnsignedInteger intervalsNumber = std::max(1, static_cast<int>(round(2 * x * delta / integrationNodesNumber_)));
    if (intervalsNumber * integrationNodesNumber_ < N)
    {
      const NumericalScalar halfLength = 0.5 * delta / intervalsNumber;
      for (UnsignedInteger n = 0; n < intervalsNumber; ++n)
      {
        const NumericalScalar a = xMin + 2.0 * n * halfLength;
        for (UnsignedInteger i = 0; i < integrationNodesNumber_; ++i)
        {
          const NumericalScalar xi = a + (1.0 + legendreNodes[i]) * halfLength;
          value += legendreWeights[i] * computePDF(xi) * std::exp(NumericalComplex(0.0, x * xi));
        }
      }
      // We factor out the scaling as all the sub intervals have the same length
      value *= halfLength;
    }
    else
    {
      const NumericalScalar a = range_.getLowerBound()[0];
      const NumericalScalar b = range_.getUpperBound()[0];
      const NumericalScalar T = 0.5 * (b - a);
      const NumericalScalar c = 0.5 * (a + b);
      const NumericalScalar dt = T / N;
      if (!isInitializedCF_)
      {
        //              const UnsignedInteger nMax(ResourceMap::GetAsUnsignedLong("Distribution-CharacteristicFunctionNMax"));
        NumericalSample locations(Box(Indices(1, 2 * N - 1)).generate());
        locations *= NumericalPoint(1, b - a);
        locations += NumericalPoint(1, a);
        pdfGrid_ = computePDF(locations).getImplementation()->getData();
        isInitializedCF_ = true;
      }
      const NumericalScalar omegaDt = x * dt;
      const NumericalScalar omegaDt2 = omegaDt * omegaDt;
      const NumericalScalar cosOmegaDt = std::cos(omegaDt);
      const NumericalScalar sinOmegaDt = std::sin(omegaDt);
      // The bound 4.3556e-4 is such that we get full double precision
      const NumericalComplex wM(std::abs(omegaDt) < 4.3556e-4 ? NumericalComplex(0.5 - omegaDt2 / 24.0, omegaDt / 6.0 * (1.0 - omegaDt2 / 40.0)) : NumericalComplex((1.0 - cosOmegaDt) / omegaDt2, (omegaDt - sinOmegaDt) / omegaDt2));
      const NumericalComplex wP(std::abs(omegaDt) < 4.3556e-4 ? NumericalComplex(0.5 - omegaDt2 / 24.0, -omegaDt / 6.0 * (1.0 - omegaDt2 / 40.0)) : NumericalComplex((1.0 - cosOmegaDt) / omegaDt2, (-omegaDt + sinOmegaDt) / omegaDt2));
      const NumericalScalar cosNOmegaDt = std::cos(N * omegaDt);
      const NumericalScalar sinNOmegaDt = std::sin(N * omegaDt);
      // The bound 4.3556e-4 is such that we get full double precision
      const NumericalScalar w = std::abs(omegaDt) < 4.3556e-4 ? std::pow(std::sin(0.5 * omegaDt) / (0.5 * omegaDt), 2) : 1.0 - omegaDt2 / 12.0;
      //      value = pdfGrid_[N] * w + pdfGrid_[0] * wM * NumericalComplex(cosNOmegaDt, -sinNOmegaDt) + pdfGrid_[2 * N] * wP * NumericalComplex(cosNOmegaDt, sinNOmegaDt);
      value = pdfGrid_[0] * wM * NumericalComplex(cosNOmegaDt, -sinNOmegaDt) + pdfGrid_[2 * N - 1] * wP * NumericalComplex(cosNOmegaDt, sinNOmegaDt);
      for (UnsignedInteger n = 1; n < N; ++n)
      {
        const NumericalScalar cosN = std::cos(n * omegaDt);
        const NumericalScalar sinN = std::sin(n * omegaDt);
        value += NumericalComplex(w * cosN * (pdfGrid_[N + n - 1] + pdfGrid_[N - n]), w * sinN * (pdfGrid_[N + n - 1] - pdfGrid_[N - n]));
      }
      return dt * value * NumericalComplex(std::cos(x * c), std::sin(x * c));
    }
  } // Continuous
  else
  {
    // Discrete
    // In the discrete case, we have a reasonably efficient algorithm both in term of speed and precision.
    if (isDiscrete())
    {
      const NumericalSample support(getSupport());
      const UnsignedInteger size = support.getSize();
      for (UnsignedInteger i = 0; i < size; ++i)
      {
        const NumericalScalar pt = support[i][0];
        value += computePDF(pt) * std::exp(NumericalComplex(0.0, x * pt));
      }
    }
    // In the composite case, no default algorithm
    else
    {
      throw NotYetImplementedException(HERE) << "In DistributionImplementation::computeCharacteristicFunction(const NumericalScalar x) const: no default algorithm to compute the characteristic function in the composite case.";
    }
  }
  return value;
}

NumericalComplex DistributionImplementation::computeCharacteristicFunction(const NumericalPoint & x) const
{
  if (dimension_ == 1) return computeCharacteristicFunction(x[0]);
  throw NotYetImplementedException(HERE) << "In DistributionImplementation::computeCharacteristicFunction(const NumericalPoint & x) const";
}

NumericalComplex DistributionImplementation::computeLogCharacteristicFunction(const NumericalScalar x) const
{
  const NumericalComplex value(computeCharacteristicFunction(x));
  const NumericalComplex result(std::log(value));
  return result;
}

NumericalComplex DistributionImplementation::computeLogCharacteristicFunction(const NumericalPoint & x) const
{
  if (dimension_ == 1) return computeLogCharacteristicFunction(x[0]);
  throw NotYetImplementedException(HERE) << "In DistributionImplementation::computeLogCharacteristicFunction(const NumericalPoint & x) const";
}

NumericalComplex DistributionImplementation::computeCharacteristicFunction(const UnsignedInteger index,
    const NumericalScalar step) const
{
  return computeCharacteristicFunction(index * step);
}

NumericalComplex DistributionImplementation::computeLogCharacteristicFunction(const UnsignedInteger index,
    const NumericalScalar step) const
{
  return computeLogCharacteristicFunction(index * step);
}

NumericalComplex DistributionImplementation::computeCharacteristicFunction(const Indices & indices,
    const NumericalPoint & step) const
{
  NumericalPoint point(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i) point[i] = indices[i] * step[i];
  return computeCharacteristicFunction(point);
}

NumericalComplex DistributionImplementation::computeLogCharacteristicFunction(const Indices & indices,
    const NumericalPoint & step) const
{
  NumericalPoint point(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i) point[i] = indices[i] * step[i];
  return computeLogCharacteristicFunction(point);
}

/* Get the generating function of the distribution, i.e. psi(z) = E(z^X) */
NumericalScalar DistributionImplementation::computeGeneratingFunction(const NumericalScalar z) const
{
  return computeGeneratingFunction(NumericalComplex(z, 0.0)).real();
}

NumericalScalar DistributionImplementation::computeLogGeneratingFunction(const NumericalScalar z) const
{
  return computeLogGeneratingFunction(NumericalComplex(z, 0.0)).real();
}

NumericalComplex DistributionImplementation::computeGeneratingFunction(const NumericalComplex & z) const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "Error:  cannot use the computeCharacteristicFunction method with distributions of dimension > 1";
  if (!isDiscrete()) throw NotDefinedException(HERE) << "Error: cannot compute the generating function for non discrete distributions.";
  const NumericalSample support(getSupport());
  const UnsignedInteger size = support.getSize();
  NumericalComplex value(0.0);
  // If the distribution is integral, the generating function is either a polynomial if the support is finite, or can be well approximated by such a polynomial
  if (isAlreadyCreatedGeneratingFunction_) value = generatingFunction_(z);
  else
    // If isIntegral, then we have to create the generating function as a polynomial
    if (isIntegral())
    {
      NumericalPoint coefficients(size);
      for (UnsignedInteger i = 0; i < size; ++i) coefficients[i] = computePDF(support[i]);
      generatingFunction_ = UniVariatePolynomial(coefficients);
      isAlreadyCreatedGeneratingFunction_ = true;
      value = generatingFunction_(z);
    }
  // The distribution is discrete but not integral
    else
    {
      for (UnsignedInteger i = 0; i < size; ++i)
      {
        const NumericalScalar pt = support[i][0];
        value += computePDF(pt) * std::pow(z, pt);
      }
    }
  return value;
}

NumericalComplex DistributionImplementation::computeLogGeneratingFunction(const NumericalComplex & z) const
{
  NumericalComplex value = computeGeneratingFunction(z);
  return std::log(value);
}

/* Get the DDF of the distributionImplementation */
NumericalSample DistributionImplementation::computeDDFSequential(const NumericalSample & inSample) const
{
  const UnsignedInteger size = inSample.getSize();
  NumericalSample outSample(size, 1);
  for (UnsignedInteger i = 0; i < size; ++i) outSample[i] = computeDDF(inSample[i]);
  return outSample;
}

/* Get the DDF of the distributionImplementation */
struct ComputeDDFPolicy
{
  const NumericalSample & input_;
  NumericalSample & output_;
  const DistributionImplementation & distribution_;

  ComputeDDFPolicy( const NumericalSample & input,
                    NumericalSample & output,
                    const DistributionImplementation & distribution)
    : input_(input)
    , output_(output)
    , distribution_(distribution)
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i) output_[i] = distribution_.computeDDF(input_[i]);
  }

}; /* end struct ComputeDDFPolicy */

NumericalSample DistributionImplementation::computeDDFParallel(const NumericalSample & inSample) const
{
  if (inSample.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << dimension_ << ", got " << inSample.getDimension();
  const UnsignedInteger size = inSample.getSize();
  NumericalSample result(size, 1);
  const ComputeDDFPolicy policy( inSample, result, *this );
  TBB::ParallelFor( 0, size, policy );
  return result;
}

NumericalSample DistributionImplementation::computeDDF(const NumericalSample & inSample) const
{
  if (inSample.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << dimension_ << ", got " << inSample.getDimension();
  if (isParallel_) return computeDDFParallel(inSample);
  else return computeDDFSequential(inSample);
}

/* Get the PDF of the distributionImplementation */
NumericalSample DistributionImplementation::computePDFSequential(const NumericalSample & inSample) const
{
  const UnsignedInteger size = inSample.getSize();
  NumericalSample outSample(size, 1);
  for (UnsignedInteger i = 0; i < size; ++i) outSample[i][0] = computePDF(inSample[i]);
  return outSample;
}

/* Get the PDF of the distributionImplementation */
struct ComputePDFPolicy
{
  const NumericalSample & input_;
  NumericalSample & output_;
  const DistributionImplementation & distribution_;

  ComputePDFPolicy( const NumericalSample & input,
                    NumericalSample & output,
                    const DistributionImplementation & distribution)
    : input_(input)
    , output_(output)
    , distribution_(distribution)
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i) output_[i][0] = distribution_.computePDF(input_[i]);
  }

}; /* end struct ComputePDFPolicy */

NumericalSample DistributionImplementation::computePDFParallel(const NumericalSample & inSample) const
{
  if (inSample.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << dimension_ << ", got " << inSample.getDimension();
  const UnsignedInteger size = inSample.getSize();
  NumericalSample result(size, 1);
  const ComputePDFPolicy policy( inSample, result, *this );
  TBB::ParallelFor( 0, size, policy );
  return result;
}

NumericalSample DistributionImplementation::computePDF(const NumericalSample & inSample) const
{
  if (inSample.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << dimension_ << ", got " << inSample.getDimension();
  if (isParallel_) return computePDFParallel(inSample);
  else return computePDFSequential(inSample);
}

/* Get the Log PDF of the distributionImplementation */
NumericalSample DistributionImplementation::computeLogPDFSequential(const NumericalSample & inSample) const
{
  const UnsignedInteger size = inSample.getSize();
  NumericalSample outSample(size, 1);
  for (UnsignedInteger i = 0; i < size; ++i) outSample[i][0] = computeLogPDF(inSample[i]);
  return outSample;
}

/* Get the LogPDF of the distributionImplementation */
struct ComputeLogPDFPolicy
{
  const NumericalSample & input_;
  NumericalSample & output_;
  const DistributionImplementation & distribution_;

  ComputeLogPDFPolicy( const NumericalSample & input,
                       NumericalSample & output,
                       const DistributionImplementation & distribution)
    : input_(input)
    , output_(output)
    , distribution_(distribution)
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i) output_[i][0] = distribution_.computeLogPDF(input_[i]);
  }

}; /* end struct ComputeLogPDFPolicy */

NumericalSample DistributionImplementation::computeLogPDFParallel(const NumericalSample & inSample) const
{
  if (inSample.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << dimension_ << ", got " << inSample.getDimension();
  const UnsignedInteger size = inSample.getSize();
  NumericalSample result(size, 1);
  const ComputeLogPDFPolicy policy( inSample, result, *this );
  TBB::ParallelFor( 0, size, policy );
  return result;
}

NumericalSample DistributionImplementation::computeLogPDF(const NumericalSample & inSample) const
{
  if (inSample.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << dimension_ << ", got " << inSample.getDimension();
  if (isParallel_) return computeLogPDFParallel(inSample);
  else return computeLogPDFSequential(inSample);
}

/* Get the DDF of the distributionImplementation */
NumericalScalar DistributionImplementation::computeDDF(const NumericalScalar scalar) const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "ERROR: cannot use the simplified interface of computeDDF with distributions of dimension > 1";
  return computeDDF(NumericalPoint(1, scalar))[0];
}

/* Get the PDF of the distributionImplementation */
NumericalScalar DistributionImplementation::computePDF(const NumericalScalar scalar) const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "ERROR: cannot use the simplified interface of computePDF with distributions of dimension > 1";
  return computePDF(NumericalPoint(1, scalar));
}

NumericalScalar DistributionImplementation::computeLogPDF(const NumericalScalar scalar) const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "ERROR: cannot use the simplified interface of computeLogPDF with distributions of dimension > 1";
  return computeLogPDF(NumericalPoint(1, scalar));
}

/* Get the CDF of the distributionImplementation */
NumericalScalar DistributionImplementation::computeCDF(const NumericalScalar scalar) const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "ERROR: cannot use the simplified interface of computeCDF with distributions of dimension > 1";
  return computeCDF(NumericalPoint(1, scalar));
}

NumericalScalar DistributionImplementation::computeComplementaryCDF(const NumericalScalar scalar) const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "ERROR: cannot use the simplified interface of computeComplementaryCDF with distributions of dimension > 1";
  return computeComplementaryCDF(NumericalPoint(1, scalar));
}

NumericalScalar DistributionImplementation::computeSurvivalFunction(const NumericalScalar scalar) const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "ERROR: cannot use the simplified interface of computeSurvivalFunction with distributions of dimension > 1";
  return computeSurvivalFunction(NumericalPoint(1, scalar));
}

/* Compute the PDF of 1D distributions over a regular grid */
NumericalSample DistributionImplementation::computePDF(const NumericalScalar xMin,
    const NumericalScalar xMax,
    const UnsignedInteger pointNumber,
    NumericalSample & grid) const
{
  return computePDF(NumericalPoint(1, xMin), NumericalPoint(1, xMax), Indices(1, pointNumber), grid);
}

/* Compute the PDF of nD distributions over a regular grid */
NumericalSample DistributionImplementation::computePDF(const NumericalPoint & xMin,
    const NumericalPoint & xMax,
    const Indices & pointNumber,
    NumericalSample & grid) const
{
  if (xMin.getDimension() != xMax.getDimension()) throw InvalidArgumentException(HERE) << "Error: the two corner points must have the same dimension. Here, dim(xMin)=" << xMin.getDimension() << " and dim(xMax)=" << xMax.getDimension();
  if (xMin.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the corner points must have the same dimension as the distribution. Here, dim(xMin)=" << xMin.getDimension() << " and distribution dimension=" << dimension_;
  if (dimension_ != pointNumber.getSize()) throw InvalidArgumentException(HERE) << "Error: the discretization must match the distribution dimension. Here, dim(discretization)=" << pointNumber.getSize() << " and distribution dimension=" << dimension_;
  Tuples::IndicesCollection indices(Tuples(pointNumber).generate());
  const UnsignedInteger size = indices.getSize();
  NumericalSample inputSample(indices.getSize(), dimension_);
  for (UnsignedInteger i = 0; i < size; ++i)
    for (UnsignedInteger j = 0; j < dimension_; ++j) inputSample[i][j] = xMin[j] + indices[i][j] * (xMax[j] - xMin[j]) / (pointNumber[j] - 1.0);
  grid = inputSample;
  return computePDF(inputSample);
}

/* Compute the log-PDF of 1D distributions over a regular grid */
NumericalSample DistributionImplementation::computeLogPDF(const NumericalScalar xMin,
    const NumericalScalar xMax,
    const UnsignedInteger pointNumber,
    NumericalSample & grid) const
{
  return computeLogPDF(NumericalPoint(1, xMin), NumericalPoint(1, xMax), Indices(1, pointNumber), grid);
}

/* Compute the log-PDF of nD distributions over a regular grid */
NumericalSample DistributionImplementation::computeLogPDF(const NumericalPoint & xMin,
    const NumericalPoint & xMax,
    const Indices & pointNumber,
    NumericalSample & grid) const
{
  if (xMin.getDimension() != xMax.getDimension()) throw InvalidArgumentException(HERE) << "Error: the two corner points must have the same dimension. Here, dim(xMin)=" << xMin.getDimension() << " and dim(xMax)=" << xMax.getDimension();
  if (xMin.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the corner points must have the same dimension as the distribution. Here, dim(xMin)=" << xMin.getDimension() << " and distribution dimension=" << dimension_;
  if (dimension_ != pointNumber.getSize()) throw InvalidArgumentException(HERE) << "Error: the discretization must match the distribution dimension. Here, dim(discretization)=" << pointNumber.getSize() << " and distribution dimension=" << dimension_;
  Tuples::IndicesCollection indices(Tuples(pointNumber).generate());
  const UnsignedInteger size = indices.getSize();
  NumericalSample inputSample(indices.getSize(), dimension_);
  for (UnsignedInteger i = 0; i < size; ++i)
    for (UnsignedInteger j = 0; j < dimension_; ++j) inputSample[i][j] = xMin[j] + indices[i][j] * (xMax[j] - xMin[j]) / (pointNumber[j] - 1.0);
  grid = inputSample;
  return computeLogPDF(inputSample);
}

/* Compute the CDF of 1D distributions over a regular grid */
NumericalSample DistributionImplementation::computeCDF(const NumericalScalar xMin,
    const NumericalScalar xMax,
    const UnsignedInteger pointNumber,
    NumericalSample & grid) const
{
  return computeCDF(NumericalPoint(1, xMin), NumericalPoint(1, xMax), Indices(1, pointNumber), grid);
}

/* Compute the CDF of nD distributions over a regular grid */
NumericalSample DistributionImplementation::computeCDF(const NumericalPoint & xMin,
    const NumericalPoint & xMax,
    const Indices & pointNumber,
    NumericalSample & grid) const
{
  if (xMin.getDimension() != xMax.getDimension()) throw InvalidArgumentException(HERE) << "Error: the two corner points must have the same dimension. Here, dim(xMin)=" << xMin.getDimension() << " and dim(xMax)=" << xMax.getDimension();
  if (xMin.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the corner points must have the same dimension as the distribution. Here, dim(xMin)=" << xMin.getDimension() << " and distribution dimension=" << dimension_;
  if (dimension_ != pointNumber.getSize()) throw InvalidArgumentException(HERE) << "Error: the discretization must match the distribution dimension. Here, dim(discretization)=" << pointNumber.getSize() << " and distribution dimension=" << dimension_;
  Tuples::IndicesCollection indices(Tuples(pointNumber).generate());
  const UnsignedInteger size = indices.getSize();
  NumericalSample inputSample(indices.getSize(), dimension_);
  for (UnsignedInteger i = 0; i < size; ++i)
    for (UnsignedInteger j = 0; j < dimension_; ++j) inputSample[i][j] = xMin[j] + indices[i][j] * (xMax[j] - xMin[j]) / (pointNumber[j] - 1.0);
  grid = inputSample;
  return computeCDF(inputSample);
}

NumericalSample DistributionImplementation::computeComplementaryCDF(const NumericalScalar xMin,
    const NumericalScalar xMax,
    const UnsignedInteger pointNumber,
    NumericalSample & grid) const
{
  if (dimension_ != 1) throw InvalidArgumentException(HERE) << "Error: cannot compute the CDF over a regular 1D grid if the dimension is > 1";
  NumericalSample result(pointNumber, 2);
  NumericalScalar x = xMin;
  NumericalScalar step = (xMax - xMin) / NumericalScalar(pointNumber - 1.0);
  grid = NumericalSample(pointNumber, 1);
  for (UnsignedInteger i = 0; i < pointNumber; ++i)
  {
    grid[i][0] = x;
    result[i][0] = x;
    result[i][1] = computeComplementaryCDF(x);
    x += step;
  }
  return result;
}

/*  Compute the quantile over a regular grid */
NumericalSample DistributionImplementation::computeQuantile(const NumericalScalar qMin,
    const NumericalScalar qMax,
    const UnsignedInteger pointNumber,
    const Bool tail) const
{
  NumericalSample grid;
  return computeQuantile(qMin, qMax, pointNumber, grid, tail);
}

NumericalSample DistributionImplementation::computeQuantile(const NumericalScalar qMin,
    const NumericalScalar qMax,
    const UnsignedInteger pointNumber,
    NumericalSample & grid,
    const Bool tail) const
{
  // First, build the regular grid for the quantile levels
  grid = NumericalSample(pointNumber, 1);
  for (UnsignedInteger i = 0; i < pointNumber; ++i) grid[i][0] = qMin + i * (qMax - qMin) / (pointNumber - 1.0);
  // Use possible parallelization
  return computeQuantile(grid.getImplementation()->getData(), tail);
}

/* Compute the quantile over a provided grid */
NumericalSample DistributionImplementation::computeQuantileSequential(const NumericalPoint & prob,
    const Bool tail) const
{
  const UnsignedInteger size = prob.getSize();
  NumericalSample result(size, dimension_);
  for ( UnsignedInteger i = 0; i < size; ++ i ) result[i] = computeQuantile(prob[i], tail);
  return result;
}

struct ComputeQuantilePolicy
{
  const NumericalPoint & prob_;
  NumericalSample & output_;
  Bool tail_;
  const DistributionImplementation & distribution_;

  ComputeQuantilePolicy( const NumericalPoint & prob,
                         NumericalSample & output,
                         const Bool tail,
                         const DistributionImplementation & distribution)
    : prob_(prob)
    , output_(output)
    , tail_(tail)
    , distribution_(distribution)
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i) output_[i] = distribution_.computeQuantile(prob_[i], tail_);
  }

}; /* end struct ComputeQuantilePolicy */

NumericalSample DistributionImplementation::computeQuantileParallel(const NumericalPoint & prob,
    const Bool tail) const
{
  const UnsignedInteger size = prob.getSize();
  NumericalSample result(size, dimension_);
  const ComputeQuantilePolicy policy( prob, result, tail, *this );
  TBB::ParallelFor( 0, size, policy );
  return result;
}

NumericalSample DistributionImplementation::computeQuantile(const NumericalPoint & prob,
    const Bool tail) const
{
  if (isParallel_) return computeQuantileParallel(prob, tail);
  else return computeQuantileSequential(prob, tail);
}

/* Get the PDF gradient of the distribution */
NumericalPoint DistributionImplementation::computePDFGradient(const NumericalPoint & point) const
{
  if (dimension_ > 1) throw NotYetImplementedException(HERE) << "DistributionImplementation::computePDFGradient(const NumericalPoint & point) const";
  // As we are in 1D, we know that the collection contains exactly one point
  const NumericalPoint initialParameters(getParameter());
  const UnsignedInteger parametersDimension = initialParameters.getDimension();
  NumericalPoint PDFGradient(parametersDimension);
  // Clone the distribution
  Implementation cloneDistribution(clone());
  // Increment for centered differences
  const NumericalScalar eps = std::pow(ResourceMap::GetAsNumericalScalar("DistFunc-Precision"), 1.0 / 3.0);
  // Increment for noncentered differences
  const NumericalScalar eps2 = std::pow(ResourceMap::GetAsNumericalScalar("DistFunc-Precision"), 1.0 / 2.0);
  NumericalPoint newParameters(initialParameters);
  for (UnsignedInteger i = 0; i < parametersDimension; ++i)
  {
    NumericalScalar delta = 0.0;
    NumericalScalar rightPDF = 0.0;
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
    NumericalScalar leftPDF = 0.0;
    try
    {
      // If something is wrong with the right point, use non-centered finite differences
      const NumericalScalar leftEpsilon = delta == 0.0 ? eps2 : eps;
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

/* ComputePDFGradient On a NumericalSample */
NumericalSample DistributionImplementation::computePDFGradient(const NumericalSample & inSample) const
{
  const UnsignedInteger size = inSample.getSize();
  NumericalSample outSample(size, getParameterDimension());
  for (UnsignedInteger i = 0; i < size; ++i) outSample[i] = computePDFGradient(inSample[i]);
  return outSample;
}

/* ComputeCDFGradient On a NumericalSample */
NumericalSample DistributionImplementation::computeCDFGradient(const NumericalSample & inSample) const
{
  const UnsignedInteger size = inSample.getSize();
  NumericalSample outSample(size, getParameterDimension());
  for (UnsignedInteger i = 0; i < size; ++i) outSample[i] = computeCDFGradient(inSample[i]);
  return outSample;
}

/* Get the CDF gradient of the distribution */
NumericalPoint DistributionImplementation::computeCDFGradient(const NumericalPoint & point) const
{
  if (dimension_ > 1) throw NotYetImplementedException(HERE) << "In DistributionImplementation::computeCDFGradient(const NumericalPoint & point) const";
  // As we are in 1D, we know that the collection contains exactly one point
  const NumericalPoint initialParameters(getParameter());
  const UnsignedInteger parametersDimension = initialParameters.getDimension();
  NumericalPoint CDFGradient(parametersDimension);
  // Clone the distribution
  Implementation cloneDistribution(clone());
  // We will use centered differences
  const NumericalScalar eps = std::pow(ResourceMap::GetAsNumericalScalar("DistFunc-Precision"), 1.0 / 3.0);
  // Increment for noncentered differences
  const NumericalScalar eps2 = std::pow(ResourceMap::GetAsNumericalScalar("DistFunc-Precision"), 1.0 / 2.0);
  NumericalPoint newParameters(initialParameters);
  for (UnsignedInteger i = 0; i < parametersDimension; ++i)
  {
    NumericalScalar delta = 0.0;
    NumericalScalar rightCDF = 0.0;
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
    NumericalScalar leftCDF = 0.0;
    try
    {
      // If something is wrong with the right point, use non-centered finite differences
      const NumericalScalar leftEpsilon = delta == 0.0 ? eps2 : eps;
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
Collection<PiecewiseHermiteEvaluationImplementation> DistributionImplementation::interpolatePDFCDF(const UnsignedInteger n)
{
  if (!isContinuous()) throw InternalException(HERE) << "Error: cannot interpolate the PDF and CDF of noncontinuous distributions.";
  if (dimension_ != 1) throw NotYetImplementedException(HERE) << "In DistributionImplementation::interpolatePDFCDF(const UnsignedInteger n): cannot interpolate CDF for multidimensional distributions.";
  const NumericalScalar xMin = getRange().getLowerBound()[0];
  const NumericalScalar xMax = getRange().getUpperBound()[0];
  const NumericalScalar mu = getMean()[0];
  // Here we use an absolute precision of 0.0 in order to force the algorithm to use all the available discretization points
  GaussKronrod algorithm(n - 1, cdfEpsilon_ * cdfEpsilon_, GaussKronrodRule::G3K7);
  const PDFWrapper pdfWrapper(this);
  NumericalScalar error = -1.0;
  NumericalPoint ai;
  NumericalPoint bi;
  NumericalSample fi;
  NumericalPoint ei;
  algorithm.integrate(pdfWrapper, xMin, mu, error, ai, bi, fi, ei);
  ai.add(mu);
  NumericalSample locationsCDF(ai.getSize(), 1);
  locationsCDF.getImplementation()->setData(ai);
  locationsCDF = locationsCDF.sort(0);
  algorithm.integrate(pdfWrapper, mu, xMax, error, ai, bi, fi, ei);
  ai.add(xMax);
  NumericalSample locationsCCDF(ai.getSize(), 1);
  locationsCCDF.getImplementation()->setData(ai);
  locationsCCDF = locationsCCDF.sort(0);
  Collection<PiecewiseHermiteEvaluationImplementation> coll(4);
  const NumericalSample valuesCDF(computeCDF(locationsCDF));
  const NumericalSample valuesPDF(computePDF(locationsCDF));
  const NumericalSample valuesDDF(computeDDF(locationsCDF));
  coll[0] = PiecewiseHermiteEvaluationImplementation(locationsCDF.getImplementation()->getData(), valuesPDF, valuesDDF);
  coll[1] = PiecewiseHermiteEvaluationImplementation(locationsCDF.getImplementation()->getData(), valuesCDF, valuesPDF);
  const NumericalSample valuesCCDF(computeComplementaryCDF(locationsCCDF));
  const NumericalSample valuesCPDF(computePDF(locationsCCDF));
  NumericalSample derivativesCCDF(valuesCPDF);
  derivativesCCDF *= NumericalPoint(1, -1.0);
  const NumericalSample valuesCDDF(computeDDF(locationsCCDF));
  coll[2] = PiecewiseHermiteEvaluationImplementation(locationsCCDF.getImplementation()->getData(), valuesCPDF, valuesCDDF);
  coll[3] = PiecewiseHermiteEvaluationImplementation(locationsCCDF.getImplementation()->getData(), valuesCCDF, derivativesCCDF);
  return coll;
}

/* Compute the DDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
NumericalScalar DistributionImplementation::computeConditionalDDF(const NumericalScalar x,
    const NumericalPoint & y) const
{
  throw NotYetImplementedException(HERE) << "In DistributionImplementation::computeConditionalDDF(const NumericalScalar x, const NumericalPoint & y) const";
}

/* Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
NumericalScalar DistributionImplementation::computeConditionalPDF(const NumericalScalar x,
    const NumericalPoint & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= dimension_) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional PDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula())) return getMarginal(conditioningDimension)->computePDF(x);
  // General case
  Indices conditioning(conditioningDimension);
  conditioning.fill();
  Indices conditioned(conditioning);
  conditioned.add(conditioningDimension);
  const Implementation conditioningDistribution(getMarginal(conditioning));
  const NumericalScalar pdfConditioning = conditioningDistribution->computePDF(y);
  if (pdfConditioning <= 0.0) return 0.0;
  NumericalPoint z(y);
  z.add(x);
  const Implementation conditionedDistribution(getMarginal(conditioned));
  const NumericalScalar pdfConditioned = conditionedDistribution->computePDF(z);
  pdfEpsilon_ = conditionedDistribution->getPDFEpsilon() + conditioningDistribution->getPDFEpsilon();
  return pdfConditioned / pdfConditioning;
}

/* Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
NumericalPoint DistributionImplementation::computeConditionalPDF(const NumericalPoint & x,
    const NumericalSample & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= dimension_) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional PDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Convert the values in x into a NumericalSample
  const UnsignedInteger size = x.getDimension();
  NumericalSampleImplementation xAsSample(size, 1);
  xAsSample.setData(x);
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula()))
    return getMarginal(conditioningDimension)->computePDF(xAsSample).getImplementation()->getData();
  // General case
  Indices conditioning(conditioningDimension);
  conditioning.fill();
  Indices conditioned(conditioning);
  conditioned.add(conditioningDimension);
  const Implementation conditioningDistribution(getMarginal(conditioning));
  const NumericalSample pdfConditioning(conditioningDistribution->computePDF(y));
  NumericalSample z(y);
  z.stack(xAsSample);
  const Implementation conditionedDistribution(getMarginal(conditioned));
  const NumericalSample pdfConditioned(conditionedDistribution->computePDF(z));
  NumericalPoint result(size);
  for (UnsignedInteger i = 0; i < size; ++i)
    if (pdfConditioning[i][0] > 0.0) result[i] = pdfConditioned[i][0] / pdfConditioning[i][0];
  return result;
}

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
NumericalScalar DistributionImplementation::computeConditionalCDF(const NumericalScalar x,
    const NumericalPoint & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= dimension_) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional CDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula())) return getMarginal(conditioningDimension)->computeCDF(x);
  // General case
  Indices conditioning(conditioningDimension);
  conditioning.fill();
  Indices conditioned(conditioning);
  conditioned.add(conditioningDimension);
  const Implementation conditioningDistribution(getMarginal(conditioning));
  const NumericalScalar pdfConditioning = conditioningDistribution->computePDF(y);
  if (pdfConditioning <= 0.0) return 0.0;
  const Implementation conditionedDistribution(getMarginal(conditioned));
  const NumericalScalar xMin = conditionedDistribution->getRange().getLowerBound()[conditioningDimension];
  if (x <= xMin) return 0.0;
  const NumericalScalar xMax = conditionedDistribution->getRange().getUpperBound()[conditioningDimension];
  if (x >= xMax) return 1.0;
  // Numerical integration with respect to x
  // Here we recreate a ConditionalPDFWrapper only if none has been created or if the parameter dimension has changed
  if (p_conditionalPDFWrapper_.isNull() || (p_conditionalPDFWrapper_->getParameter().getDimension() != y.getDimension())) p_conditionalPDFWrapper_ = new ConditionalPDFWrapper(conditionedDistribution);
  p_conditionalPDFWrapper_->setParameter(y);
  GaussKronrod algo;
  const NumericalPoint value(algo.integrate(p_conditionalPDFWrapper_, Interval(xMin, x)));
  return std::min(1.0, std::max(0.0, value[0] / pdfConditioning));
}

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
NumericalPoint DistributionImplementation::computeConditionalCDF(const NumericalPoint & x,
    const NumericalSample & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= dimension_) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional CDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Convert the values in x into a NumericalSample
  const UnsignedInteger size = x.getDimension();
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula()))
  {
    NumericalSampleImplementation xAsSample(size, 1);
    xAsSample.setData(x);
    return getMarginal(conditioningDimension)->computeCDF(xAsSample).getImplementation()->getData();
  }
  // General case
  Indices conditioning(conditioningDimension);
  conditioning.fill();
  Indices conditioned(conditioning);
  conditioned.add(conditioningDimension);
  const Implementation conditioningDistribution(getMarginal(conditioning));
  const NumericalSample pdfConditioning(conditioningDistribution->computePDF(y));
  const Implementation conditionedDistribution(getMarginal(conditioned));
  const NumericalScalar xMin = conditionedDistribution->getRange().getLowerBound()[conditioningDimension];
  const NumericalScalar xMax = conditionedDistribution->getRange().getUpperBound()[conditioningDimension];
  NumericalPoint result(size);
  // Here we recreate a ConditionalPDFWrapper only if none has been created or if the parameter dimension has changed
  if (p_conditionalPDFWrapper_.isNull() || (p_conditionalPDFWrapper_->getParameter().getDimension() != y.getDimension())) p_conditionalPDFWrapper_ = new ConditionalPDFWrapper(conditionedDistribution);
  GaussKronrod algo;
  for (UnsignedInteger i = 0; i < size; ++i)
    if (pdfConditioning[i][0] > 0.0)
    {
      if (x[i] >= xMax) result[i] = 1.0;
      else if (x[i] > xMin)
      {
        // Numerical integration with respect to x
        p_conditionalPDFWrapper_->setParameter(y[i]);
        const NumericalPoint value(algo.integrate(p_conditionalPDFWrapper_, Interval(xMin, x[i])));
        result[i] = std::min(1.0, std::max(0.0, value[0] / pdfConditioning[i][0]));
      } // xMin < x < xMax
    } // pdfConditioning[i][0] > 0
  return result;
}

/* Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
NumericalScalar DistributionImplementation::computeConditionalQuantile(const NumericalScalar q,
    const NumericalPoint & y) const
{
  return computeConditionalQuantile(NumericalPoint(1, q), NumericalSample(1, y))[0];
}

/* Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
NumericalPoint DistributionImplementation::computeConditionalQuantile(const NumericalPoint & q,
    const NumericalSample & y) const
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
    return getMarginal(conditioningDimension)->computeQuantile(q).getImplementation()->getData();
  // General case
  const NumericalScalar xMin = range_.getLowerBound()[conditioningDimension];
  const NumericalScalar xMax = range_.getUpperBound()[conditioningDimension];
  NumericalPoint result(size);
  // Here we recreate a ConditionalCDFWrapper only if none has been created or if the parameter dimension has changed
  if (p_conditionalCDFWrapper_.isNull() || (p_conditionalCDFWrapper_->getParameter().getDimension() != y.getDimension())) p_conditionalCDFWrapper_ = new ConditionalCDFWrapper(this);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    p_conditionalCDFWrapper_->setParameter(y[i]);
    Brent solver(quantileEpsilon_, cdfEpsilon_, cdfEpsilon_, quantileIterations_);
    result[i] = solver.solve(p_conditionalCDFWrapper_, q[i], xMin, xMax, 0.0, 1.0);
  }
  return result;
}

/* Quantile computation for dimension=1 */
NumericalScalar DistributionImplementation::computeScalarQuantile(const NumericalScalar prob,
    const Bool tail) const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "Error: the method computeScalarQuantile is only defined for 1D distributions";
  // This test allows to check if one can trust the current range. If not, it means that we are here to compute the range and then we cannot rely on it!
  NumericalScalar lower = range_.getLowerBound()[0];
  NumericalScalar upper = range_.getUpperBound()[0];
  // This test allows to know if the range has already been computed. If not, it is the role of the computeScalarQuantile() to do it.
  if (lower > upper)
  {
    LOGDEBUG("DistributionImplementation::computeScalarQuantile: look for a bracketing of the bounds of the range");
    // Find a rough estimate of the lower bound and the upper bound
    NumericalScalar step = 1.0;
    NumericalScalar cdf = computeCDF(lower);
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
    NumericalScalar ccdf = computeComplementaryCDF(upper);
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
  const NumericalScalar q = tail ? 1.0 - prob : prob;
  const CDFWrapper wrapper(this);
  const NumericalMathFunction f(bindMethod<CDFWrapper, NumericalPoint, NumericalPoint>(wrapper, &CDFWrapper::computeCDF, 1, 1));
  const NumericalScalar leftTau = lower;
  const NumericalScalar leftCDF = 0.0;
  const NumericalScalar rightTau = upper;
  const NumericalScalar rightCDF = 1.0;
  Brent solver(quantileEpsilon_, cdfEpsilon_, cdfEpsilon_, quantileIterations_);
  const NumericalScalar root = solver.solve(f, q, leftTau, rightTau, leftCDF, rightCDF);
  LOGDEBUG(OSS() << "root=" << root);
  return root;
} // computeScalarQuantile

/* Generic implementation of the quantile computation */
NumericalPoint DistributionImplementation::computeQuantile(const NumericalScalar prob,
    const Bool tail) const
{
  NumericalScalar marginalProb = 0.0;
  return computeQuantile(prob, tail, marginalProb);
}

NumericalPoint DistributionImplementation::computeQuantile(const NumericalScalar prob,
							   const Bool tail,
NumericalScalar & marginalProb) const
{
  const NumericalScalar q = tail ? 1.0 - prob : prob;
  marginalProb = q;
  // Special case for bording values
  if (prob < 0.0) return (tail ? range_.getUpperBound() : range_.getLowerBound());
  if (prob >= 1.0) return (tail ? range_.getLowerBound() : range_.getUpperBound());
  // Special case for dimension 1
  if (dimension_ == 1) return NumericalPoint(1, computeScalarQuantile(prob, tail));
  // Special case for independent copula
  if (hasIndependentCopula())
    {
      NumericalPoint result(dimension_);
      marginalProb = std::pow(q, 1.0 / dimension_);
      for (UnsignedInteger i = 0; i < dimension_; ++i) result[i] = getMarginal(i)->computeScalarQuantile(marginalProb);
      return result;
    }
  // Extract the marginal distributions
  Collection<Implementation> marginals(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; i++) marginals[i] = getMarginal(i);
  // The n-D quantile is defined as X(\tau) = (F_1^{-1}(\tau), ..., F_n^{-1}(\tau)),
  // with tau such as F(X(\tau)) = q.
  // As F(x) = C(F_1(x_1),...,F_n(x_n)), the constraint F(X(\tau)) = q reads:
  // C(\tau,...,\tau) = q
  // Bracketing of \tau using the Frechet Hoeffding bounds:
  // max(n\tau - n + 1, 0) <= C(\tau,...,\tau) <= \tau
  // from which we deduce that q <= \tau and \tau <= 1 - (1 - q) / n
  // Lower bound of the bracketing interval
  const QuantileWrapper wrapper(marginals, this);
  const NumericalMathFunction f(bindMethod<QuantileWrapper, NumericalPoint, NumericalPoint>(wrapper, &QuantileWrapper::computeDiagonal, 1, 1));
  NumericalScalar leftTau = q;
  NumericalScalar leftCDF = f(NumericalPoint(1, leftTau))[0];
  // Due to numerical precision issues, the theoretical bound can be slightly violated
  if (leftCDF > prob)
  {
    leftTau = 0.0;
    leftCDF = 0.0;
  }
  // Upper bound of the bracketing interval
  NumericalScalar rightTau = 1.0 - (1.0 - q) / dimension_;
  NumericalScalar rightCDF = f(NumericalPoint(1, rightTau))[0];
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

/* Get the minimum volume interval containing at least a given probability of the distributionImplementation.
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
                               const NumericalScalar prob)
    : p_distribution_(p_distribution)
    , marginals_(marginals)
    , lastB_(-SpecFunc::MaxNumericalScalar)
    , prob_(prob)
  {
    // Nothing to do
  }

  MinimumVolumeIntervalWrapper(const DistributionImplementation * p_distribution,
                               const NumericalScalar prob)
    : p_distribution_(p_distribution)
    , marginals_(0)
    , lastB_(-SpecFunc::MaxNumericalScalar)
    , prob_(prob)
  {
    // Nothing to do
  }

  // The minimum volume interval [a, b] is such that:
  // a\in[lowerBound, F^{-1}(1-p)]
  // b = F^{-1}(p+F(a))
  // f(a) = f(b) = f(F^{-1}(p+F(a)))
  // Here we compute f(F^{-1}(p+F(a))) - f(a)
  NumericalPoint operator() (const NumericalPoint & point) const
  {
    lastB_ = p_distribution_->computeQuantile(prob_ + p_distribution_->computeCDF(point))[0];
    const NumericalScalar pdfB = p_distribution_->computePDF(lastB_);
    const NumericalScalar pdfPoint = p_distribution_->computePDF(point);
    return NumericalPoint(1, pdfB - pdfPoint);
  }

  NumericalPoint objective(const NumericalPoint & point) const
  {
    lastB_ = p_distribution_->computeQuantile(prob_ + p_distribution_->computeCDF(point))[0];
    return NumericalPoint(1, lastB_ - point[0]);    
  }

  NumericalScalar getLastB() const
  {
    return lastB_;
  }

  Interval buildBilateralInterval(const NumericalScalar beta) const
  {
    const UnsignedInteger size(marginals_.getSize());
    NumericalPoint lower(size);
    NumericalPoint upper(size);
    const NumericalScalar alpha(0.5 * (1.0 - beta));
    for (UnsignedInteger i = 0; i < size; ++i)
      {
	lower[i] = marginals_[i].computeQuantile(alpha, false)[0];
	upper[i] = marginals_[i].computeQuantile(alpha, true)[0];
      }
    return Interval(lower, upper);					 
  }

  Interval buildMinimumVolumeInterval(const NumericalScalar beta) const
  {
    const UnsignedInteger size(marginals_.getSize());
    NumericalPoint lower(size);
    NumericalPoint upper(size);
    for (UnsignedInteger i = 0; i < size; ++i)
      {
	const Interval marginalIC(marginals_[i].computeMinimumVolumeInterval(beta));
	lower[i] = marginalIC.getLowerBound()[0];
	upper[i] = marginalIC.getUpperBound()[0];
      }
    return Interval(lower, upper);					 
  }

  NumericalPoint computeBilateralProbability(const NumericalPoint & beta) const
  {
    const Interval IC(buildBilateralInterval(beta[0]));
    const NumericalScalar probability = p_distribution_->computeProbability(IC);
    return NumericalPoint(1, probability);
  }

  NumericalPoint computeMinimumVolumeProbability(const NumericalPoint & beta) const
  {
    const Interval IC(buildMinimumVolumeInterval(beta[0]));
    const NumericalScalar probability = p_distribution_->computeProbability(IC);
    return NumericalPoint(1, probability);
  }

  const DistributionImplementation * p_distribution_;
  Collection<Distribution> marginals_;
  mutable NumericalScalar lastB_;
  const NumericalScalar prob_;
}; // struct MinimumVolumeIntervalWrapper

Interval DistributionImplementation::computeMinimumVolumeInterval(const NumericalScalar prob) const
{
  NumericalScalar marginalProb = -1.0;
  return computeMinimumVolumeInterval(prob, marginalProb);
}

Interval DistributionImplementation::computeMinimumVolumeInterval(const NumericalScalar prob,
    NumericalScalar & marginalProb) const
{
  if (!isContinuous()) throw NotYetImplementedException(HERE) << "In DistributionImplementation::computeMinimumVolumeInterval()";
  // If the distribution is elliptical, the minimum volume interval is equal to the bilateral confidence interval which is much cheaper to compute
  if (isElliptical())
    {
      LOGINFO("Compute the minimum volume interval using the bilateral confidence interval (elliptical case)");
      const Interval result(computeBilateralConfidenceInterval(prob, marginalProb));
      return result;
    }
  if (prob <= 0.0)
    {
      const NumericalPoint median(computeQuantile(0.5));
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
  const NumericalMathFunction function(bindMethod<MinimumVolumeIntervalWrapper, NumericalPoint, NumericalPoint>(minimumVolumeIntervalWrapper, &MinimumVolumeIntervalWrapper::computeMinimumVolumeProbability, 1, 1));
  Brent solver(quantileEpsilon_, pdfEpsilon_, pdfEpsilon_, quantileIterations_);
  // Here the equation we have to solve is P(X\in IC(\beta))=prob
  marginalProb = solver.solve(function, prob, 0.0, 1.0, 0.0, 1.0);
  const Interval IC(minimumVolumeIntervalWrapper.buildMinimumVolumeInterval(marginalProb));
  return IC;
}

/* If the density is continuous, we have to solve PDF(b) - PDF(a) == 0 with F(b)-F(a)=prob, b>=a
   ie b=F^{-1}(prob+F(a))
*/
Interval DistributionImplementation::computeUnivariateMinimumVolumeIntervalByRootFinding(const NumericalScalar prob,
    NumericalScalar & marginalProb) const
{
  const MinimumVolumeIntervalWrapper minimumVolumeIntervalWrapper(this, prob);
  const NumericalMathFunction function(bindMethod<MinimumVolumeIntervalWrapper, NumericalPoint, NumericalPoint>(minimumVolumeIntervalWrapper, &MinimumVolumeIntervalWrapper::operator(), 1, 1));
  Brent solver(quantileEpsilon_, pdfEpsilon_, pdfEpsilon_, quantileIterations_);
  const NumericalScalar xMin = range_.getLowerBound()[0];
  const NumericalScalar xMax = computeScalarQuantile(prob, true);
  const NumericalScalar a = solver.solve(function, 0.0, xMin, xMax);
  const NumericalScalar b = minimumVolumeIntervalWrapper.getLastB();
  marginalProb = prob;
  return Interval(a, b);
}

/* We minimize b-a with the constraint F(b)-F(a)=prob, b>=a
 ie b=F^{-1}(prob+F(a))
*/
Interval DistributionImplementation::computeUnivariateMinimumVolumeIntervalByOptimization(const NumericalScalar prob,
    NumericalScalar & marginalProb) const
{
  const MinimumVolumeIntervalWrapper minimumVolumeIntervalWrapper(this, prob);
  const NumericalMathFunction objective(bindMethod<MinimumVolumeIntervalWrapper, NumericalPoint, NumericalPoint>(minimumVolumeIntervalWrapper, &MinimumVolumeIntervalWrapper::objective, 1, 1));
  OptimizationProblem problem;
  problem.setObjective(objective);
  problem.setBounds(getRange());
  TNC solver(problem);
  solver.setStartingPoint(computeQuantile(prob, true));
  solver.run();
  const NumericalScalar a = solver.getResult().getOptimalPoint()[0];
  const NumericalScalar b = minimumVolumeIntervalWrapper.getLastB();
  marginalProb = prob;
  return Interval(a, b);
}

Interval DistributionImplementation::computeMinimumVolumeInterval(const NumericalScalar prob,
    NumericalPoint & marginalProb) const
{
  marginalProb = NumericalPoint(1);
  return computeMinimumVolumeInterval(prob, marginalProb[0]);
}

/* Get the product bilateral confidence interval containing a given probability of the distributionImplementation */
Interval DistributionImplementation::computeBilateralConfidenceInterval(const NumericalScalar prob) const
{
  NumericalScalar marginalProb = -1.0;
  return computeBilateralConfidenceInterval(prob, marginalProb);
}

Interval DistributionImplementation::computeBilateralConfidenceInterval(const NumericalScalar prob,
    NumericalScalar & marginalProb) const
{
  if (!isContinuous()) throw NotYetImplementedException(HERE) << "In DistributionImplementation::computeMinimumVolumeInterval()";
  if (prob <= 0.0)
  {
    const NumericalPoint median(computeQuantile(0.5));
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
  Collection<Distribution> marginals(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i) marginals[i] = getMarginal(i);
  const MinimumVolumeIntervalWrapper minimumVolumeIntervalWrapper(this, marginals, prob);
  const NumericalMathFunction function(bindMethod<MinimumVolumeIntervalWrapper, NumericalPoint, NumericalPoint>(minimumVolumeIntervalWrapper, &MinimumVolumeIntervalWrapper::computeBilateralProbability, 1, 1));
  Brent solver(quantileEpsilon_, pdfEpsilon_, pdfEpsilon_, quantileIterations_);
  marginalProb = solver.solve(function, prob, 0.0, 1.0, 0.0, 1.0);
  const Interval IC(minimumVolumeIntervalWrapper.buildBilateralInterval(marginalProb));
  return IC;
}

Interval DistributionImplementation::computeBilateralConfidenceInterval(const NumericalScalar prob,
    NumericalPoint & marginalProb) const
{
  marginalProb = NumericalPoint(1);
  const Interval result(computeBilateralConfidenceInterval(prob, marginalProb[0]));
  return result;
}

/* Get the product unilateral confidence interval containing a given probability of the distributionImplementation */
Interval DistributionImplementation::computeUnilateralConfidenceInterval(const NumericalScalar prob,
    const Bool tail) const
{
  NumericalScalar marginalProb = -1.0;
  return computeUnilateralConfidenceInterval(prob, tail, marginalProb);
}

Interval DistributionImplementation::computeUnilateralConfidenceInterval(const NumericalScalar prob,
    const Bool tail,
    NumericalScalar & marginalProb) const
{
  marginalProb = -1.0;
  if (tail)
    {
      const NumericalPoint lowerBound(computeInverseSurvivalFunction(prob, marginalProb));
      return Interval(lowerBound, range_.getUpperBound());
    }
  const NumericalPoint upperBound(computeQuantile(prob, false, marginalProb));
  return Interval(range_.getLowerBound(), upperBound);
}

Interval DistributionImplementation::computeUnilateralConfidenceInterval(const NumericalScalar prob,
    const Bool tail,
    NumericalPoint & marginalProb) const
{
  marginalProb = NumericalPoint(1);
  const Interval result(computeUnilateralConfidenceInterval(prob, tail, marginalProb[0]));
  return result;
}


/* Get the minimum volume level set containing at least a given probability of the distributionImplementation.
   The minimum volume level A(p) set is such that A(p)={x\in R^n | y(x) <= y_p}
   where y(x)=-\log X and y_p is the p-quantile of Y=pdf(X)
*/
LevelSet DistributionImplementation::computeMinimumVolumeLevelSet(const NumericalScalar prob) const
{
  NumericalScalar threshold = -1.0;
  return computeMinimumVolumeLevelSet(prob, threshold);
}

LevelSet DistributionImplementation::computeMinimumVolumeLevelSet(const NumericalScalar prob,
    NumericalScalar & threshold) const
{
  if (!isContinuous()) throw NotYetImplementedException(HERE) << "In DistributionImplementation::computeMinimumVolumeLevelSet()";
  // 1D special case here to avoid a double construction of minimumVolumeLevelSetFunction
  if ((dimension_ == 1) && (ResourceMap::GetAsBool("Distribution-MinimumVolumeLevelSetBySampling")))
    {
      LOGINFO("Compute the minimum volume level set by sampling (QMC)");
      const LevelSet result(computeUnivariateMinimumVolumeLevelSetByQMC(prob, threshold));
      return result;
    }
  NumericalMathFunction minimumVolumeLevelSetFunction(MinimumVolumeLevelSetEvaluation(this).clone());
  minimumVolumeLevelSetFunction.setGradient(MinimumVolumeLevelSetGradient(clone()).clone());
  // If dimension_ == 1 the threshold can be computed analyticaly
  NumericalScalar minusLogPDFThreshold;
  if (dimension_ == 1)
  {
    const CompositeDistribution composite(minimumVolumeLevelSetFunction, *this);
    minusLogPDFThreshold = composite.computeQuantile(prob)[0];
    LOGINFO("Compute the minimum volume level set by using a composite distribution quantile (univariate general case)");
  } // dimension == 1
  else
  {
    LOGINFO("Compute the minimum volume level set by sampling (Monte Carlo)");
    const UnsignedInteger size = ResourceMap::GetAsUnsignedInteger("Distribution-MinimumVolumeLevelSetSamplingSize");
    const NumericalSample xSample(getSample(size));
    const NumericalSample logPDFSample(computeLogPDF(xSample));
    minusLogPDFThreshold = -logPDFSample.computeQuantile(1.0 - prob)[0];
  } // dimension > 1
  threshold = std::exp(-minusLogPDFThreshold);

  return LevelSet(minimumVolumeLevelSetFunction, minusLogPDFThreshold);
}

LevelSet DistributionImplementation::computeMinimumVolumeLevelSet(const NumericalScalar prob,
    NumericalPoint & threshold) const
{
  threshold = NumericalPoint(1);
  const LevelSet result(computeMinimumVolumeLevelSet(prob, threshold[0]));
  return result;
}

LevelSet DistributionImplementation::computeUnivariateMinimumVolumeLevelSetByQMC(const NumericalScalar prob,
      NumericalScalar & threshold) const
{
  NumericalMathFunction minimumVolumeLevelSetFunction(MinimumVolumeLevelSetEvaluation(clone()).clone());
  minimumVolumeLevelSetFunction.setGradient(MinimumVolumeLevelSetGradient(clone()).clone());
  // As we are in 1D and as the function defining the composite distribution can have complex variations,
  // we use an improved sampling method to compute the quantile of the -logPDF(X) distribution
  const UnsignedInteger size = SpecFunc::NextPowerOfTwo(ResourceMap::GetAsUnsignedInteger("Distribution-MinimumVolumeLevelSetSamplingSize"));
  const NumericalSample xQMC(getSampleByQMC(size));
  const NumericalSample logPDFSample(computeLogPDF(xQMC));
  const NumericalScalar minusLogPDFThreshold = -logPDFSample.computeQuantile(1.0 - prob)[0];
  threshold = std::exp(-minusLogPDFThreshold);

  return LevelSet(minimumVolumeLevelSetFunction, minusLogPDFThreshold); 
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
  const Interval::BoolCollection finiteLowerBound(dimension_, false);
  const Interval::BoolCollection finiteUpperBound(dimension_, false);
  // Initialize the range with inverted bounds in order to inform the generic implementation of the
  // computeScalarQuantile() method that it cannot trust the current range
  range_ = Interval(NumericalPoint(dimension_, 1.0), NumericalPoint(dimension_, -1.0), finiteLowerBound, finiteUpperBound);
  setRange(Interval(computeLowerBound(), computeUpperBound(), finiteLowerBound, finiteUpperBound));
}

/* Compute the lower bound of the range */
NumericalPoint DistributionImplementation::computeLowerBound() const
{
  // For a multivariate distribution, the range is the axes aligned box that fits to the marginal ranges
  NumericalPoint lowerBound(dimension_);
  // Here, we must separate the 1D case from the nD case as the getMarginal() method is generic for 1D case and
  // would involve a circular call to computeRange()
  if (dimension_ == 1) lowerBound[0] = computeScalarQuantile(cdfEpsilon_);
  else for (UnsignedInteger i = 0; i < dimension_; ++i) lowerBound[i] = getMarginal(i)->computeScalarQuantile(cdfEpsilon_);
  return lowerBound;
}

/* Compute the upper bound of the range */
NumericalPoint DistributionImplementation::computeUpperBound() const
{
  // For a multivariate distribution, the range is the axes aligned box that fits to the marginal ranges
  NumericalPoint upperBound(dimension_);
  if (dimension_ == 1) upperBound[0] = computeScalarQuantile(cdfEpsilon_, true);
  else for (UnsignedInteger i = 0; i < dimension_; ++i) upperBound[i] = getMarginal(i)->computeScalarQuantile(cdfEpsilon_, true);
  return upperBound;
}

/* Compute the mean of the distribution */
void DistributionImplementation::computeMean() const
{
  mean_ = getShiftedMoment(1, NumericalPoint(getDimension(), 0.0));
  isAlreadyComputedMean_ = true;
}

/* Get the mean of the distribution */
NumericalPoint DistributionImplementation::getMean() const
{
  if (!isAlreadyComputedMean_) computeMean();
  return mean_;
}

/* Get the standard deviation of the distribution */
NumericalPoint DistributionImplementation::getStandardDeviation() const
{
  const NumericalPoint variance(getCenteredMoment(2));
  NumericalPoint result(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i) result[i] = std::sqrt(variance[i]);
  return result;
}

/* Get the skewness of the distribution */
NumericalPoint DistributionImplementation::getSkewness() const
{
  const NumericalPoint variance(getCenteredMoment(2));
  const NumericalPoint thirdMoment(getCenteredMoment(3));
  NumericalPoint result(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i) result[i] = thirdMoment[i] / std::pow(variance[i], 1.5);
  return result;
}

/* Get the kurtosis of the distribution */
NumericalPoint DistributionImplementation::getKurtosis() const
{
  const NumericalPoint variance(getCenteredMoment(2));
  const NumericalPoint fourthMoment(getCenteredMoment(4));
  NumericalPoint result(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i) result[i] = fourthMoment[i] / std::pow(variance[i], 2.0);
  return result;
}

/* Get the moments of the distribution */
NumericalPoint DistributionImplementation::getMoment(const UnsignedInteger n) const
{
  if (n == 0) return NumericalPoint(dimension_, 1.0);
  return getShiftedMoment(n, NumericalPoint(dimension_, 0.0));
}

/* Get the centered moments of the distribution */
NumericalPoint DistributionImplementation::getCenteredMoment(const UnsignedInteger n) const
{
  if (n == 0) throw InvalidArgumentException(HERE) << "Error: the centered moments of order 0 are undefined.";
  if (n == 1) return NumericalPoint(dimension_, 0.0);
  return getShiftedMoment(n, getMean());
}

/* Compute the covariance of the distribution */
void DistributionImplementation::computeCovariance() const
{
  if (isContinuous()) computeCovarianceContinuous();
  else if (isDiscrete()) computeCovarianceDiscrete();
  else computeCovarianceGeneral();
}

void DistributionImplementation::computeCovarianceContinuous() const
{
  // We need this to initialize the covariance matrix in two cases:
  // + this is the first call to this routine (which could be checked by testing the dimension of the distribution and the dimension of the matrix
  // + the copula has changed from a non-independent one to the independent copula
  mean_ = getMean();
  covariance_ = CovarianceMatrix(dimension_);
  // First the diagonal terms, which are the marginal covariances
  // Marginal covariances
  const NumericalPoint variance(getCenteredMoment(2));
  for (UnsignedInteger component = 0; component < dimension_; ++component) covariance_(component, component) = variance[component];
  // Off-diagonal terms if the copula is not the independent copula
  if (!hasIndependentCopula())
  {
    const IteratedQuadrature integrator = IteratedQuadrature(GaussKronrod());
    // Performs the integration for each covariance in the strictly lower triangle of the covariance matrix
    // We first loop over the coefficients because the most expensive task is to get the 2D marginal distributions

    Indices indices(2);
    for(UnsignedInteger rowIndex = 0; rowIndex < dimension_; ++rowIndex)
    {
      indices[0] = rowIndex;
      const NumericalScalar muI = mean_[rowIndex];
      for (UnsignedInteger columnIndex = rowIndex + 1; columnIndex < dimension_; ++columnIndex)
      {
        indices[1] = columnIndex;
        const NumericalScalar muJ = mean_[columnIndex];
        const Implementation marginalDistribution(getMarginal(indices));
        if (!marginalDistribution->hasIndependentCopula())
        {
          // Compute the covariance element
          const CovarianceWrapper kernel(marginalDistribution, muI, muJ);
          const Interval interval(marginalDistribution->getRange());
          const NumericalPoint value(integrator.integrate(kernel, interval));
          covariance_(rowIndex, columnIndex) = integrator.integrate(kernel.clone(), interval)[0];
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
  const NumericalPoint variance(getCenteredMoment(2));
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
      const NumericalScalar muI = mean_[rowIndex];
      for(UnsignedInteger columnIndex = rowIndex + 1; columnIndex < dimension_; ++columnIndex)
      {
        indices[1] = columnIndex;
        const NumericalScalar muJ = mean_[columnIndex];
        const Implementation marginalDistribution(getMarginal(indices));
        if (!marginalDistribution->hasIndependentCopula())
        {
          const NumericalSample support(marginalDistribution->getSupport());
          const NumericalSample samplePDF(marginalDistribution->computePDF(support));
          NumericalScalar value = 0.0;
          const UnsignedInteger size = support.getSize();
          for (UnsignedInteger i = 0; i < size; ++i) value += (support[i][0] - muI) * (support[i][1] - muJ) * samplePDF[i][0];
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
  const NumericalPoint standardDeviation(getStandardDeviation());
  for(UnsignedInteger component = 0; component < dimension_; ++component) covariance_(component, component) = standardDeviation[component] * standardDeviation[component];
  // Off-diagonal terms if the copula is not the independent copula
  if (!hasIndependentCopula())
  {
    const NumericalScalar delta = 2.0;
    Indices indices(2);
    const int N(8 * 2 * 2 * 2 * 2 * 2);
    const NumericalScalar h = 0.5 / 2 / 2 / 2 / 2 / 2;
    for(UnsignedInteger rowIndex = 0; rowIndex < dimension_; ++rowIndex)
    {
      indices[0] = rowIndex;
      const Implementation marginalI(getMarginal(rowIndex));
      const NumericalScalar mi = marginalI->computeQuantile(0.5)[0];
      const NumericalScalar di = marginalI->computeQuantile(0.75)[0] - marginalI->computeQuantile(0.25)[0];
      for(UnsignedInteger columnIndex = rowIndex + 1; columnIndex < dimension_; ++columnIndex)
      {
        indices[1] = columnIndex;
        const Implementation marginalDistribution(getMarginal(indices));
        if (!marginalDistribution->hasIndependentCopula())
        {
          const Implementation marginalJ(getMarginal(columnIndex));
          const NumericalScalar mj = marginalJ->computeQuantile(0.5)[0];
          const NumericalScalar dj = marginalJ->computeQuantile(0.75)[0] - marginalJ->computeQuantile(0.25)[0];
          NumericalPoint xij(2);
          xij[0] = mi;
          xij[1] = mj;
          NumericalScalar covarianceIJ = 0.0;
          // Then we loop over the integration points
          for(int rowNodeIndex = -N; rowNodeIndex < N + 1; ++rowNodeIndex)
          {
            const NumericalScalar hi = h * rowNodeIndex;
            const NumericalScalar expHi = std::exp(hi);
            const NumericalScalar iexpHi = 1.0 / expHi;
            const NumericalScalar sinhHi = 0.5 * (expHi - iexpHi);
            const NumericalScalar expSinhHi = std::exp(sinhHi);
            const NumericalScalar iexpSinhHi = 1.0 / expSinhHi;
            const NumericalScalar iTwoCoshSinhHi = 1.0 / (expSinhHi + iexpSinhHi);
            const NumericalScalar xip = mi + expSinhHi * iTwoCoshSinhHi * di * delta;
            const NumericalScalar wi = (expHi + iexpHi) * iTwoCoshSinhHi * iTwoCoshSinhHi;
            const NumericalScalar cdfip = marginalI->computeCDF(xip);
            for(int columnNodeIndex = -N; columnNodeIndex < N + 1; ++columnNodeIndex)
            {
              const NumericalScalar hj = h * columnNodeIndex;
              const NumericalScalar expHj = std::exp(hj);
              const NumericalScalar iexpHj = 1.0 / expHj;
              const NumericalScalar sinhHj = 0.5 * (expHj - iexpHj);
              const NumericalScalar expSinhHj = std::exp(sinhHj);
              const NumericalScalar iexpSinhHj = 1.0 / expSinhHj;
              const NumericalScalar iTwoCoshSinhHj = 1.0 / (expSinhHj + iexpSinhHj);
              const NumericalScalar xjp = mj + expSinhHj * iTwoCoshSinhHj * dj * delta;
              const NumericalScalar wj = (expHj + iexpHj) * iTwoCoshSinhHj * iTwoCoshSinhHj;
              const NumericalScalar cdfjp = marginalJ->computeCDF(xjp);
              NumericalPoint inpp(2);
              inpp[0] = xip;
              inpp[1] = xjp;
              covarianceIJ += delta * delta * di * dj * h * h * wi * wj * (marginalDistribution->computeCDF(inpp) - cdfip * cdfjp);
            } // loop over J integration nodes
          } // loop over I integration nodes
          covariance_(rowIndex, columnIndex) = covarianceIJ;
        }
      } // loop over column indices
    } // loop over row indices
  } // if !hasIndependentCopula
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
  covariance_ = getCovariance();
  CorrelationMatrix R(dimension_);
  NumericalPoint sigma(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    const NumericalScalar sigmaI = std::sqrt(covariance_(i, i));
    sigma[i] = sigmaI;
    if (sigmaI > 0.0)
      for (UnsignedInteger j = 0; j < i; ++j)
        if (sigma[j] > 0)
          R(i, j) = covariance_(i, j) / (sigmaI * sigma[j]);
  }
  return R;
}

CorrelationMatrix DistributionImplementation::getLinearCorrelation() const
{
  return getCorrelation();
}

CorrelationMatrix DistributionImplementation::getPearsonCorrelation() const
{
  return getCorrelation();
}

/* Get the Spearman correlation of the distribution */
CorrelationMatrix DistributionImplementation::getSpearmanCorrelation() const
{
  if (isCopula()) return getCorrelation();
  return getCopula()->getSpearmanCorrelation();
}

/* Get the Kendall concordance of the distribution */
CorrelationMatrix DistributionImplementation::getKendallTau() const
{
  if (isElliptical())
  {
    const CorrelationMatrix shape(getCorrelation());
    CorrelationMatrix tau(dimension_);
    for (UnsignedInteger i = 0; i < dimension_; ++i)
      for(UnsignedInteger j = 0; j < i; ++j)
        tau(i, j) = std::asin(shape(i, j)) * (2.0 / M_PI);
    return tau;
  }
  return getCopula()->getKendallTau();
}

/* Get the shape matrix of the distribution, ie the correlation matrix
   of its copula if it is elliptical */
CorrelationMatrix DistributionImplementation::getShapeMatrix() const
{
  if (!hasEllipticalCopula()) throw NotDefinedException(HERE) << "Error: the shape matrix is defined only for distributions with elliptical copulas.";
  return getCopula()->getShapeMatrix();
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
  TriangularMatrix cholesky(getCholesky());

  const TriangularMatrix inverseCholesky(cholesky.solveLinearSystem(IdentityMatrix(dimension_), false).getImplementation());

  return inverseCholesky;
}

/* Compute the nodes and weights for a 1D gauss quadrature over [-1, 1] with respect to the Lebesgue measure */
void DistributionImplementation::computeGaussNodesAndWeights() const
{
  int integrationNodesNumber(integrationNodesNumber_);
  gaussNodes_ = NumericalPoint(integrationNodesNumber);
  gaussWeights_ = NumericalPoint(integrationNodesNumber);
  // First, build a symmetric tridiagonal matrix whose eigenvalues are the nodes of the
  // gauss integration rule
  char jobz('V');
  int ljobz(1);
  NumericalPoint d(integrationNodesNumber);
  NumericalPoint e(integrationNodesNumber);
  for (UnsignedInteger i = 1; i < static_cast<UnsignedInteger>(integrationNodesNumber); ++i) e[i - 1] = 0.5 / std::sqrt(1.0 - std::pow(2.0 * i, -2));
  int ldz(integrationNodesNumber);
  SquareMatrix z(integrationNodesNumber);
  NumericalPoint work(2 * integrationNodesNumber - 2);
  int info;
  dstev_(&jobz, &integrationNodesNumber, &d[0], &e[0], &z(0, 0), &ldz, &work[0], &info, &ljobz);
  if (info != 0) throw InternalException(HERE) << "Lapack DSTEV: error code=" << info;
  for (UnsignedInteger i = 0; i < static_cast<UnsignedInteger>(integrationNodesNumber); ++i)
  {
    // Nodes
    gaussNodes_[i] = d[i];
    // Weights
    gaussWeights_[i] = 2.0 * std::pow(z(0, i), 2);
  }
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
NumericalPoint DistributionImplementation::getGaussNodesAndWeights(NumericalPoint & weights) const
{
  if (!isAlreadyComputedGaussNodesAndWeights_) computeGaussNodesAndWeights();
  weights = gaussWeights_;
  return gaussNodes_;
}


/* Get the moments of the standardized distribution */
NumericalPoint DistributionImplementation::getStandardMoment(const UnsignedInteger n) const
{
  return getStandardRepresentative()->getMoment(n);
}


/* Get the shifted moments of the distribution */
NumericalPoint DistributionImplementation::getShiftedMoment(const UnsignedInteger n,
    const NumericalPoint & shift) const
{
  if (isContinuous()) return computeShiftedMomentContinuous(n, shift);
  if (isDiscrete()) return computeShiftedMomentDiscrete(n, shift);
  return computeShiftedMomentGeneral(n, shift);
}

NumericalPoint DistributionImplementation::computeShiftedMomentContinuous(const UnsignedInteger n,
    const NumericalPoint & shift) const
{
  if (shift.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the shift dimension must match the distribution dimension.";
  if (n == 0) return NumericalPoint(dimension_, 1.0);
  NumericalPoint moment(dimension_);
  // For each component
  GaussKronrod algo;
  for(UnsignedInteger component = 0; component < dimension_; ++component)
  {
    const Implementation marginalDistribution(getMarginal(component));
    const ShiftedMomentWrapper integrand(n, shift[component], marginalDistribution);
    const NumericalScalar a = marginalDistribution->getRange().getLowerBound()[0];
    const NumericalScalar b = marginalDistribution->getRange().getUpperBound()[0];
    moment[component] = algo.integrate(integrand, Interval(a, b))[0];
  } // End of each component
  return moment;
}

NumericalPoint DistributionImplementation::computeShiftedMomentDiscrete(const UnsignedInteger n,
    const NumericalPoint & shift) const
{
  if (n == 0) throw InvalidArgumentException(HERE) << "Error: the centered moments of order 0 are undefined.";
  if (shift.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the shift dimension must match the distribution dimension.";
  NumericalPoint moment(dimension_);
  const NumericalSample support(getSupport());
  const NumericalSample pdfSupport(computePDF(support));
  for (UnsignedInteger i = 0; i < support.getSize(); ++i)
    for (UnsignedInteger j = 0; j < dimension_; ++j)
      moment[j] += std::pow(support[i][j] - shift[j], static_cast<int>(n)) * pdfSupport[i][0];
  return moment;
}

NumericalPoint DistributionImplementation::computeShiftedMomentGeneral(const UnsignedInteger n,
    const NumericalPoint & shift) const
{
  if (n == 0) throw InvalidArgumentException(HERE) << "Error: the centered moments of order 0 are undefined.";
  if (shift.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the shift dimension must match the distribution dimension.";
  NumericalPoint moment(dimension_);
  const NumericalScalar epsilon = std::sqrt(quantileEpsilon_);
  const UnsignedInteger MaximumLevel = ResourceMap::GetAsUnsignedInteger("Distribution-DefaultLevelNumber") + 3;
  // For each component
  for(UnsignedInteger component = 0; component < dimension_; ++component)
  {
    NumericalScalar h = 0.5;
    UnsignedInteger N = 6;
    const Implementation marginalDistribution(getMarginal(component));
    const NumericalScalar shiftComponent = shift[component];
    // Central term
    moment[component] = h * 0.5 * std::pow(marginalDistribution->computeQuantile(0.5)[0], static_cast<int>(n));
    // First block
    for (UnsignedInteger j = 1; j <= N; ++j)
    {
      const NumericalScalar hj = h * j;
      const NumericalScalar expHj = std::exp(hj);
      const NumericalScalar iexpHj = 1.0 / expHj;
      const NumericalScalar sinhHj = 0.5 * (expHj - iexpHj);
      const NumericalScalar expSinhHj = std::exp(sinhHj);
      const NumericalScalar iexpSinhHj = 1.0 / expSinhHj;
      const NumericalScalar iTwoCoshSinhHj = 1.0 / (expSinhHj + iexpSinhHj);
      const NumericalScalar xjm = iexpSinhHj * iTwoCoshSinhHj;
      const NumericalScalar xjp = expSinhHj * iTwoCoshSinhHj;
      const NumericalScalar wj = (expHj + iexpHj) * iTwoCoshSinhHj * iTwoCoshSinhHj;
      moment[component] += h * wj * (std::pow(marginalDistribution->computeQuantile(xjm)[0] - shiftComponent, static_cast<int>(n)) + std::pow(marginalDistribution->computeQuantile(xjp)[0] - shiftComponent, static_cast<int>(n)));
    } // End of first block
    //values[0] = moment[component];
    // Sequential addition of half-blocks
    NumericalScalar error = 1.0;
    UnsignedInteger level = 0;
    while( (error > epsilon) && (level < MaximumLevel))
    {
      ++level;
      h *= 0.5;
      moment[component] *= 0.5;
      NumericalScalar delta = 0.0;
      for (UnsignedInteger j = 0; j <= N; ++j)
      {
        const NumericalScalar hj = h * (2 * j + 1);
        const NumericalScalar expHj = std::exp(hj);
        const NumericalScalar iexpHj = 1.0 / expHj;
        const NumericalScalar sinhHj = 0.5 * (expHj - iexpHj);
        const NumericalScalar expSinhHj = std::exp(sinhHj);
        const NumericalScalar iexpSinhHj = 1.0 / expSinhHj;
        const NumericalScalar iTwoCoshSinhHj = 1.0 / (expSinhHj + iexpSinhHj);
        const NumericalScalar xjm = iexpSinhHj * iTwoCoshSinhHj;
        const NumericalScalar xjp = expSinhHj * iTwoCoshSinhHj;
        NumericalScalar wj = (expHj + iexpHj) * iTwoCoshSinhHj * iTwoCoshSinhHj;
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
NumericalSample DistributionImplementation::getSupport(const Interval & interval) const
{
  throw NotYetImplementedException(HERE) << "In DistributionImplementation::getSupport(const Interval & interval) const";
}

/* Get the support on the whole range */
NumericalSample DistributionImplementation::getSupport() const
{
  return getSupport(getRange());
}

/* Get the discrete probability levels */
NumericalPoint DistributionImplementation::getProbabilities() const
{
  if (!isDiscrete())
    throw InternalException(HERE) << "Error: cannot return probability levels of a non discrete distribution.";

  return computePDF(getSupport()).getImplementation()->getData();
}

/* Get the PDF singularities inside of the range - 1D only */
NumericalPoint DistributionImplementation::getSingularities() const
{
  if (dimension_ != 1) throw NotDefinedException(HERE) << "Error: cannot ask for PDF singularities for multivariate distributions.";
  return NumericalPoint(0);
}

/* Compute the density generator of the elliptical generator, i.e.
 *  the function phi such that the density of the distribution can
 *  be written as p(x) = phi(t(x-mu)R(x-mu))
 */
NumericalScalar DistributionImplementation::computeDensityGenerator(const NumericalScalar betaSquare) const
{
  throw NotYetImplementedException(HERE) << "In DistributionImplementation::computeDensityGenerator(const NumericalScalar betaSquare) const";
}

/* Compute the derivative of the density generator */
NumericalScalar DistributionImplementation::computeDensityGeneratorDerivative(const NumericalScalar betaSquare) const
{
  throw NotYetImplementedException(HERE) << "In DistributionImplementation::computeDensityGeneratorDerivative(const NumericalScalar betaSquare) const";
}

/* Compute the seconde derivative of the density generator */
NumericalScalar DistributionImplementation::computeDensityGeneratorSecondDerivative(const NumericalScalar betaSquare) const
{
  throw NotYetImplementedException(HERE) << "In DistributionImplementation::computeDensityGeneratorSecondDerivative(const NumericalScalar betaSquare) const";
}

/* Get the i-th marginal distribution */
DistributionImplementation::Implementation DistributionImplementation::getMarginal(const UnsignedInteger i) const
{
  if ((dimension_ == 1) && (i == 0)) return clone();
  if (isCopula() && (i < dimension_)) return new Uniform(0.0, 1.0);
  return MarginalDistribution(*this, i).clone();
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
DistributionImplementation::Implementation DistributionImplementation::getMarginal(const Indices & indices) const
{
  if ((dimension_ == 1) && (indices[0] == 0)) return clone();
  if ((dimension_ == 2) && (indices.getSize() == 1) && (indices[0] < dimension_)) return new Uniform(0.0, 1.0);
  if ((dimension_ == 2) && (indices.getSize() == 2) && (indices[0] == 0) && (indices[1] == 1)) return clone();
  return MarginalDistribution(*this, indices).clone();
}

/* Get the copula of a distribution */
DistributionImplementation::Implementation DistributionImplementation::getCopula() const
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
    DistributionCollection collection(1);
    collection[0] = *this;
    // Get the marginal transformation evaluation implementation
    MarginalTransformationEvaluation evaluation(collection, DistributionCollection(1, Normal()));
    // We have to correct the direction because the output collection corresponds to the standard space, so there is no parameter to take into account.
    evaluation.setDirection(MarginalTransformationEvaluation::FROM);
    const EvaluationImplementation p_evaluation(evaluation.clone());
    // Get the marginal transformation gradient implementation
    const GradientImplementation p_gradient = new MarginalTransformationGradient(evaluation);
    // Get the marginal transformation hessian implementation
    const HessianImplementation p_hessian = new MarginalTransformationHessian(evaluation);
    InverseIsoProbabilisticTransformation inverseTransformation(p_evaluation, p_gradient, p_hessian);
    NumericalPointWithDescription parameters(getParameter());
    const UnsignedInteger parametersDimension = parameters.getDimension();
    Description parametersDescription(parameters.getDescription());
    const String name(parameters.getName());
    for (UnsignedInteger i = 0; i < parametersDimension; i++) parametersDescription[i] = OSS() << name << "_" << parametersDescription[i];
    parameters.setDescription(parametersDescription);
    inverseTransformation.setParameter(parameters);
    return inverseTransformation;
  }
  // General case, Rosenblatt transformation
  return NumericalMathFunctionImplementation(new RosenblattEvaluation(clone()));
}

/* Get the inverse isoprobabilist transformation */
DistributionImplementation::InverseIsoProbabilisticTransformation DistributionImplementation::getInverseIsoProbabilisticTransformation() const
{
  // Special case for dimension 1
  if (dimension_ == 1)
  {
    DistributionCollection collection(1);
    collection[0] = *this;
    // Get the marginal transformation evaluation implementation
    MarginalTransformationEvaluation evaluation(DistributionCollection(1, Normal()), collection);
    // We have to correct the direction because the input collection corresponds to the standard space, so there is no parameter to take into account.
    evaluation.setDirection(MarginalTransformationEvaluation::TO);
    const EvaluationImplementation p_evaluation(evaluation.clone());
    // Get the marginal transformation gradient implementation
    const GradientImplementation p_gradient = new MarginalTransformationGradient(evaluation);
    // Get the marginal transformation hessian implementation
    const HessianImplementation p_hessian = new MarginalTransformationHessian(evaluation);
    InverseIsoProbabilisticTransformation inverseTransformation(p_evaluation, p_gradient, p_hessian);
    NumericalPointWithDescription parameters(getParameter());
    const UnsignedInteger parametersDimension = parameters.getDimension();
    Description parametersDescription(parameters.getDescription());
    const String name(parameters.getName());
    for (UnsignedInteger i = 0; i < parametersDimension; i++) parametersDescription[i] = OSS() << name << "_" << parametersDescription[i];
    parameters.setDescription(parametersDescription);
    inverseTransformation.setParameter(parameters);
    return inverseTransformation;
  }
  // General case, inverse Rosenblatt transformation
  return NumericalMathFunctionImplementation(new InverseRosenblattEvaluation(clone()));
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
DistributionImplementation::Implementation DistributionImplementation::getStandardDistribution() const
{
  if (!isAlreadyComputedStandardDistribution_) computeStandardDistribution();
  return p_standardDistribution_;
}

/* Get the standard representative in the parametric family, associated with the standard moments */
DistributionImplementation::Implementation DistributionImplementation::getStandardRepresentative() const
{
  return clone();
}

/* Compute the radial distribution CDF */
NumericalScalar DistributionImplementation::computeRadialDistributionCDF(const NumericalScalar radius,
    const Bool tail) const
{
  throw NotYetImplementedException(HERE) << "In DistributionImplementation::computeRadialDistributionCDF(const NumericalScalar radius, const Bool tail) const";
}


/* Draw the PDF of a discrete distribution */
Graph DistributionImplementation::drawDiscretePDF(const NumericalScalar xMin,
    const NumericalScalar xMax,
    const UnsignedInteger pointNumber) const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: cannot draw the PDF of a multidimensional discrete distribution this way.";
  if (xMax < xMin - ResourceMap::GetAsNumericalScalar("DiscreteDistribution-SupportEpsilon")) throw InvalidArgumentException(HERE) << "Error: cannot draw a PDF with xMax < xMin, here xmin=" << xMin << " and xmax=" << xMax;
  const String title(OSS() << getDescription()[0] << " PDF");
  const NumericalSample support(getSupport(Interval(xMin, xMax)));
  // First the vertical bars
  const String xName(getDescription()[0]);
  Graph graphPDF(title, xName, "PDF", true, "topright");
  NumericalPoint point(2);
  point[0] = xMin - ResourceMap::GetAsNumericalScalar("DiscreteDistribution-SupportEpsilon");
  const NumericalSample gridY(computePDF(support));

  NumericalSample data(0, 2);
  data.add(point);
  for (UnsignedInteger i = 0; i < support.getSize(); ++i)
  {
    point[0] = support[i][0];
    data.add(point);
    point[1] = gridY[i][0];
    data.add(point);
    point[1] = 0.0;
    data.add(point);
  }
  point[0] = xMax + ResourceMap::GetAsNumericalScalar("DiscreteDistribution-SupportEpsilon");
  point[1] = 0.0;
  data.add(point);
  graphPDF.add(Curve(data, "red", "solid", 2, title));
  NumericalPoint boundingBox(graphPDF.getBoundingBox());
  const NumericalScalar yMin = boundingBox[2];
  const NumericalScalar yMax = gridY.computeQuantilePerComponent(0.99)[0];
  boundingBox[2] = yMin - 0.1 * (yMax - yMin);
  boundingBox[3] = yMax + 0.1 * (yMax - yMin);
  graphPDF.setBoundingBox(boundingBox);
  return graphPDF;
}

/* Draw the PDF of the distribution when its dimension is 1 */
Graph DistributionImplementation::drawPDF(const NumericalScalar xMin,
    const NumericalScalar xMax,
    const UnsignedInteger pointNumber) const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "Error: can draw a PDF only if dimension equals 1, here dimension=" << dimension_;
  if (xMax <= xMin) throw InvalidArgumentException(HERE) << "Error: cannot draw a PDF with xMax <= xMin, here xmin=" << xMin << " and xmax=" << xMax;
  if (pointNumber < 2) throw InvalidArgumentException(HERE) << "Error: cannot draw a PDF with a point number < 2";
  if (isDiscrete()) return drawDiscretePDF(xMin, xMax, pointNumber);
  // Discretization of the x axis
  const String title(OSS() << getDescription()[0] << " PDF");
  NumericalSample gridX;
  const NumericalSample gridY(computePDF(xMin, xMax, pointNumber, gridX));
  Curve curvePDF(gridX, gridY);
  curvePDF.setColor("red");
  curvePDF.setLegend(title);
  curvePDF.setLineStyle("solid");
  curvePDF.setLineWidth(2);
  const String xName(getDescription()[0]);
  Graph graphPDF(title, xName, "PDF", true, "topright");
  graphPDF.add(curvePDF);
  NumericalPoint boundingBox(graphPDF.getBoundingBox());
  const NumericalScalar yMin = boundingBox[2];
  const NumericalScalar yMax = gridY.computeQuantilePerComponent(0.99)[0];
  boundingBox[2] = yMin - 0.1 * (yMax - yMin);
  boundingBox[3] = yMax + 0.1 * (yMax - yMin);
  graphPDF.setBoundingBox(boundingBox);
  return graphPDF;
}

/* Draw the PDF of the distribution when its dimension is 1 */
Graph DistributionImplementation::drawPDF(const UnsignedInteger pointNumber) const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: this method is available only for 1D distributions";
  // For discrete distributions, use the numerical range to define the drawing range
  const NumericalScalar xMin = computeQuantile(ResourceMap::GetAsNumericalScalar("Distribution-QMin"))[0];
  const NumericalScalar xMax = computeQuantile(ResourceMap::GetAsNumericalScalar("Distribution-QMax"))[0];
  const NumericalScalar delta = 2.0 * (xMax - xMin) * (1.0 - 0.5 * (ResourceMap::GetAsNumericalScalar("Distribution-QMax" ) - ResourceMap::GetAsNumericalScalar("Distribution-QMin")));
  if (isDiscrete())
  {
    NumericalScalar a = std::max(xMin - delta, range_.getLowerBound()[0] - 1.0);
    NumericalScalar b = std::min(xMax + delta, range_.getUpperBound()[0] + 1.0);
    if (b <= a)
    {
      a -= 1.0;
      b += 1.0;
    }
    return drawPDF(a, b, pointNumber);
  }
  return drawPDF(xMin - delta, xMax + delta, pointNumber);
}

/* Draw the PDF of a 1D marginal */
Graph DistributionImplementation::drawMarginal1DPDF(const UnsignedInteger marginalIndex,
    const NumericalScalar xMin,
    const NumericalScalar xMax,
    const UnsignedInteger pointNumber) const
{
  Graph marginalGraph(getMarginal(marginalIndex)->drawPDF(xMin, xMax, pointNumber));
  marginalGraph.setTitle(OSS() << getDescription() << "->" << description_[marginalIndex] << " component PDF");
  return marginalGraph;
}

/* Draw the PDF of the distribution when its dimension is 2 */
Graph DistributionImplementation::drawPDF(const NumericalPoint & xMin,
    const NumericalPoint & xMax,
    const Indices & pointNumber) const
{
  if (!(pointNumber[0] >= 2 && pointNumber[1] >= 2)) throw InvalidArgumentException(HERE) << "Error: the discretization must have at least 2 points per component";
  NumericalPoint discretization(2);
  NumericalPoint scaling(2);
  NumericalPoint origin(2);
  const NumericalScalar nX = pointNumber[0] - 2;
  discretization[0] = nX;
  // Discretization of the first component
  NumericalSample x(Box(NumericalPoint(1, nX)).generate());
  origin[0] = xMin[0];
  scaling[0] = xMax[0] - xMin[0];
  x *= NumericalPoint(1, scaling[0]);
  x += NumericalPoint(1, origin[0]);
  const NumericalScalar nY = pointNumber[1] - 2;
  discretization[1] = nY;
  // Discretization of the second component
  NumericalSample y(Box(NumericalPoint(1, nY)).generate());
  origin[1] = xMin[1];
  scaling[1] = xMax[1] - xMin[1];
  y *= NumericalPoint(1, scaling[1]);
  y += NumericalPoint(1, origin[1]);
  NumericalSample xy;
  // Compute the output sample, using possible parallelism or optimized implementation
  const NumericalSample z(computePDF(xMin, xMax, pointNumber, xy));
  const String xName(description_[0]);
  const String yName(description_[1]);
  const String title(OSS() << getDescription() << " iso-PDF");
  Graph graph(title, xName, yName, true, "topright");
  Contour isoValues(Contour(x, y, z, NumericalPoint(0), Description(0), true, title));
  isoValues.buildDefaultLevels();
  isoValues.buildDefaultLabels();
  graph.add(isoValues);
  return graph;
}

/* Draw the PDF of the distribution when its dimension is 2 */
Graph DistributionImplementation::drawPDF(const NumericalPoint & xMin,
    const NumericalPoint & xMax) const
{
  return drawPDF(xMin, xMax, Indices(2, ResourceMap::GetAsUnsignedInteger("Distribution-DefaultPointNumber")));
}

/* Draw the PDF of the distribution when its dimension is 2 */
Graph DistributionImplementation::drawPDF(const Indices & pointNumber) const
{
  if (pointNumber.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: pointNumber must be of size 2, here size=" << pointNumber.getSize();
  NumericalPoint xMin(2);
  if (isCopula()) xMin = NumericalPoint(2, 0.0);
  else
  {
    xMin[0] = getMarginal(0)->computeQuantile(ResourceMap::GetAsNumericalScalar("Distribution-QMin"))[0];
    xMin[1] = getMarginal(1)->computeQuantile(ResourceMap::GetAsNumericalScalar("Distribution-QMin"))[0];
  }
  NumericalPoint xMax(2);
  if (isCopula()) xMax = NumericalPoint(2, 1.0);
  else
  {
    xMax[0] = getMarginal(0)->computeQuantile(ResourceMap::GetAsNumericalScalar("Distribution-QMax"))[0];
    xMax[1] = getMarginal(1)->computeQuantile(ResourceMap::GetAsNumericalScalar("Distribution-QMax"))[0];
  }
  NumericalPoint delta(2, 0.0);
  if (!isCopula()) delta = (2.0 * (xMax - xMin) * (1.0 - 0.5 * (ResourceMap::GetAsNumericalScalar("Distribution-QMax" ) - ResourceMap::GetAsNumericalScalar("Distribution-QMin"))));
  const Interval intersection(getRange().intersect(Interval(xMin - delta, xMax + delta)));
  Graph graph(drawPDF(intersection.getLowerBound(), intersection.getUpperBound(), pointNumber));
  // Add a border for a copula
  if (isCopula())
  {
    const Drawable drawable(graph.getDrawable(0));
    NumericalSample data(5, 2);
    data[1][0] = 1.0;
    data[2]    = NumericalPoint(2, 1.0);
    data[3][1] = 1.0;
    Curve square(data);
    square.setColor("blue");
    graph.setDrawable(square, 0);
    graph.add(drawable);
  }
  return graph;
}

/* Draw the PDF of a 2D marginal */
Graph DistributionImplementation::drawMarginal2DPDF(const UnsignedInteger firstMarginal,
    const UnsignedInteger secondMarginal,
    const NumericalPoint & xMin,
    const NumericalPoint & xMax,
    const Indices & pointNumber) const
{
  Indices indices(2);
  indices[0] = firstMarginal;
  indices[1] = secondMarginal;
  Graph marginalGraph(getMarginal(indices)->drawPDF(xMin, xMax, pointNumber));
  marginalGraph.setTitle(OSS() << getDescription() << "->[" << description_[firstMarginal] << ", " << description_[secondMarginal] << "] components iso-PDF");
  return marginalGraph;
}

/* Draw the PDF of the distribution when its dimension is 1 or 2 */
Graph DistributionImplementation::drawPDF() const
{
  UnsignedInteger dimension = getDimension();
  // Generic interface for the 1D and 2D cases
  if (dimension == 1) return drawPDF(ResourceMap::GetAsUnsignedInteger("Distribution-DefaultPointNumber"));
  if (dimension == 2) return drawPDF(Indices(2, ResourceMap::GetAsUnsignedInteger("Distribution-DefaultPointNumber")));
  throw InvalidDimensionException(HERE) << "Error: can draw a PDF only if dimension equals 1 or 2, here dimension=" << dimension;
}


/* Draw the log-PDF of a discrete distribution */
Graph DistributionImplementation::drawDiscreteLogPDF(const NumericalScalar xMin,
    const NumericalScalar xMax,
    const UnsignedInteger pointNumber) const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: cannot draw the PDF of a multidimensional discrete distribution this way.";
  if (xMax < xMin - ResourceMap::GetAsNumericalScalar("DiscreteDistribution-SupportEpsilon")) throw InvalidArgumentException(HERE) << "Error: cannot draw a PDF with xMax < xMin, here xmin=" << xMin << " and xmax=" << xMax;
  const String title(OSS() << getDescription()[0] << " PDF");
  const NumericalSample support(getSupport(Interval(xMin, xMax)));
  // First the vertical bars
  const String xName(getDescription()[0]);
  Graph graphLogPDF(title, xName, "PDF", true, "topright");
  NumericalPoint point(2);
  point[0] = xMin - ResourceMap::GetAsNumericalScalar("DiscreteDistribution-SupportEpsilon");
  const NumericalSample gridY(computeLogPDF(support));

  NumericalSample data(0, 2);
  data.add(point);
  for (UnsignedInteger i = 0; i < support.getSize(); ++i)
  {
    point[0] = support[i][0];
    data.add(point);
    point[1] = gridY[i][0];
    data.add(point);
    point[1] = 0.0;
    data.add(point);
  }
  point[0] = xMax + ResourceMap::GetAsNumericalScalar("DiscreteDistribution-SupportEpsilon");
  point[1] = 0.0;
  data.add(point);
  graphLogPDF.add(Curve(data, "red", "solid", 2, title));
  NumericalPoint boundingBox(graphLogPDF.getBoundingBox());
  const NumericalScalar yMin = boundingBox[2];
  const NumericalScalar yMax = gridY.computeQuantilePerComponent(0.99)[0];
  boundingBox[2] = yMin - 0.1 * (yMax - yMin);
  boundingBox[3] = yMax + 0.1 * (yMax - yMin);
  graphLogPDF.setBoundingBox(boundingBox);
  return graphLogPDF;
}

/* Draw the log-PDF of the distribution when its dimension is 1 */
Graph DistributionImplementation::drawLogPDF(const NumericalScalar xMin,
    const NumericalScalar xMax,
    const UnsignedInteger pointNumber) const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "Error: can draw a PDF only if dimension equals 1, here dimension=" << dimension_;
  if (xMax <= xMin) throw InvalidArgumentException(HERE) << "Error: cannot draw a PDF with xMax <= xMin, here xmin=" << xMin << " and xmax=" << xMax;
  if (pointNumber < 2) throw InvalidArgumentException(HERE) << "Error: cannot draw a PDF with a point number < 2";
  if (isDiscrete()) return drawDiscreteLogPDF(xMin, xMax, pointNumber);
  // Discretization of the x axis
  const String title(OSS() << getDescription()[0] << " log PDF");
  NumericalSample gridX;
  const NumericalSample gridY(computeLogPDF(xMin, xMax, pointNumber, gridX));
  Curve curvePDF(gridX, gridY);
  curvePDF.setColor("red");
  curvePDF.setLegend(title);
  curvePDF.setLineStyle("solid");
  curvePDF.setLineWidth(2);
  const String xName(getDescription()[0]);
  Graph graphPDF(title, xName, "log PDF", true, "topright");
  graphPDF.add(curvePDF);
  NumericalPoint boundingBox(graphPDF.getBoundingBox());
  const NumericalScalar yMin = boundingBox[2];
  const NumericalScalar yMax = gridY.computeQuantilePerComponent(0.99)[0];
  boundingBox[2] = yMin - 0.1 * (yMax - yMin);
  boundingBox[3] = yMax + 0.1 * (yMax - yMin);
  graphPDF.setBoundingBox(boundingBox);
  return graphPDF;
}

/* Draw the log-PDF of the distribution when its dimension is 1 */
Graph DistributionImplementation::drawLogPDF(const UnsignedInteger pointNumber) const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: this method is available only for 1D distributions";
  // For discrete distributions, use the numerical range to define the drawing range
  const NumericalScalar xMin = computeQuantile(ResourceMap::GetAsNumericalScalar("Distribution-QMin"))[0];
  const NumericalScalar xMax = computeQuantile(ResourceMap::GetAsNumericalScalar("Distribution-QMax"))[0];
  const NumericalScalar delta = 2.0 * (xMax - xMin) * (1.0 - 0.5 * (ResourceMap::GetAsNumericalScalar("Distribution-QMax" ) - ResourceMap::GetAsNumericalScalar("Distribution-QMin")));
  if (isDiscrete())
  {
    NumericalScalar a = std::max(xMin - delta, range_.getLowerBound()[0] - 1.0);
    NumericalScalar b = std::min(xMax + delta, range_.getUpperBound()[0] + 1.0);
    if (b <= a)
    {
      a -= 1.0;
      b += 1.0;
    }
    return drawLogPDF(a, b, pointNumber);
  }
  return drawLogPDF(xMin - delta, xMax + delta, pointNumber);
}

/* Draw the log-PDF of a 1D marginal */
Graph DistributionImplementation::drawMarginal1DLogPDF(const UnsignedInteger marginalIndex,
    const NumericalScalar xMin,
    const NumericalScalar xMax,
    const UnsignedInteger pointNumber) const
{
  Graph marginalGraph(getMarginal(marginalIndex)->drawLogPDF(xMin, xMax, pointNumber));
  marginalGraph.setTitle(OSS() << getDescription() << "->" << description_[marginalIndex] << " component log PDF");
  return marginalGraph;
}

/* Draw the log-PDF of the distribution when its dimension is 2 */
Graph DistributionImplementation::drawLogPDF(const NumericalPoint & xMin,
    const NumericalPoint & xMax,
    const Indices & pointNumber) const
{
  if (!(pointNumber[0] >= 2 && pointNumber[1] >= 2)) throw InvalidArgumentException(HERE) << "Error: the discretization must have at least 2 points per component";
  NumericalPoint discretization(2);
  NumericalPoint scaling(2);
  NumericalPoint origin(2);
  const NumericalScalar nX = pointNumber[0] - 2;
  discretization[0] = nX;
  // Discretization of the first component
  NumericalSample x(Box(NumericalPoint(1, nX)).generate());
  origin[0] = xMin[0];
  scaling[0] = xMax[0] - xMin[0];
  x *= NumericalPoint(1, scaling[0]);
  x += NumericalPoint(1, origin[0]);
  const NumericalScalar nY = pointNumber[1] - 2;
  discretization[1] = nY;
  // Discretization of the second component
  NumericalSample y(Box(NumericalPoint(1, nY)).generate());
  origin[1] = xMin[1];
  scaling[1] = xMax[1] - xMin[1];
  y *= NumericalPoint(1, scaling[1]);
  y += NumericalPoint(1, origin[1]);
  NumericalSample xy;
  // Compute the output sample, using possible parallelism or optimized implementation
  const NumericalSample z(computeLogPDF(xMin, xMax, pointNumber, xy));
  const String xName(description_[0]);
  const String yName(description_[1]);
  const String title(OSS() << getDescription() << " iso-log PDF");
  Graph graph(title, xName, yName, true, "topright");
  Contour isoValues(Contour(x, y, z, NumericalPoint(0), Description(0), true, title));
  isoValues.buildDefaultLevels();
  isoValues.buildDefaultLabels();
  graph.add(isoValues);
  return graph;
}

/* Draw the log-PDF of the distribution when its dimension is 2 */
Graph DistributionImplementation::drawLogPDF(const NumericalPoint & xMin,
    const NumericalPoint & xMax) const
{
  return drawLogPDF(xMin, xMax, Indices(2, ResourceMap::GetAsUnsignedInteger("Distribution-DefaultPointNumber")));
}

/* Draw the log-PDF of the distribution when its dimension is 2 */
Graph DistributionImplementation::drawLogPDF(const Indices & pointNumber) const
{
  if (pointNumber.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: pointNumber must be of size 2, here size=" << pointNumber.getSize();
  NumericalPoint xMin(2);
  if (isCopula()) xMin = NumericalPoint(2, 0.0);
  else
  {
    xMin[0] = getMarginal(0)->computeQuantile(ResourceMap::GetAsNumericalScalar("Distribution-QMin"))[0];
    xMin[1] = getMarginal(1)->computeQuantile(ResourceMap::GetAsNumericalScalar("Distribution-QMin"))[0];
  }
  NumericalPoint xMax(2);
  if (isCopula()) xMax = NumericalPoint(2, 1.0);
  else
  {
    xMax[0] = getMarginal(0)->computeQuantile(ResourceMap::GetAsNumericalScalar("Distribution-QMax"))[0];
    xMax[1] = getMarginal(1)->computeQuantile(ResourceMap::GetAsNumericalScalar("Distribution-QMax"))[0];
  }
  NumericalPoint delta(2, 0.0);
  if (!isCopula()) delta = (2.0 * (xMax - xMin) * (1.0 - 0.5 * (ResourceMap::GetAsNumericalScalar("Distribution-QMax" ) - ResourceMap::GetAsNumericalScalar("Distribution-QMin"))));
  const Interval intersection(getRange().intersect(Interval(xMin - delta, xMax + delta)));
  Graph graph(drawLogPDF(intersection.getLowerBound(), intersection.getUpperBound(), pointNumber));
  // Add a border for a copula
  if (isCopula())
  {
    const Drawable drawable(graph.getDrawable(0));
    NumericalSample data(5, 2);
    data[1][0] = 1.0;
    data[2]    = NumericalPoint(2, 1.0);
    data[3][1] = 1.0;
    Curve square(data);
    square.setColor("blue");
    graph.setDrawable(square, 0);
    graph.add(drawable);
  }
  return graph;
}

/* Draw the log-PDF of a 2D marginal */
Graph DistributionImplementation::drawMarginal2DLogPDF(const UnsignedInteger firstMarginal,
    const UnsignedInteger secondMarginal,
    const NumericalPoint & xMin,
    const NumericalPoint & xMax,
    const Indices & pointNumber) const
{
  Indices indices(2);
  indices[0] = firstMarginal;
  indices[1] = secondMarginal;
  Graph marginalGraph(getMarginal(indices)->drawLogPDF(xMin, xMax, pointNumber));
  marginalGraph.setTitle(OSS() << getDescription() << "->[" << description_[firstMarginal] << ", " << description_[secondMarginal] << "] components iso-log PDF");
  return marginalGraph;
}

/* Draw the log-PDF of the distribution when its dimension is 1 or 2 */
Graph DistributionImplementation::drawLogPDF() const
{
  UnsignedInteger dimension = getDimension();
  // Generic interface for the 1D and 2D cases
  if (dimension == 1) return drawLogPDF(ResourceMap::GetAsUnsignedInteger("Distribution-DefaultPointNumber"));
  if (dimension == 2) return drawLogPDF(Indices(2, ResourceMap::GetAsUnsignedInteger("Distribution-DefaultPointNumber")));
  throw InvalidDimensionException(HERE) << "Error: can draw a log-PDF only if dimension equals 1 or 2, here dimension=" << dimension;
}


/* Draw the CDF of a discrete distribution */
Graph DistributionImplementation::drawDiscreteCDF(const NumericalScalar xMin,
    const NumericalScalar xMax,
    const UnsignedInteger pointNumber) const
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
  if (xMax <= xMin) throw InvalidArgumentException(HERE) << "Error: cannot draw a PDF with xMax >= xMin, here xmin=" << xMin << " and xmax=" << xMax;
  // Create the graph that will store the staircase representing the empirical CDF
  const String title(OSS() << getDescription()[0] << " CDF");
  const NumericalSample support(getSupport(Interval(xMin, xMax)));
  const NumericalSample gridY(computeCDF(support));
  const UnsignedInteger size = support.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "empty range (" << xMin << ", " << xMax << ")" << ", support is (" << getSupport().getMin()[0] << ", " << getSupport().getMax()[0] << ")";
  const String xName(getDescription()[0]);
  Graph graphCDF(title, xName, "CDF", true, "topleft");
  NumericalSample data(size + 2, 2);
  data[0][0] = xMin;
  data[0][1] = computeCDF(xMin);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const NumericalScalar x = support[i][0];
    data[i + 1][0] = x;
    data[i + 1][1] = gridY[i][0];
  }
  if (support[size - 1][0] == xMax) data[size + 1] = data[size];
  else
  {
    data[size + 1][0] = xMax;
    data[size + 1][1] = computeCDF(xMax);
  }
  graphCDF.add(Staircase(data, "red", "solid", 2, "s", title));
  NumericalPoint boundingBox(graphCDF.getBoundingBox());
  const NumericalScalar yMin = boundingBox[2];
  const NumericalScalar yMax = gridY.computeQuantilePerComponent(0.99)[0];
  boundingBox[2] = yMin - 0.1 * (yMax - yMin);
  boundingBox[3] = yMax + 0.1 * (yMax - yMin);
  graphCDF.setBoundingBox(boundingBox);
  return graphCDF;
}

/* Draw the CDF of the distribution when its dimension is 1 */
Graph DistributionImplementation::drawCDF(const NumericalScalar xMin,
    const NumericalScalar xMax,
    const UnsignedInteger pointNumber) const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "Error: can draw a CDF only if dimension equals 1, here dimension=" << dimension_;
  if (xMax <= xMin) throw InvalidArgumentException(HERE) << "Error: cannot draw a CDF with xMax >= xMin, here xmin=" << xMin << " and xmax=" << xMax;
  if (pointNumber < 2) throw InvalidArgumentException(HERE) << "Error: cannot draw a CDF with a point number < 2";
  if (isDiscrete()) return drawDiscreteCDF(xMin, xMax, pointNumber);
  const String title(OSS() << getDescription()[0] << " CDF");
  NumericalSample dataX;
  const NumericalSample dataY(computeCDF(xMin, xMax, pointNumber, dataX));
  Curve curveCDF(dataX, dataY);
  curveCDF.setColor("red");
  curveCDF.setLegend(title);
  curveCDF.setLineStyle("solid");
  curveCDF.setLineWidth(2);
  const String xName(getDescription()[0]);
  Graph graphCDF(title, xName, "CDF", true, "topleft");
  graphCDF.add(curveCDF);
  NumericalPoint boundingBox(graphCDF.getBoundingBox());
  const NumericalScalar yMin = boundingBox[2];
  const NumericalScalar yMax = dataY.computeQuantilePerComponent(0.99)[0];
  boundingBox[2] = yMin - 0.1 * (yMax - yMin);
  boundingBox[3] = yMax + 0.1 * (yMax - yMin);
  graphCDF.setBoundingBox(boundingBox);
  return graphCDF;
}

/* Draw the CDF of the distribution when its dimension is 1 */
Graph DistributionImplementation::drawCDF(const UnsignedInteger pointNumber) const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: this method is available only for 1D distributions";
  // For discrete distributions, use the numerical range to define the drawing range
  const NumericalScalar xMin = computeQuantile(ResourceMap::GetAsNumericalScalar("Distribution-QMin"))[0];
  const NumericalScalar xMax = computeQuantile(ResourceMap::GetAsNumericalScalar("Distribution-QMax"))[0];
  const NumericalScalar delta = 2.0 * (xMax - xMin) * (1.0 - 0.5 * (ResourceMap::GetAsNumericalScalar("Distribution-QMax" ) - ResourceMap::GetAsNumericalScalar("Distribution-QMin")));
  if (isDiscrete())
  {
    NumericalScalar a = std::max(xMin - delta, range_.getLowerBound()[0] - 1.0);
    NumericalScalar b = std::min(xMax + delta, range_.getUpperBound()[0] + 1.0);
    if (b <= a)
    {
      a -= 1.0;
      b += 1.0;
    }
    return drawCDF(a, b, pointNumber);
  }
  return drawCDF(xMin - delta, xMax + delta, pointNumber);
}

/* Draw the CDF of a 1D marginal */
Graph DistributionImplementation::drawMarginal1DCDF(const UnsignedInteger marginalIndex,
    const NumericalScalar xMin,
    const NumericalScalar xMax,
    const UnsignedInteger pointNumber) const
{
  Graph marginalGraph(getMarginal(marginalIndex)->drawCDF(xMin, xMax, pointNumber));
  marginalGraph.setTitle(OSS() << getDescription() << "->" << description_[marginalIndex] << " component CDF");
  return marginalGraph;
}

/* Draw the CDF of the distribution when its dimension is 2 */
Graph DistributionImplementation::drawCDF(const NumericalPoint & xMin,
    const NumericalPoint & xMax,
    const Indices & pointNumber) const
{
  if (xMin.getDimension() != 2) throw InvalidArgumentException(HERE) << "Error: expected xMin to be of dimension 2, here dimension=" << xMin.getDimension();
  if (xMax.getDimension() != 2) throw InvalidArgumentException(HERE) << "Error: expected xMax to be of dimension 2, here dimension=" << xMax.getDimension();
  if (pointNumber.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: expected pointNumber to be of size 2, here size=" << pointNumber.getSize();
  if (!(pointNumber[0] >= 2 && pointNumber[1] >= 2)) throw InvalidArgumentException(HERE) << "Error: the discretization must have at least 2 points per component";
  NumericalPoint discretization(2);
  NumericalPoint scaling(2);
  NumericalPoint origin(2);
  const NumericalScalar nX = pointNumber[0] - 2;
  discretization[0] = nX;
  // Discretization of the first component
  NumericalSample x(Box(NumericalPoint(1, nX)).generate());
  origin[0] = xMin[0];
  scaling[0] = xMax[0] - xMin[0];
  x *= NumericalPoint(1, scaling[0]);
  x += NumericalPoint(1, origin[0]);
  const NumericalScalar nY = pointNumber[1] - 2;
  discretization[1] = nY;
  // Discretization of the second component
  NumericalSample y(Box(NumericalPoint(1, nY)).generate());
  origin[1] = xMin[1];
  scaling[1] = xMax[1] - xMin[1];
  y *= NumericalPoint(1, scaling[1]);
  y += NumericalPoint(1, origin[1]);
  // Discretization of the XY plane
  NumericalSample xy(Box(discretization).generate());
  xy *= scaling;
  xy += origin;
  // Compute the output sample, using possible parallelism
  const NumericalSample z(computeCDF(xy));
  const String xName(description_[0]);
  const String yName(description_[1]);
  const String title(OSS() << getDescription() << " iso-CDF");
  Graph graph(title, xName, yName, true, "topright");
  Contour isoValues(Contour(x, y, z, NumericalPoint(0), Description(0), true, title));
  isoValues.buildDefaultLevels();
  isoValues.buildDefaultLabels();
  graph.add(isoValues);
  return graph;
}

/* Draw the CDF of the distribution when its dimension is 2 */
Graph DistributionImplementation::drawCDF(const NumericalPoint & xMin,
    const NumericalPoint & xMax) const
{
  return drawCDF(xMin, xMax, Indices(2, ResourceMap::GetAsUnsignedInteger("Distribution-DefaultPointNumber")));
}

/* Draw the CDF of the distribution when its dimension is 2 */
Graph DistributionImplementation::drawCDF(const Indices & pointNumber) const
{
  if (pointNumber.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: expected pointNumber to be of size 2, here size=" << pointNumber.getSize();
  NumericalPoint xMin(2);
  if (isCopula()) xMin = NumericalPoint(2, 0.0);
  else
  {
    xMin[0] = getMarginal(0)->computeQuantile(ResourceMap::GetAsNumericalScalar("Distribution-QMin"))[0];
    xMin[1] = getMarginal(1)->computeQuantile(ResourceMap::GetAsNumericalScalar("Distribution-QMin"))[0];
  }
  NumericalPoint xMax(2);
  if (isCopula()) xMax = NumericalPoint(2, 1.0);
  else
  {
    xMax[0] = getMarginal(0)->computeQuantile(ResourceMap::GetAsNumericalScalar("Distribution-QMax"))[0];
    xMax[1] = getMarginal(1)->computeQuantile(ResourceMap::GetAsNumericalScalar("Distribution-QMax"))[0];
  }
  NumericalPoint delta(2, 0.0);
  if (!isCopula()) delta = (2.0 * (xMax - xMin) * (1.0 - 0.5 * (ResourceMap::GetAsNumericalScalar("Distribution-QMax" ) - ResourceMap::GetAsNumericalScalar("Distribution-QMin"))));
  return drawCDF(xMin - delta, xMax + delta, pointNumber);
}

/* Draw the CDF of a 2D marginal */
Graph DistributionImplementation::drawMarginal2DCDF(const UnsignedInteger firstMarginal,
    const UnsignedInteger secondMarginal,
    const NumericalPoint & xMin,
    const NumericalPoint & xMax,
    const Indices & pointNumber) const
{
  Indices indices(2);
  indices[0] = firstMarginal;
  indices[1] = secondMarginal;
  Graph marginalGraph(getMarginal(indices)->drawCDF(xMin, xMax, pointNumber));
  marginalGraph.setTitle(OSS() << getDescription() << "->[" << description_[firstMarginal] << ", " << description_[secondMarginal] << "] components iso-CDF");
  return marginalGraph;
}

/* Draw the CDF of the distribution when its dimension is 1 or 2 */
Graph DistributionImplementation::drawCDF() const
{
  const UnsignedInteger dimension = getDimension();
  // Generic interface for the 1D and 2D cases
  if (dimension == 1) return drawCDF(ResourceMap::GetAsUnsignedInteger("Distribution-DefaultPointNumber"));
  if (dimension == 2) return drawCDF(Indices(2, ResourceMap::GetAsUnsignedInteger("Distribution-DefaultPointNumber")));
  throw InvalidDimensionException(HERE) << "Error: can draw a CDF only if dimension equals 1 or 2, here dimension=" << dimension;
}

/* Draw the quantile of the distribution when its dimension is 1 or 2 */
Graph DistributionImplementation::drawQuantile(const UnsignedInteger pointNumber) const
{
  const NumericalScalar qMin = SpecFunc::NumericalScalarEpsilon;
  const NumericalScalar qMax = 1.0 - qMin;
  return drawQuantile(qMin, qMax, pointNumber);
}

Graph DistributionImplementation::drawQuantile(const NumericalScalar qMin,
    const NumericalScalar qMax,
    const UnsignedInteger pointNumber) const
{
  // Generic interface for the 1D and 2D cases
  if (dimension_ == 1) return drawQuantile1D(qMin, qMax, pointNumber);
  if (dimension_ == 2) return drawQuantile2D(qMin, qMax, pointNumber);
  throw InvalidDimensionException(HERE) << "Error: can draw the quantiles only if dimension equals 1 or 2, here dimension=" << dimension_;
}

/* Draw the quantile of the distribution when its dimension is 1 */
Graph DistributionImplementation::drawQuantile1D(const NumericalScalar qMin,
    const NumericalScalar qMax,
    const UnsignedInteger pointNumber) const
{
  const String title(OSS() << getDescription()[0] << " Quantile");
  NumericalSample dataX;
  const NumericalSample dataY(computeQuantile(qMin, qMax, pointNumber, dataX));
  Curve curveQuantile(dataX, dataY);
  curveQuantile.setColor("red");
  curveQuantile.setLegend(title);
  curveQuantile.setLineStyle("solid");
  curveQuantile.setLineWidth(2);
  const String xName(getDescription()[0]);
  Graph graphQuantile(title, "p", xName, true, "topleft");
  graphQuantile.add(curveQuantile);
  NumericalPoint boundingBox(graphQuantile.getBoundingBox());
  const NumericalScalar yMin = boundingBox[2];
  const NumericalScalar yMax = dataY.computeQuantilePerComponent(0.99)[0];
  boundingBox[2] = yMin - 0.1 * (yMax - yMin);
  boundingBox[3] = yMax + 0.1 * (yMax - yMin);
  graphQuantile.setBoundingBox(boundingBox);
  return graphQuantile;
}

/* Draw the quantile of the distribution when its dimension is 2 */
Graph DistributionImplementation::drawQuantile2D(const NumericalScalar qMin,
    const NumericalScalar qMax,
    const UnsignedInteger pointNumber) const
{
  const String title(OSS() << getDescription() << " Quantile");
  const NumericalSample data(computeQuantile(qMin, qMax, pointNumber));
  Curve curveQuantile(data);
  curveQuantile.setColor("red");
  curveQuantile.setLegend(title);
  curveQuantile.setLineStyle("solid");
  curveQuantile.setLineWidth(2);
  const String xName(getDescription()[0]);
  const String yName(getDescription()[1]);
  Graph graphQuantile(title, xName, yName, true, "topleft");
  graphQuantile.add(drawCDF(data.getMin(), data.getMax()).getDrawable(0));
  graphQuantile.add(curveQuantile);
  NumericalPoint bb(graphQuantile.getBoundingBox());
  const NumericalScalar delta = bb[3] - bb[2];
  bb[3] += 0.05 * delta;
  bb[2] -= 0.05 * delta;
  graphQuantile.setBoundingBox(bb);
  Description legends(2);
  legends[0] = "iso-CDF";
  legends[1] = "quantile";
  graphQuantile.setLegends(legends);
  return graphQuantile;
}

/* Parameters value and description accessor */
DistributionImplementation::NumericalPointWithDescriptionCollection DistributionImplementation::getParametersCollection() const
{
  // Use compact accessor
  NumericalPointWithDescription parameters(getParameter());
  parameters.setDescription(getParameterDescription());
  parameters.setName(getDescription()[0]);
  return NumericalPointWithDescriptionCollection(1, parameters);
}

void DistributionImplementation::setParametersCollection(const NumericalPointWithDescriptionCollection & parametersCollection)
{
  if (getDimension() == 1)
  {
    if (parametersCollection.getSize() != 1) throw InvalidArgumentException(HERE) << "Expected collection of size 1, got " << parametersCollection.getSize();
    setParameter(parametersCollection[0]);
  }

  // Get the actual collection of parameters to check the description and the size
  const NumericalPointWithDescriptionCollection actualParameters(getParametersCollection());
  const UnsignedInteger size = actualParameters.getSize();
  if (parametersCollection.getSize() != size) throw InvalidArgumentException(HERE) << "Error: the given parameters collection has an invalid size (" << parametersCollection.getSize() << "), it should be " << size;
  NumericalPointCollection coll(0);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger dimension = actualParameters[i].getDimension();
    if (parametersCollection[i].getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given parameters collection has an invalid dimension at index " << i;
    coll.add(parametersCollection[i]);
  }
  setParametersCollection(coll);
}

void DistributionImplementation::setParametersCollection(const NumericalPointCollection & parametersCollection)
{
  const UnsignedInteger size = parametersCollection.getSize();
  NumericalPoint newParameters;
  for (UnsignedInteger i = 0; i < size; ++ i) newParameters.add(parametersCollection[i]);
  setParameter(newParameters);
}

/* Parameters value accessor */
NumericalPoint DistributionImplementation::getParameter() const
{
  return NumericalPoint();
}

void DistributionImplementation::setParameter(const NumericalPoint & parameters)
{
  if (parameters.getSize() != 0) throw InvalidArgumentException(HERE) << "Error: expected 0 parameters, got " << parameters.getSize();
}

/* Parameters description accessor */
Description DistributionImplementation::getParameterDescription() const
{
  return Description();
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
    LOGINFO(OSS() << "Warning! The description of the distribution " << getName() << " is " << description << " and cannot identify uniquely the marginal distribution. Use default description instead.");
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
NumericalScalar DistributionImplementation::getPDFEpsilon() const
{
  return pdfEpsilon_;
}

/* Accessor to CDF computation precision */
NumericalScalar DistributionImplementation::getCDFEpsilon() const
{
  return cdfEpsilon_;
}

/* Get a positon indicator for a 1D distribution */
NumericalScalar DistributionImplementation::getPositionIndicator() const
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
    return computeQuantile(0.5)[0];
  }
}

/* Get a dispersion indicator for a 1D distribution */
NumericalScalar DistributionImplementation::getDispersionIndicator() const
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
    return computeQuantile(0.75)[0] - computeQuantile(0.25)[0];
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
}

/* Transformation of distributions by usual functions */
DistributionImplementation::Implementation DistributionImplementation::cos() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  const NumericalScalar a = getRange().getLowerBound()[0];
  const NumericalScalar b = getRange().getUpperBound()[0];
  const SignedInteger nMin = static_cast< SignedInteger > (ceil(a / M_PI));
  const SignedInteger nMax = static_cast< SignedInteger > (floor(b / M_PI));
  NumericalPoint bounds(1, a);
  NumericalPoint values(1, std::cos(a));
  for (SignedInteger n = nMin; n <= nMax; ++n)
  {
    bounds.add(n * M_PI);
    values.add(n % 2 == 0 ? 1.0 : -1.0);
  }
  bounds.add(b);
  values.add(std::cos(b));
  return CompositeDistribution(NumericalMathFunction("x", "cos(x)"), clone(), bounds, values).clone();
}

DistributionImplementation::Implementation DistributionImplementation::sin() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  const NumericalScalar a = getRange().getLowerBound()[0];
  const NumericalScalar b = getRange().getUpperBound()[0];
  const SignedInteger nMin = static_cast< SignedInteger > (ceil(a / M_PI - 0.5));
  const SignedInteger nMax = static_cast< SignedInteger > (floor(b / M_PI - 0.5));
  NumericalPoint bounds(1, a);
  NumericalPoint values(1, std::sin(a));
  for (SignedInteger n = nMin; n <= nMax; ++n)
  {
    bounds.add((n + 0.5) * M_PI);
    values.add(n % 2 == 0 ? 1.0 : -1.0);
  }
  bounds.add(b);
  values.add(std::sin(b));
  return CompositeDistribution(NumericalMathFunction("x", "sin(x)"), clone(), bounds, values).clone();
}

DistributionImplementation::Implementation DistributionImplementation::tan() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  const NumericalScalar a = getRange().getLowerBound()[0];
  const NumericalScalar b = getRange().getUpperBound()[0];
  const SignedInteger nMin = static_cast< SignedInteger > (ceil(a / M_PI - 0.5));
  const SignedInteger nMax = static_cast< SignedInteger > (floor(b / M_PI - 0.5));
  // Compute the lower bound and upper bound of the support of tan(X)
  NumericalScalar sumPDF = 0.0;
  for (SignedInteger n = nMin; n <= nMax; ++n) sumPDF += computePDF((n + 0.5) * M_PI);
  const NumericalScalar bound = std::tan(M_PI_2 - quantileEpsilon_ / sumPDF);
  NumericalPoint bounds(1, a);
  NumericalPoint values(1, std::tan(a));
  for (SignedInteger n = nMin; n <= nMax; ++n)
  {
    bounds.add((n + 0.5) * M_PI);
    values.add(bound);
    bounds.add((n + 0.5) * M_PI);
    values.add(-bound);
  }
  bounds.add(b);
  values.add(std::tan(b));
  return CompositeDistribution(NumericalMathFunction("x", "tan(x)"), clone(), bounds, values).clone();
}

DistributionImplementation::Implementation DistributionImplementation::acos() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  const NumericalScalar a = getRange().getLowerBound()[0];
  if (a < -1.0) throw InvalidArgumentException(HERE) << "Error: cannot take the arc cos of a random variable that takes values less than -1 with positive probability.";
  const NumericalScalar b = getRange().getUpperBound()[0];
  if (b > 1.0) throw InvalidArgumentException(HERE) << "Error: cannot take the arc cos of a random variable that takes values greater than 1 with positive probability.";
  NumericalPoint bounds(1, a);
  NumericalPoint values(1, std::acos(a));
  bounds.add(b);
  values.add(std::acos(b));
  return CompositeDistribution(NumericalMathFunction("x", "acos(x)"), clone(), bounds, values).clone();
}

DistributionImplementation::Implementation DistributionImplementation::asin() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  const NumericalScalar a = getRange().getLowerBound()[0];
  if (a < -1.0) throw InvalidArgumentException(HERE) << "Error: cannot take the arc sin of a random variable that takes values less than -1 with positive probability.";
  const NumericalScalar b = getRange().getUpperBound()[0];
  if (b > 1.0) throw InvalidArgumentException(HERE) << "Error: cannot take the arc sin of a random variable that takes values greater than 1 with positive probability.";
  NumericalPoint bounds(1, a);
  NumericalPoint values(1, std::asin(a));
  bounds.add(b);
  values.add(std::asin(b));
  return CompositeDistribution(NumericalMathFunction("x", "asin(x)"), clone(), bounds, values).clone();
}

DistributionImplementation::Implementation DistributionImplementation::atan() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  const NumericalScalar a = getRange().getLowerBound()[0];
  NumericalPoint bounds(1, a);
  NumericalPoint values(1, std::atan(a));
  const NumericalScalar b = getRange().getUpperBound()[0];
  bounds.add(b);
  values.add(std::atan(b));
  return CompositeDistribution(NumericalMathFunction("x", "atan(x)"), clone(), bounds, values).clone();
}

DistributionImplementation::Implementation DistributionImplementation::cosh() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  const NumericalScalar a = getRange().getLowerBound()[0];
  NumericalPoint bounds(1, a);
  NumericalPoint values(1, std::cosh(a));
  const NumericalScalar b = getRange().getUpperBound()[0];
  if ((a < 0.0) && (b > 0.0))
  {
    bounds.add(0.0);
    values.add(1.0);
  }
  bounds.add(b);
  values.add(std::cosh(b));
  return CompositeDistribution(NumericalMathFunction("x", "cosh(x)"), clone(), bounds, values).clone();
}

DistributionImplementation::Implementation DistributionImplementation::sinh() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  const NumericalScalar a = getRange().getLowerBound()[0];
  const NumericalScalar b = getRange().getUpperBound()[0];
  NumericalPoint bounds(1, a);
  NumericalPoint values(1, std::sinh(a));
  bounds.add(b);
  values.add(std::sinh(b));
  return CompositeDistribution(NumericalMathFunction("x", "sinh(x)"), clone(), bounds, values).clone();
}

DistributionImplementation::Implementation DistributionImplementation::tanh() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  const NumericalScalar a = getRange().getLowerBound()[0];
  const NumericalScalar b = getRange().getUpperBound()[0];
  NumericalPoint bounds(1, a);
  NumericalPoint values(1, std::tanh(a));
  bounds.add(b);
  values.add(std::tanh(b));
  return CompositeDistribution(NumericalMathFunction("x", "tanh(x)"), clone(), bounds, values).clone();
}

DistributionImplementation::Implementation DistributionImplementation::acosh() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  const NumericalScalar a = getRange().getLowerBound()[0];
  if (a < 1.0) throw InvalidArgumentException(HERE) << "Error: cannot take the arc cosh of a random variable that takes values less than 1 with positive probability.";
  const NumericalScalar b = getRange().getUpperBound()[0];
  NumericalPoint bounds(1, a);
  NumericalPoint values(1, SpecFunc::Acosh(a));
  bounds.add(b);
  values.add(SpecFunc::Acosh(b));
  return CompositeDistribution(NumericalMathFunction("x", "acosh(x)"), clone(), bounds, values).clone();
}

DistributionImplementation::Implementation DistributionImplementation::asinh() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  const NumericalScalar a = getRange().getLowerBound()[0];
  const NumericalScalar b = getRange().getUpperBound()[0];
  NumericalPoint bounds(1, a);
  NumericalPoint values(1, SpecFunc::Asinh(a));
  bounds.add(b);
  values.add(SpecFunc::Asinh(b));
  return CompositeDistribution(NumericalMathFunction("x", "asinh(x)"), clone(), bounds, values).clone();
}

DistributionImplementation::Implementation DistributionImplementation::atanh() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  const NumericalScalar a = getRange().getLowerBound()[0];
  if (a < -1.0) throw InvalidArgumentException(HERE) << "Error: cannot take the arc tanh of a random variable that takes values less than -1 with positive probability.";
  const NumericalScalar b = getRange().getUpperBound()[0];
  if (b > 1.0) throw InvalidArgumentException(HERE) << "Error: cannot take the arc tanh of a random variable that takes values greater than 1 with positive probability.";
  NumericalPoint bounds(1, a);
  // F_Y(y)=P(atanh(X)<y)<->P(X<tanh(y))=F_X(tanh(y))
  // y s.t. F_Y(y)=epsilon<->y=atanh(F_X^{-1}(epsilon))

  NumericalPoint values(1, a == -1.0 ? SpecFunc::Atanh(computeQuantile(quantileEpsilon_)[0]) : SpecFunc::Atanh(a));
  bounds.add(b);
  values.add(b == 1.0 ? SpecFunc::Atanh(computeQuantile(quantileEpsilon_, true)[0]) : SpecFunc::Atanh(b));
  return CompositeDistribution(NumericalMathFunction("x", "atanh(x)"), clone(), bounds, values).clone();
}

DistributionImplementation::Implementation DistributionImplementation::exp() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  // Check if we can reuse an existing class
  if (getClassName() == "Normal")
  {
    NumericalPoint parameters(getParameter());
    return LogNormal(parameters[0], parameters[1]).clone();
  }
  if (getClassName() == "Uniform")
  {
    NumericalPoint parameters(getParameter());
    return LogUniform(parameters[0], parameters[1]).clone();
  }
  const NumericalScalar a = getRange().getLowerBound()[0];
  const NumericalScalar b = getRange().getUpperBound()[0];
  NumericalPoint bounds(1, a);
  NumericalPoint values(1, std::exp(a));
  bounds.add(b);
  values.add(std::exp(b));
  return CompositeDistribution(NumericalMathFunction("x", "exp(x)"), clone(), bounds, values).clone();
}

DistributionImplementation::Implementation DistributionImplementation::log() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  // Check if we can reuse an existing class
  if (getClassName() == "LogNormal")
  {
    NumericalPoint parameters(getParameter());
    if (parameters[2] == 0.0) return Normal(parameters[0], parameters[1]).clone();
  }
  if (getClassName() == "LogUniform")
  {
    NumericalPoint parameters(getParameter());
    return Uniform(parameters[0], parameters[1]).clone();
  }
  const NumericalScalar a = getRange().getLowerBound()[0];
  if (a < 0.0) throw NotDefinedException(HERE) << "Error: cannot take the logarithm of a random variable that takes negative values with positive probability.";
  const NumericalScalar b = getRange().getUpperBound()[0];
  NumericalPoint bounds(1, a);
  NumericalPoint values(1, (a == 0.0 ? std::log(computeQuantile(quantileEpsilon_)[0]) : std::log(a)));
  bounds.add(b);
  values.add(std::log(b));
  return CompositeDistribution(NumericalMathFunction("x", "log(x)"), clone(), bounds, values).clone();
}

DistributionImplementation::Implementation DistributionImplementation::ln() const
{
  return log();
}

DistributionImplementation::Implementation DistributionImplementation::pow(const NumericalScalar exponent) const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  // First, the case where the exponent is integer
  if (trunc(exponent) == exponent) return pow(static_cast< SignedInteger >(trunc(exponent)));
  const NumericalScalar a = getRange().getLowerBound()[0];
  if (a < 0.0) throw NotDefinedException(HERE) << "Error: cannot take a fractional power of a random variable that takes negative values with positive probability.";

  NumericalMathFunction toPower("x", String(OSS() << (exponent < 0.0 ? "x^(" : "x^") << exponent << (exponent < 0.0 ? ")" : "")));
  NumericalPoint bounds(1, a);
  NumericalPoint values(1, (a == 0.0 ? (exponent < 0.0 ? std::pow(computeQuantile(quantileEpsilon_)[0], exponent) : 0.0) : std::pow(a, exponent)));
  const NumericalScalar b = getRange().getUpperBound()[0];
  bounds.add(b);
  values.add(std::pow(b, exponent));
  return CompositeDistribution(toPower, clone(), bounds, values).clone();
}

DistributionImplementation::Implementation DistributionImplementation::pow(const SignedInteger exponent) const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  if (exponent == 0.0) return Dirac(NumericalPoint(1, 1.0)).clone();
  const NumericalScalar a = getRange().getLowerBound()[0];
  NumericalMathFunction toPower("x", String(OSS() << (exponent < 0.0 ? "x^(" : "x^") << exponent << (exponent < 0.0 ? ")" : "")));
  // Easy case: a >= 0
  if (a >= 0.0)
  {
    NumericalPoint bounds(1, a);
    NumericalPoint values(1, (a == 0.0 ? (exponent < 0.0 ? std::pow(computeQuantile(quantileEpsilon_)[0], 1.0 * exponent) : 0.0) : std::pow(a, 1.0 * exponent)));
    const NumericalScalar b = getRange().getUpperBound()[0];
    bounds.add(b);
    values.add(std::pow(b, 1.0 * exponent));
    return CompositeDistribution(toPower, clone(), bounds, values).clone();
  }
  // Easy case: b <= 0
  NumericalPoint bounds(1, a);
  NumericalPoint values(1, std::pow(a, 1.0 * exponent));
  const NumericalScalar b = getRange().getUpperBound()[0];
  if (b <= 0.0)
  {
    bounds.add(b);
    values.add(b == 0.0 ? (exponent < 0.0 ? std::pow(computeQuantile(quantileEpsilon_, true)[0], 1.0 * exponent) : 0.0) : std::pow(b, 1.0 * exponent));
    return CompositeDistribution(toPower, clone(), bounds, values).clone();
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
      return CompositeDistribution(toPower, clone(), bounds, values).clone();
    }
    // A singularity at 0 for negative exponent
    bounds.add(0.0);
    values.add(-SpecFunc::MaxNumericalScalar);
    bounds.add(0.0);
    values.add(SpecFunc::MaxNumericalScalar);
    bounds.add(b);
    values.add(std::pow(b, 1.0 * exponent));
    return CompositeDistribution(NumericalMathFunction("x", String(OSS() << "x^(" << exponent << ")")), clone(), bounds, values).clone();
  }
  // For even exponent, the behaviour changes at 0
  bounds.add(0.0);
  values.add(exponent > 0 ? 0.0 : SpecFunc::MaxNumericalScalar);
  bounds.add(b);
  values.add(std::pow(b, 1.0 * exponent));
  return CompositeDistribution(toPower, clone(), bounds, values).clone();
}

DistributionImplementation::Implementation DistributionImplementation::sqr() const
{
  // Check if we can reuse an existing class
  if (getClassName() == "Chi")
  {
    NumericalPoint parameters(getParameter());
    return ChiSquare(parameters[0]).clone();
  }
  return pow(static_cast< SignedInteger >(2));
}

DistributionImplementation::Implementation DistributionImplementation::inverse() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  const NumericalScalar a = getRange().getLowerBound()[0];
  NumericalPoint bounds(1, a);
  // Easy case: a >= 0
  if (a >= 0.0)
  {
    NumericalPoint values(1, (a == 0.0 ? 1.0 / computeQuantile(quantileEpsilon_)[0] : 1.0 / a));
    const NumericalScalar b = getRange().getUpperBound()[0];
    bounds.add(b);
    if (getRange().getFiniteUpperBound()[0])
      values.add(1.0 / b);
    else
      values.add(0.0);
    return CompositeDistribution(NumericalMathFunction("x", "1.0 / x"), clone(), bounds, values).clone();
  }
  // Here, a < 0
  NumericalPoint values(1);
  if (getRange().getFiniteLowerBound()[0])
    values[0] = 1.0 / a;
  else
    values[0] = 0.0;
  const NumericalScalar b = getRange().getUpperBound()[0];
  // Easy case: b <= 0
  if (b <= 0.0)
  {
    bounds.add(b);
    values.add(b == 0.0 ? 1.0 / computeQuantile(quantileEpsilon_, true)[0] : 1.0 / b);
    return CompositeDistribution(NumericalMathFunction("x", "1.0 / x"), clone(), bounds, values).clone();
  }
  // Difficult case: a < 0 < b
  // A singularity at 0
  bounds.add(0.0);
  // The CDF of Y=1/X is
  // F_Y(y)=[F_X(0) - F_X(1 / y)]1_{y < 0} +
  //        [F_X(0) + 1 - F_X(1 / y)]1_{y > 0} +
  //        F_X(0)1_{y = 0}
  // so the bounds for Y are obtained when X->0^- and X->0^+
  values.add(1.0 / computeQuantile(computeCDF(0.0) - quantileEpsilon_)[0]);
  bounds.add(0.0);
  values.add(1.0 / computeQuantile(computeCDF(0.0) + quantileEpsilon_)[0]);
  bounds.add(b);
  if (getRange().getFiniteUpperBound()[0])
    values.add(1.0 / b);
  else
    values.add(0.0);
  return CompositeDistribution(NumericalMathFunction("x", "1.0 / x"), clone(), bounds, values).clone();
}

DistributionImplementation::Implementation DistributionImplementation::sqrt() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  // Check if we can reuse an existing class
  if (getClassName() == "ChiSquare")
  {
    NumericalPoint parameters(getParameter());
    return Chi(parameters[0]).clone();
  }
  const NumericalScalar a = getRange().getLowerBound()[0];
  if (a < 0.0) throw NotDefinedException(HERE) << "Error: cannot take the square root of a random variable that takes negative values with positive probability.";
  NumericalPoint bounds(1, a);
  NumericalPoint values(1, std::sqrt(a));
  const NumericalScalar b = getRange().getUpperBound()[0];
  bounds.add(b);
  values.add(std::sqrt(b));
  return CompositeDistribution(NumericalMathFunction("x", "sqrt(x)"), clone(), bounds, values).clone();
}

DistributionImplementation::Implementation DistributionImplementation::cbrt() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  const NumericalScalar a = getRange().getLowerBound()[0];
  NumericalPoint bounds(1, a);
  NumericalPoint values(1, SpecFunc::Cbrt(a));
  const NumericalScalar b = getRange().getUpperBound()[0];
  bounds.add(b);
  values.add(SpecFunc::Cbrt(b));
  return CompositeDistribution(NumericalMathFunction("x", "cbrt(x)"), clone(), bounds, values).clone();
}

DistributionImplementation::Implementation DistributionImplementation::abs() const
{
  if (getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distribution must be univariate.";
  const NumericalScalar a = getRange().getLowerBound()[0];
  NumericalPoint bounds(1, a);
  NumericalPoint values(1, std::abs(a));
  const NumericalScalar b = getRange().getUpperBound()[0];
  if ((a < 0.0) && (b > 0.0))
  {
    bounds.add(0.0);
    values.add(0.0);
  }
  bounds.add(b);
  values.add(std::abs(b));
  return CompositeDistribution(NumericalMathFunction("x", "abs(x)"), clone(), bounds, values).clone();
}

END_NAMESPACE_OPENTURNS

