//                                               -*- C++ -*-
/**
 *  @file  HODLRMatrix.cxx
 *  @brief HODLR compressed matrix (TypedInterfaceObject wrapper)
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
 */
#include "openturns/HODLRMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(HODLRMatrix)

HODLRMatrix::HODLRMatrix()
  : TypedInterfaceObject<HODLRMatrixImplementation>(new HODLRMatrixImplementation())
{
}

HODLRMatrix::HODLRMatrix(const Implementation & i)
  : TypedInterfaceObject<HODLRMatrixImplementation>(i)
{
}

HODLRMatrix::HODLRMatrix(const HODLRMatrixImplementation & i)
  : TypedInterfaceObject<HODLRMatrixImplementation>(i.clone())
{
}

HODLRMatrix HODLRMatrix::copy() const
{
  return HODLRMatrix(new HODLRMatrixImplementation(*getImplementation()));
}

UnsignedInteger HODLRMatrix::getNbRows() const
{
  return getImplementation()->getNbRows();
}

UnsignedInteger HODLRMatrix::getNbColumns() const
{
  return getImplementation()->getNbColumns();
}

void HODLRMatrix::assemble(const HODLRRealAssemblyFunction& f, char symmetry)
{
  copyOnWrite();
  getImplementation()->assemble(f, symmetry);
}

void HODLRMatrix::assemble(const HODLRRealAssemblyFunction &f,
                            const HODLRMatrixParameters & parameters,
                            char symmetry)
{
  copyOnWrite();
  getImplementation()->assemble(f, parameters, symmetry);
}

void HODLRMatrix::factorize()
{
  copyOnWrite();
  getImplementation()->factorize();
}

void HODLRMatrix::setPermutation(const Indices& permutation)
{
  copyOnWrite();
  getImplementation()->setPermutation(permutation);
}

Indices HODLRMatrix::getPermutation() const
{
  return getImplementation()->getPermutation();
}

void HODLRMatrix::scale(Scalar alpha)
{
  copyOnWrite();
  getImplementation()->scale(alpha);
}

void HODLRMatrix::gemv(char trans, Scalar alpha, const Point& x, Scalar beta, Point& y) const
{
  getImplementation()->gemv(trans, alpha, x, beta, y);
}

void HODLRMatrix::applyFactor(Point& y, const Point& x) const
{
  getImplementation()->applyFactor(y, x);
}

void HODLRMatrix::addIdentity(Scalar alpha)
{
  copyOnWrite();
  getImplementation()->addIdentity(alpha);
}

void HODLRMatrix::applyNugget()
{
  copyOnWrite();
  getImplementation()->applyNugget();
}

Scalar HODLRMatrix::norm() const
{
  return getImplementation()->norm();
}

Point HODLRMatrix::getDiagonal() const
{
  return getImplementation()->getDiagonal();
}

/** Get the regularization shift applied during the last factorization */
Scalar HODLRMatrix::getRegularizationShift() const
{
  return getImplementation()->getRegularizationShift();
}

Point HODLRMatrix::solve(const Point& b, Bool trans) const
{
  return getImplementation()->solve(b, trans);
}

Matrix HODLRMatrix::solve(const Matrix& m, Bool trans) const
{
  return getImplementation()->solve(m, trans);
}

Point HODLRMatrix::solveLower(const Point& b, Bool trans) const
{
  return getImplementation()->solveLower(b, trans);
}

Matrix HODLRMatrix::solveLower(const Matrix& m, Bool trans) const
{
  return getImplementation()->solveLower(m, trans);
}

Scalar HODLRMatrix::logDeterminant() const
{
  return getImplementation()->logDeterminant();
}

std::pair<size_t, size_t> HODLRMatrix::compressionRatio() const
{
  return getImplementation()->compressionRatio();
}

std::pair<size_t, size_t> HODLRMatrix::fullrkRatio() const
{
  return getImplementation()->fullrkRatio();
}

void HODLRMatrix::dump(const String & name) const
{
  getImplementation()->dump(name);
}

String HODLRMatrix::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << " implementation=" << getImplementation()->__repr__();
}

String HODLRMatrix::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

Bool HODLRMatrix::operator == (const HODLRMatrix & rhs) const
{
  return (*(getImplementation()) == *(rhs.getImplementation()));
}

END_NAMESPACE_OPENTURNS
