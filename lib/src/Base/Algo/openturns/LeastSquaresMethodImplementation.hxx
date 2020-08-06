//                                               -*- C++ -*-
/**
 *  @brief LeastSquares solving algorithm implementation
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
#ifndef OPENTURNS_LEASTSQUARESMETHODIMPLEMENTATION_HXX
#define OPENTURNS_LEASTSQUARESMETHODIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Sample.hxx"
#include "openturns/DesignProxy.hxx"
#include "openturns/Collection.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LeastSquaresMethodImplementation
 *
 * LeastSquares solving algorithm implementation
 */
class OT_API LeastSquaresMethodImplementation
  : public PersistentObject
{
  friend class KFold;
  friend class CorrectedLeaveOneOut;
  friend class LeastSquaresMetaModelSelection;
  CLASSNAME

public:
  typedef Collection<Function> FunctionCollection;

  /** Default constructor */
  LeastSquaresMethodImplementation();

  /** Parameters constructor */
  LeastSquaresMethodImplementation(const DesignProxy & proxy,
                                   const Point & weight,
                                   const Indices & indices);

  /** Parameters constructor */
  LeastSquaresMethodImplementation(const DesignProxy & proxy,
                                   const Indices & indices);

  /** Parameters constructor */
  explicit LeastSquaresMethodImplementation(const Matrix & matrix);

  /** Virtual constructor */
  LeastSquaresMethodImplementation * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Input sample accessor */
  virtual Sample getInputSample() const;

  /** Weight accessor */
  virtual Point getWeight() const;

  /** Basis accessor */
  virtual FunctionCollection getBasis() const;

  /** Current indices accessor */
  virtual Indices getCurrentIndices() const;

  /** Initial indices accessor */
  virtual Indices getInitialIndices() const;

  /** Solve least-squares problem, ie x=\argmin |D(Mx-b)|^2 */
  virtual Point solve(const Point & rhs);
  virtual Point solveNormal(const Point & rhs);

  /** Get the inverse of the Gram matrix */
  virtual CovarianceMatrix getGramInverse() const;

  /** Get the scaled inverse H = Psi.G^{-1}.Psi^t of the Gram matrix */
  virtual SymmetricMatrix getH() const;

  /** Get the diagonal of H */
  virtual Point getHDiag() const;

  /** Get the diagonal of the Gram inverse */
  virtual Point getGramInverseDiag() const;

  /** Get the trace of the Gram inverse */
  virtual Scalar getGramInverseTrace() const;

  /** Update */
  virtual void update(const Indices & addedIndices,
                      const Indices & conservedIndices,
                      const Indices & removedIndices,
                      const Bool row = false);

  /** Destroy the internal decomposition of the design matrix */
  virtual void trashDecomposition();

  /** Build the weighted design matrix */
  virtual MatrixImplementation computeWeightedDesign(const Bool whole = false) const;
protected:
  virtual MatrixImplementation computeWeightedDesign(const Indices & indices) const;
public:

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Weight accessor, check for positivness and uniformity, compute the square roots */
  void setWeight(const Point & weight);

  /** Proxy to the input sample and the basis */
  DesignProxy proxy_;

  /** Weights for the least-squares norm. The size is positive if and only if the weights are not uniform. */
  Point weight_;

  /** Square-roots of the weights for the least-squares norm. The size is positive if and only if the weights are not uniform. */
  Point weightSqrt_;

  /** Flag to tell if the points have uniform weights */
  Bool hasUniformWeight_;

  /** Current indices */
  Indices currentIndices_;

  /** Initial indices */
  Indices initialIndices_;

}; /* class LeastSquaresMethodImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LEASTSQUARESMETHODIMPLEMENTATION_HXX */
