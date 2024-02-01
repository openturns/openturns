//                                               -*- C++ -*-
/**
 *  @brief A factory for building BasisSequence objects
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_BASISSEQUENCEFACTORYIMPLEMENTATION_HXX
#define OPENTURNS_BASISSEQUENCEFACTORYIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/BasisSequence.hxx"
#include "openturns/Sample.hxx"
#include "openturns/DesignProxy.hxx"
#include "openturns/LeastSquaresMethod.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class BasisSequenceFactoryImplementation
 *
 * A factory for building BasisSequence objects
 */
class OT_API BasisSequenceFactoryImplementation
  : public PersistentObject
{
  friend class LeastSquaresMetaModelSelection;
  friend class SparseMethod;
  CLASSNAME
public:
  typedef Collection<Function> FunctionCollection;

  /** Default constructor */
  BasisSequenceFactoryImplementation();

  /** Virtual constructor */
  BasisSequenceFactoryImplementation * clone() const override;

  /** Method to create new BasisSequence objects */
  virtual BasisSequence build(const Sample & x,
                              const Sample & y,
                              const FunctionCollection & psi,
                              const Indices & indices);

#ifndef SWIG

  virtual BasisSequence build(const Sample & y,
                              const Indices & indices,
                              const DesignProxy & proxy);

  virtual BasisSequence build(LeastSquaresMethod & method,
                              const Sample & y);

  virtual void initialize();
  virtual void updateBasis(LeastSquaresMethod & method,
                           const Sample & y);
#endif

  /** Stopping criterion on the L1-norm of the coefficients accessor */
  void setMaximumRelativeConvergence(const Scalar coefficientsPaths);
  Scalar getMaximumRelativeConvergence() const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Selection history accessor */
  virtual Collection<Indices> getSelectionHistory(Collection<Point> & coefficientsHistory) const;

protected:
  /** Stopping criterion on the L1-norm of the coefficients */
  Scalar maximumRelativeConvergence_ = 0.0;

  /** The collection of indices of the vectors in the partial basis with respect to the orthogonal basis */
  Indices currentIndices_;

  /** The ranks of the added vectors in the current partial basis */
  Indices addedPsi_k_ranks_;

  /** The ranks of the vectors common between the previous partial basis and the current partial basis, as found in the previous partial basis */
  Indices conservedPsi_k_ranks_;

  /** The ranks of the removed vectors in the previous partial basis */
  Indices removedPsi_k_ranks_;
}; /* class BasisSequenceFactoryImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_BASISSEQUENCEFACTORYIMPLEMENTATION_HXX */
