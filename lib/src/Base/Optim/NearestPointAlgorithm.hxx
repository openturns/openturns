//                                               -*- C++ -*-
/**
 *  @brief NearestPointAlgorithm implements an algorithm for finding the
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
#ifndef OPENTURNS_NEARESTPOINTALGORITHM_HXX
#define OPENTURNS_NEARESTPOINTALGORITHM_HXX

#include "OTprivate.hxx"
#include "TypedInterfaceObject.hxx"
#include "NumericalMathFunction.hxx"
#include "NearestPointAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class NearestPointAlgorithm
 * NearestPointAlgorithm implements an interface to an algorithm for finding the
 * point of a manifold defined implicitely the nearest to the origin
 */

class OT_API NearestPointAlgorithm
  : public TypedInterfaceObject<NearestPointAlgorithmImplementation>
{

  CLASSNAME;
public:

  typedef Pointer<NearestPointAlgorithmImplementation>   Implementation;
  typedef NearestPointAlgorithmImplementationResult              Result;

  /** Default constructor */
  NearestPointAlgorithm();

  /** Constructor from an implementation */
  NearestPointAlgorithm(const NearestPointAlgorithmImplementation & implementation);

  /** Constructor from a Pointer to an implementation */
  NearestPointAlgorithm(const Implementation & p_implementation);

  /** Constructor with parameters */
  explicit NearestPointAlgorithm(const NumericalMathFunction & levelFunction);

  /** Performs the actual computation. Must be overloaded by the actual optimisation algorithm */
  virtual void run();

  /** Starting point accessor */
  NumericalPoint getStartingPoint() const;

  /** Starting point accessor */
  void setStartingPoint(const NumericalPoint & startingPoint);

  /** Level function accessor */
  NumericalMathFunction getLevelFunction() const;

  /** Level function accessor */
  void setLevelFunction(const NumericalMathFunction & levelFunction);

  /** Level value accessor */
  NumericalScalar getLevelValue() const;

  /** Level value accessor */
  void setLevelValue(const NumericalScalar levelValue);

  /** Result accessor */
  Result getResult() const;

  /** Result accessor */
  void setResult(const Result & result);

  /** Maximum iterations number accessor */
  UnsignedInteger getMaximumIterationsNumber() const;

  /** Maximum iterations number accessor */
  void setMaximumIterationsNumber(const UnsignedInteger maximumIterationsNumber);

  /** Maximum absolute error accessor */
  NumericalScalar getMaximumAbsoluteError() const;

  /** Maximum absolute error accessor */
  void setMaximumAbsoluteError(const NumericalScalar maximumAbsoluteError);

  /** Maximum relative error accessor */
  NumericalScalar getMaximumRelativeError() const;

  /** Maximum relative error accessor */
  void setMaximumRelativeError(const NumericalScalar maximumRelativeError);

  /** Maximum residual error accessor */
  NumericalScalar getMaximumResidualError() const;

  /** Maximum residual error accessor */
  void setMaximumResidualError(const NumericalScalar maximumResidualError);

  /** Maximum constraint error accessor */
  NumericalScalar getMaximumConstraintError() const;

  /** Maximum constraint error accessor */
  void setMaximumConstraintError(const NumericalScalar maximumConstraintError);

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

  /** Verbose accessor */
  Bool getVerbose() const;
  void setVerbose(const Bool verbose);

} ; /* class NearestPointAlgorithm */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NEARESTPOINTALGORITHM_HXX */
