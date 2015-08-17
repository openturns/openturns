//                                               -*- C++ -*-
/**
 *  @brief TNC is an actual implementation for a bound-constrained optimization algorithm
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
#ifndef OPENTURNS_TNC_HXX
#define OPENTURNS_TNC_HXX

#include "OTprivate.hxx"
#include "TNCSpecificParameters.hxx"
#include "BoundConstrainedAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class TNC
 * TNC is an actual implementation for
 * BoundConstrainedAlgorithm using the TNC library
 */
class OT_API TNC :
  public BoundConstrainedAlgorithmImplementation
{
  CLASSNAME;
public:


  /** Default constructor */
  TNC();

  /** Constructor with parameters: no constraint, starting from the origin */
  explicit TNC(const NumericalMathFunction & objectiveFunction,
               const Bool verbose = false);

  /** Constructor with parameters: bound constraints, starting from the given point */
  TNC(const TNCSpecificParameters & parameters,
      const NumericalMathFunction & objectiveFunction,
      const Interval & boundConstraints,
      const NumericalPoint & startingPoint,
      const OptimizationProblemObsolete optimization  = Result::MINIMIZATION,
      const Bool verbose = false);

  /** Virtual constructor */
  virtual TNC * clone() const;

  /** Performs the actual computation. Must be overloaded by the actual optimisation algorithm */
  void run();

  /** Specific parameters accessor */
  TNCSpecificParameters getSpecificParameters() const;

  /** Specific parameters accessor */
  void setSpecificParameters(const TNCSpecificParameters & specificParameters);

  /** String converter */
  String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private:

  /**
   * Function that computes the objective function and its gradient
   * corrected to correspond with the optimization problem (minimization or maximization)
   */
  static int ComputeObjectiveAndConstraint(double *x, double *f, double *g, void *state);

  TNCSpecificParameters specificParameters_;

  /// temporary, used to track input/outputs
  NumericalSample evaluationInputHistory_;
  NumericalSample evaluationOutputHistory_;

}; /* class TNC */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TNC_HXX */
