//                                               -*- C++ -*-
/**
 *  @brief LeastSquares solving algorithm interface
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

#include "openturns/LeastSquaresMethod.hxx"
#include "openturns/LeastSquaresMethodImplementation.hxx"
#include "openturns/SVDMethod.hxx"
#include "openturns/CholeskyMethod.hxx"
#include "openturns/QRMethod.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(LeastSquaresMethod)

/* Default constructor */
LeastSquaresMethod::LeastSquaresMethod()
  : TypedInterfaceObject<LeastSquaresMethodImplementation>(new LeastSquaresMethodImplementation())
{
  // Nothing to do
}

/* Parameters constructor */
LeastSquaresMethod::LeastSquaresMethod (const DesignProxy & proxy,
                                        const Point & weight,
                                        const Indices & indices)
  : TypedInterfaceObject<LeastSquaresMethodImplementation>(new SVDMethod(proxy, weight, indices))
{
  // Nothing to do
}

/* Parameters constructor */
LeastSquaresMethod::LeastSquaresMethod (const DesignProxy & proxy,
                                        const Indices & indices)
  : TypedInterfaceObject<LeastSquaresMethodImplementation>(new SVDMethod(proxy, indices))
{
  // Nothing to do
}

/* Parameters constructor */
LeastSquaresMethod::LeastSquaresMethod (const Matrix & matrix)
  : TypedInterfaceObject<LeastSquaresMethodImplementation>(new SVDMethod(matrix))
{
  // Nothing to do
}

/* Constructor from implementation */
LeastSquaresMethod::LeastSquaresMethod(const Implementation & p_implementation)
  : TypedInterfaceObject<LeastSquaresMethodImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor from implementation */
LeastSquaresMethod::LeastSquaresMethod(const LeastSquaresMethodImplementation & implementation)
  : TypedInterfaceObject<LeastSquaresMethodImplementation>(implementation.clone())
{
  // Nothing to do
}

/* String converter */
String LeastSquaresMethod::__repr__() const
{
  return getImplementation()->__repr__();
}

String LeastSquaresMethod::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

Point LeastSquaresMethod::solve(const Point & rhs)
{
  return getImplementation()->solve(rhs);
}

Point LeastSquaresMethod::solveNormal(const Point & rhs)
{
  return getImplementation()->solveNormal(rhs);
}

Point LeastSquaresMethod::getHDiag() const
{
  return getImplementation()->getHDiag();
}

SymmetricMatrix LeastSquaresMethod::getH() const
{
  return getImplementation()->getH();
}

CovarianceMatrix LeastSquaresMethod::getGramInverse() const
{
  return getImplementation()->getGramInverse();
}

Point LeastSquaresMethod::getGramInverseDiag() const
{
  return getImplementation()->getGramInverseDiag();
}

Scalar LeastSquaresMethod::getGramInverseTrace() const
{
  return getImplementation()->getGramInverseTrace();
}

void LeastSquaresMethod::update(const Indices& addedIndices,
                                const Indices& conservedIndices,
                                const Indices& removedIndices,
                                const Bool row)
{
  getImplementation()->update(addedIndices, conservedIndices, removedIndices, row);
}

LeastSquaresMethod::FunctionCollection LeastSquaresMethod::getBasis() const
{
  return getImplementation()->getBasis();
}

Indices LeastSquaresMethod::getCurrentIndices() const
{
  return getImplementation()->getCurrentIndices();
}

Indices LeastSquaresMethod::getInitialIndices() const
{
  return getImplementation()->getInitialIndices();
}

Sample LeastSquaresMethod::getInputSample() const
{
  return getImplementation()->getInputSample();
}

Point LeastSquaresMethod::getWeight() const
{
  return getImplementation()->getWeight();
}

Matrix LeastSquaresMethod::computeWeightedDesign(bool whole) const
{
  return getImplementation()->computeWeightedDesign(whole);
}

LeastSquaresMethod LeastSquaresMethod::Build(const String name,
    const DesignProxy & proxy,
    const Point & weight,
    const Indices & indices)
{
  if      (name == "SVD")      return SVDMethod(proxy, weight, indices);
  else if (name == "Cholesky") return CholeskyMethod(proxy, weight, indices);
  else if (name == "QR")       return QRMethod(proxy, weight, indices);
  else throw InvalidArgumentException(HERE) << "Error: invalid value for decomposition method: " << name;
  return LeastSquaresMethod();
}


LeastSquaresMethod LeastSquaresMethod::Build(const String name,
    const DesignProxy & proxy,
    const Indices & indices)
{
  if      (name == "SVD")      return SVDMethod(proxy, indices);
  else if (name == "Cholesky") return CholeskyMethod(proxy, indices);
  else if (name == "QR")       return QRMethod(proxy, indices);
  else throw InvalidArgumentException(HERE) << "Error: invalid value for decomposition method: " << name;
  return LeastSquaresMethod();
}

LeastSquaresMethod LeastSquaresMethod::Build(const String name,
    const Matrix & matrix)
{
  if      (name == "SVD")      return SVDMethod(matrix);
  else if (name == "Cholesky") return CholeskyMethod(matrix);
  else if (name == "QR")       return QRMethod(matrix);
  else throw InvalidArgumentException(HERE) << "Error: invalid value for decomposition method: " << name;
  return LeastSquaresMethod();
}

END_NAMESPACE_OPENTURNS
