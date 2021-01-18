//                                               -*- C++ -*-
/**
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_PRODUCTCOVARIANCEMODEL_HXX
#define OPENTURNS_PRODUCTCOVARIANCEMODEL_HXX

#include "openturns/CovarianceModel.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Collection.hxx"
#include "openturns/Indices.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ProductCovarianceModel
 */

class OT_API ProductCovarianceModel
  : public CovarianceModelImplementation
{

  CLASSNAME

public:

  typedef PersistentCollection<CovarianceModel> CovarianceModelPersistentCollection;
  typedef Collection<CovarianceModel>           CovarianceModelCollection;

  /** Default constructor */
  explicit ProductCovarianceModel(const UnsignedInteger inputDimension = 1);

  /** Parameters constructor */
  explicit ProductCovarianceModel(const CovarianceModelCollection & collection);

  /** Virtual copy constructor */
  ProductCovarianceModel * clone() const override;

  /** Computation of the covariance function */
  using CovarianceModelImplementation::computeAsScalar;
  Scalar computeAsScalar(const Point &tau) const override;
#ifndef SWIG
  Scalar computeAsScalar(const Collection<Scalar>::const_iterator & s_begin,
                         const Collection<Scalar>::const_iterator & t_begin) const override;
#endif

  /** Gradient */
  Matrix partialGradient(const Point & s,
                         const Point & t) const override;

  /** Collection accessor */
  const CovarianceModelCollection & getCollection() const;

  /** Marginal accessor */
  CovarianceModel getMarginal(const UnsignedInteger index) const override;

  /** Scale accessor */
  void setScale(const Point & scale) override;

  /** Is it a stationary covariance model ? */
  Bool isStationary() const override;

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

  /** Parameter accessor */
  void setFullParameter(const Point & parameter) override;
  Point getFullParameter() const override;
  Description getFullParameterDescription() const override;
  void setActiveParameter(const Indices & active) override;

protected:
  void setCollection(const CovarianceModelCollection & collection);

private:
  /** The collection of marginal models */
  CovarianceModelPersistentCollection collection_;

  /** Handle extra parameters of the marginal models */
  Indices extraParameterNumber_;

} ; /* class ProductCovarianceModel */

END_NAMESPACE_OPENTURNS

#endif
