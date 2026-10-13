//                                               -*- C++ -*-
/**
 *  @brief LeastSquares solving algorithm interface
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

#include "openturns/LinearLeastSquaresMethod.hxx"
#include "openturns/LinearLeastSquaresMethodImplementation.hxx"
#include "openturns/SVDMethod.hxx"
#include "openturns/CholeskyMethod.hxx"
#include "openturns/QRMethod.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(LinearLeastSquaresMethod)

/* Default constructor */
LinearLeastSquaresMethod::LinearLeastSquaresMethod()
  : TypedInterfaceObject<LinearLeastSquaresMethodImplementation>(new LinearLeastSquaresMethodImplementation())
{
  // Nothing to do
}

/* Parameters constructor */
LinearLeastSquaresMethod::LinearLeastSquaresMethod (const DesignProxy & proxy,
                                        const Point & weight,
                                        const Indices & indices)
  : TypedInterfaceObject<LinearLeastSquaresMethodImplementation>(new SVDMethod(proxy, weight, indices))
{
  // Nothing to do
}

/* Parameters constructor */
LinearLeastSquaresMethod::LinearLeastSquaresMethod (const DesignProxy & proxy,
                                        const Indices & indices)
  : TypedInterfaceObject<LinearLeastSquaresMethodImplementation>(new SVDMethod(proxy, indices))
{
  // Nothing to do
}

/* Parameters constructor */
LinearLeastSquaresMethod::LinearLeastSquaresMethod (const Matrix & matrix)
  : TypedInterfaceObject<LinearLeastSquaresMethodImplementation>(new SVDMethod(matrix))
{
  // Nothing to do
}

/* Constructor from implementation */
LinearLeastSquaresMethod::LinearLeastSquaresMethod(const Implementation & p_implementation)
  : TypedInterfaceObject<LinearLeastSquaresMethodImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor from implementation */
LinearLeastSquaresMethod::LinearLeastSquaresMethod(const LinearLeastSquaresMethodImplementation & implementation)
  : TypedInterfaceObject<LinearLeastSquaresMethodImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from implementation pointer */
LinearLeastSquaresMethod::LinearLeastSquaresMethod(LinearLeastSquaresMethodImplementation * p_implementation)
  : TypedInterfaceObject<LinearLeastSquaresMethodImplementation>(p_implementation)
{
  // Nothing to do
}

/* String converter */
String LinearLeastSquaresMethod::__repr__() const
{
  return getImplementation()->__repr__();
}

String LinearLeastSquaresMethod::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

Point LinearLeastSquaresMethod::solve(const Point & rhs)
{
  return getImplementation()->solve(rhs);
}

Point LinearLeastSquaresMethod::solveNormal(const Point & rhs)
{
  return getImplementation()->solveNormal(rhs);
}

Point LinearLeastSquaresMethod::solveNormalGram(const Point & rhs)
{
  return getImplementation()->solveNormalGram(rhs);
}

Point LinearLeastSquaresMethod::getHDiag() const
{
  return getImplementation()->getHDiag();
}

SymmetricMatrix LinearLeastSquaresMethod::getH() const
{
  return getImplementation()->getH();
}

CovarianceMatrix LinearLeastSquaresMethod::getGramInverse() const
{
  return getImplementation()->getGramInverse();
}

Point LinearLeastSquaresMethod::getGramInverseDiag() const
{
  return getImplementation()->getGramInverseDiag();
}

Scalar LinearLeastSquaresMethod::getGramInverseTrace() const
{
  return getImplementation()->getGramInverseTrace();
}

void LinearLeastSquaresMethod::update(const Indices& addedIndices,
                                const Indices& conservedIndices,
                                const Indices& removedIndices,
                                const Bool row)
{
  getImplementation()->update(addedIndices, conservedIndices, removedIndices, row);
}

LinearLeastSquaresMethod::FunctionCollection LinearLeastSquaresMethod::getBasis() const
{
  return getImplementation()->getBasis();
}

Indices LinearLeastSquaresMethod::getCurrentIndices() const
{
  return getImplementation()->getCurrentIndices();
}

Indices LinearLeastSquaresMethod::getInitialIndices() const
{
  return getImplementation()->getInitialIndices();
}

Sample LinearLeastSquaresMethod::getInputSample() const
{
  return getImplementation()->getInputSample();
}

Point LinearLeastSquaresMethod::getWeight() const
{
  return getImplementation()->getWeight();
}

Matrix LinearLeastSquaresMethod::computeWeightedDesign(bool whole) const
{
  return getImplementation()->computeWeightedDesign(whole);
}

Matrix LinearLeastSquaresMethod::computeDesign(bool whole) const
{
  return getImplementation()->computeDesign(whole);
}

LinearLeastSquaresMethod LinearLeastSquaresMethod::Build(const String name,
    const DesignProxy & proxy,
    const Point & weight,
    const Indices & indices)
{
  if      (name == "SVD")      return SVDMethod(proxy, weight, indices);
  else if (name == "Cholesky") return CholeskyMethod(proxy, weight, indices);
  else if (name == "QR")       return QRMethod(proxy, weight, indices);
  else throw InvalidArgumentException(HERE) << "Error: invalid value for decomposition method: " << name;
  return LinearLeastSquaresMethod();
}


LinearLeastSquaresMethod LinearLeastSquaresMethod::Build(const String name,
    const DesignProxy & proxy,
    const Indices & indices)
{
  if      (name == "SVD")      return SVDMethod(proxy, indices);
  else if (name == "Cholesky") return CholeskyMethod(proxy, indices);
  else if (name == "QR")       return QRMethod(proxy, indices);
  else throw InvalidArgumentException(HERE) << "Error: invalid value for decomposition method: " << name;
  return LinearLeastSquaresMethod();
}

LinearLeastSquaresMethod LinearLeastSquaresMethod::Build(const String name,
    const Matrix & matrix)
{
  if      (name == "SVD")      return SVDMethod(matrix);
  else if (name == "Cholesky") return CholeskyMethod(matrix);
  else if (name == "QR")       return QRMethod(matrix);
  else throw InvalidArgumentException(HERE) << "Error: invalid value for decomposition method: " << name;
  return LinearLeastSquaresMethod();
}

END_NAMESPACE_OPENTURNS
