//                                               -*- C++ -*-
/**
 *  @brief SORMResult implements the First Order Reliability Method
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_SORMResult_HXX
#define OPENTURNS_SORMResult_HXX

#include "AnalyticalResult.hxx"
#include "Event.hxx"
#include "SquareMatrix.hxx"
#include "Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class SORMResult
 * SORMResult stores the SORM result
 */
class OT_API SORMResult:
  public AnalyticalResult
{
  CLASSNAME;
public:

  /** Standard constructor */
  SORMResult(const NumericalPoint & standardSpaceDesignPoint,
             const Event & limitStateVariable,
             const Bool isStandardPointOriginInFailureSpace);

  /* Default constructor (required by SWIG :-<) */
  SORMResult();

  /** Virtual constructor */
  virtual SORMResult * clone() const;

  /** EventProbabilityBreitung accessor */
  NumericalScalar getEventProbabilityBreitung() const;

  /** EventProbabilityHohenBichler accessor */
  NumericalScalar getEventProbabilityHohenBichler() const;

  /** EventProbabilityTvedt accessor */
  NumericalScalar getEventProbabilityTvedt() const;

  /** GeneralisedReliabilityIndexBreitung accessor */
  NumericalScalar getGeneralisedReliabilityIndexBreitung() const;

  /** GeneralisedReliabilityIndexHohenBichler accessor */
  NumericalScalar getGeneralisedReliabilityIndexHohenBichler() const;

  /** GeneralisedReliabilityIndexTvedt accessor */
  NumericalScalar getGeneralisedReliabilityIndexTvedt() const;

  /** SortedCurvatures accessor */
  NumericalPoint getSortedCurvatures() const;

  /** String converter */
  String __repr__() const;

  /** String converter */
  String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private:

  /** the function that evaluates the curvatures of the standard limite state fucntion at the standard design point */
  void computeSortedCurvatures() const;

  /** type of gradients and hessian attributes are due to the constraint : limitStateVariable is scalar only in Open Turns v std */
  /** It should be a SymmetricMatrix but there is a bug in linear algebra with these matrices */
  SquareMatrix hessianLimitStateFunction_;
  NumericalPoint gradientLimitStateFunction_;
  mutable NumericalPoint sortedCurvatures_;
  mutable Bool isAlreadyComputedSortedCurvatures_;
  mutable NumericalScalar eventProbabilityBreitung_;
  mutable NumericalScalar eventProbabilityHohenBichler_;
  mutable NumericalScalar eventProbabilityTvedt_;
  mutable NumericalScalar generalisedReliabilityIndexBreitung_;
  mutable NumericalScalar generalisedReliabilityIndexHohenBichler_;
  mutable NumericalScalar generalisedReliabilityIndexTvedt_;
  Distribution standardDistribution_;
  Distribution standardMarginal_;
}; // class Result

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SORMResult_HXX */
