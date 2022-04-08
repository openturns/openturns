//                                               -*- C++ -*-
/**
 *  @brief Abstract class for algorithms generating optimized LHS
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/OptimalLHSExperiment.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/LHSExperiment.hxx"
#include "openturns/PersistentObjectFactory.hxx"


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(OptimalLHSExperiment)

static const Factory<OptimalLHSExperiment> Factory_OptimalLHSExperiment;


/* Default constructor */
OptimalLHSExperiment::OptimalLHSExperiment(const LHSExperiment & lhs)
  : WeightedExperimentImplementation(lhs.getDistribution(), lhs.getSize())
  , spaceFilling_()
{
  setLHS(lhs);
}

/* Default constructor */
OptimalLHSExperiment::OptimalLHSExperiment(const LHSExperiment & lhs,
    const SpaceFilling & spaceFilling)
  : WeightedExperimentImplementation(lhs.getDistribution(), lhs.getSize())
  , spaceFilling_(spaceFilling)
{
  setLHS(lhs);
}

void OptimalLHSExperiment::setLHS(const LHSExperiment & lhs)
{
  lhs_ = lhs;
  setSize(lhs.getSize());
  setDistribution(lhs.getDistribution());

  // marginal extraction can be costly
  const Distribution distribution(getDistribution());
  const UnsignedInteger dimension = distribution.getDimension();
  ComposedDistribution::DistributionCollection marginals(dimension);
  for (UnsignedInteger j = 0; j < dimension; ++ j)
    marginals[j] = distribution.getMarginal(j);
  // Build the iso-probabilistic transformation
  transformation_ = MarginalTransformationEvaluation(marginals, MarginalTransformationEvaluation::TO);
}

/* Virtual constructor method */
OptimalLHSExperiment * OptimalLHSExperiment::clone() const
{
  return new OptimalLHSExperiment(*this);
}

/** Attributes for dimension, size, randomDesign */
LHSExperiment OptimalLHSExperiment::getLHS() const
{
  return lhs_;
}

/** Attributes for SpaceFilling */
SpaceFilling OptimalLHSExperiment::getSpaceFilling() const
{
  return spaceFilling_;
}


LHSResult OptimalLHSExperiment::getResult() const
{
  return result_;
}

/* String converter */
String OptimalLHSExperiment::__repr__() const
{
  OSS oss;
  oss << "class=" << OptimalLHSExperiment::GetClassName()
      << " name=" << getName()
      << " lhs=" << lhs_
      << " spaceFilling=" << spaceFilling_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void OptimalLHSExperiment::save(Advocate & adv) const
{
  WeightedExperimentImplementation::save( adv );
  adv.saveAttribute("lhs_", lhs_);
  adv.saveAttribute("spaceFilling_", spaceFilling_);
}

/* Method load() reloads the object from the StorageManager */
void OptimalLHSExperiment::load(Advocate & adv)
{
  WeightedExperimentImplementation::load( adv );
  adv.loadAttribute("lhs_", lhs_);
  setLHS(lhs_);
  adv.loadAttribute("spaceFilling_", spaceFilling_);
}


END_NAMESPACE_OPENTURNS
