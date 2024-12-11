//                                               -*- C++ -*-
/**
 *  @brief This class exposes the Fejer experiments
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
#include "openturns/FejerExperiment.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Tuples.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FejerExperiment
 */


CLASSNAMEINIT(FejerExperiment)

static const Factory<FejerExperiment> Factory_FejerExperiment;


/* Parameters constructor */
FejerExperiment::FejerExperiment(const Indices & discretization,
                                 const RuleType ruleType)
  : WeightedExperimentImplementation()
  , discretization_(discretization)
  , ruleType_(ruleType)
{
  if (!discretization.getSize())
    throw InvalidArgumentException(HERE) << "FejerExperiment discretization dimension cannot be zero.";
}

/* Virtual constructor */
FejerExperiment * FejerExperiment::clone() const
{
  return new FejerExperiment(*this);
}

Bool FejerExperiment::hasUniformWeights() const
{
  for (UnsignedInteger i = 0; i < discretization_.getSize(); ++ i)
    if (discretization_[i] > 1) return false;
  return true;
}

/* Sample generation */
Sample FejerExperiment::generateWithWeights(Point & weightsOut) const
{
  // First, generate the 1D marginal rules over [0, 1]
  const UnsignedInteger dimension = discretization_.getSize();
  if (!(dimension > 0))
    throw InvalidArgumentException(HERE) << "Error: expected a positive dimension, here dimension is " << dimension;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const UnsignedInteger integrationNodesNumber = discretization_[i];
    if (!(integrationNodesNumber > 0))
      throw InvalidArgumentException(HERE) << "Error: the discretization must be positive, here discretization[" << i << "] has " << integrationNodesNumber << " nodes.";
  }
  Collection<Point> marginalNodes(dimension);
  Collection<Point> marginalWeights(dimension);
  switch (ruleType_)
  {
    case FEJERTYPE1:
      generateNodesAndWeightsFejerType1(marginalNodes, marginalWeights);
      break;
    case FEJERTYPE2:
      generateNodesAndWeightsFejerType2(marginalNodes, marginalWeights);
      break;
    case CLENSHAWCURTIS:
      generateNodesAndWeightsClenshawCurtis(marginalNodes, marginalWeights);
      break;
    default:
      throw InvalidArgumentException(HERE) << "Invalid FejerExperiment rule; must be FEJERTYPE1, FEJERTYPE2 or CLENSHAWCURTIS, got " << ruleType_;
  } /* end switch */
  // Now we have marginal nodes & weights,
  // we generate the nD rule over [0, 1]^n
  IndicesCollection allTuples(Tuples(discretization_).generate());
  const UnsignedInteger size = allTuples.getSize();
  Sample nodes(size, dimension);
  weightsOut = Point(size, 1.0);

  for (UnsignedInteger i = 0; i < size; ++i)
  {
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      nodes(i, j) = marginalNodes[j][allTuples(i, j)];
      weightsOut[i] *= marginalWeights[j][allTuples(i, j)];
    }
  } // tuples

  // Adapt the nodes to the bounds of the interval
  if (bounds_.getDimension())
  {
    const Point halfDelta((bounds_.getUpperBound() - bounds_.getLowerBound()) * 0.5);
    nodes = nodes * halfDelta + halfDelta + bounds_.getLowerBound();
  }

  return nodes;
}

/* Size accessor */
void FejerExperiment::setSize(const UnsignedInteger /*size*/)
{
  throw NotDefinedException(HERE) << "in FejerExperiment::setSize";
}

/* Nesting level accessor */
void FejerExperiment::setLevels(const Indices & levels)
{
  const UnsignedInteger size = levels.getSize();
  if (size != discretization_.getSize())
    throw InvalidArgumentException(HERE) << "FejerExperiment nesting levels must match discretization size, got " << size;
  Indices discretization(levels);
  if (useNestedLevels_)
  {
    switch (ruleType_)
    {
      case FEJERTYPE1:
      {
        if (useNestedLevels_)
          throw InvalidArgumentException(HERE) << "FejerExperiment: Type1 rule does not yield nested nodes";
        break;
      }
      case FEJERTYPE2:
      {
        for (UnsignedInteger i = 0; i < size; ++ i)
          discretization[i] = SpecFunc::IPow(2.0, levels[i]) - 1;
        break;
      }
      case CLENSHAWCURTIS:
      {
        for (UnsignedInteger i = 0; i < size; ++ i)
          discretization[i] = SpecFunc::IPow(2.0, levels[i]) + 1;
        break;
      }
      default:
        throw InvalidArgumentException(HERE) << "FejerExperiment: Invalid rule";
    }
  }
  setDiscretization(discretization);
}

/* Use nested levels flag accessor */
void FejerExperiment::setUseNestedLevels(const Bool useNestedLevels)
{
  useNestedLevels_ = useNestedLevels;
}

Bool FejerExperiment::getUseNestedLevels() const
{
  return useNestedLevels_;
}

UnsignedInteger FejerExperiment::getSize() const
{
  // only known at generation time
  return generate().getSize();
}

/* Return isRandom flag */
Bool FejerExperiment::isRandom() const
{
  return false;
}


/** Rule type accessor */
void FejerExperiment::setRuleType(const RuleType ruleType)
{
  ruleType_ = ruleType;
}

FejerExperiment::RuleType FejerExperiment::getRuleType() const
{
  return ruleType_;
}


/* String converter */
String FejerExperiment::__repr__() const
{
  OSS oss(true);
  oss << "class=" << FejerExperiment::GetClassName();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void FejerExperiment::save(Advocate & adv) const
{
  WeightedExperimentImplementation::save(adv);
  adv.saveAttribute("discretization_", discretization_);
  adv.saveAttribute("ruleType_", static_cast<UnsignedInteger>(ruleType_));
  adv.saveAttribute("bounds_", bounds_);
  adv.saveAttribute("useNestedLevels_", useNestedLevels_);
}

/* Method load() reloads the object from the StorageManager */
void FejerExperiment::load(Advocate & adv)
{
  WeightedExperimentImplementation::load(adv);
  adv.loadAttribute("discretization_", discretization_);
  UnsignedInteger ruleType;
  adv.loadAttribute("ruleType_", ruleType);
  ruleType_ = static_cast<RuleType>(ruleType);
  adv.loadAttribute("bounds_", bounds_);
  adv.loadAttribute("useNestedLevels_", useNestedLevels_);
}


void FejerExperiment::generateNodesAndWeightsClenshawCurtis(Collection<Point> & marginalNodes,
    Collection<Point> & marginalWeights) const
{
  const UnsignedInteger dimension = discretization_.getSize();
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const UnsignedInteger integrationNodesNumber = discretization_[i];
    if (!(integrationNodesNumber > 0))
      throw InvalidArgumentException(HERE) << "Error: the discretization must be positive, here discretization[" << i << "] is null.";

    // special case for n=1
    if (integrationNodesNumber == 1)
    {
      marginalNodes[i] = Point({0.0});
      marginalWeights[i] = Point({2.0});
      continue;
    }

    // Check if we already computed this 1D rule
    // We use the value 'dimension' as a guard
    UnsignedInteger indexAlreadyComputed = dimension;
    for (UnsignedInteger j = 0; j < i; ++j)
      if (discretization_[j] == integrationNodesNumber)
      {
        indexAlreadyComputed = j;
        break;
      }
    // If indexAlreadyComputed < dimension we found a match
    if (indexAlreadyComputed < dimension)
    {
      marginalNodes[i] = marginalNodes[indexAlreadyComputed];
      marginalWeights[i] = marginalWeights[indexAlreadyComputed];
    } // A match found
    else
    {
      Point mni(integrationNodesNumber);
      Point mwi(integrationNodesNumber);
      for (UnsignedInteger k = 0; k < integrationNodesNumber; ++k)
      {
        const Scalar theta_k = k * M_PI / (integrationNodesNumber - 1);
        // Nodes
        mni[k] = std::cos(theta_k);
        // Weights
        Scalar term = 1.0;
        const UnsignedInteger halfNodesNumber = (integrationNodesNumber - 1) / 2;
        for (UnsignedInteger l = 1; l <= halfNodesNumber; ++l)
        {
          const Scalar bj = l < halfNodesNumber ? 2.0 : 1.0;
          term -= bj / (4.0 * l * l - 1) * std::cos(2 * l * theta_k);
        }
        Scalar ck = 2.0;
        if (k == 0 || k == (integrationNodesNumber - 1))
          ck = 1.0;
        mwi[k] = ck / (integrationNodesNumber - 1) * term;
      }
      marginalNodes[i] = mni;
      marginalWeights[i] = mwi;
    } // No match found
  }   // For i
}

void FejerExperiment::generateNodesAndWeightsFejerType1(Collection<Point> & marginalNodes,
    Collection<Point> & marginalWeights) const
{
  const UnsignedInteger dimension = discretization_.getSize();
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const UnsignedInteger integrationNodesNumber = discretization_[i];
    // Check if we already computed this 1D rule
    // We use the value 'dimension' as a guard
    UnsignedInteger indexAlreadyComputed = dimension;
    for (UnsignedInteger j = 0; j < i; ++j)
      if (discretization_[j] == integrationNodesNumber)
      {
        indexAlreadyComputed = j;
        break;
      }
    // If indexAlreadyComputed < dimension we found a match
    if (indexAlreadyComputed < dimension)
    {
      marginalNodes[i] = marginalNodes[indexAlreadyComputed];
      marginalWeights[i] = marginalWeights[indexAlreadyComputed];
    } // A match found
    else
    {
      Point mni(integrationNodesNumber);
      Point mwi(integrationNodesNumber);
      for (UnsignedInteger k = 0; k < integrationNodesNumber; ++k)
      {
        // Nodes
        const Scalar theta_k = (k + 1.0 / 2.) * M_PI / (integrationNodesNumber);
        mni[k] = std::cos(theta_k);
        // Weights
        Scalar term = 0.0;
        const UnsignedInteger halfNodesNumber = integrationNodesNumber / 2;
        for (UnsignedInteger j = 1; j <= halfNodesNumber; ++j)
        {
          term += (1.0 / (4.0 * j * j - 1.0)) * std::cos(2.0 * j * theta_k);
        }
        mwi[k] = (2.0 / integrationNodesNumber) * (1.0 - 2.0 * term);
      }
      marginalNodes[i] = mni;
      marginalWeights[i] = mwi;
    } // No match found
  }   // For i
}

void FejerExperiment::generateNodesAndWeightsFejerType2(Collection<Point> & marginalNodes,
    Collection<Point> & marginalWeights) const
{
  // First, generate the 1D marginal rules over [0, 1]
  const UnsignedInteger dimension = discretization_.getSize();
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const UnsignedInteger integrationNodesNumber = discretization_[i];
    // Check if we already computed this 1D rule
    // We use the value 'dimension' as a guard
    UnsignedInteger indexAlreadyComputed = dimension;
    for (UnsignedInteger j = 0; j < i; ++j)
    {
      if (discretization_[j] == integrationNodesNumber)
      {
        indexAlreadyComputed = j;
        break;
      }
    }
    // If indexAlreadyComputed < dimension we found a match
    if (indexAlreadyComputed < dimension)
    {
      marginalNodes[i] = marginalNodes[indexAlreadyComputed];
      marginalWeights[i] = marginalWeights[indexAlreadyComputed];
    } // A match found
    else
    {
      Point mni(integrationNodesNumber);
      Point mwi(integrationNodesNumber);
      for (UnsignedInteger k = 0; k < integrationNodesNumber; ++k)
      {
        const Scalar theta_k = (k + 1.0) * M_PI / (integrationNodesNumber + 1);
        // Nodes
        mni[k] = std::cos(theta_k);
        Scalar sum_sinus = 0.0;
        const UnsignedInteger halfNodesNumber = (integrationNodesNumber + 1) / 2;
        for (UnsignedInteger j = 1; j <= halfNodesNumber; ++j)
          sum_sinus += std::sin((2.0 * j - 1.0) * theta_k) / (2.0 * j - 1.0);
        // Weights
        mwi[k] = 4.0 / (integrationNodesNumber + 1) * std::sin(theta_k) * sum_sinus;
      }
      marginalNodes[i] = mni;
      marginalWeights[i] = mwi;
    } // No match found
  }   // For i
}

/* Discretization accessor */
void FejerExperiment::setDiscretization(const Indices & discretization)
{
  if (!discretization.getSize())
    throw InvalidArgumentException(HERE) << "FejerExperiment discretization dimension cannot be zero.";
  discretization_ = discretization;
}

Indices FejerExperiment::getDiscretization() const
{
  return discretization_;
}

/* Bounds accessor */
void FejerExperiment::setBounds(const Interval & bounds)
{
  if (bounds.getDimension() && discretization_.getSize() != bounds.getDimension())
    throw InvalidArgumentException(HERE) << "FejerExperiment discretization and bounds dimension do not match.";
  bounds_ = bounds;
}

Interval FejerExperiment::getBounds() const
{
  return bounds_;
}

END_NAMESPACE_OPENTURNS
