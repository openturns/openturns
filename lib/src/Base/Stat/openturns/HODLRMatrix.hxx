//                                               -*- C++ -*-
/**
 *  @file  HODLRMatrix.hxx
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
#ifndef OPENTURNS_HODLRMATRIX_HXX
#define OPENTURNS_HODLRMATRIX_HXX

#include "openturns/HODLRMatrixImplementation.hxx"
#include "openturns/TypedInterfaceObject.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API HODLRMatrix :
  public TypedInterfaceObject<HODLRMatrixImplementation>
{
  CLASSNAME

  HODLRMatrix();
  HODLRMatrix(const Implementation & i);
  HODLRMatrix(const HODLRMatrixImplementation & i);

public:
  UnsignedInteger getNbRows() const;
  UnsignedInteger getNbColumns() const;

  HODLRMatrix copy() const;

  void assemble(const HODLRRealAssemblyFunction& f, char symmetry);
  void assemble(const HODLRRealAssemblyFunction &f, const HODLRMatrixParameters& parameters, char symmetry);
  void factorize();

  void setPermutation(const Indices& permutation);
  Indices getPermutation() const;

  void scale(Scalar alpha);
  void gemv(char trans, Scalar alpha, const Point& x, Scalar beta, Point& y) const;
  void applyFactor(Point& y, const Point& x) const;

  void addIdentity(Scalar alpha);
  void applyNugget();
  Scalar norm() const;
  Point getDiagonal() const;

  /** Get the regularization shift applied during the last factorization */
  Scalar getRegularizationShift() const;

  Point solve(const Point& b, Bool trans = false) const;
  Matrix solve(const Matrix& m, Bool trans = false) const;
  Point solveLower(const Point& b, Bool trans = false) const;
  Matrix solveLower(const Matrix& m, Bool trans = false) const;

  Scalar logDeterminant() const;
  std::pair<size_t, size_t> compressionRatio() const;
  std::pair<size_t, size_t> fullrkRatio() const;
  void dump(const String & name) const;

  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  using TypedInterfaceObject<HODLRMatrixImplementation>::operator ==;
  Bool operator == (const HODLRMatrix & rhs) const;
};

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_HODLRMATRIX_HXX */
