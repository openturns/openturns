//                                               -*- C++ -*-
/**
 *  @brief Class for the creation of a numerical math hessian implementation
 *         form a numerical math evaluation implementation by using centered
 *         finite difference formula.
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/CenteredFiniteDifferenceHessian.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(CenteredFiniteDifferenceHessian)

static const Factory<CenteredFiniteDifferenceHessian> Factory_CenteredFiniteDifferenceHessian;

/* Default constructor */
CenteredFiniteDifferenceHessian::CenteredFiniteDifferenceHessian()
  : FiniteDifferenceHessian()
{
  // Nothing to do
}

/* Parameter constructor */
CenteredFiniteDifferenceHessian::CenteredFiniteDifferenceHessian(const Point & epsilon,
    const Evaluation & evaluation)
  : FiniteDifferenceHessian(epsilon, evaluation)
{
  // Nothing to do
}

/* Parameter constructor */
CenteredFiniteDifferenceHessian::CenteredFiniteDifferenceHessian(const Scalar epsilon,
    const Evaluation & evaluation)
  : FiniteDifferenceHessian(epsilon, evaluation)
{
  // Nothing to do
}

/* Parameter constructor */
CenteredFiniteDifferenceHessian::CenteredFiniteDifferenceHessian(const FiniteDifferenceStep & finiteDifferenceStep,
    const Evaluation & evaluation)
  : FiniteDifferenceHessian(finiteDifferenceStep, evaluation)
{
  // Nothing to do
}

/* Virtual constructor */
CenteredFiniteDifferenceHessian * CenteredFiniteDifferenceHessian::clone() const
{
  return new CenteredFiniteDifferenceHessian(*this);
}

/* Comparison operator */
Bool CenteredFiniteDifferenceHessian::operator ==(const CenteredFiniteDifferenceHessian & other) const
{
  return (getEpsilon() == other.getEpsilon());
}

/* String converter */
String CenteredFiniteDifferenceHessian::__repr__() const
{
  OSS oss(true);
  oss << "class=" << CenteredFiniteDifferenceHessian::GetClassName()
      << " name=" << getName()
      << " epsilon=" << getEpsilon().__repr__()
      << " evaluation=" << evaluation_.getImplementation()->__repr__();
  return oss;
}

/* String converter */
String CenteredFiniteDifferenceHessian::__str__(const String & ) const
{
  return OSS(false) << "CenteredFiniteDifferenceHessian epsilon : " << getEpsilon();
}

/* Here is the interface that all derived class must implement */

/* Hessian () */
SymmetricTensor CenteredFiniteDifferenceHessian::hessian(const Point & inP) const
{
  const UnsignedInteger inputDimension = inP.getDimension();
  Point step(finiteDifferenceStep_.operator()(inP));
  if (inputDimension != step.getDimension()) throw InvalidArgumentException(HERE) << "Invalid input dimension";
  /* At which points do we have to compute the evaluation for the centered finite difference. We need 2*dim^2+1 points. */
  Sample gridPoints(2 * inputDimension * inputDimension + 1, inP);
  UnsignedInteger index = 1;
  for(UnsignedInteger i = 1; i < inputDimension; ++i)
    for(UnsignedInteger j = 0; j < i; ++j)
    {
      gridPoints(index, i) += step[i];
      gridPoints(index, j) += step[j];
      ++index;
      gridPoints(index, i) += step[i];
      gridPoints(index, j) -= step[j];
      ++index;
      gridPoints(index, i) -= step[i];
      gridPoints(index, j) -= step[j];
      ++index;
      gridPoints(index, i) -= step[i];
      gridPoints(index, j) += step[j];
      ++index;
    } // For j
  for(UnsignedInteger i = 0; i < inputDimension; ++i)
  {
    /* Special case for the diagonal terms, in order to avoid computing twice
     *  f(x) = f(x + e_i - e_i) = f(x - e_i + e_i) */
    gridPoints(index, i) += 2.0 * step[i];
    ++index;
    gridPoints(index, i) -= 2.0 * step[i];
    ++index;
  } // For i
  /* Evaluate the evaluation */
  Sample gridValues(evaluation_.operator()(gridPoints));
  /* Get the center value */
  Point center(gridValues[0]);
  /* Compute the hessian */
  UnsignedInteger outputDimension = evaluation_.getOutputDimension();
  SymmetricTensor result(inputDimension, outputDimension);
  UnsignedInteger diagonalOffset = 1 + 2 * inputDimension * (inputDimension - 1);
  Scalar scale = -1.0;
  UnsignedInteger offDiagonalOffset = 1;
  for (UnsignedInteger i = 0; i < inputDimension; ++i)
  {
    // Diagonal term
    /* result(i, i, k) = (f_k(x + 2 * e_i) - 2 * f_k(x) + f_k(x + 2 * e_i)) / (4 * e_i * e_i) */
    scale = 1.0 / (4.0 * step[i] * step[i]);
    for (UnsignedInteger k = 0; k < outputDimension; ++k)
    {
      result(i, i, k) = scale * (gridValues(diagonalOffset + 2 * i, k) - 2.0 * center[k] + gridValues(diagonalOffset + 2 * i + 1, k));
    } // k
    // Compute only the upper part of each sheet, as the hessian is symmetric
    for (UnsignedInteger j = 0; j < i; ++j)
    {
      scale = 1.0 / (4.0 * step[i] * step[j]);
      for (UnsignedInteger k = 0; k < outputDimension; ++k)
        /* result(i, j, k) = (f_k(x + e_i + e_j) - f_k(x + e_i - e_j) + f_k(x - e_i - e_j) - f_k(x - e_i + e_j)) / (4 * e_i * e_j) ~ d2f_k / dx_idx_j */
        result(i, j, k) = scale * (gridValues(offDiagonalOffset + 4 * j, k) - gridValues(offDiagonalOffset + 4 * j + 1, k) + gridValues(offDiagonalOffset + 4 * j + 2, k) - gridValues(offDiagonalOffset + 4 * j + 3, k));
    } // j
    offDiagonalOffset += 4 * i;
  } // i
  return result;
}


END_NAMESPACE_OPENTURNS
