//                                               -*- C++ -*-
/**
 *  @brief SORMResult implements the First Order Reliability Method
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_SORMResult_HXX
#define OPENTURNS_SORMResult_HXX

#include "openturns/AnalyticalResult.hxx"
#include "openturns/Event.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class SORMResult
 * SORMResult stores the SORM result
 */
class OT_API SORMResult:
  public AnalyticalResult
{
  CLASSNAME
public:

  /** Standard constructor */
  SORMResult(const Point & standardSpaceDesignPoint,
             const RandomVector & limitStateVariable,
             const Bool isStandardPointOriginInFailureSpace);

  /* Default constructor (required by SWIG :-<) */
  SORMResult();

  /** Virtual constructor */
  virtual SORMResult * clone() const;

  /** EventProbabilityBreitung accessor */
  Scalar getEventProbabilityBreitung() const;

  /** EventProbabilityHohenBichler accessor */
  Scalar getEventProbabilityHohenBichler() const;

  /** EventProbabilityTvedt accessor */
  Scalar getEventProbabilityTvedt() const;

  /** GeneralisedReliabilityIndexBreitung accessor */
  Scalar getGeneralisedReliabilityIndexBreitung() const;

  /** GeneralisedReliabilityIndexHohenBichler accessor */
  Scalar getGeneralisedReliabilityIndexHohenBichler() const;

  /** GeneralisedReliabilityIndexTvedt accessor */
  Scalar getGeneralisedReliabilityIndexTvedt() const;

  /** SortedCurvatures accessor */
  Point getSortedCurvatures() const;

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
  Point gradientLimitStateFunction_;
  mutable Point sortedCurvatures_;
  mutable Bool isAlreadyComputedSortedCurvatures_;
  mutable Scalar eventProbabilityBreitung_;
  mutable Scalar eventProbabilityHohenBichler_;
  mutable Scalar eventProbabilityTvedt_;
  mutable Scalar generalisedReliabilityIndexBreitung_;
  mutable Scalar generalisedReliabilityIndexHohenBichler_;
  mutable Scalar generalisedReliabilityIndexTvedt_;
  Distribution standardDistribution_;
  Distribution standardMarginal_;
}; // class Result

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SORMResult_HXX */
