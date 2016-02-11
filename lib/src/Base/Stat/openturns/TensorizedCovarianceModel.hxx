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
#ifndef OPENTURNS_TENSORIZEDCOVARIANCEMODEL_HXX
#define OPENTURNS_TENSORIZEDCOVARIANCEMODEL_HXX

#include "openturns/StationaryCovarianceModel.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Collection.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class TensorizedCovarianceModel
 */

class OT_API TensorizedCovarianceModel
  : public CovarianceModelImplementation
{

  CLASSNAME;

public:

  typedef PersistentCollection<CovarianceModel> CovarianceModelPersistentCollection;
  typedef Collection<CovarianceModel>           CovarianceModelCollection;

  /** Default constructor */
  explicit TensorizedCovarianceModel(const UnsignedInteger dimension = 1);

  /** Parameters constructor */
  explicit TensorizedCovarianceModel(const CovarianceModelCollection & collection);

  /** Parameters constructor */
  TensorizedCovarianceModel(const CovarianceModelCollection & collection,
                            const NumericalPoint & scale);

  /** Virtual copy constructor */
  TensorizedCovarianceModel * clone() const;

  /** Computation of the covariance function */
  using CovarianceModelImplementation::operator();
  CovarianceMatrix operator() (const NumericalPoint & s,
                               const NumericalPoint & t) const;

  /** Gradient */
  virtual Matrix partialGradient(const NumericalPoint & s,
                                 const NumericalPoint & t) const;

  /** Collection accessor */
  const CovarianceModelCollection & getCollection() const;

  /** Marginal accessor */
  virtual Implementation getMarginal(const UnsignedInteger index) const;

  /** Parameters accessor */
  void setParameter(const NumericalPoint & parameter);
  NumericalPoint getParameter() const;
  Description getParameterDescription() const;

  /** Scale accessor */
  void setScale(const NumericalPoint & scale);

  /** Amplitude accessor */
  void setAmplitude(const NumericalPoint & amplitude);

  /** Is it a stationary covariance model ? */
  virtual Bool isStationary() const;

  /** Is it a diagonal covariance model ? */
  virtual Bool isDiagonal() const;

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

private:
  /** The collection of marginal models */
  CovarianceModelPersistentCollection collection_;

} ; /* class TensorizedCovarianceModel */

END_NAMESPACE_OPENTURNS

#endif
