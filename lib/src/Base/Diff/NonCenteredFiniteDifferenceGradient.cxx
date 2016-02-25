//                                               -*- C++ -*-
/**
 *  @brief Class for the creation of a numerical math gradient implementation
 *         form a numerical math evaluation implementation by using noncentered
 *         finite difference formula.
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
#include <cstdlib>

#include "openturns/NonCenteredFiniteDifferenceGradient.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(NonCenteredFiniteDifferenceGradient);

static const Factory<NonCenteredFiniteDifferenceGradient> RegisteredFactory;

/* Default constructor */
NonCenteredFiniteDifferenceGradient::NonCenteredFiniteDifferenceGradient() :
  FiniteDifferenceGradient()
{
  // Nothing to do
}

/* Parameter constructor */
NonCenteredFiniteDifferenceGradient::NonCenteredFiniteDifferenceGradient(const NumericalPoint & epsilon,
    const EvaluationImplementation & p_evaluation)
  : FiniteDifferenceGradient(epsilon, p_evaluation)
{
}

/* Parameter constructor */
NonCenteredFiniteDifferenceGradient::NonCenteredFiniteDifferenceGradient(const NumericalScalar epsilon,
    const EvaluationImplementation & p_evaluation)
  : FiniteDifferenceGradient(epsilon, p_evaluation)
{
}

/* Parameter constructor */
NonCenteredFiniteDifferenceGradient::NonCenteredFiniteDifferenceGradient(const FiniteDifferenceStep & step,
    const EvaluationImplementation & p_evaluation)
  : FiniteDifferenceGradient(step, p_evaluation)
{
  // Nothing to do
}

/* Virtual constructor */
NonCenteredFiniteDifferenceGradient * NonCenteredFiniteDifferenceGradient::clone() const
{
  return new NonCenteredFiniteDifferenceGradient(*this);
}


/* String converter */
String NonCenteredFiniteDifferenceGradient::__repr__() const
{
  OSS oss(true);
  oss << "class=" << NonCenteredFiniteDifferenceGradient::GetClassName()
      << " name=" << getName()
      << " epsilon=" << getEpsilon()
      << " evaluation=" << p_evaluation_->__repr__();
  return oss;
}

/* String converter */
String NonCenteredFiniteDifferenceGradient::__str__(const String & offset) const
{
  return OSS(false) << offset << "NonCenteredFiniteDifferenceGradient epsilon : " << getEpsilon();
}

/* Here is the interface that all derived class must implement */

/* Gradient () */
Matrix NonCenteredFiniteDifferenceGradient::gradient(const NumericalPoint & inP) const
{
  const UnsignedInteger inputDimension(inP.getDimension());
  NumericalPoint step(finiteDifferenceStep_.operator()(inP));
  if (inputDimension != step.getDimension()) throw InvalidArgumentException(HERE) << "Invalid input dimension";
  /* At which points do we have to compute the evaluation for the decentered finite difference. We need 1+dim pionts. */
  NumericalSample gridPoints(inputDimension + 1, inP);
  for(UnsignedInteger i = 0; i < inputDimension; ++i) gridPoints[i][i] += step[i];
  /* Evaluate the evaluation */
  NumericalSample gridValues(p_evaluation_->operator()(gridPoints));
  /* Get the value at the center of the grid */
  NumericalPoint center(gridValues[inputDimension]);
  /* Compute the gradient */
  Matrix result(p_evaluation_->getInputDimension(), p_evaluation_->getOutputDimension());
  for (UnsignedInteger i = 0; i < result.getNbRows(); ++i)
    for (UnsignedInteger j = 0; j < result.getNbColumns(); ++j)
      /* result(i, j) = (f_j(x + e_i) - f_j(x)) / e_i ~ df_j / dx_i */
      result(i, j) = (gridValues[i][j] - center[j]) / step[i];
  return result;
}

END_NAMESPACE_OPENTURNS
