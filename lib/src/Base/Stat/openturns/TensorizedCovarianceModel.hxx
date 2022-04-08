//                                               -*- C++ -*-
/**
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_TENSORIZEDCOVARIANCEMODEL_HXX
#define OPENTURNS_TENSORIZEDCOVARIANCEMODEL_HXX

#include "openturns/CovarianceModel.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Collection.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class TensorizedCovarianceModel
 */

class OT_API TensorizedCovarianceModel
  : public CovarianceModelImplementation
{

  CLASSNAME

public:

  typedef PersistentCollection<CovarianceModel> CovarianceModelPersistentCollection;
  typedef Collection<CovarianceModel>           CovarianceModelCollection;

  /** Default constructor */
  explicit TensorizedCovarianceModel(const UnsignedInteger dimension = 1);

  /** Parameters constructor */
  explicit TensorizedCovarianceModel(const CovarianceModelCollection & collection);

  /** Parameters constructor */
  TensorizedCovarianceModel(const CovarianceModelCollection & collection,
                            const Point & scale);

  /** Virtual copy constructor */
  TensorizedCovarianceModel * clone() const override;

  /** Computation of the covariance function */
  using CovarianceModelImplementation::operator();
  SquareMatrix operator() (const Point & s, const Point & t) const override;
  SquareMatrix operator() (const Point & tau) const override;

  /** Computation of the covariance function */
  using CovarianceModelImplementation::computeAsScalar;
  Scalar computeAsScalar(const Point &s, const Point &t) const override;
  Scalar computeAsScalar(const Point &tau) const override;
#ifndef SWIG
  Scalar computeAsScalar(const Collection<Scalar>::const_iterator &s_begin,
                         const Collection<Scalar>::const_iterator &t_begin) const override;
#endif
  Scalar computeAsScalar(const Scalar tau) const override;

  /** Gradient */
  Matrix partialGradient(const Point & s, const Point & t) const override;

  /** Collection accessor */
  const CovarianceModelCollection & getCollection() const;

  /** Marginal accessor */
  CovarianceModel getMarginal(const UnsignedInteger index) const override;

  /** Scale accessor */
  void setScale(const Point & scale) override;

  /** Amplitude accessor */
  void setAmplitude(const Point & amplitude) override;

  /** Is it a stationary covariance model ? */
  Bool isStationary() const override;

  /** Is it a diagonal covariance model ? */
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

protected:
  /** Parameter accessor */
  void setFullParameter(const Point & parameter) override;
  Point getFullParameter() const override;
  Description getFullParameterDescription() const override;

  void setCollection(const CovarianceModelCollection & collection);

private:
  /** The collection of marginal models */
  CovarianceModelPersistentCollection collection_;

} ; /* class TensorizedCovarianceModel */

END_NAMESPACE_OPENTURNS

#endif
