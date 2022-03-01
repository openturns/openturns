
//                                               -*- C++ -*-
/**
 * @brief HSICEstimatorConditionalSensitivity implements the sensivity analysis method based on HSIC indices
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_HSICESTIMATORCONDITIONALSENSITIVITY_HXX
#define OPENTURNS_HSICESTIMATORCONDITIONALSENSITIVITY_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/HSICEstimatorImplementation.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/CovarianceModel.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Graph.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class HSICEstimatorConditionalSensitivity
 *
 */
class OT_API HSICEstimatorConditionalSensitivity
  : public HSICEstimatorImplementation
{
  CLASSNAME

public:

  typedef Collection<CovarianceModel> CovarianceModelCollection;

public:

  /** Default */
  HSICEstimatorConditionalSensitivity();

  /** Constructor */
  HSICEstimatorConditionalSensitivity(const CovarianceModelCollection & covarianceList, const Sample & X, const Sample & Y, const HSICStat & estimatorType,
                                      const Function & weightFunction);

  /** Virtual constructor */
  HSICEstimatorConditionalSensitivity* clone() const override;

  /** Get the weight function */
  Function getWeightFunction() const;

  /** Set the weight function */
  void setWeightFunction(const Function & weightFunction);

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** Compute the p-values with asymptotic formula */
  void computePValuesAsymptotic() const override;

  /** Compute the weight matrix from the weight function */
  SquareMatrix computeWeightMatrix(const Sample & Y) const override;
};


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_HSICESTIMATORCONDITIONALSENSITIVITY_HXX */
