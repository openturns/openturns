//                                               -*- C++ -*-
/**
 *  @brief This class build a stationary covariance model using a time grid and a collection of covariance matrices
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
#ifndef OPENTURNS_USERDEFINEDSTATIONARYCOVARIANCEMODEL_HXX
#define OPENTURNS_USERDEFINEDSTATIONARYCOVARIANCEMODEL_HXX

#include "openturns/StationaryCovarianceModel.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Collection.hxx"
#include "openturns/RegularGrid.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class UserDefinedStationaryCovarianceModel
 */

class OT_API UserDefinedStationaryCovarianceModel
  : public StationaryCovarianceModel
{

  CLASSNAME;

public:

  typedef PersistentCollection<CovarianceMatrix>          CovarianceMatrixPersistentCollection;
  typedef Collection<CovarianceMatrix>                    CovarianceMatrixCollection;

  /** Default onstructor */
  UserDefinedStationaryCovarianceModel();

  /** Standard onstructor */
  UserDefinedStationaryCovarianceModel(const Mesh & mesh,
                                       const CovarianceMatrixCollection & covarianceCollection);

  /** Virtual copy constructor */
  virtual UserDefinedStationaryCovarianceModel * clone() const;

  /** Computation of the covariance function */
  using StationaryCovarianceModel::operator();
  CovarianceMatrix operator() (const Point & tau) const;

  /** Time grid/mesh accessor */
  Mesh getMesh() const;
  RegularGrid getTimeGrid() const;

  /** String converter */
  String __repr__() const;

  /** String converter */
  String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private:

  /** Collection of covariance functions */
  CovarianceMatrixPersistentCollection covarianceCollection_;

  /** Mesh of evaluation */
  Pointer<Mesh> p_mesh_;

} ; /* class UserDefinedStationaryCovarianceModel */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_USERDEFINEDSTATIONARYCOVARIANCEMODEL_HXX */
