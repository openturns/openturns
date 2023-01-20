//                                               -*- C++ -*-
/**
 *  @brief TNC is an actual implementation for OptimizationAlgorithmImplementation using the TNC library
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
  CLASSNAME
public:


  /** Default constructor */
  TNC();

  /** Constructor with parameters */
  explicit TNC(const OptimizationProblem & problem);

  /** Constructor with parameters */
  TNC(const OptimizationProblem & problem,
      const Point & scale,
      const Point & offset,
      const UnsignedInteger maxCGit,
      const Scalar eta,
      const Scalar stepmx,
      const Scalar accuracy,
      const Scalar fmin,
      const Scalar rescale);

  /** Virtual constructor */
  TNC * clone() const override;

  /** Performs the actual computation. Must be overloaded by the actual optimisation algorithm */
  void run() override;

  /** Scale accessor */
  Point getScale() const;
  void setScale(const Point & scale);

  /** Offset accessor */
  Point getOffset() const;
  void setOffset(const Point & offset);

  /** MaxCGit accessor */
  UnsignedInteger getMaxCGit() const;
  void setMaxCGit(const UnsignedInteger maxCGit);

  /** Eta accessor */
  Scalar getEta() const;
  void setEta(const Scalar eta);

  /** Stepmx accessor */
  Scalar getStepmx() const;
  void setStepmx(const Scalar stepmx);

  /** Accuracy accessor */
  Scalar getAccuracy() const;
  void setAccuracy(const Scalar accuracy);

  /** Fmin accessor */
  Scalar getFmin() const;
  void setFmin(const Scalar fmin);

  /** Rescale accessor */
  Scalar getRescale() const;
  void setRescale(const Scalar rescale);

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Ignore failure return codes */
  void setIgnoreFailure(const Bool ignoreFailure);
  Bool getIgnoreFailure() const;

protected:
  /** Check whether this problem can be solved by this solver.  Must be overloaded by the actual optimisation algorithm */
  void checkProblem(const OptimizationProblem & problem) const override;

private:

  /** Function that computes the objective function and its gradient */
  static int ComputeObjectiveAndGradient(double *x, double *f, double *g, void *state);

  /** Specific parameters */
  Point scale_;
  Point offset_;
  UnsignedInteger maxCGit_;
  Scalar eta_;
  Scalar stepmx_;
  Scalar accuracy_;
  Scalar fmin_;
  Scalar rescale_;

  /// temporary, used to track input/outputs
  Sample evaluationInputHistory_;
  Sample evaluationOutputHistory_;

  void * p_nfeval_ = nullptr;

  /** Whether to ignore failure return codes */
  Bool ignoreFailure_ = false;

}; /* class TNC */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TNC_HXX */
