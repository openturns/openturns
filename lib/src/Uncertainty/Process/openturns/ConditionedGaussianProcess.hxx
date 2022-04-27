//                                               -*- C++ -*-
/**
 *  @brief A class which implements the ConditionedGaussianProcess
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
#ifndef OPENTURNS_CONDITIONEDGAUSSIANPROCESS_HXX
#define OPENTURNS_CONDITIONEDGAUSSIANPROCESS_HXX

#include "openturns/GaussianProcess.hxx"
#include "openturns/KrigingResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ConditionedGaussianProcess
 *
 * The implementation of a Gaussian process, conditioned by observations & kriging
 */
class OT_API ConditionedGaussianProcess
  : public GaussianProcess
{
  CLASSNAME

public:

  /** Default constructor */
  ConditionedGaussianProcess();

  /** Standard constructor  */
  ConditionedGaussianProcess(const KrigingResult & result,
                             const Mesh & mesh);

  /** Virtual constructor */
  ConditionedGaussianProcess * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** String converter  - pretty print */
  String __str__(const String & offset = "") const override;

  /** Realization accessor */
  Field getRealization() const override;

  /** Is the underlying a Gaussian process ? */
  Bool isNormal() const override;

  /** Is the underlying a stationary process ? */
  Bool isStationary() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;


  /** Mesh accessor */
  using GaussianProcess::setMesh;
  void setMesh(const Mesh & mesh) override;

  using GaussianProcess::setTimeGrid;
  void setTimeGrid(const RegularGrid & timeGrid) override;

  /** Set sampling method accessor */
  using GaussianProcess::setSamplingMethod;
  void setSamplingMethod(const UnsignedInteger samplingMethod);

protected:
  /** Initialization of the process */
  void initialize();

private:
  /** KrigingResult */
  KrigingResult krigingResult_;

  /** Trend part - conditionned by the mesh points */
  Sample trendEvaluationMesh_;

  /** Known values indices */
  Indices knownValuesIndices_;
}; /* class ConditionedGaussianProcess */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CONDITIONEDGAUSSIANPROCESS_HXX */
