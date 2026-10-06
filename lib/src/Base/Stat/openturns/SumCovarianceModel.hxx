//                                               -*- C++ -*-
/**
 *  @brief Sum of covariance models on the same input/output spaces
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
#ifndef OPENTURNS_SUMCOVARIANCEMODEL_HXX
#define OPENTURNS_SUMCOVARIANCEMODEL_HXX

#include "openturns/CovarianceModel.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Collection.hxx"
#include "openturns/Indices.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SumCovarianceModel
 *
 * Sum of covariance models sharing the same input and output dimensions.
 * There is no unique scale, amplitude, output correlation or nugget
 * at the sum level: every hyperparameter lives in the members and the
 * full parameter is the concatenation of the member full parameters.
 */
class OT_API SumCovarianceModel
  : public CovarianceModelImplementation
{

  CLASSNAME

public:

  typedef PersistentCollection<CovarianceModel> CovarianceModelPersistentCollection;
  typedef Collection<CovarianceModel>           CovarianceModelCollection;

  /** Default constructor: sum of two 1D absolute exponential models */
  explicit SumCovarianceModel(const UnsignedInteger inputDimension = 1);

  /** Collection constructor */
  explicit SumCovarianceModel(const CovarianceModelCollection & collection);

  /** Build the sum of two models, flattening nested sums and folding
   *  identical atoms into scaled models, as LinearCombinationFunction::add */
  static CovarianceModel add(const CovarianceModel & left,
                             const CovarianceModel & right);

  /** Comparison operator */
  using CovarianceModelImplementation::operator ==;
  Bool operator ==(const SumCovarianceModel & other) const;

  /** Virtual copy constructor */
  SumCovarianceModel * clone() const override;

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

  /** Gradient with respect to the parameters: row-wise concatenation
   *  of member gradients following the active parameter order */
  Matrix parameterGradient(const Point & s,
                           const Point & t) const override;

  /** Collection accessor */
  CovarianceModelCollection getCollection() const;

  /** Marginal accessors */
  CovarianceModel getMarginal(const UnsignedInteger index) const override;
  CovarianceModel getMarginal(const Indices & indices) const override;
  using CovarianceModelImplementation::getMarginal;

  /** Generic scale/amplitude/correlation/nugget accessors are not defined
   *  at the sum level: use the members or the full parameter instead.
   *  They throw NotDefinedException.
   */
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

  /** Parameter accessors: concatenation of member full parameters */
  void setFullParameter(const Point & parameter) override;
  Point getFullParameter() const override;
  Description getFullParameterDescription() const override;
  void setActiveParameter(const Indices & active) override;

protected:
  void setCollection(const CovarianceModelCollection & collection);

  /** Append a model to a sum collection, flattening nested sums and
   *  summing the factors of identical atoms */
  static void appendMerged(CovarianceModelCollection & collection,
                           const CovarianceModel & model);

private:
  /** The collection of summed models, all sharing input/output dimensions */
  CovarianceModelPersistentCollection collection_;

  Bool equals(const CovarianceModelImplementation & other) const override;

} ; /* class SumCovarianceModel */

END_NAMESPACE_OPENTURNS

#endif
