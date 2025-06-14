//                                               -*- C++ -*-
/**
 *  @brief The DeconditionedDistribution distribution
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/DeconditionedDistribution.hxx"
#include "openturns/JointDistribution.hxx"
#include "openturns/Dirac.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/Box.hxx"
#include "openturns/WeightedExperiment.hxx"
#include "openturns/GaussProductExperiment.hxx"
#include "openturns/LowDiscrepancyExperiment.hxx"
#include "openturns/MonteCarloExperiment.hxx"
#include "openturns/SobolSequence.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/IdentityFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DeconditionedDistribution)

static const Factory<DeconditionedDistribution> Factory_DeconditionedDistribution;


/* Default constructor */
DeconditionedDistribution::DeconditionedDistribution()
  : Mixture()
{
  setName("DeconditionedDistribution");
  const Description inVars(Description::BuildDefault(1, "y"));
  const Description outVars(Description::BuildDefault(2, "theta"));
  const Description formulas = {inVars[0], inVars[0] + " + 1"};
  SymbolicFunction linkFunction(inVars, formulas);
  Description description(inVars);
  description.add(outVars);
  linkFunction.setDescription(description);
  setConditionedAndConditioningDistributionsAndLinkFunction(Uniform(), Uniform(), linkFunction);
}

/* Parameters constructor */
DeconditionedDistribution::DeconditionedDistribution(const Distribution & conditionedDistribution,
    const Distribution & conditioningDistribution)
  : DeconditionedDistribution(conditionedDistribution, conditioningDistribution, IdentityFunction(conditioningDistribution.getDimension()))
{
  // Nothing to do
}

/* Parameters constructor */
DeconditionedDistribution::DeconditionedDistribution(const Distribution & conditionedDistribution,
    const Distribution & conditioningDistribution,
    const Function & linkFunction)
  : Mixture()
{
  setName("DeconditionedDistribution");
  // The dimension and range are computed using the upper class through this call
  setConditionedAndConditioningDistributionsAndLinkFunction(conditionedDistribution, conditioningDistribution, linkFunction);
}

/* Comparison operator */
Bool DeconditionedDistribution::operator ==(const DeconditionedDistribution & other) const
{
  if (this == &other) return true;
  return (conditionedDistribution_ == other.conditionedDistribution_) && (conditioningDistribution_ == other.conditioningDistribution_) && (linkFunction_ == other.linkFunction_);
}

Bool DeconditionedDistribution::equals(const DistributionImplementation & other) const
{
  const DeconditionedDistribution* p_other = dynamic_cast<const DeconditionedDistribution*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String DeconditionedDistribution::__repr__() const
{
  OSS oss(true);
  oss << "class=" << DeconditionedDistribution::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " conditioned distribution=" << conditionedDistribution_
      << " conditioning distribution=" << conditioningDistribution_
      << " link function=" << linkFunction_;
  return oss;
}

String DeconditionedDistribution::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(X with X|Theta~" << conditionedDistribution_.getImplementation()->getClassName() << "(Theta), Theta=f(Y), f=" << linkFunction_.getEvaluation().__str__() << ", Y~" << conditioningDistribution_.__str__() << ")";
  return oss;
}

/* Virtual constructor */
DeconditionedDistribution * DeconditionedDistribution::clone() const
{
  return new DeconditionedDistribution(*this);
}

/* Get one realization of the distribution */
Point DeconditionedDistribution::getRealization() const
{
  Distribution deconditioned(conditionedDistribution_);
  deconditioned.setParameter(linkFunction_(conditioningDistribution_.getRealization()));
  return deconditioned.getRealization();
}

DeconditionedDistribution::PointWithDescriptionCollection DeconditionedDistribution::getParametersCollection() const
{
  return conditioningDistribution_.getParametersCollection();
}

/* Parameters value accessor */
Point DeconditionedDistribution::getParameter() const
{
  return conditioningDistribution_.getParameter();
}

void DeconditionedDistribution::setParameter(const Point & parameter)
{
  Distribution conditioningDistribution(conditioningDistribution_);
  conditioningDistribution.setParameter(parameter);
  const Scalar w = getWeight();
  Distribution conditionedDistribution(conditionedDistribution_);
  *this = DeconditionedDistribution(conditionedDistribution, conditioningDistribution);
  setWeight(w);
}

Description DeconditionedDistribution::getParameterDescription() const
{
  return conditioningDistribution_.getParameterDescription();
}

/* Conditioned distribution accessor */
void DeconditionedDistribution::setConditionedDistribution(const Distribution & conditionedDistribution)
{
  if (conditionedDistribution != conditionedDistribution_) setConditionedAndConditioningDistributionsAndLinkFunction(conditionedDistribution, conditioningDistribution_, linkFunction_);
}

Distribution DeconditionedDistribution::getConditionedDistribution() const
{
  return conditionedDistribution_;
}


/* Conditioning distribution accessor */
void DeconditionedDistribution::setConditioningDistribution(const Distribution & conditioningDistribution)
{
  if (conditioningDistribution != conditioningDistribution_) setConditionedAndConditioningDistributionsAndLinkFunction(conditionedDistribution_, conditioningDistribution, linkFunction_);
}

Distribution DeconditionedDistribution::getConditioningDistribution() const
{
  return conditioningDistribution_;
}

/* Link function accessor */
void DeconditionedDistribution::setLinkFunction(const Function & linkFunction)
{
  if (!(linkFunction == linkFunction_)) setConditionedAndConditioningDistributionsAndLinkFunction(conditionedDistribution_, conditioningDistribution_, linkFunction);
}

Function DeconditionedDistribution::getLinkFunction() const
{
  return linkFunction_;
}


void DeconditionedDistribution::setConditionedAndConditioningDistributionsAndLinkFunction(const Distribution & conditionedDistribution,
    const Distribution & conditioningDistribution,
    const Function & linkFunction)
{
  const UnsignedInteger conditioningDimension = conditioningDistribution.getDimension();
  const UnsignedInteger conditionedParametersDimension = conditionedDistribution.getParameterDimension();
  // We must check that the conditioning distribution has the same dimension as the input dimension of the link function and that the conditioning distribution has the same dimension as the input dimension of the link function
  if (conditionedParametersDimension != linkFunction.getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: expected a link function with output dimension equal to the number of parameters of the conditioned distribution. Here, output dimension=" << linkFunction.getOutputDimension() << " and parameters number=" << conditionedParametersDimension;
  if (conditioningDimension != linkFunction.getInputDimension()) throw InvalidArgumentException(HERE) << "Error: expected a link function with input dimension equal to the conditioning distribution dimension. Here, input dimension=" << linkFunction.getInputDimension() << " and conditioning dimension=" << conditioningDimension;
  if ((!conditioningDistribution.isDiscrete()) && (!conditioningDistribution.getCopula().isContinuous())) throw InvalidArgumentException(HERE) << "Error: expected a conditioning distribution with a continuous copula if the conditioning distribution is not fully discrete";
  conditionedDistribution_ = conditionedDistribution;
  conditioningDistribution_ = conditioningDistribution;
  linkFunction_ = linkFunction;
  setDimension(conditionedDistribution.getDimension());
  // Start the discretisation into a Mixture
  // Here, implements some knowledge based selection of the integration method
  // For now, only basic Legendre
  // Gather the indices of the discrete marginals
  // Analyse the marginal distributions
  Collection< Sample > discreteSupports(0, Sample(0, 1));
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
    else if(marginal.isDiscrete())
    {
      const Sample support(marginal.getSupport());
      // Special case for Dirac distributions. It can be either a Dirac distribution or a UserDefined distribution, so we check the support directly.
      if (support.getSize() == 1)
      {
        diracMarginalsIndices_.add(i);
        diracValues_.add(support(0, 0));
      } // Dirac marginal
      // Truly random discrete distribution
      else
      {
        discreteMarginalsIndices_.add(i);
        discreteSupports.add(support);
      } // Random discrete marginal
    } // Discrete marginal
    else
      throw InvalidArgumentException(HERE) << "Error: the conditioning marginal distributions must be either continuous or discrete, here marginal " << i << "=" << marginal << " is neither continuous nor discrete";
  } // Loop over the marginal distributions
  // Integration measure for the continuous parameters
  const UnsignedInteger continuousDimension = continuousMarginalsIndices_.getSize();
  UnsignedInteger continuousAtomsNumber = 0;
  if (continuousDimension > 0)
  {
    const JointDistribution measure(Collection< Distribution >(continuousDimension, Uniform()));
    // Create the DOE for continuous integration
    const String method(ResourceMap::GetAsString("DeconditionedDistribution-ContinuousDiscretizationMethod"));
    const UnsignedInteger maximumIntegrationNumber = ResourceMap::GetAsUnsignedInteger( "DeconditionedDistribution-MaximumIntegrationNodesNumber" );
    // All these quantities are needed for the upper class algorithms even if locally they are only useful for GaussProduct
    const UnsignedInteger maximumNumber = static_cast< UnsignedInteger > (round(std::pow(maximumIntegrationNumber, 1.0 / continuousDimension)));
    const UnsignedInteger candidateNumber = ResourceMap::GetAsUnsignedInteger( "DeconditionedDistribution-MarginalIntegrationNodesNumber" );
    if (candidateNumber > maximumNumber) LOGWARN(OSS() << "Warning! The requested number of marginal integration nodes=" << candidateNumber << " would lead to an excessive number of integration nodes=" << std::pow(candidateNumber, 1.0 * continuousDimension) << ". It has been reduced to " << maximumNumber << ". You should increase the ResourceMap key \"DeconditionedDistribution-MaximumIntegrationNodesNumber\" or decrease the ResourceMap key \"DeconditionedDistribution-MarginalIntegrationNodesNumber\"");
    WeightedExperiment experiment;
    if (method == "GaussProduct")
      experiment = GaussProductExperiment(measure, Indices(continuousDimension, std::min(maximumNumber, candidateNumber)));
    else if (method == "QMC")
      experiment = LowDiscrepancyExperiment(SobolSequence(), measure, maximumIntegrationNumber);
    else
      experiment = MonteCarloExperiment(measure, maximumIntegrationNumber);
    continuousNodes_ = experiment.generateWithWeights(continuousWeights_);
    // Also adapt the integration nodes number in the upper class
    setIntegrationNodesNumber(std::min(maximumNumber, candidateNumber));
    // Normalization factor for the weights
    // Not needed as the Mixture will be automatically normalized
    // const Scalar normalizationFactor(Interval(continuousLowerBounds, continuousUpperBounds).getVolume());
    continuousAtomsNumber = continuousNodes_.getSize();
  } // continuousDimension > 0

  // Integration measure for the discrete parameters
  const UnsignedInteger discreteDimension = discreteMarginalsIndices_.getSize();
  if ((continuousDimension > 0) && (discreteDimension > 0) && !conditioningDistribution_.hasIndependentCopula()) throw NotYetImplementedException(HERE) << "Error: the case with mixed discrete and continuous marginal conditioning distribution with dependence is not yet supported";
  UnsignedInteger discreteAtomsNumber = 0;
  if (discreteDimension > 0)
  {
    Point levels(discreteDimension);
    for (UnsignedInteger i = 0; i < discreteDimension; ++i) levels[i] = discreteSupports[i].getSize() - 2;
    const Sample fractions(Box(levels).generate());
    discreteAtomsNumber = fractions.getSize();
    discreteNodes_ = Sample(discreteAtomsNumber, discreteDimension);
    for (UnsignedInteger i = 0; i < discreteAtomsNumber; ++i)
    {
      Point discreteNode(discreteDimension);
      for (UnsignedInteger j = 0; j < discreteDimension; ++j)
      {
        const Scalar rho = fractions(i, j);
        const UnsignedInteger length = discreteSupports[j].getSize();
        const UnsignedInteger index = static_cast<UnsignedInteger>(round(rho * (length - 1)));
        discreteNode[j] = discreteSupports[j][index][0];
      }
      discreteNodes_[i] = discreteNode;
    } // Loop over the discrete atoms
  } // discreteDimension > 0
  // Integration measure for the Dirac parameters
  const UnsignedInteger diracDimension = diracMarginalsIndices_.getSize();
  /********************************
   * Build the equivalent mixture
   ********************************/
  // Zeroth case: all Dirac
  if (diracDimension == conditioningDimension)
  {
    Collection< Distribution > atoms(1, conditionedDistribution);
    atoms[0].setParameter(linkFunction_(diracValues_));
    // Hide warnings
    Log::Severity oldSeverity = Log::Flags();
    Log::Show(oldSeverity & ~Log::WARN);
    setDistributionCollection(atoms);
    Log::Show(oldSeverity & ~Log::WARN);
    return;
  }
  // First case: only Dirac and stochastic discrete marginals with at least one stochastic discrete marginal
  if (continuousDimension == 0)
  {
    Collection< Distribution > atoms(0);
    Point y(conditioningDimension);
    // First, the Dirac components.
    for (UnsignedInteger i = 0; i < diracDimension; ++i)
      y[diracMarginalsIndices_[i]] = diracValues_[i];
    // Second, the discrete components
    // For each combination of the discrete components
    Sample currentY(discreteAtomsNumber, y);
    for (UnsignedInteger i = 0; i < discreteAtomsNumber; ++i)
    {
      // Get the discrete values
      for (UnsignedInteger j = 0; j < discreteDimension; ++j)
        currentY(i, discreteMarginalsIndices_[j]) = discreteNodes_(i, j);
    }
    const Sample yPDF(conditioningDistribution.computePDF(currentY));
    const Sample parameters(linkFunction_(currentY));
    for (UnsignedInteger i = 0; i < discreteAtomsNumber; ++i)
    {
      const Scalar wI = yPDF(i, 0);
      if (wI > 0.0)
        try
        {
          Distribution dist(conditionedDistribution);
          dist.setWeight(wI);
          dist.setParameter(parameters[i]);
          atoms.add(dist);
        }
        catch (...)
        {
          LOGDEBUG(OSS() << "In DeconditionedDistribution, skip atom with parameter " << parameters[i] << " not compatible with conditioned distribution " << conditionedDistribution);
        }
    } // Discrete measure
    // Now, update the underlying Mixture
    // Hide warnings
    Log::Severity oldSeverity = Log::Flags();
    Log::Show(oldSeverity & ~Log::WARN);
    setDistributionCollection(atoms);
    Log::Show(oldSeverity & ~Log::WARN);
    return;
  } // No continuous marginal
  // Second case: only Dirac and continuous marginals with at least one continuous marginal
  if (discreteDimension == 0)
  {
    Collection< Distribution > atoms(0);
    Point y(conditioningDimension);
    // First, the Dirac components
    for (UnsignedInteger i = 0; i < diracDimension; ++i)
      y[diracMarginalsIndices_[i]] = diracValues_[i];
    // Continuous part using Gauss integration
    Sample currentY(continuousAtomsNumber, y);
    for (UnsignedInteger i = 0; i < continuousAtomsNumber; ++i)
    {
      // Complete the filling of theta using the Gauss integration node
      for (UnsignedInteger j = 0; j < continuousDimension; ++j)
        currentY(i, continuousMarginalsIndices_[j]) = continuousLowerBounds_[j] + 0.5 * (1.0 + continuousNodes_(i, j)) * (continuousUpperBounds_[j] - continuousLowerBounds_[j]);
    }
    const Sample yPDF(conditioningDistribution.computePDF(currentY));
    const Sample parameters(linkFunction_(currentY));
    for (UnsignedInteger i = 0; i < continuousAtomsNumber; ++i)
    {
      const Scalar wI = yPDF(i, 0) * continuousWeights_[i];
      if (wI > 0.0)
        try
        {
          Distribution dist(conditionedDistribution);
          dist.setWeight(wI);
          dist.setParameter(parameters[i]);
          atoms.add(dist);
        }
        catch (...)
        {
          LOGDEBUG(OSS() << "In DeconditionedDistribution, skip atom with parameter " << parameters[i] << " not compatible with conditioned distribution " << conditionedDistribution);
        }
    } // Continuous measure
    // Now, update the underlying Mixture
    // Hide warnings
    Log::Severity oldSeverity = Log::Flags();
    Log::Show(oldSeverity & ~Log::WARN);
    setDistributionCollection(atoms);
    Log::Show(oldSeverity & ~Log::WARN);
    return;
  } // No discrete marginal
  // Third case: Dirac, stochastic discrete and continuous marginal distributions with at least one stochastic discrete marginal and one continuous marginal
  Collection< Distribution > atoms(0);
  Point y(conditioningDimension);
  // First, the Dirac components
  for (UnsignedInteger i = 0; i < diracDimension; ++i)
    y[diracMarginalsIndices_[i]] = diracValues_[i];
  // Second, the discrete components
  // For each combination of the discrete components
  Sample currentYs(0, y);
  for (UnsignedInteger i = 0; i < discreteAtomsNumber; ++i)
  {
    Point currentY(y);
    // Get the discrete values
    for (UnsignedInteger j = 0; j < discreteDimension; ++j)
      currentY[discreteMarginalsIndices_[j]] = discreteNodes_(i, j);
    // Now, complete by the discretization of the continuous part using Gauss integration
    for (UnsignedInteger j = 0; j < continuousAtomsNumber; ++j)
    {
      // Complete the filling of theta using the Gauss integration node
      for (UnsignedInteger k = 0; k < continuousDimension; ++k)
        currentY[continuousMarginalsIndices_[k]] = continuousLowerBounds_[k] + 0.5 * (1.0 + continuousNodes_(j, k)) * (continuousUpperBounds_[k] - continuousLowerBounds_[k]);
      currentYs.add(currentY);
    }
  }
  const Sample yPDF(conditioningDistribution.computePDF(currentYs));
  const Sample parameters(linkFunction_(currentYs));
  for (UnsignedInteger i = 0; i < currentYs.getSize(); ++i)
  {
    const Scalar wI = yPDF(i, 0) * continuousWeights_[i % continuousAtomsNumber];
    if (wI > 0.0)
      try
      {
        Distribution dist(conditionedDistribution);
        dist.setWeight(wI);
        dist.setParameter(parameters[i]);
        atoms.add(dist);
      }
      catch (...)
      {
        LOGDEBUG(OSS() << "In DeconditionedDistribution, skip atom with parameter " << parameters[i] << " not compatible with conditioned distribution " << conditionedDistribution);
      }
  }
  // Now, update the underlying Mixture
  // Hide warnings
  Log::Severity oldSeverity = Log::Flags();
  Log::Show(oldSeverity & ~Log::WARN);
  setDistributionCollection(atoms);
  Log::Show(oldSeverity & ~Log::WARN);
}

/* Compute the expectation of f(\theta)1_{\theta\leq \theta^*} with respect to the prior distribution of \theta */
Point DeconditionedDistribution::computeExpectation(const Function & f,
    const Point & thetaStar) const
{
  const Scalar epsilon = ResourceMap::GetAsScalar("Distribution-SupportEpsilon");
  const UnsignedInteger conditioningDimension = conditioningDistribution_.getDimension();
  if (f.getInputDimension() != conditioningDimension) throw InvalidArgumentException(HERE) << "Error: the given function must have an input dimension=" << f.getInputDimension() << " equal to the conditioning dimension=" << conditioningDimension;
  if (thetaStar.getDimension() != conditioningDimension) throw InvalidArgumentException(HERE) << "Error: the given upper bound must have a dimension=" << thetaStar.getDimension() << " equal to the conditioning dimension=" << conditioningDimension;
  const UnsignedInteger outputDimension = f.getOutputDimension();
  Point result(outputDimension);
  // Here, we reuse the analysis made in the underlying deconditioned distribution
  const UnsignedInteger continuousDimension = continuousMarginalsIndices_.getSize();
  const UnsignedInteger continuousAtomsNumber = continuousNodes_.getSize();
  const UnsignedInteger discreteDimension = discreteMarginalsIndices_.getSize();
  const UnsignedInteger discreteAtomsNumber = discreteNodes_.getSize();
  const UnsignedInteger diracDimension = diracMarginalsIndices_.getSize();
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
    Point theta(conditioningDimension);
    // First, the Dirac components. It can be either a Dirac distribution or a userDefined distribution, so we check the support directly
    for (UnsignedInteger i = 0; i < diracDimension; ++i)
    {
      const Scalar value = diracValues_[i];
      // If the hyper rectangle does not intersect the manifold that supports the total mass, then value = 0
      if (value > thetaStar[i] + epsilon) return result;
      theta[diracMarginalsIndices_[i]] = value;
    }
    // Second, the discrete components
    // For each combination of the discrete components
    Sample currentThetas(0, theta);
    for (UnsignedInteger i = 0; i < discreteAtomsNumber; ++i)
    {
      Point currentTheta(theta);
      // Get the discrete values
      Bool rejectNode = false;
      for (UnsignedInteger j = 0; j < discreteDimension; ++j)
      {
        const Scalar value = discreteNodes_(i, j);
        currentTheta[discreteMarginalsIndices_[j]] = value;
        rejectNode = (value > thetaStar[i] + epsilon);
        if (rejectNode) break;
      }
      // Skip the current integration point if the current sub-manifold is outside of the integration region
      if (!rejectNode)
      {
        currentThetas.add(currentTheta);
      }
    }
    if (currentThetas.getSize() == 0)
      return result;
    const Sample currentThetasPDF(conditioningDistribution_.computePDF(currentThetas));
    const Sample fCurrentThetas(f(currentThetas));
    for (UnsignedInteger i = 0; i < currentThetas.getSize(); ++i)
    {
      result += currentThetasPDF(i, 0) * fCurrentThetas[i];
    } // Discrete measure
    return result;
  } // No continuous marginal

  // Second case: no discrete marginal
  if (discreteDimension == 0)
  {
    // The current value of theta in the discretization
    Point theta(conditioningDimension);
    // The sub point associated with the continuous components
    Point subPoint(continuousDimension);
    for (UnsignedInteger i = 0; i < continuousDimension; ++i)
      subPoint[i] = std::min(continuousUpperBounds_[i], thetaStar[continuousMarginalsIndices_[i]]);
    // First, the Dirac components
    for (UnsignedInteger i = 0; i < diracDimension; ++i)
    {
      const Scalar value = diracValues_[i];
      // If the hyper rectangle does not intersect the manifold that supports the total mass, then cdf = 0
      if (value > thetaStar[i] + epsilon) return result;
      theta[diracMarginalsIndices_[i]] = value;
    }
    // Continuous part using Gauss integration
    Sample currentTheta(continuousAtomsNumber, theta);
    for (UnsignedInteger i = 0; i < continuousAtomsNumber; ++i)
    {
      // Complete the filling of theta using the Gauss integration node
      for (UnsignedInteger j = 0; j < continuousDimension; ++j)
        currentTheta(i, continuousMarginalsIndices_[j]) = continuousLowerBounds_[j] + 0.5 * (1.0 + continuousNodes_(i, j)) * (subPoint[j] - continuousLowerBounds_[j]);
    }
    const Sample thetaPDF(conditioningDistribution_.computePDF(currentTheta));
    const Sample fTheta(f(currentTheta));
    for (UnsignedInteger i = 0; i < continuousAtomsNumber; ++i)
    {
      // Current contribution to the CDF
      result += (thetaPDF(i, 0) * continuousWeights_[i]) * fTheta[i];
    } // Continuous measure
    result *= Interval(continuousLowerBounds_, subPoint).getVolume();
    return result;
  } // No discrete marginal

  // Third case: continuous and discrete marginals
  Point theta(conditioningDimension);
  // The sub point associated with the continuous components
  Point subPoint(continuousDimension);
  for (UnsignedInteger i = 0; i < continuousDimension; ++i)
    subPoint[i] = std::min(continuousUpperBounds_[i], thetaStar[continuousMarginalsIndices_[i]]);
  // First, the Dirac components
  for (UnsignedInteger i = 0; i < diracDimension; ++i)
  {
    const Scalar value = diracValues_[i];
    // If the hyper rectangle does not intersect the manifold that supports the total mass, then cdf = 0
    if (value > thetaStar[i] + epsilon) return result;
    theta[diracMarginalsIndices_[i]] = value;
  }
  // Second, the discrete components
  // For each combination of the discrete components
  Sample currentThetas(0, theta);
  for (UnsignedInteger i = 0; i < discreteAtomsNumber; ++i)
  {
    Point currentTheta(theta);
    // Get the discrete values
    Bool rejectNode = false;
    for (UnsignedInteger j = 0; j < discreteDimension; ++j)
    {
      const Scalar value = discreteNodes_(i, j);
      currentTheta[discreteMarginalsIndices_[j]] = value;
      rejectNode = (value > thetaStar[i] + epsilon);
      if (rejectNode) break;
    }
    // Skip the current integration point if the current sub-manifold is outside of the integration region
    if (!rejectNode)
    {
      for (UnsignedInteger j = 0; j < continuousAtomsNumber; ++j)
      {
        // Complete the filling of theta using the Gauss integration node
        for (UnsignedInteger k = 0; k < continuousDimension; ++k)
          currentTheta[continuousMarginalsIndices_[k]] = continuousLowerBounds_[k] + 0.5 * (1.0 + continuousNodes_(j, k)) * (subPoint[k] - continuousLowerBounds_[k]);
        currentThetas.add(currentTheta);
      } // Continuous atoms
    }
  }
  if (currentThetas.getSize() > 0)
  {
    const Sample currentThetasPDF(conditioningDistribution_.computePDF(currentThetas));
    const Sample fCurrentThetas(f(currentThetas));
    for (UnsignedInteger i = 0; i < currentThetas.getSize(); ++i)
    {
      // Now, complete by the discretization of the continuous part using Gauss integration
      const Point q(currentThetasPDF(i, 0) * fCurrentThetas[i]);
      result += continuousWeights_[i % continuousAtomsNumber] * q;
    }
  }
  result *= Interval(continuousLowerBounds_, subPoint).getVolume();
  return result;
}

/* Get the i-th marginal distribution */
Distribution DeconditionedDistribution::getMarginal(const UnsignedInteger i) const
{
  const UnsignedInteger dimension = getDimension();
  if (i >= dimension) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  if (dimension == 1) return clone();
  // Waiting for a better implementation
  return Mixture::getMarginal(i);
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution DeconditionedDistribution::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension = getDimension();
  if (!indices.check(dimension)) throw InvalidArgumentException(HERE) << "The indices of a marginal distribution must be in the range [0, dim-1] and must be different";
  if (dimension == 1) return clone();
  // Waiting for a better implementation
  return Mixture::getMarginal(indices);
} // getMarginal(Indices)

/* Method save() stores the object through the StorageManager */
void DeconditionedDistribution::save(Advocate & adv) const
{
  Mixture::save(adv);
  adv.saveAttribute( "conditionedDistribution_", conditionedDistribution_ );
  adv.saveAttribute( "conditioningDistribution_", conditioningDistribution_ );
  adv.saveAttribute( "linkFunction_", linkFunction_ );
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
void DeconditionedDistribution::load(Advocate & adv)
{
  Mixture::load(adv);
  adv.loadAttribute( "conditionedDistribution_", conditionedDistribution_ );
  adv.loadAttribute( "conditioningDistribution_", conditioningDistribution_ );
  adv.loadAttribute( "linkFunction_", linkFunction_ );
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
