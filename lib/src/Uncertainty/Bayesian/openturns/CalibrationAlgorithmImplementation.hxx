//                                               -*- C++ -*-
/**
 *  @brief Default CalibrationAlgorithmImplementation
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_CALIBRATIONALGORITHMIMPLEMENTATION_HXX
#define OPENTURNS_CALIBRATIONALGORITHMIMPLEMENTATION_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/PersistentObject.hxx"
#include "openturns/CalibrationResult.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/Indices.hxx"
#include "openturns/Interval.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/CovarianceMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CalibrationAlgorithmImplementation
 *
 * @brief The class implements the concept of CalibrationAlgorithm.
 *
 * This class is abstract so it can not be instantiated. It must be derived.
 * @see CalibrationAlgorithm
 */
class OT_API CalibrationAlgorithmImplementation
  : public PersistentObject
{
  CLASSNAME
public:

  /** Default constructor */
  CalibrationAlgorithmImplementation();

  /** Parameter constructor */
  CalibrationAlgorithmImplementation(const Function & model,
                                     const Sample & inputObservations,
                                     const Sample & outputObservations,
                                     const Distribution & parameterPrior);

  /** String converter */
  String __repr__() const override;

  /** Model accessor */
  Function getModel() const;

  /** Input observations accessor */
  Sample getInputObservations() const;

  /** Output observations accessor */
  Sample getOutputObservations() const;

  /** Prior parameter distribution accessor */
  Distribution getParameterPrior() const;

  /** Parameter bounds accessor */
  Interval getBounds() const;
  void setBounds(const Interval & bounds);

  /** Performs the actual computation. Must be overloaded by the actual calibration algorithm */
  virtual void run();

  /** Calibration result accessor */
  CalibrationResult getResult() const;
  void setResult(const CalibrationResult & result);

  /* Here is the interface that all derived class must implement */

  /** Virtual constructor */
  CalibrationAlgorithmImplementation * clone() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /* Indices of the parameters that are not fixed at a bound: all the
   * directions when bounds is empty, the free ones otherwise. */
  static Indices ComputeFreeParameterIndices(const Point & thetaStar,
      const Interval & bounds);

  /* Inverse of the Gram matrix on the free directions of a design matrix,
   * padded with zeros in the directions fixed at a bound. When checkRank is
   * true, a rank-deficient free block raises instead of returning a non-finite
   * covariance. */
  static CovarianceMatrix ComputeFreeGramInverse(const Matrix & design,
      const Indices & freeIndices,
      const String & methodName,
      const Bool checkRank = true);

  /* Inverse of a Hessian matrix on the free directions, padded with zeros in
   * the directions fixed at a bound. */
  static CovarianceMatrix ComputeFreeHessianInverse(const Matrix & hessian,
      const Indices & freeIndices);

  /* Minimize ||design.increment - rightHandSide||^2 subject to the increment
   * lying in the parameter bounds shifted by -startingPoint, and return the
   * optimal increment. Solving in the increment variable keeps the right-hand
   * side accurate when the starting point is large. The problem is solved with
   * the first algorithm returned by OptimizationAlgorithm::Build(), i.e. the
   * HiGHS quadratic solver when OpenTURNS is built with HiGHS support. */
  static Point SolveBoundedLeastSquaresIncrement(const Matrix & design,
      const Point & rightHandSide,
      const Interval & bounds,
      const Point & startingPoint);

  /* Truncated Gaussian posterior: Normal(mean, covariance) truncated to the
   * parameter bounds. The mode of the returned distribution is the
   * constrained maximizer of the Gaussian density, while its mean is the
   * expectation of the truncated distribution and differs from the mode in
   * general. */
  static Distribution ComputeTruncatedPosterior(const Point & mean,
      const CovarianceMatrix & covariance,
      const Interval & bounds);

  /* Model to calibrate */
  Function model_;

  /* The input observations */
  Sample inputObservations_;

  /* The output observations */
  Sample outputObservations_;

  /* The parameter prior distribution */
  Distribution parameterPrior_;

  /* The parameter bounds: an empty interval means no bounds */
  Interval bounds_;

  /* The calibration result */
  CalibrationResult result_;

}; /* class CalibrationAlgorithmImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CALIBRATIONALGORITHMIMPLEMENTATION_HXX */
