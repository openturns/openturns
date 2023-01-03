//                                               -*- C++ -*-
/**
 *  @brief This class builds a covariance model using a mesh and a covariance
 *         function given as a collection of Covariance Matrix or a global
 *         covariance matrix
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_USERDEFINEDCOVARIANCEMODEL_HXX
#define OPENTURNS_USERDEFINEDCOVARIANCEMODEL_HXX

#include "openturns/CovarianceModel.hxx"
#include "openturns/RegularGrid.hxx"
#include "openturns/Mesh.hxx"
#include "openturns/NearestNeighbourAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class UserDefinedCovarianceModel
 */

class OT_API UserDefinedCovarianceModel
  : public CovarianceModelImplementation
{

  CLASSNAME

public:

  /** Default constructor */
  UserDefinedCovarianceModel();

  /** Standard constructor */
  UserDefinedCovarianceModel(const Mesh & mesh,
                             const CovarianceMatrix & covariance);

  /** Virtual copy constructor */
  UserDefinedCovarianceModel * clone() const override;

  /** Computation of the covariance function */
  using CovarianceModelImplementation::computeAsScalar;
  Scalar computeAsScalar(const Point & s,
                         const Point & t) const override;
#ifndef SWIG
  Scalar computeAsScalar(const Collection<Scalar>::const_iterator &s_begin,
                         const Collection<Scalar>::const_iterator &t_begin) const override;
#endif

  /** Computation of the covariance function */
  using CovarianceModelImplementation::operator();
  SquareMatrix operator() (const Point & s, const Point & t) const override;

private:
  SquareMatrix operator() (const UnsignedInteger i, const UnsignedInteger j) const;

public:

  /** Discretize the covariance function on a given TimeGrid/Mesh */
  using CovarianceModelImplementation::discretize;
  CovarianceMatrix discretize(const Sample & vertices) const override;
  Sample discretizeRow(const Sample & vertices,
                       const UnsignedInteger p) const override;
  TriangularMatrix discretizeAndFactorize(const Sample & vertices) const override;

  /** Mesh accessor */
  Mesh getMesh() const;

  /** TimeGrid accessor */
  RegularGrid getTimeGrid() const;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** Covariance matrix of the native discretization */
  CovarianceMatrix covariance_;

  /** Mesh of the native discretization */
  Pointer<Mesh> p_mesh_;

  /** NearestNeighbourAlgorithm to speed-up nearest neighbour search */
  NearestNeighbourAlgorithm nearestNeighbour_;

} ; /* class UserDefinedCovarianceModel */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_USERDEFINEDSPECTRALMODEL_HXX */
