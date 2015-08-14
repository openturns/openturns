//                                               -*- C++ -*-
/**
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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

#include "StationaryCovarianceModel.hxx"
#include "PersistentCollection.hxx"
#include "Collection.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ProductCovarianceModel
 */

class OT_API ProductCovarianceModel
  : public CovarianceModelImplementation
{

  CLASSNAME;

public:

  typedef PersistentCollection<CovarianceModel> CovarianceModelPersistentCollection;
  typedef Collection<CovarianceModel>           CovarianceModelCollection;

  /** Default constructor */
  explicit ProductCovarianceModel(const UnsignedInteger spatialDimension = 1);

  /** Parameters constructor */
  ProductCovarianceModel(const CovarianceModelCollection & collection);

  /** Virtual copy constructor */
  ProductCovarianceModel * clone() const;

  /** Computation of the covariance function */
  using CovarianceModelImplementation::operator();
  CovarianceMatrix operator() (const NumericalPoint & s,
                               const NumericalPoint & t) const;

  /** Case for 1D output dimension */
  using CovarianceModelImplementation::computeAsScalar;
  NumericalScalar computeAsScalar(const NumericalPoint & s,
                                  const NumericalPoint & t) const;

  /** Gradient */
  virtual Matrix partialGradient(const NumericalPoint & s,
                                 const NumericalPoint & t) const;

  /** Collection accessor */
  const CovarianceModelCollection & getCollection() const;

  /** Marginal accessor */
  virtual Implementation getMarginal(const UnsignedInteger index) const;

  /** Parameters accessor */
  void setParameters(const NumericalPoint & parameters);
  NumericalPointWithDescription getParameters() const;

  /** Scale accessor */
  void setScale(const NumericalPoint & scale);
  NumericalPoint getScale() const;

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

  /** Gradient  evaluation in case dimension = 1 */
  Matrix partialGradient1D(const NumericalPoint & s,
                           const NumericalPoint & t) const;

private:
  /** The collection of marginal models */
  CovarianceModelPersistentCollection collection_;

} ; /* class ProductCovarianceModel */

END_NAMESPACE_OPENTURNS

#endif
