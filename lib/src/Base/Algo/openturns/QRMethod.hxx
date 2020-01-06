//                                               -*- C++ -*-
/**
 *  @brief QR-decomposition based LS solver
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
#ifndef OPENTURNS_QRMETHOD_HXX
#define OPENTURNS_QRMETHOD_HXX

#include "openturns/LeastSquaresMethodImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class QRMethod
 *
 * QR-decomposition based LS solver
 */
class OT_API QRMethod
  : public LeastSquaresMethodImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  QRMethod();

  /** Parameters constructor */
  QRMethod(const DesignProxy & proxy,
           const Point & weight,
           const Indices & indices);

  /** Parameters constructor */
  QRMethod(const DesignProxy & proxy,
           const Indices & indices);

  /** Parameters constructor */
  explicit QRMethod(const Matrix & matrix);

  /** Virtual constructor */
  virtual QRMethod * clone() const;

  /** String converter */
  String __repr__() const;

  /** Solve least-squares problem, ie x=\argmin |D(Mx-b)|^2 */
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

  Matrix q_;
  mutable Matrix r_;

}; /* class QRMethod */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_QRMETHOD_HXX */
