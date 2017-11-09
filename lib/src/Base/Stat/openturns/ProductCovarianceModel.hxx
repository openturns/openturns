//                                               -*- C++ -*-
/**
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
#ifndef OPENTURNS_PRODUCTCOVARIANCEMODEL_HXX
#define OPENTURNS_PRODUCTCOVARIANCEMODEL_HXX

#include "openturns/StationaryCovarianceModel.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Collection.hxx"

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
  explicit ProductCovarianceModel(const UnsignedInteger spatialDimension = 1);

  /** Parameters constructor */
  explicit ProductCovarianceModel(const CovarianceModelCollection & collection);

  /** Virtual copy constructor */
  ProductCovarianceModel * clone() const;

  /** Computation of the covariance function */
  using CovarianceModelImplementation::computeStandardRepresentative;
  Scalar computeStandardRepresentative(const Point & s,
                                       const Point & t) const;
#ifndef SWIG
  Scalar computeStandardRepresentative(const Collection<Scalar>::const_iterator & s_begin,
    const Collection<Scalar>::const_iterator & t_begin) const;
#endif

  /** Gradient */
  virtual Matrix partialGradient(const Point & s,
                                 const Point & t) const;

  /** Collection accessor */
  const CovarianceModelCollection & getCollection() const;

  /** Marginal accessor */
  virtual Implementation getMarginal(const UnsignedInteger index) const;

  /** Scale accessor */
  void setScale(const Point & scale);

  /** Is it a stationary covariance model ? */
  virtual Bool isStationary() const;

  /** String converter */
  String __repr__() const;

  /** String converter */
  String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:
  void setCollection(const CovarianceModelCollection & collection);

  /** Parameter accessor */
  virtual void setFullParameter(const Point & parameter);
  virtual Point getFullParameter() const;
  virtual Description getFullParameterDescription() const;

private:
  /** The collection of marginal models */
  CovarianceModelPersistentCollection collection_;

} ; /* class ProductCovarianceModel */

END_NAMESPACE_OPENTURNS

#endif
