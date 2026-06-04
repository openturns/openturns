//                                               -*- C++ -*-
/**
 *  @brief The linear model validation
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
#ifndef OPENTURNS_LINEARMODELVALIDATION_HXX
#define OPENTURNS_LINEARMODELVALIDATION_HXX

#include "openturns/Sample.hxx"
#include "openturns/LinearModelResult.hxx"
#include "openturns/MetaModelValidation.hxx"
#include "openturns/SplitterImplementation.hxx"
#include "openturns/LeaveOneOutSplitter.hxx"
#include "openturns/KFoldSplitter.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LinearModelValidation
 *
 * The linear model validation
 */

class OT_API LinearModelValidation :
  public MetaModelValidation
{
  CLASSNAME

public:
  /** Default constructor */
  LinearModelValidation();

  /** Parameter constructor based on result */
  explicit LinearModelValidation(const LinearModelResult & linearModelResult);

  /** Parameter constructor based on result and leave-one-out */
  LinearModelValidation(const LinearModelResult & linearModelResult,
                        const LeaveOneOutSplitter & splitter);

  /** Parameter constructor based on result and K-Fold */
  LinearModelValidation(const LinearModelResult & linearModelResult,
                        const KFoldSplitter & splitter);

  /** Virtual constructor */
  LinearModelValidation * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Linear model accessor */
  LinearModelResult getLinearModelResult() const;

  /** Get the splitter */
  SplitterImplementation getSplitter() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** Initialize the object*/
  void initialize();

  /** linear model result */
  LinearModelResult linearModelResult_;

  /** Splitter parameter */
  SplitterImplementation splitter_;

  /** Compute cross-validation leave-one-out predictions */
  static Sample ComputeMetamodelLeaveOneOutPredictions(const LinearModelResult & linearModelResult,
      const LeaveOneOutSplitter & splitter);

  /** Compute cross-validation K-Fold predictions */
  static Sample ComputeMetamodelKFoldPredictions(const LinearModelResult & linearModelResult,
      const KFoldSplitter & splitter);
}; /* class LinearModelValidation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LINEARMODELVALIDATION_HXX */
