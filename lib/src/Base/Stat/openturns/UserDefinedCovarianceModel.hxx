//                                               -*- C++ -*-
/**
 *  @brief This class builds a spectral model using a frequency grid and a spectral function
 *  given as a collection of Covariance Matrix
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
#ifndef OPENTURNS_USERDEFINEDCOVARIANCEMODEL_HXX
#define OPENTURNS_USERDEFINEDCOVARIANCEMODEL_HXX

#include "openturns/CovarianceModel.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Collection.hxx"
#include "openturns/RegularGrid.hxx"
#include "openturns/Mesh.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class UserDefinedCovarianceModel
 */

class OT_API UserDefinedCovarianceModel
  : public CovarianceModelImplementation
{

  CLASSNAME;

public:

  typedef PersistentCollection<CovarianceMatrix>          CovarianceMatrixPersistentCollection;
  typedef Collection<CovarianceMatrix>                    CovarianceMatrixCollection;

  /** Default onstructor */
  UserDefinedCovarianceModel();

  /** Standard onstructor */
  UserDefinedCovarianceModel(const Mesh & mesh,
                             const CovarianceMatrixCollection & spectralFunction);

  /** Virtual copy constructor */
  virtual UserDefinedCovarianceModel * clone() const;

  /** Computation of the covariance function */
  using CovarianceModelImplementation::operator();
  CovarianceMatrix operator() (const Point & s,
                               const Point & t) const;
private:
  CovarianceMatrix operator() (const UnsignedInteger i,
                               const UnsignedInteger j) const;
public:

  /** Discretize the covariance function on a given TimeGrid/Mesh */
  using CovarianceModelImplementation::discretize;
  virtual CovarianceMatrix discretize(const Mesh & mesh) const;
  virtual Sample discretizeRow(const Sample & vertices,
                               const UnsignedInteger p) const;

  /** Mesh accessor */
  Mesh getMesh() const;

  /** TimeGrid accessor */
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

} ; /* class UserDefinedCovarianceModel */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_USERDEFINEDSPECTRALMODEL_HXX */
