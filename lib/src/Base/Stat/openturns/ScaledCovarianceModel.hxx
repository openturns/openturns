//                                               -*- C++ -*-
/**
 *  @brief Covariance model scaled by a positive factor
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
#ifndef OPENTURNS_SCALEDCOVARIANCEMODEL_HXX
#define OPENTURNS_SCALEDCOVARIANCEMODEL_HXX

#include "openturns/CovarianceModel.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ScaledCovarianceModel
 *
 * Covariance model wrapping an inner model multiplied by a strictly
 * positive factor. Scale, amplitude, output correlation and nugget are
 * delegated to the inner model; the factor is appended to the full
 * parameter. The inner model is kept as-is (no amplitude reset), so the
 * factor times the inner amplitude ridge must be handled with active
 * sets at fitting time.
 */
class OT_API ScaledCovarianceModel
  : public CovarianceModelImplementation
{

  CLASSNAME

public:

  /** Default constructor */
  explicit ScaledCovarianceModel();

  /** Parameters constructor */
  ScaledCovarianceModel(const CovarianceModel & kernel,
                        const Scalar factor);

  /** Comparison operator */
  using CovarianceModelImplementation::operator ==;
  Bool operator ==(const ScaledCovarianceModel & other) const;

  /** Virtual copy constructor */
  ScaledCovarianceModel * clone() const override;

  /** Computation of the covariance function */
  using CovarianceModelImplementation::operator();
  SquareMatrix operator()(const Point & s,
                          const Point & t) const override;
  SquareMatrix operator()(const Point & tau) const override;

  /** Scalar computation, output dimension 1 only */
  using CovarianceModelImplementation::computeAsScalar;
  Scalar computeAsScalar(const Point & s,
                         const Point & t) const override;
  Scalar computeAsScalar(const Point & tau) const override;
#ifndef SWIG
  Scalar computeAsScalar(const Collection<Scalar>::const_iterator & s_begin,
                         const Collection<Scalar>::const_iterator & t_begin) const override;
#endif
  Scalar computeAsScalar(const Scalar s,
                         const Scalar t) const override;
  Scalar computeAsScalar(const Scalar tau) const override;

  /** Gradient with respect to the input */
  Matrix partialGradient(const Point & s,
                         const Point & t) const override;

  /** Gradient with respect to the parameters: scaled inner gradient
   *  plus one row for the factor when active */
  Matrix parameterGradient(const Point & s,
                           const Point & t) const override;

  /** Kernel accessor */
  CovarianceModel getKernel() const;
  void setKernel(const CovarianceModel & kernel);

  /** Factor accessor, distinct from the input scale vector */
  Scalar getScaleFactor() const;
  void setScaleFactor(const Scalar factor);

  /** Marginal accessor */
  CovarianceModel getMarginal(const UnsignedInteger index) const override;
  CovarianceModel getMarginal(const Indices & indices) const override;
  using CovarianceModelImplementation::getMarginal;

  /** Delegated accessors */
  Point getScale() const override;
  void setScale(const Point & scale) override;
  Point getAmplitude() const override;
  void setAmplitude(const Point & amplitude) override;
  CorrelationMatrix getOutputCorrelation() const override;
  void setOutputCorrelation(const CorrelationMatrix & correlation) override;
  Scalar getNuggetFactor() const override;
  void setNuggetFactor(const Scalar nuggetFactor) override;

  /** Is it a stationary covariance model? */
  Bool isStationary() const override;

  /** Is it a diagonal covariance model? */
  Bool isDiagonal() const override;

  /** Is it safe to compute discretize etc in parallel? */
  Bool isParallel() const override;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Parameter accessors: inner full parameter plus trailing factor */
  void setFullParameter(const Point & parameter) override;
  Point getFullParameter() const override;
  Description getFullParameterDescription() const override;
  void setActiveParameter(const Indices & active) override;

private:
  /** The wrapped model */
  CovarianceModel kernel_;

  /** The positive scaling factor */
  Scalar factor_ = 1.0;

  Bool equals(const CovarianceModelImplementation & other) const override;

} ; /* class ScaledCovarianceModel */

END_NAMESPACE_OPENTURNS

#endif
