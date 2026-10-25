//                                               -*- C++ -*-
/**
 *  @brief Class for the creation of a gradient implementation
 *         from an evaluation implementation by using noncentered
 *         finite difference formula.
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

#include "openturns/NonCenteredFiniteDifferenceGradient.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(NonCenteredFiniteDifferenceGradient)

static const Factory<NonCenteredFiniteDifferenceGradient> Factory_NonCenteredFiniteDifferenceGradient;

/* Default constructor */
NonCenteredFiniteDifferenceGradient::NonCenteredFiniteDifferenceGradient() :
  FiniteDifferenceGradient()
{
  // Nothing to do
}

/* Parameter constructor */
NonCenteredFiniteDifferenceGradient::NonCenteredFiniteDifferenceGradient(const Point & epsilon,
    const Evaluation & evaluation)
  : FiniteDifferenceGradient(epsilon, evaluation)
{
}

/* Parameter constructor */
NonCenteredFiniteDifferenceGradient::NonCenteredFiniteDifferenceGradient(const Scalar epsilon,
    const Evaluation & evaluation)
  : FiniteDifferenceGradient(epsilon, evaluation)
{
}

/* Parameter constructor */
NonCenteredFiniteDifferenceGradient::NonCenteredFiniteDifferenceGradient(const FiniteDifferenceStep & step,
    const Evaluation & evaluation)
  : FiniteDifferenceGradient(step, evaluation)
{
  // Nothing to do
}

/* Virtual constructor */
NonCenteredFiniteDifferenceGradient * NonCenteredFiniteDifferenceGradient::clone() const
{
  return new NonCenteredFiniteDifferenceGradient(*this);
}


/* Comparison operator */
Bool NonCenteredFiniteDifferenceGradient::operator ==(const NonCenteredFiniteDifferenceGradient & ) const
{
  return true;
}

Bool NonCenteredFiniteDifferenceGradient::equals(const GradientImplementation & other) const
{
  const NonCenteredFiniteDifferenceGradient * p_other = dynamic_cast<const NonCenteredFiniteDifferenceGradient *>(&other);
  return p_other && (*this == *p_other);
}


/* String converter */
String NonCenteredFiniteDifferenceGradient::__repr__() const
{
  OSS oss(true);
  oss << "class=" << NonCenteredFiniteDifferenceGradient::GetClassName()
      << " name=" << getName()
      << " epsilon=" << getEpsilon()
      << " evaluation=" << evaluation_.__repr__();
  return oss;
}

/* String converter */
String NonCenteredFiniteDifferenceGradient::__str__(const String & ) const
{
  return OSS(false) << "NonCenteredFiniteDifferenceGradient epsilon : " << getEpsilon();
}

/* Here is the interface that all derived class must implement */

/* Gradient () */
Matrix NonCenteredFiniteDifferenceGradient::gradient(const Point & inP) const
{
  const UnsignedInteger inputDimension = inP.getDimension();
  const UnsignedInteger outputDimension = evaluation_.getOutputDimension();
  if (finiteDifferenceSteps_.getSize() != outputDimension && finiteDifferenceSteps_.getSize() != 1) throw InvalidArgumentException(HERE) << "Invalid number of finite difference steps " << finiteDifferenceSteps_.getSize() << ", should be 1 or " << outputDimension;
  const Bool broadcastStep = (finiteDifferenceSteps_.getSize() == 1);
  const Point commonStep(broadcastStep ? finiteDifferenceSteps_[0](inP) : Point());
  // Resolve the step of each output, broadcasting the single step if any
  Collection<Point> steps(outputDimension);
  for (UnsignedInteger j = 0; j < outputDimension; ++j)
  {
    steps[j] = broadcastStep ? commonStep : finiteDifferenceSteps_[j](inP);
    if (inputDimension != steps[j].getDimension()) throw InvalidArgumentException(HERE) << "Invalid input dimension " << inputDimension << ", expected " << steps[j].getDimension();
  }
  // A single grid suffices when all outputs share the same steps
  Bool sharedSteps = true;
  for (UnsignedInteger j = 1; j < outputDimension; ++j)
    if (steps[j] != steps[0])
    {
      sharedSteps = false;
      break;
    }
  /* Compute the gradient */
  Matrix result(evaluation_.getInputDimension(), outputDimension);
  if (sharedSteps)
  {
    const Point step(steps[0]);
    /* At which points do we have to compute the evaluation for the decentered finite difference. We need 1+dim points. */
    Sample gridPoints(inputDimension + 1, inP);
    for(UnsignedInteger i = 0; i < inputDimension; ++i) gridPoints(i + 1, i) += step[i];
    /* Evaluate the evaluation */
    const Sample gridValues(evaluation_.operator()(gridPoints));
    /* Get the value at the center of the grid */
    const Point gridCenter(gridValues[0]);
    for (UnsignedInteger i = 0; i < result.getNbRows(); ++i)
    {
      // Recompute the actual step in order to take into account the round-off error in inP[i] + step[i]
      const Scalar hi = gridPoints(i + 1, i) - inP[i];
      for (UnsignedInteger j = 0; j < result.getNbColumns(); ++j)
        /* result(i, j) = (f_j(x + e_i) - f_j(x)) / e_i ~ df_j / dx_i */
        result(i, j) = (gridValues(i + 1, j) - gridCenter[j]) / hi;
    }
    return result;
  }
  // One grid per output as each output uses its own steps, at the
  // price of outputDimension times more function evaluations.
  // The center value is shared by all the per-output grids.
  const Point center(evaluation_(inP));
  for (UnsignedInteger j = 0; j < outputDimension; ++j)
  {
    const Point step(steps[j]);
    /* At which points do we have to compute the evaluation for the decentered finite difference. We need 1+dim points. */
    Sample gridPoints(inputDimension + 1, inP);
    for(UnsignedInteger i = 0; i < inputDimension; ++i) gridPoints(i + 1, i) += step[i];
    /* Evaluate the evaluation */
    const Sample gridValues(evaluation_.operator()(gridPoints));
    for (UnsignedInteger i = 0; i < result.getNbRows(); ++i)
    {
      // Recompute the actual step in order to take into account the round-off error in inP[i] + step[i]
      const Scalar hi = gridPoints(i + 1, i) - inP[i];
      /* result(i, j) = (f_j(x + e_i) - f_j(x)) / e_i ~ df_j / dx_i */
      result(i, j) = (gridValues(i + 1, j) - center[j]) / hi;
    }
  } // For j
  return result;
}

END_NAMESPACE_OPENTURNS
