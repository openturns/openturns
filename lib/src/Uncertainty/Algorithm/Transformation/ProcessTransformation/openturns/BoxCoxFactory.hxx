//                                               -*- C++ -*-
/**
 *  @brief This class is enables to estimate parameters of the BoxCox model
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
#ifndef OPENTURNS_BOXCOXFACTORY_HXX
#define OPENTURNS_BOXCOXFACTORY_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Point.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Field.hxx"
#include "openturns/BoxCoxTransform.hxx"
#include "openturns/Graph.hxx"
#include "openturns/OptimizationAlgorithm.hxx"
#include "openturns/CovarianceModel.hxx"
#include "openturns/Basis.hxx"
#include "openturns/GeneralLinearModelResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class BoxCoxFactory
 */

class OT_API BoxCoxFactory
  : public PersistentObject
{

  CLASSNAME

public:

  typedef GeneralLinearModelResult::BasisCollection BasisCollection;

  /** Default constructor without parameters */
  BoxCoxFactory();

  /** Virtual copy constructor */
  BoxCoxFactory * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Build the factory from data by estimating the best \lambda which maximizes the log-likelihood function */
  BoxCoxTransform build(const Field & timeSeries) const;
  BoxCoxTransform build(const Field & timeSeries,
                        const Point & shift) const;
  BoxCoxTransform build(const Field & timeSeries,
                        const Point & shift,
                        Graph & graph) const;
  BoxCoxTransform build(const Sample & sample) const;
  BoxCoxTransform build(const Sample & sample,
                        const Point & shift) const;
  BoxCoxTransform build(const Sample & sample,
                        const Point & shift,
                        Graph & graph) const;

  /** Build the factory from data by estimating the best generalized linear model */
  BoxCoxTransform build(const Sample & inputSample,
                        const Sample & outputSample,
                        const CovarianceModel & covarianceModel,
                        const Basis & basis,
                        const Point & shift,
                        GeneralLinearModelResult & result);

  BoxCoxTransform build(const Sample & inputSample,
                        const Sample & outputSample,
                        const CovarianceModel & covarianceModel,
                        const BasisCollection & basis,
                        const Point & shift,
                        GeneralLinearModelResult & result);

  BoxCoxTransform build(const Sample & inputSample,
                        const Sample & outputSample,
                        const CovarianceModel & covarianceModel,
                        const Point & shift,
                        GeneralLinearModelResult & result);

  /** Optimization solver accessor */
  OptimizationAlgorithm getOptimizationAlgorithm() const;
  void setOptimizationAlgorithm(const OptimizationAlgorithm & solver);

protected:

  /** Optimization solver */
  OptimizationAlgorithm solver_;

  void checkGLMData(const Sample & inputSample,
                    const Sample & outputSample,
                    const CovarianceModel & covarianceModel,
                    const BasisCollection & basis);

} ; /* class BoxCoxFactory */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_BOXCOXFACTORY_HXX */
