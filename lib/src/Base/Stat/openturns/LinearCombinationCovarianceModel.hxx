//                                               -*- C++ -*-
/**
 *  @brief Linear combination of scalar covariance models
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
#ifndef OPENTURNS_LINEARCOMBINATIONCOVARIANCEMODEL_HXX
#define OPENTURNS_LINEARCOMBINATIONCOVARIANCEMODEL_HXX

#include "openturns/CovarianceModel.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Collection.hxx"
#include "openturns/Indices.hxx"
#include "openturns/Point.hxx"
#include "openturns/Description.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LinearCombinationCovarianceModel
 *
 * Covariance model written as a nonnegative linear combination of
 * scalar covariance models, with a shared output correlation matrix.
 * The coefficients are variance weights: the amplitude of an atom is
 * the square root of its coefficient.
 */
class OT_API LinearCombinationCovarianceModel
  : public CovarianceModelImplementation
{

  CLASSNAME

public:

  typedef PersistentCollection<CovarianceModel> CovarianceModelPersistentCollection;
  typedef Collection<CovarianceModel>           CovarianceModelCollection;

  /** Default constructor: one scalar absolute exponential atom */
  LinearCombinationCovarianceModel();

  /** Collection constructor */
  LinearCombinationCovarianceModel(const CovarianceModelCollection & atoms,
                                   const Point & coefficients,
                                   const UnsignedInteger outputDimension = 1);

  /** Comparison operator */
  using CovarianceModelImplementation::operator ==;
  Bool operator ==(const LinearCombinationCovarianceModel & other) const;

  /** Virtual copy constructor */
  LinearCombinationCovarianceModel * clone() const override;

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
  Scalar computeAsScalar(const Scalar * s,
                         const Scalar * t) const override;
#endif
  Scalar computeAsScalar(const Scalar s,
                         const Scalar t) const override;
  Scalar computeAsScalar(const Scalar tau) const override;

  /** Gradient with respect to the input */
  Matrix partialGradient(const Point & s,
                         const Point & t) const override;

  /** Gradient with respect to the parameters */
  Matrix parameterGradient(const Point & s,
                           const Point & t) const override;

  /** Atom accessor */
  CovarianceModelCollection getAtoms() const;
  void setAtoms(const CovarianceModelCollection & atoms);

  /** Variance weight accessor */
  Point getCoefficients() const;
  void setCoefficients(const Point & coefficients);

  /** Amplitude accessor: square root of the variance weights */
  Point getAmplitudes() const;
  void setAmplitudes(const Point & amplitudes);

  /** Generic scale accessor: no unique value at the combination level */
  Point getScale() const override;
  void setScale(const Point & scale) override;

  /** Amplitude accessor: total smooth standard deviation */
  Point getAmplitude() const override;
  void setAmplitude(const Point & amplitude) override;

  /** Managed nugget accessor */
  Scalar getNuggetFactor() const override;
  void setNuggetFactor(const Scalar nuggetFactor) override;
  Scalar getNuggetVariance() const;
  void setNuggetVariance(const Scalar nuggetVariance);
  void activateNuggetFactor(const Bool isNuggetFactorActive) override;

  /** Marginal accessors */
  CovarianceModel getMarginal(const UnsignedInteger index) const override;
  CovarianceModel getMarginal(const Indices & indices) const override;
  using CovarianceModelImplementation::getMarginal;

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

  /** Parameter accessors */
  void setFullParameter(const Point & parameter) override;
  Point getFullParameter() const override;
  Description getFullParameterDescription() const override;
  void setActiveParameter(const Indices & active) override;

protected:

  /** Build the sum of two scalar models, flattening nested combinations
   *  and folding identical atoms */
  static CovarianceModel Add(const CovarianceModel & left,
                             const CovarianceModel & right);

  /** Multiply a scalar model or a combination by a positive factor */
  static CovarianceModel Scale(const CovarianceModel & model,
                               const Scalar factor);

  /** Append a model with a variance weight, flattening nested
   *  combinations, normalizing legacy atoms and folding identical atoms */
  static void AppendAtom(CovarianceModelCollection & atoms,
                         Point & coefficients,
                         const CovarianceModel & model,
                         const Scalar weight,
                         const UnsignedInteger inputDimension);

private:

  /** The covariance model handle implements its algebra with Add and Scale */
  friend class CovarianceModel;

  void setAtomsAndCoefficients(const CovarianceModelCollection & atoms,
                               const Point & coefficients,
                               const UnsignedInteger outputDimension,
                               const Bool resetActive);

  Scalar smoothVariance() const;
  Scalar zeroLagVariance(const CovarianceModel & atom) const;
  void updateBaseState();
  void initializeActiveParameter();

  /** The scalar atoms */
  CovarianceModelPersistentCollection atoms_;

  /** The variance weights, one per atom */
  Point coefficients_;

  /** Index of the managed Dirac atom, valid when hasNugget_ is true */
  UnsignedInteger nuggetIndex_ = 0;
  Bool hasNugget_ = false;

  Bool equals(const CovarianceModelImplementation & other) const override;

} ; /* class LinearCombinationCovarianceModel */

END_NAMESPACE_OPENTURNS

#endif
