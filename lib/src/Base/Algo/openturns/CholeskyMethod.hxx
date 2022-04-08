//                                               -*- C++ -*-
/**
 *  @brief Cholesky decomposition based LS solver
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_CHOLESKYMETHOD_HXX
#define OPENTURNS_CHOLESKYMETHOD_HXX

#include "openturns/LeastSquaresMethodImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CholeskyMethod
 *
 * Cholesky decomposition based LS solver
 */
class OT_API CholeskyMethod
  : public LeastSquaresMethodImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  CholeskyMethod();

  /** Parameters constructor */
  CholeskyMethod(const DesignProxy & proxy,
                 const Point & weight,
                 const Indices & indices);

  /** Parameters constructor */
  CholeskyMethod(const DesignProxy & proxy,
                 const Indices & indices);

  /** Parameters constructor */
  explicit CholeskyMethod(const Matrix & matrix);

  /** Virtual constructor */
  CholeskyMethod * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Solve least-squares problem, ie x=\argmin |Mx-b|^2 */
  Point solve(const Point & rhs) override;
  Point solveNormal(const Point & rhs) override;

  /** Update */
  void update(const Indices & addedIndices,
              const Indices & conservedIndices,
              const Indices & removedIndices,
              const Bool row = false) override;

  void trashDecomposition() override;

  CovarianceMatrix getGramInverse() const override;
  SymmetricMatrix getH() const override;

  Point getGramInverseDiag() const override;
  Point getHDiag() const override;

  Scalar getGramInverseTrace() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  // cholesky decomposition A=LL^T (lower triangular)
  mutable TriangularMatrix l_;

}; /* class CholeskyMethod */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CHOLESKYMETHOD_HXX */
