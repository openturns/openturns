//                                               -*- C++ -*-
/**
 *  @brief Result of GEV time-varying likelihood
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
#ifndef OPENTURNS_TIMEVARYINGRESULT_HXX
#define OPENTURNS_TIMEVARYINGRESULT_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionFactoryResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API TimeVaryingResult
  : public PersistentObject
{
  CLASSNAME
public:
  TimeVaryingResult();
  
  TimeVaryingResult(const Function & parameterFunction,
                    const Mesh & mesh,
                    const Distribution & parameterDistribution,
                    const Scalar logLikelihood);

  TimeVaryingResult * clone() const override;

  Point getOptimalParameter() const;

  Function getParameterFunction() const;
  
  /** Accessor to the distribution of the parameter */
  void setParameterDistribution(const Distribution & parameterDistribution);
  Distribution getParameterDistribution() const;
  
  void setLogLikelihood(const Scalar logLikelihood);
  Scalar getLogLikelihood() const;
  
  Graph drawParameterFunction(const UnsignedInteger parameterIndex = 0) const;

  /** Accessor to the distribution at a given time */
  Distribution getDistribution(const Scalar t);

  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  Function parameterFunction_;
  Mesh mesh_;
  Distribution parameterDistribution_;
  Scalar logLikelihood_ = 0.0;

};

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TIMEVARYINGRESULT_HXX */
