//                                               -*- C++ -*-
/**
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
#ifndef OPENTURNS_TENSORIZEDCOVARIANCEMODEL_HXX
#define OPENTURNS_TENSORIZEDCOVARIANCEMODEL_HXX

#include "openturns/StationaryCovarianceModel.hxx"
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
  TensorizedCovarianceModel * clone() const;

  /** Computation of the covariance function */
  using CovarianceModelImplementation::operator();
  CovarianceMatrix operator() (const Point & s,
                               const Point & t) const;

  /** Gradient */
  virtual Matrix partialGradient(const Point & s,
                                 const Point & t) const;

  /** Collection accessor */
  const CovarianceModelCollection & getCollection() const;

  /** Marginal accessor */
  virtual CovarianceModel getMarginal(const UnsignedInteger index) const;

  /** Scale accessor */
  void setScale(const Point & scale);

  /** Amplitude accessor */
  void setAmplitude(const Point & amplitude);

  /** Is it a stationary covariance model ? */
  virtual Bool isStationary() const;

  /** Is it a diagonal covariance model ? */
  virtual Bool isDiagonal() const;

  /** Is it a composite covariance model ? */
  virtual Bool isComposite() const;

  /** String converter */
  String __repr__() const;

  /** String converter */
  String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:
  /** Parameter accessor */
  virtual void setFullParameter(const Point & parameter);
  virtual Point getFullParameter() const;
  virtual Description getFullParameterDescription() const;

  void setCollection(const CovarianceModelCollection & collection);

private:
  /** The collection of marginal models */
  CovarianceModelPersistentCollection collection_;

} ; /* class TensorizedCovarianceModel */

END_NAMESPACE_OPENTURNS

#endif
