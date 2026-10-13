//                                               -*- C++ -*-
/**
 *  @brief Class for the creation of a gradient implementation
 *         from an evaluation implementation by using centered
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

#include "openturns/CenteredFiniteDifferenceGradient.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(CenteredFiniteDifferenceGradient)

static const Factory<CenteredFiniteDifferenceGradient> Factory_CenteredFiniteDifferenceGradient;

/* Default constructor */
CenteredFiniteDifferenceGradient::CenteredFiniteDifferenceGradient() :
  FiniteDifferenceGradient()
{
  // Nothing to do
}

/* Parameter constructor */
CenteredFiniteDifferenceGradient::CenteredFiniteDifferenceGradient(const Point & epsilon,
    const Evaluation & evaluation)
  : FiniteDifferenceGradient(epsilon, evaluation)

{
}

/* Parameter constructor */
CenteredFiniteDifferenceGradient::CenteredFiniteDifferenceGradient(const Scalar epsilon,
    const Evaluation & evaluation)
  : FiniteDifferenceGradient(epsilon, evaluation)
{
}

/* Parameter constructor */
CenteredFiniteDifferenceGradient::CenteredFiniteDifferenceGradient(const FiniteDifferenceStep & step,
    const Evaluation & evaluation)
  : FiniteDifferenceGradient(step, evaluation)
{
  // Nothing to do
}

/* Virtual constructor */
CenteredFiniteDifferenceGradient * CenteredFiniteDifferenceGradient::clone() const
{
  return new CenteredFiniteDifferenceGradient(*this);
}


/* Comparison operator */
Bool CenteredFiniteDifferenceGradient::operator ==(const CenteredFiniteDifferenceGradient & ) const
{
  return true;
}

Bool CenteredFiniteDifferenceGradient::equals(const GradientImplementation & other) const
{
  const CenteredFiniteDifferenceGradient * p_other = dynamic_cast<const CenteredFiniteDifferenceGradient *>(&other);
  return p_other && (*this == *p_other);
}


/* String converter */
String CenteredFiniteDifferenceGradient::__repr__() const
{
  OSS oss(true);
  oss << "class=" << CenteredFiniteDifferenceGradient::GetClassName()
      << " name=" << getName()
      << " epsilon=" << getEpsilon().__repr__()
      << " evaluation=" << evaluation_.__repr__();
  return oss;
}

/* String converter */
String CenteredFiniteDifferenceGradient::__str__(const String & ) const
{
  return OSS(false) << "CenteredFiniteDifferenceGradient epsilon : " << getEpsilon();
}

/* Here is the interface that all derived class must implement */

/* Gradient () */
Matrix CenteredFiniteDifferenceGradient::gradient(const Point & inP) const
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
    if (inputDimension != steps[j].getDimension()) throw InvalidArgumentException(HERE) << "Invalid input dimension " << inputDimension << ", should be " << steps[j].getDimension();
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
    /* At which points do we have to compute the evaluation for the centered finite difference. We need 2*dim points. */
    Sample gridPoints(2 * inputDimension, inP);
    for(UnsignedInteger i = 0; i < inputDimension; ++i)
    {
      gridPoints(2 * i, i) += step[i];
      gridPoints(2 * i + 1, i) -= step[i];
    } // For i
    /* Evaluate the evaluation */
    const Sample gridValues(evaluation_.operator()(gridPoints));
    for (UnsignedInteger i = 0; i < result.getNbRows(); ++i)
    {
      // Recompute the actual step in order to take into account the round-off error in inP[i] + step[i] and inP[i] - step[i]
      const Scalar hi = gridPoints(2 * i, i) - gridPoints(2 * i + 1, i);
      for (UnsignedInteger j = 0; j < result.getNbColumns(); ++j)
        /* result(i, j) = (f_j(x + e_i) - f_j(x - e_i)) / (2 * e_i) ~ df_j / dx_i */
        result(i, j) = (gridValues(2 * i, j) - gridValues(2 * i + 1, j)) / hi;
    }
    return result;
  }
  // One grid per output as each output uses its own steps, at the
  // price of outputDimension times more function evaluations
  for (UnsignedInteger j = 0; j < outputDimension; ++j)
  {
    const Point step(steps[j]);
    /* At which points do we have to compute the evaluation for the centered finite difference. We need 2*dim points. */
    Sample gridPoints(2 * inputDimension, inP);
    for(UnsignedInteger i = 0; i < inputDimension; ++i)
    {
      gridPoints(2 * i, i) += step[i];
      gridPoints(2 * i + 1, i) -= step[i];
    } // For i
    /* Evaluate the evaluation */
    const Sample gridValues(evaluation_.operator()(gridPoints));
    for (UnsignedInteger i = 0; i < result.getNbRows(); ++i)
    {
      // Recompute the actual step in order to take into account the round-off error in inP[i] + step[i] and inP[i] - step[i]
      const Scalar hi = gridPoints(2 * i, i) - gridPoints(2 * i + 1, i);
      /* result(i, j) = (f_j(x + e_i) - f_j(x - e_i)) / (2 * e_i) ~ df_j / dx_i */
      result(i, j) = (gridValues(2 * i, j) - gridValues(2 * i + 1, j)) / hi;
    }
  } // For j
  return result;
}

END_NAMESPACE_OPENTURNS
