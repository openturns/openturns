//                                               -*- C++ -*-
/**
 *  @brief Sparse approximation LS solver
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
  SparseMethod * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Input sample accessor */
  Sample getInputSample() const override;

  /** Weight accessor */
  Point getWeight() const override;

  /** Basis accessor */
  FunctionCollection getBasis() const override;

  /** Current indices accessor */
  Indices getCurrentIndices() const override;

  /** Initial indices accessor */
  Indices getInitialIndices() const override;

  /** Solve least-squares problem, ie x=\argmin |D(Mx-b)|^2 */
  Point solve(const Point & rhs) override;

  CovarianceMatrix getGramInverse() const override;

  /** Update */
  void update(const Indices & addedIndices,
              const Indices & conservedIndices,
              const Indices & removedIndices,
              const Bool row = false) override;

  void trashDecomposition() override;

  /** Build the weighted design matrix */
  MatrixImplementation computeWeightedDesign(const Bool whole = false) const override;

public:


  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  LeastSquaresMethod method_;

  /** Algorithm that builds the BasisSequence */
  BasisSequenceFactory basisSequenceFactory_;

  /** Basis selection algorithm */
  FittingAlgorithm fittingAlgorithm_;

}; /* class SparseMethod */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SPARSEMETHOD_HXX */
