//                                               -*- C++ -*-
/**
 *  @brief Class for the creation of a numerical math gradient implementation
 *         form a numerical math evaluation implementation by using centered
 *         finite difference formula.
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <cstdlib>

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


/* String converter */
String CenteredFiniteDifferenceGradient::__repr__() const
{
  OSS oss(true);
  oss << "class=" << CenteredFiniteDifferenceGradient::GetClassName()
      << " name=" << getName()
      << " epsilon=" << getEpsilon().__repr__()
      << " evaluation=" << evaluation_.getImplementation()->__repr__();
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
//     std::cout << "CenteredFiniteDifferenceGradient::gradient inP="<<inP[0]<<std::endl;

  const Point step(finiteDifferenceStep_.operator()(inP));
  if (inputDimension != step.getDimension()) throw InvalidArgumentException(HERE) << "Invalid input dimension";
  /* At which points do we have to compute the evaluation for the centered finite difference. We need 2*dim points. */
  Sample gridPoints(2 * inputDimension, inP);
  for(UnsignedInteger i = 0; i < inputDimension; ++i)
  {
    gridPoints(2 * i, i) += step[i];
    gridPoints(2 * i + 1, i) -= step[i];
  } // For i
  
//   std::cout << "CenteredFiniteDifferenceGradient::gradient gridPoints="<<gridPoints(0,0)<<"|"<<gridPoints(1,0)<<std::endl;
  
  /* Evaluate the evaluation */
  const Sample gridValues(evaluation_.operator()(gridPoints));
//   std::cout << "CenteredFiniteDifferenceGradient::gradient gridValues="<<gridValues(0,0)<<"|"<<gridValues(1,0)<<std::endl;
  /* Compute the gradient */
  Matrix result(evaluation_.getInputDimension(), evaluation_.getOutputDimension());
  for (UnsignedInteger i = 0; i < result.getNbRows(); ++i)
    {
      // Recompute the actual step in order to take into account the round-off error in inP[i] + step[i] and inP[i] - step[i]
      const Scalar hi = gridPoints(2 * i, i) - gridPoints(2 * i + 1, i);
      for (UnsignedInteger j = 0; j < result.getNbColumns(); ++j)
        /* result(i, j) = (f_j(x + e_i) - f_j(x - e_i)) / (2 * e_i) ~ df_j / dx_i */
        result(i, j) = (gridValues(2 * i, j) - gridValues(2 * i + 1, j)) / hi;
    }
//   std::cout << "CenteredFiniteDifferenceGradient::gradient result="<<result(0,0)<<"|"<<result(1,0)<<std::endl;
  return result;
}

END_NAMESPACE_OPENTURNS
