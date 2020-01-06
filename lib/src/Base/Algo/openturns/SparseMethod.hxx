//                                               -*- C++ -*-
/**
 *  @brief Sparse approximation LS solver
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
#ifndef OPENTURNS_SPARSEMETHOD_HXX
#define OPENTURNS_SPARSEMETHOD_HXX

#include "openturns/LeastSquaresMethodImplementation.hxx"
#include "openturns/LeastSquaresMethod.hxx"
#include "openturns/BasisSequenceFactory.hxx"
#include "openturns/FittingAlgorithm.hxx"
#include "openturns/LARS.hxx"
#include "openturns/CorrectedLeaveOneOut.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SparseMethod
 *
 * Sparse approximation LS solver
 */
class OT_API SparseMethod
  : public LeastSquaresMethodImplementation
{
  CLASSNAME
public:
  typedef LeastSquaresMethodImplementation::FunctionCollection FunctionCollection;

  /** Default constructor */
  SparseMethod();

  /** Parameters constructor */
  SparseMethod(const LeastSquaresMethod & method,
               const BasisSequenceFactory & basisSequenceFactory = LARS(),
               const FittingAlgorithm & fittingAlgorithm = CorrectedLeaveOneOut());

  /** Virtual constructor */
  virtual SparseMethod * clone() const;

  /** String converter */
  String __repr__() const;

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
  Point solve(const Point & rhs);

  virtual CovarianceMatrix getGramInverse() const;

  /** Update */
  virtual void update(const Indices & addedIndices,
                      const Indices & conservedIndices,
                      const Indices & removedIndices,
                      const Bool row = false);

  virtual void trashDecomposition();

  /** Build the weighted design matrix */
  virtual MatrixImplementation computeWeightedDesign(const Bool whole = false) const;

public:


  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  LeastSquaresMethod method_;

  /** Algorithm that builds the BasisSequence */
  BasisSequenceFactory basisSequenceFactory_;

  /** Basis selection algorithm */
  FittingAlgorithm fittingAlgorithm_;

}; /* class SparseMethod */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SPARSEMETHOD_HXX */
