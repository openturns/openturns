//                                               -*- C++ -*-
/**
 *  @brief
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
 *  Id:      $Id$
 */
#include "openturns/UserDefinedStationaryCovarianceModel.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

// TEMPLATE_CLASSNAMEINIT(PersistentCollection< CovarianceMatrix >);
// static const Factory<PersistentCollection<CovarianceMatrix> > RegisteredFactory1;

CLASSNAMEINIT(UserDefinedStationaryCovarianceModel);

static const Factory<UserDefinedStationaryCovarianceModel> RegisteredFactory;

/* Constructor with parameters */
UserDefinedStationaryCovarianceModel::UserDefinedStationaryCovarianceModel()
  : StationaryCovarianceModel()
  , covarianceCollection_(0)
  , p_mesh_(Mesh().clone())
{
  spatialDimension_ = 1;
  dimension_ = 0;
}

// Classical constructor
// For a stationary model, we need N covariance matrices with N the number of time stamps in the time grid
UserDefinedStationaryCovarianceModel::UserDefinedStationaryCovarianceModel(const Mesh & mesh,
                                                                           const CovarianceMatrixCollection & covarianceFunction)
  : StationaryCovarianceModel()
  , covarianceCollection_(0)
{
  const UnsignedInteger size(mesh.getVerticesNumber());
  if (size != covarianceFunction.getSize())
    throw InvalidArgumentException(HERE) << "Error: for a non stationary covariance model, sizes are incoherents"
                                         << " mesh size = " << size << "covariance function size = " << covarianceFunction.getSize();
  p_mesh_ = mesh.clone();
  spatialDimension_ = mesh.getDimension();
  covarianceCollection_ = CovarianceMatrixCollection(size);
  // put the first element
  covarianceCollection_[0] = covarianceFunction[0];
  dimension_ = covarianceCollection_[0].getDimension();
  // put the next elements if dimension is ok
  for (UnsignedInteger k = 1; k < size; ++k)
    {
      if (covarianceFunction[k].getDimension() != dimension_)
        throw InvalidArgumentException(HERE) << " Error with dimension; the covariance matrices should be of same dimension";
      covarianceCollection_[k] = covarianceFunction[k];
    }
}

/* Virtual constructor */
UserDefinedStationaryCovarianceModel * UserDefinedStationaryCovarianceModel::clone() const
{
  return new UserDefinedStationaryCovarianceModel(*this);
}

/* Computation of the covariance function */
CovarianceMatrix UserDefinedStationaryCovarianceModel::operator()(const NumericalPoint & tau) const
{
  if (tau.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: expected a shift of dimension=" << spatialDimension_ << ", got dimension=" << tau.getDimension();
  // If the grid size is one, return the covariance function
  // else find in the grid the nearest instant values with nonnegative first component
  if (p_mesh_->getVerticesNumber() == 1) return covarianceCollection_[0];

  if (tau[0] < 0.0) return covarianceCollection_[p_mesh_->getNearestVertexIndex(tau * (-1.0))];
  return covarianceCollection_[p_mesh_->getNearestVertexIndex(tau)];
}

/* Time grid/mesh accessor */
Mesh UserDefinedStationaryCovarianceModel::getMesh() const
{
  return *p_mesh_;
}

/* Mesh accessor */
RegularGrid UserDefinedStationaryCovarianceModel::getTimeGrid() const
{
  return RegularGrid(*p_mesh_);
}

/* String converter */
String UserDefinedStationaryCovarianceModel::__repr__() const
{
  OSS oss(true);
  oss << "class=" << UserDefinedStationaryCovarianceModel::GetClassName()
      << " mesh=" << p_mesh_->__repr__()
      << " covarianceCollection=" << covarianceCollection_;
  return oss;
}

/* String converter */
String UserDefinedStationaryCovarianceModel::__str__(const String & offset) const
{
  return __repr__();
}

/* Method save() stores the object through the StorageManager */
void UserDefinedStationaryCovarianceModel::save(Advocate & adv) const
{
  StationaryCovarianceModel::save(adv);
  adv.saveAttribute( "mesh_", *p_mesh_);
  adv.saveAttribute( "covarianceCollection_", covarianceCollection_);
}

/* Method load() reloads the object from the StorageManager */
void UserDefinedStationaryCovarianceModel::load(Advocate & adv)
{
  StationaryCovarianceModel::load(adv);
  TypedInterfaceObject<Mesh> mesh;
  adv.loadAttribute( "mesh_", mesh);
  p_mesh_ = mesh.getImplementation();
  adv.loadAttribute( "covarianceCollection_", covarianceCollection_);
}

END_NAMESPACE_OPENTURNS
