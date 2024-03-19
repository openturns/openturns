//                                               -*- C++ -*-
/**
 *  @brief The linear model validation
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LinearModelValidation
 *
 * The linear model analysis
 */

class OT_API LinearModelValidation :
  public MetaModelValidation
{
  CLASSNAME

public:
  enum CrossValidationMethod {LEAVEONEOUT = 0, KFOLD = 1};

  /** Default constructor */
  LinearModelValidation();

  /** Parameter constructor */
  explicit LinearModelValidation(const LinearModelResult & linearModelResult,
                                 const CrossValidationMethod cvMethod, 
                                 const UnsignedInteger & kParameter = ResourceMap::GetAsUnsignedInteger("LinearModelValidation-DefaultKFoldParameter"));

  /** Virtual constructor */
  LinearModelValidation * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Linear model accessor */
  LinearModelResult getLinearModelResult() const;

  /** Get the K parameter */
  UnsignedInteger getKParameter() const;

  /** Get the CV Method */
  UnsignedInteger getMethod() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** Initialize the object*/
  void initialize();

  /** linear model result */
  LinearModelResult linearModelResult_;

  /** K-parameter */
  UnsignedInteger kParameter_;

  /** Cross-validation method */
  CrossValidationMethod cvMethod_;
  
  /** Compute cross-validation predictions */
  static Sample ComputeMetamodelCrossValidationPredictions(const LinearModelResult & linearModelResult, 
                                                           const CrossValidationMethod cvMethod, 
                                                           const UnsignedInteger & kParameter);
}; /* class LinearModelValidation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LINEARMODELVALIDATION_HXX */
