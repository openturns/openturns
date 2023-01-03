//                                               -*- C++ -*-
/**
 *  @brief SimulatedAnnealingLHS
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SimulatedAnnealingLHS.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/Log.hxx"
#include "openturns/SpaceFillingC2.hxx"
#include "openturns/SpaceFillingPhiP.hxx"
#include "openturns/SpaceFillingMinDist.hxx"
#include "openturns/KPermutationsDistribution.hxx"


namespace OT
{

CLASSNAMEINIT(SimulatedAnnealingLHS)

static const Factory<SimulatedAnnealingLHS> Factory_SimulatedAnnealingLHS;


/* Default constructor */
SimulatedAnnealingLHS::SimulatedAnnealingLHS()
  : OptimalLHSExperiment()
{
  // Nothing to do
}

/* Geometric temperature profil */
SimulatedAnnealingLHS::SimulatedAnnealingLHS(const LHSExperiment & lhs,
    const SpaceFilling & spaceFilling,
    const TemperatureProfile & profile)
  : OptimalLHSExperiment(lhs, spaceFilling)
  , profile_(profile)
  , standardInitialDesign_()
{
}

/* SimulatedAnnealingLHS constructor with LHS*/
SimulatedAnnealingLHS::SimulatedAnnealingLHS (const Sample & initialDesign,
    const Distribution & distribution,
    const SpaceFilling & spaceFilling,
    const TemperatureProfile & profile)
  : OptimalLHSExperiment()
  , profile_(profile)
  , standardInitialDesign_(initialDesign)
{
  spaceFilling_ = spaceFilling;
  if (initialDesign.getSize() == 0) throw InvalidArgumentException(HERE) << "Initial design must not be empty";
  if (initialDesign.getDimension() != distribution.getDimension()) throw InvalidArgumentException(HERE) << "Initial design dimension " << initialDesign.getDimension() << " does not match distribution dimension " << distribution.getDimension();
  // Transform the initial design into a standard design
  // dummy lhs, only distribution is needed
  // It is used to
  setLHS(LHSExperiment(distribution, 1));
  standardInitialDesign_ = transformation_(initialDesign);
}

/* Virtual constructor method */
SimulatedAnnealingLHS * SimulatedAnnealingLHS::clone() const
{
  return new SimulatedAnnealingLHS(*this);
}

/* Generate design method */
Sample SimulatedAnnealingLHS::generateWithWeights(Point & weights) const
{
  weights = Point(size_, 1.0 / size_);
  return generateWithRestart(0);
}

Sample SimulatedAnnealingLHS::generateWithRestart(UnsignedInteger nRestart) const
{
  // LHSResult returns resuls for all restarts
  LHSResult result(spaceFilling_, nRestart);
  for(UnsignedInteger index = 0; index < nRestart + 1; ++index)
  {
    // history has dimension 3 :crit, proba & temperature
    // Total size depends on convergency
    Sample history(0, 3);
    Description historyDescription(3);
    historyDescription[0] = spaceFilling_.getImplementation()->getName() + " criterion";
    historyDescription[1] = "Probability";
    historyDescription[2] = "Temperature";
    history.setDescription(historyDescription);
    LOGDEBUG("Starting simulated annealing process");

    // Starting sample, in the [0,1]^d space
    Sample standardOptimalDesign(standardInitialDesign_.getSize() > 0 ? standardInitialDesign_ : lhs_.generateStandard());

    // Starting implementation
    UnsignedInteger iteration(0);
    Scalar T(profile_.getT0());
    Scalar optimalValue(spaceFilling_.evaluate(standardOptimalDesign));
    const UnsignedInteger iMax(profile_.getIMax());
    while(iteration < iMax && T > 0)
    {
      LOGDEBUG(OSS() << "Current iteration =" <<  iteration << ", current temperature =" << T);
      // Generate rows & column indexes
      const UnsignedInteger columnIndex = RandomGenerator::IntegerGenerate(standardOptimalDesign.getDimension());
      const UnsignedInteger row1 = RandomGenerator::IntegerGenerate(standardOptimalDesign.getSize());
      const UnsignedInteger row2 = RandomGenerator::IntegerGenerate(standardOptimalDesign.getSize());
      if (row1 == row2) continue;
      // WARNING: bernoulliTrial is computed here and not in the else clause below to avoid
      //          numerical discrepancies between x86 and x86_64.  When optimalValue and
      //          newCriterion are almost equal, architectures may go to different
      //          branches.  In order to have the same random generator state,
      //          RandomGenerator::Generate() is called here.
      const Scalar bernoulliTrial = RandomGenerator::Generate();
      const Scalar newCriterion = spaceFilling_.perturbLHS(standardOptimalDesign, optimalValue, row1, row2, columnIndex);
      Scalar criteriaDifference = 1.0;
      // In case of minimization, we hope newCiterion to be less or equal than current optimalValue
      if (spaceFilling_.isMinimizationProblem())
        criteriaDifference = std::min(std::exp((optimalValue - newCriterion) / T), 1.0);
      else
        criteriaDifference = std::min(std::exp((newCriterion - optimalValue) / T), 1.0);
      // Decision with respect to criteriaDifference
      if ((optimalValue >= newCriterion && spaceFilling_.isMinimizationProblem()) || (optimalValue < newCriterion && !spaceFilling_.isMinimizationProblem()))
      {
        std::swap(standardOptimalDesign(row1, columnIndex), standardOptimalDesign(row2, columnIndex));
        optimalValue = newCriterion;
      }
      else
      {
        if (bernoulliTrial < criteriaDifference)
        {
          std::swap(standardOptimalDesign(row1, columnIndex), standardOptimalDesign(row2, columnIndex));
          optimalValue = newCriterion;
        }
      }
      LOGDEBUG(OSS() << "Current optimal value =" << optimalValue);
      Point historyElement(3);
      historyElement[0] = optimalValue;
      historyElement[1] = criteriaDifference;
      historyElement[2] = T;
      history.add(historyElement);
      // Update iteration, temperature
      ++ iteration;
      T = profile_(iteration);
    }
    LOGDEBUG("End of simulated annealing process");
    // Add elements to result
    result.add(transformation_(standardOptimalDesign), optimalValue, SpaceFillingC2().evaluate(standardOptimalDesign), SpaceFillingPhiP().evaluate(standardOptimalDesign), SpaceFillingMinDist().evaluate(standardOptimalDesign), history);
  }
  result_ = result;
  Sample sample(result.getOptimalDesign());
  sample.setDescription(lhs_.getDistribution().getDescription());
  return sample;
}

/* String converter */
String SimulatedAnnealingLHS::__repr__() const
{
  OSS oss;
  oss << "class=" << SimulatedAnnealingLHS::GetClassName()
      << " name=" << getName()
      << " bounds=" << lhs_.getDistribution().getRange()
      << " spaceFilling=" << spaceFilling_
      << " profile=" << profile_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void SimulatedAnnealingLHS::save(Advocate & adv) const
{
  OptimalLHSExperiment::save( adv );
  adv.saveAttribute( "profile_", profile_ );
  adv.saveAttribute( "standardInitialDesign_", standardInitialDesign_ );
}

/* Method load() reloads the object from the StorageManager */
void SimulatedAnnealingLHS::load(Advocate & adv)
{
  OptimalLHSExperiment::load( adv );
  adv.loadAttribute( "profile_", profile_ );
  adv.loadAttribute( "standardInitialDesign_", standardInitialDesign_ );
}


} /* namespace OT */
