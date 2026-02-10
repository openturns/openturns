//                                               -*- C++ -*-
/**
 *  @brief Cross-validation of a Gaussian Process Regression surrogate model
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
 *
 */
#ifndef OPENTURNS_GAUSSIANPROCESSREGRESSIONCROSSVALIDATION_HXX
#define OPENTURNS_GAUSSIANPROCESSREGRESSIONCROSSVALIDATION_HXX

#include "openturns/Sample.hxx"
#include "openturns/GaussianProcessRegressionResult.hxx"
#include "openturns/MetaModelValidation.hxx"
#include "openturns/SplitterImplementation.hxx"
#include "openturns/LeaveOneOutSplitter.hxx"
#include "openturns/KFoldSplitter.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class GaussianProcessRegressionCrossValidation
 *
 * The validation of a chaos expansion
 */

class OT_API GaussianProcessRegressionCrossValidation
  : public MetaModelValidation
{
  CLASSNAME

public:
  typedef Collection<Function>           FunctionCollection;

  /** Default constructor */
  GaussianProcessRegressionCrossValidation();

  /** Parameter constructor */
  explicit GaussianProcessRegressionCrossValidation(const GaussianProcessRegressionResult & gprResult);

  /** Parameter constructor */
  GaussianProcessRegressionCrossValidation(const GaussianProcessRegressionResult & gprResult,
                            const LeaveOneOutSplitter & splitter);

  /** Parameter constructor */
  GaussianProcessRegressionCrossValidation(const GaussianProcessRegressionResult & gprResult,
                            const KFoldSplitter & splitter);

  /** Virtual constructor */
  GaussianProcessRegressionCrossValidation * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Result accessor */
  GaussianProcessRegressionResult getGaussianProcessRegressionResult() const;

  /** Get the splitter */
  SplitterImplementation getSplitter() const;

  /** Get the Leave-One-Out prediction standard deviations */
  Point getLeaveOneOutStandardDeviations() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

private:

  /** The functional chaos result */
  GaussianProcessRegressionResult gaussianProcessRegressionResult_;

  /** Splitter parameter */
  SplitterImplementation splitter_;

  /** LOO prediction standard deviations  */
  Point leaveOneOutStandardDeviations_;

  /* Compute cross-validation metamodel predictions */
  Sample computeGPRLeaveOneOutPredictions(
    const GaussianProcessRegressionResult & gaussianProcessRegressionResult);

  /* Compute cross-validation metamodel predictions */
  static Sample ComputeMetamodelKFoldPredictions(
    const GaussianProcessRegressionResult & gaussianProcessRegressionResult, const KFoldSplitter & splitter);

} ; /* class GaussianProcessRegressionCrossValidation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_GAUSSIANPROCESSREGRESSIONCROSSVALIDATION_HXX */
