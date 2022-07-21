//                                               -*- C++ -*-
/**
 *  @brief This class build a stationary covariance model using a time grid and a collection of covariance matrices
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
#ifndef OPENTURNS_USERDEFINEDSTATIONARYCOVARIANCEMODEL_HXX
#define OPENTURNS_USERDEFINEDSTATIONARYCOVARIANCEMODEL_HXX

#include "openturns/CovarianceModelImplementation.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Collection.hxx"
#include "openturns/RegularGrid.hxx"
#include "openturns/RegularGridNearestNeighbour.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class UserDefinedStationaryCovarianceModel
 */

class OT_API UserDefinedStationaryCovarianceModel
  : public CovarianceModelImplementation
{

  CLASSNAME

public:

  typedef PersistentCollection<SquareMatrix>      SquareMatrixPersistentCollection;
  typedef Collection<SquareMatrix>                SquareMatrixCollection;

  /** Default constructor */
  UserDefinedStationaryCovarianceModel();

  /** Standard constructor */
  UserDefinedStationaryCovarianceModel(const RegularGrid & mesh,
                                       const SquareMatrixCollection & covarianceCollection);

  /** Virtual copy constructor */
  UserDefinedStationaryCovarianceModel * clone() const override;

  /** Computation of the covariance function */
  using CovarianceModelImplementation::computeAsScalar;
  Scalar computeAsScalar(const Point &tau) const override;
#ifndef SWIG
  Scalar computeAsScalar(const Collection<Scalar>::const_iterator &s_begin,
                         const Collection<Scalar>::const_iterator &t_begin) const override;
#endif

  /** Computation of the covariance function */
  using CovarianceModelImplementation::operator();
  SquareMatrix operator() (const Point & tau) const override;

  /** Time grid/mesh accessor */
  RegularGrid getTimeGrid() const;

  /** Discretize the covariance function on a given TimeGrid */
  using CovarianceModelImplementation::discretize;
  CovarianceMatrix discretize(const Mesh & mesh) const override;
  CovarianceMatrix discretize(const Sample & vertices) const override;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** Collection of covariance functions */
  SquareMatrixPersistentCollection covarianceCollection_;

  /** Mesh of evaluation */
  RegularGrid mesh_;

  /** Evaluation of nearest neighbour on a RegularGrid */
  RegularGridNearestNeighbour nearestNeighbour_;

} ; /* class UserDefinedStationaryCovarianceModel */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_USERDEFINEDSTATIONARYCOVARIANCEMODEL_HXX */
