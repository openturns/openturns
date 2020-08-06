//                                               -*- C++ -*-
/**
 *  @brief A class which implements the Gaussian process
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
#ifndef OPENTURNS_GAUSSIANPROCESS_HXX
#define OPENTURNS_GAUSSIANPROCESS_HXX

#include "openturns/ProcessImplementation.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/CovarianceModel.hxx"
#include "openturns/TrendTransform.hxx"
#include "openturns/HMatrix.hxx"
#include "openturns/Process.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class GaussianProcess
 *
 * The implementation of the Gaussian process class
 */
class OT_API GaussianProcess
  : public ProcessImplementation
{
  CLASSNAME

public:

  /** Some typedefs to ease reading */

  /** Default constructor */
  GaussianProcess();

  /** Standard constructor  */
  GaussianProcess(const TrendTransform & trend,
                  const CovarianceModel & covarianceModel,
                  const Mesh & mesh);

  /** Standard constructor  */
  GaussianProcess(const CovarianceModel & covarianceModel,
                  const Mesh & mesh);

  /** Virtual constructor */
  GaussianProcess * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** String converter  - pretty print */
  String __str__(const String & offset = "") const override;

  /** Mesh accessor */
  void setMesh(const Mesh & mesh) override;
  void setTimeGrid(const RegularGrid & timeGrid) override;

  /** Set sampling method accessor */
  void setSamplingMethod(const UnsignedInteger samplingMethod);

  /** Realization accessor */
  Field getRealization() const override;
private:
  Sample getRealizationCholesky() const;
  Sample getRealizationGibbs() const;
  Sample getRealizationHMatrix() const;
public:

  /** Covariance model accessor */
  CovarianceModel getCovarianceModel() const override;

  /** Trend accessor */
  TrendTransform getTrend() const override;

  /** Check if the process is stationary */
  Bool isStationary() const override;

  /** Check if the process trend is stationary */
  Bool isTrendStationary() const;

  /** Check if the process is Normal */
  Bool isNormal() const override;

  /** Get the process corresponding to indices components */
  Process getMarginal(const Indices & indices) const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /** Initialization of the process */
  void initialize() const;

  /** Check if the trend function is stationary */
  void checkStationaryTrend() const;


  /** Covariance model */
  CovarianceModel covarianceModel_;

  /** Cholesky factor  */
  mutable TriangularMatrix covarianceCholeskyFactor_;

  /** Cholesky factor  */
  mutable HMatrix covarianceHMatrix_;

  /** Flag to manage process initialization */
  mutable Bool isInitialized_;

  /** Flag to tell if the process has a stationary trend */
  mutable Bool hasStationaryTrend_;
  mutable Bool checkedStationaryTrend_;

  /** Trend function */
  TrendTransform trend_;

  mutable Point stationaryTrendValue_;

  /** Sampling method */
  UnsignedInteger samplingMethod_;

}; /* class GaussianProcess */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NORMALPROCESS_HXX */
