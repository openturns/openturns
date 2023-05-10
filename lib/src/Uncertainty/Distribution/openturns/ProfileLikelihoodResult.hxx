//                                               -*- C++ -*-
/**
 *  @brief Result of GEV profile likelihood
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
#ifndef OPENTURNS_PROFILELIKELIHOODRESULT_HXX
#define OPENTURNS_PROFILELIKELIHOODRESULT_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionFactoryLikelihoodResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API ProfileLikelihoodResult
  : public DistributionFactoryLikelihoodResult
{
  CLASSNAME
public:
  ProfileLikelihoodResult();

  ProfileLikelihoodResult(const Distribution & distribution,
                          const Distribution & parameterDistribution,
                          const Scalar logLikelihood,
                          const Function & profileLikelihood,
                          const Scalar parameter);

  ProfileLikelihoodResult * clone() const override;

  /** Confidence level accessor */
  void setConfidenceLevel(const Scalar confidenceLevel);
  Scalar getConfidenceLevel() const;

  /** Confidence interval of the outer parameter */
  Interval getParameterConfidenceInterval() const;

  /** Profile likelihood accessor */
  Function getProfileLikelihoodFunction() const;
  Graph drawProfileLikelihoodFunction() const;

  /** Parameter value accessor */
  Scalar getParameter() const;

  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  UnsignedInteger getParameterIndex() const;
  Scalar getThreshold() const;

  Function profileLikelihoodFunction_;
  Scalar confidenceLevel_ = 0.95;
  Scalar parameter_ = 0.0;
};

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PROFILELIKELIHOODRESULT_HXX */
