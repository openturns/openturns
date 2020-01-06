//                                               -*- C++ -*-
/**
 *  @brief OPT++ solvers
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_OPTPP_HXX
#define OPENTURNS_OPTPP_HXX

#include "openturns/OptimizationAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OPTppImplementation;
/**
 * @class OPTpp
 * OPTpp solvers
 */
class OT_API OPTpp:
  public OptimizationAlgorithmImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  OPTpp(const String & algoName = "OptQNIPS");

  /** Constructor with parameters */
  explicit OPTpp(const OptimizationProblem & problem,
                 const String & algoName = "OptQNIPS");

  /** Virtual constructor */
  virtual OPTpp * clone() const;

  /** Problem accessor */
  virtual void setProblem(const OptimizationProblem & problem);

  /** Performs the actual computation. Must be overloaded by the actual optimisation algorithm */
  virtual void run();

  /** Result accessor */
  virtual OptimizationResult getResult() const;

  /** OPTpp algorithm names accessor */
  static Description GetAlgorithmNames();

  /** Minimum step accessor */
  void setMinimumStep(const Scalar minimumStep);
  Scalar getMinimumStep() const;

  /** Maximum step accessor */
  void setMaximumStep(const Scalar maximumStep);
  Scalar getMaximumStep() const;

  /** Maximum line search tolerance accessor */
  void setLineSearchTolerance(const Scalar lineSearchTolerance);
  Scalar getLineSearchTolerance() const;

  /** Maximum line search iteration number accessor */
  void setMaximumLineSearchIterationNumber(const UnsignedInteger maximumLineSearchIterationNumber);
  UnsignedInteger getMaximumLineSearchIterationNumber() const;

  /** Trust region size accessor */
  void setTrustRegionSize(const Scalar trustRegionSize);
  Scalar getTrustRegionSize() const;

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
  void initialize(const String & algoName);

  /** We want to plimp the implementation to encapsulate OPT++ headers without multiple inheritance */
  Pointer<OPTppImplementation> p_impl_;
};


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_OPTPP_HXX */
