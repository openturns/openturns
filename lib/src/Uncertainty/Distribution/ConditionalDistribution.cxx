//                                               -*- C++ -*-
/**
 *  @brief The ConditionalDistribution distribution
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include "ConditionalDistribution.hxx"
#include "ComposedDistribution.hxx"
#include "Dirac.hxx"
#include "Uniform.hxx"
#include "Box.hxx"
#include "GaussProductExperiment.hxx"
#include "PersistentObjectFactory.hxx"
#include "SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ConditionalDistribution);

static const Factory<ConditionalDistribution> RegisteredFactory;


/* Default constructor */
ConditionalDistribution::ConditionalDistribution()
  : Mixture()
  , conditionedDistribution_()
  , conditioningDistribution_()
  , linkFunction_()
  , discreteMarginalsIndices_(0)
  , diracMarginalsIndices_(0)
  , continuousMarginalsIndices_(0)
  , continuousLowerBounds_(0)
  , continuousUpperBounds_(0)
  , continuousNodes_()
  , continuousWeights_(0)
  , discreteNodes_()
  , diracValues_(0)
{
  setName("ConditionalDistribution");
  const Description inVars(Description::BuildDefault(1, "y"));
  const Description outVars(Description::BuildDefault(2, "theta"));
  Description formulas(2);
  formulas[0] = inVars[0];
  formulas[1] = String(OSS() << inVars[0] << " + 1");
  setConditionedAndConditioningDistributionsAndLinkFunction(Uniform(), Uniform(), NumericalMathFunction(inVars, outVars, formulas));
  isParallel_ = false;
}

/* Parameters constructor */
ConditionalDistribution::ConditionalDistribution(const Distribution & conditionedDistribution,
    const Distribution & conditioningDistribution)
  : Mixture()
  , conditionedDistribution_()
  , conditioningDistribution_()
  , linkFunction_()
  , discreteMarginalsIndices_(0)
  , diracMarginalsIndices_(0)
  , continuousMarginalsIndices_(0)
  , continuousLowerBounds_(0)
  , continuousUpperBounds_(0)
  , continuousNodes_()
  , continuousWeights_(0)
  , discreteNodes_()
  , diracValues_(0)
{
  setName("ConditionalDistribution");
  // The dimension and range are computed using the upper class through this call
  setConditionedAndConditioningDistributionsAndLinkFunction(conditionedDistribution, conditioningDistribution, NumericalMathFunction(Description::BuildDefault(conditioningDistribution.getDimension(), "y"), Description::BuildDefault(conditioningDistribution.getDimension(), "theta"), Description::BuildDefault(conditioningDistribution.getDimension(), "y")));
}

/* Parameters constructor */
ConditionalDistribution::ConditionalDistribution(const Distribution & conditionedDistribution,
    const Distribution & conditioningDistribution,
    const NumericalMathFunction & linkFunction)
  : Mixture()
  , conditionedDistribution_()
  , conditioningDistribution_()
  , linkFunction_()
  , discreteMarginalsIndices_(0)
  , diracMarginalsIndices_(0)
  , continuousMarginalsIndices_(0)
  , continuousLowerBounds_(0)
  , continuousUpperBounds_(0)
  , continuousNodes_()
  , continuousWeights_(0)
  , discreteNodes_()
  , diracValues_(0)
{
  setName("ConditionalDistribution");
  // The dimension and range are computed using the upper class through this call
  setConditionedAndConditioningDistributionsAndLinkFunction(conditionedDistribution, conditioningDistribution, linkFunction);
}

/* Comparison operator */
Bool ConditionalDistribution::operator ==(const ConditionalDistribution & other) const
{
  if (this == &other) return true;
  return (conditionedDistribution_ == other.conditionedDistribution_) && (conditioningDistribution_ == other.conditioningDistribution_) && (linkFunction_ == other.linkFunction_);
}

/* String converter */
String ConditionalDistribution::__repr__() const
{
  OSS oss(true);
  oss << "class=" << ConditionalDistribution::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " conditioned distribution=" << conditionedDistribution_
      << " conditioning distribution=" << conditioningDistribution_
      << " link function=" << linkFunction_;
  return oss;
}

String ConditionalDistribution::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(X with X|Theta~" << conditionedDistribution_.getImplementation()->getClassName() << "(Theta), Theta=f(Y), f=" << linkFunction_.getEvaluation()->__str__() << ", Y~" << conditioningDistribution_.__str__() << ")";
  return oss;
}

/* Virtual constructor */
ConditionalDistribution * ConditionalDistribution::clone() const
{
  return new ConditionalDistribution(*this);
}

/* Get one realization of the distribution */
NumericalPoint ConditionalDistribution::getRealization() const
{
  Distribution deconditioned(conditionedDistribution_);
  deconditioned.setParameters(linkFunction_(conditioningDistribution_.getRealization()));
  return deconditioned.getRealization();
}

ConditionalDistribution::NumericalPointWithDescriptionCollection ConditionalDistribution::getParametersCollection() const
{
  return conditioningDistribution_.getParametersCollection();
}

/* Parameters value accessor */
NumericalPoint ConditionalDistribution::getParameters() const
{
  return conditioningDistribution_.getParameters();
}

void ConditionalDistribution::setParameters(const NumericalPoint & parameters)
{
  Distribution conditioningDistribution(conditioningDistribution_);
  conditioningDistribution.setParameters(parameters);
  const NumericalScalar w = getWeight();
  Distribution conditionedDistribution(conditionedDistribution_);
  *this = ConditionalDistribution(conditionedDistribution, conditioningDistribution);
  setWeight(w);
}

Description ConditionalDistribution::getParametersDescription() const
{
  return conditioningDistribution_.getParametersDescription();
}

/* Conditioned distribution accessor */
void ConditionalDistribution::setConditionedDistribution(const Distribution & conditionedDistribution)
{
  if (conditionedDistribution != conditionedDistribution_) setConditionedAndConditioningDistributionsAndLinkFunction(conditionedDistribution, conditioningDistribution_, linkFunction_);
}

Distribution ConditionalDistribution::getConditionedDistribution() const
{
  return conditionedDistribution_;
}


/* Conditioning distribution accessor */
void ConditionalDistribution::setConditioningDistribution(const Distribution & conditioningDistribution)
{
  if (conditioningDistribution != conditioningDistribution_) setConditionedAndConditioningDistributionsAndLinkFunction(conditionedDistribution_, conditioningDistribution, linkFunction_);
}

Distribution ConditionalDistribution::getConditioningDistribution() const
{
  return conditioningDistribution_;
}

/* Link function accessor */
void ConditionalDistribution::setLinkFunction(const NumericalMathFunction & linkFunction)
{
  if (!(linkFunction == linkFunction_)) setConditionedAndConditioningDistributionsAndLinkFunction(conditionedDistribution_, conditioningDistribution_, linkFunction);
}

NumericalMathFunction ConditionalDistribution::getLinkFunction() const
{
  return linkFunction_;
}


void ConditionalDistribution::setConditionedAndConditioningDistributionsAndLinkFunction(const Distribution & conditionedDistribution,
    const Distribution & conditioningDistribution,
    const NumericalMathFunction & linkFunction)
{
  const UnsignedInteger conditioningDimension(conditioningDistribution.getDimension());
  const UnsignedInteger conditionedParametersDimension(conditionedDistribution.getParametersNumber());
  // We must check that the conditioning distribution has the same dimension as the input dimension of the link function and that the conditioning distribution has the same dimension as the input dimension of the link function
  if (conditionedParametersDimension != linkFunction.getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: expected a link function with output dimension equal to the number of parameters of the conditioned distribution.";
  if (conditioningDimension != linkFunction.getInputDimension()) throw InvalidArgumentException(HERE) << "Error: expected a link function with input dimension equal to the conditioning distribution dimension.";
  conditionedDistribution_ = conditionedDistribution;
  conditioningDistribution_ = conditioningDistribution;
  linkFunction_ = linkFunction;
  setDimension(conditioningDimension + conditionedDistribution.getDimension());
  // Start the discretisation into a Mixture
  const Interval bounds(conditioningDistribution.getRange());
  // Here, implements some knowledge based selection of the integration method
  // For now, only basic Legendre
  // Gather the indices of the discrete marginals
  // Analyse the marginal distributions
  Collection< NumericalSample > discreteSupports(0, NumericalSample(0, 1));
  for (UnsignedInteger i = 0; i < conditioningDimension; ++i)
  {
    const Distribution marginal(conditioningDistribution.getMarginal(i));
    // Continuous marginal: get the associated range
    if (marginal.isContinuous())
    {
      continuousMarginalsIndices_.add(i);
      continuousLowerBounds_.add(marginal.getRange().getLowerBound()[0]);
      continuousUpperBounds_.add(marginal.getRange().getUpperBound()[0]);
    } // Continuous marginal
    // Discrete marginal
    else
    {
      const NumericalSample support(marginal.getSupport());
      // Special case for Dirac distributions. It can be either a Dirac distribution or a UserDefined distribution, so we check the support directly.
      if (support.getSize() == 1)
      {
        diracMarginalsIndices_.add(i);
        diracValues_.add(support[0][0]);
      } // Dirac marginal
      // Truly random discrete distribution
      else
      {
        discreteMarginalsIndices_.add(i);
        discreteSupports.add(support);
      } // Random discrete marginal
    } // Discrete marginal
  } // Loop over the marginal distributions
  // Integration measure for the continuous parameters
  const UnsignedInteger continuousDimension(continuousMarginalsIndices_.getSize());
  UnsignedInteger continuousAtomsNumber(0);
  if (continuousDimension > 0)
  {
    const ComposedDistribution measure(Collection< Distribution >(continuousDimension, Uniform()));
    const UnsignedInteger maximumNumber(static_cast< UnsignedInteger > (round(std::pow(ResourceMap::GetAsUnsignedInteger( "ConditionalDistribution-MaximumIntegrationNodesNumber" ), 1.0 / continuousDimension))));
    const UnsignedInteger candidateNumber(ResourceMap::GetAsUnsignedInteger( "ConditionalDistribution-MarginalIntegrationNodesNumber" ));
    if (candidateNumber > maximumNumber) LOGWARN(OSS() << "Warning! The requested number of marginal integration nodes=" << candidateNumber << " would lead to an excessive number of integration nodes=" << std::pow(maximumNumber, 1.0 * continuousDimension) << ". It has been reduced to " << maximumNumber << ". You should increase the ResourceMap key \"ConditionalDistribution-MaximumIntegrationNodesNumber\"");

    GaussProductExperiment experiment(measure, Indices(continuousDimension, std::min(maximumNumber, candidateNumber)));
    continuousNodes_ = experiment.generateWithWeights(continuousWeights_);
    // Also adapt the integration nodes number in the upper class
    setIntegrationNodesNumber(std::min(maximumNumber, candidateNumber));
    // Normalization factor for the weights
    // Not needed as the Mixture will be automatically normalized
    // const NumericalScalar normalizationFactor(Interval(continuousLowerBounds, continuousUpperBounds).getNumericalVolume());
    continuousAtomsNumber = continuousNodes_.getSize();
  } // continuousDimension > 0

  // Integration measure for the discrete parameters
  const UnsignedInteger discreteDimension(discreteMarginalsIndices_.getSize());
  UnsignedInteger discreteAtomsNumber(0);
  if (discreteDimension > 0)
  {
    NumericalPoint levels(discreteDimension);
    for (UnsignedInteger i = 0; i < discreteDimension; ++i) levels[i] = discreteSupports[i].getSize() - 2;
    const NumericalSample fractions(Box(levels).generate());
    discreteAtomsNumber = fractions.getSize();
    discreteNodes_ = NumericalSample(discreteAtomsNumber, discreteDimension);
    for (UnsignedInteger i = 0; i < discreteAtomsNumber; ++i)
    {
      NumericalPoint discreteNode(discreteDimension);
      for (UnsignedInteger j = 0; j < discreteDimension; ++j)
      {
        const NumericalScalar rho(fractions[i][j]);
        const UnsignedInteger length(discreteSupports[j].getSize());
        const UnsignedInteger index(static_cast<UnsignedInteger>(round(rho * (length - 1))));
        discreteNode[j] = discreteSupports[j][index][0];
      }
      discreteNodes_[i] = discreteNode;
    } // Loop over the discrete atoms
  } // discreteDimension > 0
  // Integration measure for the Dirac parameters
  const UnsignedInteger diracDimension(diracMarginalsIndices_.getSize());
  // Build the equivalent mixture
  // Zeroth case: all Dirac
  if (diracDimension == conditioningDimension)
  {
    Collection< Distribution > atoms(1, conditionedDistribution);
    atoms[0].setParameters(diracValues_);
    setDistributionCollection(atoms);
    return;
  }
  // First case: only Dirac and stochastic discrete marginals with at least one stochastic discrete marginal
  if (continuousDimension == 0)
  {
    const UnsignedInteger totalSize(discreteAtomsNumber);
    Collection< Distribution > atoms(totalSize);
    NumericalPoint y(conditioningDimension);
    UnsignedInteger atomIndex(0);
    // First, the Dirac components.
    for (UnsignedInteger i = 0; i < diracDimension; ++i)
      y[diracMarginalsIndices_[i]] = diracValues_[i];
    // Second, the discrete components
    // For each combination of the discrete components
    for (UnsignedInteger i = 0; i < discreteAtomsNumber; ++i)
    {
      NumericalPoint currentY(y);
      // Get the discrete values
      const NumericalPoint discreteNode(discreteNodes_[i]);
      for (UnsignedInteger j = 0; j < discreteDimension; ++j)
        currentY[discreteMarginalsIndices_[j]] = discreteNode[j];
      const NumericalScalar w(conditioningDistribution.computePDF(currentY));
      Distribution dist(conditionedDistribution);
      dist.setWeight(w);
      dist.setParameters(linkFunction_(currentY));
      atoms[atomIndex] = dist;
      ++atomIndex;
    } // Discrete measure
    // Now, update the underlying Mixture
    setDistributionCollection(atoms);
    return;
  } // No continuous marginal
  // Second case: only Dirac and continuous marginals with at least one continuous marginal
  if (discreteDimension == 0)
  {
    const UnsignedInteger totalSize(continuousAtomsNumber);
    Collection< Distribution > atoms(totalSize);
    NumericalPoint y(conditioningDimension);
    UnsignedInteger atomIndex(0);
    // First, the Dirac components
    for (UnsignedInteger i = 0; i < diracDimension; ++i)
      y[diracMarginalsIndices_[i]] = diracValues_[i];
    // Continuous part using Gauss integration
    for (UnsignedInteger i = 0; i < continuousAtomsNumber; ++i)
    {
      NumericalPoint currentY(y);
      // Complete the filling of theta using the Gauss integration node
      const NumericalPoint continuousNode(continuousNodes_[i]);
      for (UnsignedInteger j = 0; j < continuousDimension; ++j)
        currentY[continuousMarginalsIndices_[j]] = continuousLowerBounds_[j] + 0.5 * (1.0 + continuousNode[j]) * (continuousUpperBounds_[j] - continuousLowerBounds_[j]);
      const NumericalScalar w(conditioningDistribution.computePDF(currentY) * continuousWeights_[i]);
      Distribution dist(conditionedDistribution);
      dist.setWeight(w);
      dist.setParameters(linkFunction_(currentY));
      atoms[atomIndex] = dist;
      LOGDEBUG(OSS() << "i=" << i << ", w=" << w << ", Y=" << currentY << ", dist=" << dist.__str__());
      ++atomIndex;
    } // Continuous measure
    // Now, update the underlying Mixture
    setDistributionCollection(atoms);
    return;
  } // No discrete marginal
  // Third case: Dirac, stochastic discrete and continuous marginal distributions with at least one stochastic discrete marginal and one continuous marginal
  const UnsignedInteger totalSize(continuousAtomsNumber * discreteAtomsNumber);
  Collection< Distribution > atoms(totalSize);
  NumericalPoint y(conditioningDimension);
  UnsignedInteger atomIndex(0);
  // First, the Dirac components
  for (UnsignedInteger i = 0; i < diracDimension; ++i)
    y[diracMarginalsIndices_[i]] = diracValues_[i];
  // Second, the discrete components
  // For each combination of the discrete components
  for (UnsignedInteger i = 0; i < discreteAtomsNumber; ++i)
  {
    NumericalPoint currentY(y);
    // Get the discrete values
    const NumericalPoint discreteNode(discreteNodes_[i]);
    for (UnsignedInteger j = 0; j < discreteDimension; ++j)
      currentY[discreteMarginalsIndices_[j]] = discreteNode[j];
    // Now, complete by the discretization of the continuous part using Gauss integration
    for (UnsignedInteger j = 0; j < continuousAtomsNumber; ++j)
    {
      // Complete the filling of theta using the Gauss integration node
      const NumericalPoint continuousNode(continuousNodes_[j]);
      for (UnsignedInteger k = 0; k < continuousDimension; ++k)
        currentY[continuousMarginalsIndices_[k]] = continuousLowerBounds_[k] + 0.5 * (1.0 + continuousNode[k]) * (continuousUpperBounds_[k] - continuousLowerBounds_[k]);
      const NumericalScalar w(conditioningDistribution.computePDF(currentY) * continuousWeights_[j]);
      Distribution dist(conditionedDistribution);
      dist.setWeight(w);
      dist.setParameters(linkFunction_(currentY));
      atoms[atomIndex] = dist;
      ++atomIndex;
    } // Continuous atoms
  } // Overall discretization
  // Now, update the underlying Mixture
  setDistributionCollection(atoms);
}

/* Compute the expectation of f(\theta)1_{\theta\leq \theta^*} with respect to the prior distribution of \theta */
NumericalPoint ConditionalDistribution::computeExpectation(const NumericalMathFunction & f,
    const NumericalPoint & thetaStar) const
{
  const NumericalScalar epsilon(ResourceMap::GetAsNumericalScalar("DiscreteDistribution-SupportEpsilon"));
  const UnsignedInteger conditioningDimension(conditioningDistribution_.getDimension());
  if (f.getInputDimension() != conditioningDimension) throw InvalidArgumentException(HERE) << "Error: the given function must have an input dimension=" << f.getInputDimension() << " equal to the conditioning dimension=" << conditioningDimension;
  if (thetaStar.getDimension() != conditioningDimension) throw InvalidArgumentException(HERE) << "Error: the given upper bound must have a dimension=" << thetaStar.getDimension() << " equal to the conditioning dimension=" << conditioningDimension;
  const UnsignedInteger outputDimension(f.getOutputDimension());
  NumericalPoint result(outputDimension);
  // Here, we reuse the analysis made in the underlying conditional distribution
  const UnsignedInteger continuousDimension(continuousMarginalsIndices_.getSize());
  const UnsignedInteger continuousAtomsNumber(continuousNodes_.getSize());
  const UnsignedInteger discreteDimension(discreteMarginalsIndices_.getSize());
  const UnsignedInteger discreteAtomsNumber(discreteNodes_.getSize());
  const UnsignedInteger diracDimension(diracMarginalsIndices_.getSize());
  // Compute the expectation by numerical integration
  // Zeroth case: all Dirac
  if (diracDimension == conditioningDimension)
  {
    for (UnsignedInteger i = 0; i < diracDimension; ++i)
      if (diracValues_[i] > thetaStar[i] + epsilon) return result;
    result = f(diracValues_);
    return result;
  }
  // First case: no continuous marginal
  if (continuousDimension == 0)
  {
    NumericalPoint theta(conditioningDimension);
    // First, the Dirac components. It can be either a Dirac distribution or a userDefined distribution, so we check the support directly
    for (UnsignedInteger i = 0; i < diracDimension; ++i)
    {
      const NumericalScalar value(diracValues_[i]);
      // If the hyper rectangle does not intersect the manifold that supports the total mass, then value = 0
      if (value > thetaStar[i] + epsilon) return result;
      theta[diracMarginalsIndices_[i]] = value;
    }
    // Second, the discrete components
    // For each combination of the discrete components
    for (UnsignedInteger i = 0; i < discreteAtomsNumber; ++i)
    {
      NumericalPoint currentTheta(theta);
      // Get the discrete values
      const NumericalPoint discreteNode(discreteNodes_[i]);
      Bool rejectNode = false;
      for (UnsignedInteger j = 0; j < discreteDimension; ++j)
      {
        const NumericalScalar value(discreteNode[j]);
        currentTheta[discreteMarginalsIndices_[j]] = value;
        rejectNode = (value > thetaStar[i] + epsilon);
        if (rejectNode) break;
      }
      // Skip the current integration point if the current sub-manifold is outside of the integration region
      if (rejectNode) continue;
      const NumericalScalar w(conditioningDistribution_.computePDF(currentTheta));
      result += w * f(currentTheta);
    } // Discrete measure
    return result;
  } // No continuous marginal

  // Second case: no discrete marginal
  if (discreteDimension == 0)
  {
    // The current value of theta in the discretization
    NumericalPoint theta(conditioningDimension);
    // The sub point associated with the continuous components
    NumericalPoint subPoint(continuousDimension);
    for (UnsignedInteger i = 0; i < continuousDimension; ++i)
      subPoint[i] = std::min(continuousUpperBounds_[i], thetaStar[continuousMarginalsIndices_[i]]);
    // First, the Dirac components
    for (UnsignedInteger i = 0; i < diracDimension; ++i)
    {
      const NumericalScalar value(diracValues_[i]);
      // If the hyper rectangle does not intersect the manifold that supports the total mass, then cdf = 0
      if (value > thetaStar[i] + epsilon) return result;
      theta[diracMarginalsIndices_[i]] = value;
    }
    // Continuous part using Gauss integration
    for (UnsignedInteger i = 0; i < continuousAtomsNumber; ++i)
    {
      NumericalPoint currentTheta(theta);
      // Complete the filling of theta using the Gauss integration node
      const NumericalPoint continuousNode(continuousNodes_[i]);
      for (UnsignedInteger j = 0; j < continuousDimension; ++j)
        currentTheta[continuousMarginalsIndices_[j]] = continuousLowerBounds_[j] + 0.5 * (1.0 + continuousNode[j]) * (subPoint[j] - continuousLowerBounds_[j]);
      // Current contribution to the CDF
      const NumericalScalar w(conditioningDistribution_.computePDF(currentTheta) * continuousWeights_[i]);
      const NumericalPoint fTheta(f(currentTheta));
      result += w * f(currentTheta);
    } // Continuous measure
    result *= Interval(continuousLowerBounds_, subPoint).getNumericalVolume();
    return result;
  } // No discrete marginal

  // Third case: continuous and discrete marginals
  NumericalPoint theta(conditioningDimension);
  // The sub point associated with the continuous components
  NumericalPoint subPoint(continuousDimension);
  for (UnsignedInteger i = 0; i < continuousDimension; ++i)
    subPoint[i] = std::min(continuousUpperBounds_[i], thetaStar[continuousMarginalsIndices_[i]]);
  // First, the Dirac components
  for (UnsignedInteger i = 0; i < diracDimension; ++i)
  {
    const NumericalScalar value(diracValues_[i]);
    // If the hyper rectangle does not intersect the manifold that supports the total mass, then cdf = 0
    if (value > thetaStar[i] + epsilon) return result;
    theta[diracMarginalsIndices_[i]] = value;
  }
  // Second, the discrete components
  // For each combination of the discrete components
  for (UnsignedInteger i = 0; i < discreteAtomsNumber; ++i)
  {
    NumericalPoint currentTheta(theta);
    // Get the discrete values
    const NumericalPoint discreteNode(discreteNodes_[i]);
    Bool rejectNode;
    for (UnsignedInteger j = 0; j < discreteDimension; ++j)
    {
      const NumericalScalar value(discreteNode[j]);
      currentTheta[discreteMarginalsIndices_[j]] = value;
      rejectNode = (value > thetaStar[i] + epsilon);
      if (rejectNode) break;
    }
    // Skip the current integration point if the current sub-manifold is outside of the integration region
    if (rejectNode) continue;
    // Now, complete by the discretization of the continuous part using Gauss integration
    for (UnsignedInteger j = 0; j < continuousAtomsNumber; ++j)
    {
      // Complete the filling of theta using the Gauss integration node
      const NumericalPoint continuousNode(continuousNodes_[j]);
      for (UnsignedInteger k = 0; k < continuousDimension; ++k)
        currentTheta[continuousMarginalsIndices_[k]] = continuousLowerBounds_[k] + 0.5 * (1.0 + continuousNode[k]) * (subPoint[k] - continuousLowerBounds_[k]);
      const NumericalScalar w(conditioningDistribution_.computePDF(currentTheta) * continuousWeights_[j]);
      const NumericalPoint fTheta(f(currentTheta));
      result += w * f(currentTheta);
    } // Continuous atoms
  } // Overall discretization
  result *= Interval(continuousLowerBounds_, subPoint).getNumericalVolume();
  return result;
}

/* Get the i-th marginal distribution */
ConditionalDistribution::Implementation ConditionalDistribution::getMarginal(const UnsignedInteger i) const
{
  const UnsignedInteger dimension(getDimension());
  if (i >= dimension) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  if (dimension == 1) return clone();
  // Waiting for a better implementation
  return Mixture::getMarginal(i);
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
ConditionalDistribution::Implementation ConditionalDistribution::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension(getDimension());
  if (!indices.check(dimension - 1)) throw InvalidArgumentException(HERE) << "The indices of a marginal distribution must be in the range [0, dim-1] and  must be different";
  if (dimension == 1) return clone();
  // Waiting for a better implementation
  return Mixture::getMarginal(indices);
} // getMarginal(Indices)

/* Method save() stores the object through the StorageManager */
void ConditionalDistribution::save(Advocate & adv) const
{
  Mixture::save(adv);
  adv.saveAttribute( "conditionedDistribution_", conditionedDistribution_ );
  adv.saveAttribute( "conditioningDistribution_", conditioningDistribution_ );
  adv.saveAttribute( "discreteMarginalsIndices_", discreteMarginalsIndices_ );
  adv.saveAttribute( "diracMarginalsIndices_", diracMarginalsIndices_ );
  adv.saveAttribute( "continuousMarginalsIndices_", continuousMarginalsIndices_ );
  adv.saveAttribute( "continuousLowerBounds_", continuousLowerBounds_ );
  adv.saveAttribute( "continuousUpperBounds_", continuousUpperBounds_ );
  adv.saveAttribute( "continuousNodes_", continuousNodes_ );
  adv.saveAttribute( "continuousWeights_", continuousWeights_ );
  adv.saveAttribute( "discreteNodes_", discreteNodes_ );
  adv.saveAttribute( "diracValues_", diracValues_ );
}

/* Method load() reloads the object from the StorageManager */
void ConditionalDistribution::load(Advocate & adv)
{
  Mixture::load(adv);
  adv.loadAttribute( "conditionedDistribution_", conditionedDistribution_ );
  adv.loadAttribute( "conditioningDistribution_", conditioningDistribution_ );
  adv.loadAttribute( "discreteMarginalsIndices_", discreteMarginalsIndices_ );
  adv.loadAttribute( "diracMarginalsIndices_", diracMarginalsIndices_ );
  adv.loadAttribute( "continuousMarginalsIndices_", continuousMarginalsIndices_ );
  adv.loadAttribute( "continuousLowerBounds_", continuousLowerBounds_ );
  adv.loadAttribute( "continuousUpperBounds_", continuousUpperBounds_ );
  adv.loadAttribute( "continuousNodes_", continuousNodes_ );
  adv.loadAttribute( "continuousWeights_", continuousWeights_ );
  adv.loadAttribute( "discreteNodes_", discreteNodes_ );
  adv.loadAttribute( "diracValues_", diracValues_ );
  // The range is computed using the upper class
}


END_NAMESPACE_OPENTURNS
