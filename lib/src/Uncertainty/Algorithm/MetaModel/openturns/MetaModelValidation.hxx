//                                               -*- C++ -*-
/**
 *  @brief Implementation for metamodel validation
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
#ifndef OPENTURNS_METAMODELVALIDATION_HXX
#define OPENTURNS_METAMODELVALIDATION_HXX

#include "openturns/Distribution.hxx"
#include "openturns/GridLayout.hxx"
#include "openturns/LeaveOneOutSplitter.hxx"
#include "openturns/KFoldSplitter.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MetaModelValidation
 *
 */

class OT_API MetaModelValidation
  : public PersistentObject
{
  CLASSNAME
public:


  /** Default constructor */
  MetaModelValidation();

  /** Constructor with parameters */
  MetaModelValidation(const Sample & outputSample,
                      const Sample & metamodelPredictions);

  /** Virtual constructor */
  MetaModelValidation * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Sample accessors */
  virtual Sample getOutputSample() const;
  virtual Sample getMetamodelPredictions() const;

  /** Compute R2 score */
  Point computeR2Score() const;

  /** Compute mean squared error */
  Point computeMeanSquaredError() const;

  /** Get residual sample */
  Sample getResidualSample() const;

  /** Get residual distribution */
  Distribution getResidualDistribution(const Bool smooth = true) const;

  /** Draw model vs metamodel validation graph */
  GridLayout drawValidation() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  void initialize() const;

  /** The output sample */
  Sample outputSample_;

  /** The meta model */
  Sample metaModelPredictions_;

  /** Residual sample */
  mutable Sample residual_;

  /** Mean squared error  */
  mutable Point meanSquaredError_;

  /** R2 score */
  mutable Point r2Score_;

  /** Compute cross-validation leave-one-out predictions */
  static Sample ComputeMetamodelLeaveOneOutPredictions(const Sample & outputSample,
      const Sample & residual,
      const Point & leverages,
      const LeaveOneOutSplitter & splitter);

  /** Compute cross-validation K-Fold predictions */
  static Sample ComputeMetamodelKFoldPredictions(const Sample & outputSample,
      const Sample & residual,
      const SymmetricMatrix & projectionMatrix,
      const KFoldSplitter & splitter);

private:

}; /* class MetaModelValidation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_METAMODELVALIDATION_HXX */
