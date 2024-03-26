//                                               -*- C++ -*-
/**
 *  @brief Result of GEV covariates likelihood
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
#ifndef OPENTURNS_COVARIATESRESULT_HXX
#define OPENTURNS_COVARIATESRESULT_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionFactory.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/GridLayout.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API CovariatesResult
  : public PersistentObject
{
  CLASSNAME
public:
  CovariatesResult();

  CovariatesResult(const DistributionFactory & factory,
                    const Function & parameterFunction,
                    const Sample & covariates,
                    const Distribution & parameterDistribution,
                    const LinearFunction & normalizationFunction,
                    const Scalar logLikelihood);

  CovariatesResult * clone() const override;

  /** Accessor to the optimal parameter */
  Point getOptimalParameter() const;

  /** Accessor to the parameter function */
  Function getParameterFunction() const;

  /** Accessor to the covariates */
  Sample getCovariates() const;

  /** Accessor to the normalization function */
  LinearFunction getNormalizationFunction() const;

  /** Accessor to the distribution of the parameter */
  void setParameterDistribution(const Distribution & parameterDistribution);
  Distribution getParameterDistribution() const;

  /** Accessor to the likelihood value */
  void setLogLikelihood(const Scalar logLikelihood);
  Scalar getLogLikelihood() const;

  /** Draw parameter according to 1 or 2 covariates
      the reference point sets the values of the frozen covariates */
  GridLayout drawParameterFunction1D(const UnsignedInteger parameterIndex,
                                     const Point & referencePoint = Point()) const;
  GridLayout drawParameterFunction2D(const UnsignedInteger parameterIndex,
                                     const Point & referencePoint = Point()) const;

  /** Accessor to the distribution for a given covariate value */
  Distribution getDistribution(const Point & covariates) const;

  /** Draw quantile according to 1 or 2 covariates
      the reference point sets the values of the frozen covariates */
  GridLayout drawQuantileFunction1D(const Scalar p,
                                    const Point & referencePoint = Point()) const;
  GridLayout drawQuantileFunction2D(const Scalar p,
                                    const Point & referencePoint = Point()) const;

  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  DistributionFactory factory_;
  Function parameterFunction_;
  Sample covariates_;
  Distribution parameterDistribution_;
  LinearFunction normalizationFunction_;
  Scalar logLikelihood_ = 0.0;

};

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COVARIATESRESULT_HXX */
