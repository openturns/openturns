//                                               -*- C++ -*-
/**
 *  @brief CMinpack solver
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
#ifndef OPENTURNS_CMINPACK_HXX
#define OPENTURNS_CMINPACK_HXX

#include "openturns/OptimizationAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CMinpack
 * CMinpack solver
 */
class OT_API CMinpack :
  public OptimizationAlgorithmImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  CMinpack();

  /** Constructor with parameters */
  explicit CMinpack(const OptimizationProblem & problem);

  /** Virtual constructor */
  virtual CMinpack * clone() const;

  /** Performs the actual computation. Must be overloaded by the actual optimisation algorithm */
  void run();

  /** String converter */
  String __repr__() const;

  /** String converter */
  String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

  static Bool IsAvailable();

protected:
  /** Check whether this problem can be solved by this solver.  Must be overloaded by the actual optimisation algorithm */
  virtual void checkProblem(const OptimizationProblem & problem) const;

private:
  static int ComputeObjectiveJacobian(void *p, int m, int n, const Scalar *x, Scalar *fvec, Scalar *fjac, int ldfjac, int iflag);

  // variable transformation to handle bound constraints
  static void Transform(Point & x, int n, const Interval & bounds, Point & jacfac);
  static void InverseTransform(Point & x, int n, const Interval & bounds);

  // temporary, used to track input/outputs
  Sample evaluationInputHistory_;
  Sample evaluationOutputHistory_;
};


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CMINPACK_HXX */
