//                                               -*- C++ -*-
/**
 *  @brief ActiveLearningUFunction implements the "U" active learning
 *  criterion used in reliability algorithms
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

#include "openturns/ActiveLearningUFunction.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include <cmath>

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class ActiveLearningUFunction
 */

CLASSNAMEINIT(ActiveLearningUFunction)

static const Factory<ActiveLearningUFunction> Factory_ActiveLearningUFunction;

/** Default constructor */
ActiveLearningUFunction::ActiveLearningUFunction()
  : ActiveLearningReliabilityFunctionImplementation()
{
  // Nothing to do
}

/* Constructor with parameters */
ActiveLearningUFunction::ActiveLearningUFunction(const Scalar reliabilityThreshold,
                                                 const Scalar learningThreshold)
  : ActiveLearningReliabilityFunctionImplementation(reliabilityThreshold,
                                                    learningThreshold)
{
}

/* Virtual constructor */
ActiveLearningUFunction * ActiveLearningUFunction::clone() const
{
  return new ActiveLearningUFunction(*this);
}


/* String converter */
String ActiveLearningUFunction::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << " derived from " << ActiveLearningReliabilityFunctionImplementation::__repr__();
  return oss;
}


/* Compute the U criterion for one point.

   Reference: Echard B., Gayton N., Lemaire M., "AK-MCS: An active learning
   reliability method combining Kriging and Monte Carlo Simulation",
   Structural Safety, 33(2), 145-154, 2011.
   The U value is the distance, in standard deviations of the Gaussian process,
   between the surrogate prediction and the limit-state threshold. */
Scalar ActiveLearningUFunction::computeAsScalar(const Point & x) const
{
  const Scalar mx = gprCov_.getConditionalMean(x)[0];
  const Scalar sk2 = gprCov_.getConditionalMarginalVariance(x);
  // Null variance, typically on a design point: the point carries no
  // information and must never be selected by the minimization
  if (!(sk2 > 0.0)) return SpecFunc::MaxScalar;
  const Scalar sk = std::sqrt(sk2);
  const Scalar u = std::abs(mx - reliabilityThreshold_) / sk;

  return u;
}

/* Test convergence of the U learning rule: the candidate pool is considered
   learned when its smallest U value reaches the learning threshold.

   Reference: Echard B., Gayton N., Lemaire M., "AK-MCS: An active learning
   reliability method combining Kriging and Monte Carlo Simulation",
   Structural Safety, 33(2), 145-154, 2011. */
Bool ActiveLearningUFunction::checkConvergenceLearning(const Sample & criterionValues) const
{
  if (criterionValues.getSize() == 0) throw InvalidArgumentException(HERE) << "Cannot check learning convergence on an empty sample of criterion values";
  const Scalar min_U = criterionValues.getMin()[0];

  return (min_U >= learningThreshold_);
}

/* Return sample corresponding to U criterion*/
Sample ActiveLearningUFunction::getInfillSample(const Sample & inputSample,
                                                const Sample & criterionValues) const
{
  const Indices selection(1, criterionValues.argsort(true)[0]);

  return inputSample.select(selection);
}

/* Selection direction: the infill point minimizes U */
Bool ActiveLearningUFunction::isMaximization() const
{
  return false;
}

/* Method save() stores the object through the StorageManager */
void ActiveLearningUFunction::save(Advocate & adv) const
{
  ActiveLearningReliabilityFunctionImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void ActiveLearningUFunction::load(Advocate & adv)
{
  ActiveLearningReliabilityFunctionImplementation::load(adv);
}




END_NAMESPACE_OPENTURNS
