//                                               -*- C++ -*-
/**
 *  @brief A class which implements the ConditionedNormalProcess
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OPENTURNS_CONDITIONEDNORMALPROCESS_HXX
#define OPENTURNS_CONDITIONEDNORMALPROCESS_HXX

#include "openturns/TemporalNormalProcess.hxx"
#include "openturns/KrigingResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ConditionedNormalProcess
 *
 * The implementation of a Normal process, conditioned by observations & kriging
 */
class OT_API ConditionedNormalProcess
  : public TemporalNormalProcess
{
  CLASSNAME;

public:

  /** Default constructor */
  ConditionedNormalProcess();

  /** Standard constructor  */
  ConditionedNormalProcess(const KrigingResult & result,
                           const Mesh & mesh);

  /** Virtual constructor */
  virtual ConditionedNormalProcess * clone() const;

  /** String converter */
  String __repr__() const;

  /** String converter  - pretty print */
  String __str__(const String & offset = "") const;

  /** Realization accessor */
  Field getRealization() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);


  /** Mesh accessor */
  using TemporalNormalProcess::setMesh;
  void setMesh(const Mesh & mesh);

  using TemporalNormalProcess::setTimeGrid;
  void setTimeGrid(const RegularGrid & timeGrid);

  /** Set sampling method accessor */
  using TemporalNormalProcess::setSamplingMethod;
  void setSamplingMethod(const UnsignedInteger samplingMethod);

protected:
  /** Initialization of the process */
  void initialize();

private:
  /** KrigingResult */
  KrigingResult krigingResult_;

  /** Trend part - conditionned by the mesh points */
  NumericalSample trendEvaluationMesh_;

}; /* class ConditionedNormalProcess */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CONDITIONEDNORMALPROCESS_HXX */
