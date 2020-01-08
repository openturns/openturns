//                                               -*- C++ -*-
/**
 *  @brief SVD-decomposition based LS solver
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
#ifndef OPENTURNS_SVDMETHOD_HXX
#define OPENTURNS_SVDMETHOD_HXX

#include "openturns/Matrix.hxx"
#include "openturns/LeastSquaresMethodImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SVDMethod
 *
 * SVD-decomposition based LS solver
 */
class OT_API SVDMethod
  : public LeastSquaresMethodImplementation
{
  CLASSNAME
public:
  /** Default constructor */
  SVDMethod();

  /** Default constructor */
  SVDMethod(const DesignProxy & proxy,
            const Point & weight,
            const Indices & indices);

  /** Default constructor */
  SVDMethod(const DesignProxy & proxy,
            const Indices & indices);

  /** Default constructor */
  explicit SVDMethod(const Matrix & matrix);

  /** Virtual constructor */
  virtual SVDMethod * clone() const;

  /** String converter */
  String __repr__() const;

  /** Solve least-squares problem, ie x=\argmin |Mx-b|^2 */
  Point solve(const Point & rhs);
  Point solveNormal(const Point & rhs);

  CovarianceMatrix getGramInverse() const;

  Point getGramInverseDiag() const;
  Point getHDiag() const;

  SymmetricMatrix getH() const;


  Scalar getGramInverseTrace() const;


  /** Update */
  void update(const Indices & addedIndices,
              const Indices & conservedIndices,
              const Indices & removedIndices,
              const Bool row = false);

  void trashDecomposition();

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:
  Matrix u_;
  SquareMatrix vT_;
  Point singularValues_;

}; /* class SVDMethod */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SVDMETHOD_HXX */
