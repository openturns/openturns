//                                               -*- C++ -*-
/**
 *  @brief TNC is an actual implementation for OptimizationAlgorithmImplementation using the TNC library
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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

#include "openturns/OTprivate.hxx"
#include "openturns/OptimizationAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class TNC
 * TNC is an actual implementation for
 * OptimizationAlgorithmImplementation using the TNC library
 */
class OT_API TNC :
  public OptimizationAlgorithmImplementation
{
  CLASSNAME;
public:


  /** Default constructor */
  TNC();

  /** Constructor with parameters */
  explicit TNC(const OptimizationProblem & problem);

  /** Constructor with parameters */
  TNC(const OptimizationProblem & problem,
      const NumericalPoint & scale,
      const NumericalPoint & offset,
      const UnsignedInteger maxCGit,
      const NumericalScalar eta,
      const NumericalScalar stepmx,
      const NumericalScalar accuracy,
      const NumericalScalar fmin,
      const NumericalScalar rescale);

  /** Virtual constructor */
  virtual TNC * clone() const;

  /** Performs the actual computation. Must be overloaded by the actual optimisation algorithm */
  void run();

  /** Scale accessor */
  NumericalPoint getScale() const;
  void setScale(const NumericalPoint & scale);

  /** Offset accessor */
  NumericalPoint getOffset() const;
  void setOffset(const NumericalPoint & offset);

  /** MaxCGit accessor */
  UnsignedInteger getMaxCGit() const;
  void setMaxCGit(const UnsignedInteger maxCGit);

  /** Eta accessor */
  NumericalScalar getEta() const;
  void setEta(const NumericalScalar eta);

  /** Stepmx accessor */
  NumericalScalar getStepmx() const;
  void setStepmx(const NumericalScalar stepmx);

  /** Accuracy accessor */
  NumericalScalar getAccuracy() const;
  void setAccuracy(const NumericalScalar accuracy);

  /** Fmin accessor */
  NumericalScalar getFmin() const;
  void setFmin(const NumericalScalar fmin);

  /** Rescale accessor */
  NumericalScalar getRescale() const;
  void setRescale(const NumericalScalar rescale);

  /** String converter */
  String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:
  /** Check whether this problem can be solved by this solver.  Must be overloaded by the actual optimisation algorithm */
  void checkProblem(const OptimizationProblem & problem) const;

private:

  /** Function that computes the objective function and its gradient */
  static int ComputeObjectiveAndGradient(double *x, double *f, double *g, void *state);

  /** Specific parameters */
  NumericalPoint scale_;
  NumericalPoint offset_;
  UnsignedInteger maxCGit_;
  NumericalScalar eta_;
  NumericalScalar stepmx_;
  NumericalScalar accuracy_;
  NumericalScalar fmin_;
  NumericalScalar rescale_;

  /// temporary, used to track input/outputs
  NumericalSample evaluationInputHistory_;
  NumericalSample evaluationOutputHistory_;

  void * p_nfeval_;

}; /* class TNC */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TNC_HXX */
