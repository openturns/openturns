//                                               -*- C++ -*-
/**
 *  @brief LeastSquares solving algorithm implementation
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OPENTURNS_LEASTSQUARESMETHODIMPLEMENTATION_HXX
#define OPENTURNS_LEASTSQUARESMETHODIMPLEMENTATION_HXX

#include "PersistentObject.hxx"
#include "NumericalSample.hxx"
#include "FunctionalBasis.hxx"
#include "DesignProxy.hxx"

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
  CLASSNAME;

public:

  /** Default constructor */
  LeastSquaresMethodImplementation();

  /** Parameters constructor */
  LeastSquaresMethodImplementation(const DesignProxy & proxy,
                                   const NumericalSample & outputSample,
                                   const NumericalPoint & weight,
                                   const Indices & indices);

  /** Parameters constructor */
  LeastSquaresMethodImplementation(const DesignProxy & proxy,
                                   const NumericalSample & outputSample,
                                   const Indices & indices);

  /** Virtual constructor */
  virtual LeastSquaresMethodImplementation * clone() const;

  /** String converter */
  virtual String __repr__() const;

  /** Input sample accessor */
  virtual NumericalSample getInputSample() const;

  /** Output sample accessor */
  virtual NumericalSample getOutputSample() const;

  /** Weight accessor */
  virtual NumericalPoint getWeight() const;

  /** Basis accessor */
  virtual Basis getBasis() const;

  /** Current indices accessor */
  virtual Indices getCurrentIndices() const;

  /** Initial indices accessor */
  virtual Indices getInitialIndices() const;

  /** Solve least-squares problem, ie x=\argmin |D(Mx-b)|^2 */
  virtual NumericalPoint solve(const NumericalPoint & rhs);
  virtual NumericalPoint solveNormal(const NumericalPoint & rhs);

  /** Get the inverse of the Gram matrix */
  virtual CovarianceMatrix getGramInverse() const;

  /** Get the scaled inverse H = Psi.G^{-1}.Psi^t of the Gram matrix */
  virtual SymmetricMatrix getH() const;

  /** Get the diagonal of H */
  virtual NumericalPoint getHDiag() const;

  /** Get the trace of the Gram inverse */
  virtual NumericalScalar getGramInverseTrace() const;

  /** Update */
  virtual void update(const Indices & addedIndices,
                      const Indices & conservedIndices,
                      const Indices & removedIndices,
                      const Bool row = false);

  /** Destroy the internal decomposition of the design matrix */
  virtual void trashDecomposition();

  /** Build the current functions */
  virtual Basis buildCurrentBasis() const;

  /** Build the weighted design matrix */
  virtual MatrixImplementation computeWeightedDesign(const Bool whole = false) const;
protected:
  virtual MatrixImplementation computeWeightedDesign(const Indices & indices) const;
public:

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:
  /** Weight accessor, check for positivness and uniformity, compute the square roots */
  void setWeight(const NumericalPoint & weight);

  /** Proxy to the input sample and the basis */
  DesignProxy proxy_;

  /** Target values */
  NumericalSample outputSample_;

  /** Weights for the least-squares norm. The size is positive if and only if the weights are not uniform. */
  NumericalPoint weight_;

  /** Square-roots of the weights for the least-squares norm. The size is positive if and only if the weights are not uniform. */
  NumericalPoint weightSqrt_;

  /** Flag to tell if the points have uniform weights */
  Bool hasUniformWeight_;

  /** Current indices */
  Indices currentIndices_;

  /** Initial indices */
  Indices initialIndices_;

}; /* class LeastSquaresMethodImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LEASTSQUARESMETHODIMPLEMENTATION_HXX */
