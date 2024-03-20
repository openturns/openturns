//                                               -*- C++ -*-
/**
 *  @brief This class build a stationary covariance model using a time grid and a collection of covariance matrices
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/UserDefinedStationaryCovarianceModel.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(UserDefinedStationaryCovarianceModel)

static const Factory<UserDefinedStationaryCovarianceModel> Factory_UserDefinedStationaryCovarianceModel;

/* Constructor with parameters */
UserDefinedStationaryCovarianceModel::UserDefinedStationaryCovarianceModel()
  : CovarianceModelImplementation()
  , covarianceCollection_(0)
  , mesh_()
  , nearestNeighbour_()
{
  inputDimension_ = 1;
  outputDimension_ = 0;
  isStationary_ = true;
}

// Classical constructor
// For a stationary model, we need N covariance matrices with N the number of time stamps in the time grid
UserDefinedStationaryCovarianceModel::UserDefinedStationaryCovarianceModel(const RegularGrid & mesh,
    const SquareMatrixCollection & covarianceFunction)
  : CovarianceModelImplementation()
  , covarianceCollection_(0)
  , mesh_(mesh)
  , nearestNeighbour_(mesh)
{
  isStationary_ = true;
  const UnsignedInteger size = mesh.getVerticesNumber();
  if (size != covarianceFunction.getSize())
    throw InvalidArgumentException(HERE) << "Error: for a non stationary covariance model, sizes are incoherent"
                                         << " mesh size = " << size << "covariance function size = " << covarianceFunction.getSize();
  inputDimension_ = mesh.getDimension();
  covarianceCollection_ = SquareMatrixCollection(size);
  // put the first element
  covarianceCollection_[0] = covarianceFunction[0];
  outputDimension_ = covarianceCollection_[0].getDimension();
  // put the next elements if dimension is ok
  for (UnsignedInteger k = 1; k < size; ++k)
  {
    if (covarianceFunction[k].getDimension() != outputDimension_)
      throw InvalidArgumentException(HERE) << " Error with dimension; the covariance matrices should be of same dimension";
    covarianceCollection_[k] = covarianceFunction[k];
  }
}

/* Virtual constructor */
UserDefinedStationaryCovarianceModel * UserDefinedStationaryCovarianceModel::clone() const
{
  return new UserDefinedStationaryCovarianceModel(*this);
}

Scalar UserDefinedStationaryCovarianceModel::computeAsScalar(const Point &tau) const
{
  if (outputDimension_ > 1)
    throw InvalidArgumentException(HERE) << "Error : UserDefinedStationaryCovarianceModel::computeAsScalar(tau) should be only used if output dimension is 1. Here, output dimension = " << outputDimension_;
  if (tau.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "In UserDefinedStationaryCovarianceModel::computeStandardRepresentative: expected a shift of dimension=" << getInputDimension() << ", got dimension=" << tau.getDimension();
  // We filter the collection & return corresponding values
  if (mesh_.getN() == 1)
    return covarianceCollection_[0](0, 0);
  UnsignedInteger index;
  if (tau[0] < 0.0)
    index = nearestNeighbour_.query(tau * (-1.0));
  else
    index = nearestNeighbour_.query(tau);
  // index
  return covarianceCollection_[index](0, 0);
}

Scalar UserDefinedStationaryCovarianceModel::computeAsScalar(const Collection<Scalar>::const_iterator &s_begin,
    const Collection<Scalar>::const_iterator &t_begin) const
{
  if (outputDimension_ != 1)
    throw InvalidArgumentException(HERE) << "Error : UserDefinedStationaryCovarianceModel::computeAsScalar(it, it) should be only used if output dimension is 1. Here, output dimension = " << outputDimension_;

  // Work on iterators
  // Unfortunately there is no other solution then generating the tau point
  Collection<Scalar>::const_iterator s_it = s_begin;
  Collection<Scalar>::const_iterator t_it = t_begin;
  Point tau(inputDimension_, 0.0);
  for (UnsignedInteger i = 0; i < inputDimension_; ++i, ++s_it, ++t_it)
  {
    tau[i] = *s_it - *t_it;
  }
  return computeAsScalar(tau);
}

/* Computation of the covariance function */
SquareMatrix UserDefinedStationaryCovarianceModel::operator()(const Point & tau) const
{
  if (tau.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: expected a shift of dimension=" << inputDimension_ << ", got dimension=" << tau.getDimension();
  // If the grid size is one, return the covariance function
  // else find in the grid the nearest instant values with nonnegative first component
  if (mesh_.getN() == 1) return covarianceCollection_[0];

  if (tau[0] < 0.0) return covarianceCollection_[nearestNeighbour_.query(tau * (-1.0))];
  return covarianceCollection_[nearestNeighbour_.query(tau)];
}

CovarianceMatrix UserDefinedStationaryCovarianceModel::discretize(const Mesh & mesh) const
{
  return discretize(RegularGrid(mesh));
}

CovarianceMatrix UserDefinedStationaryCovarianceModel::discretize(const Sample & vertices) const
{
  return discretize(Mesh(vertices));
}

/* Mesh accessor */
RegularGrid UserDefinedStationaryCovarianceModel::getTimeGrid() const
{
  return mesh_;
}

/* String converter */
String UserDefinedStationaryCovarianceModel::__repr__() const
{
  OSS oss(true);
  oss << "class=" << UserDefinedStationaryCovarianceModel::GetClassName()
      << " mesh=" << mesh_.__repr__()
      << " covarianceCollection=" << covarianceCollection_;
  return oss;
}

/* String converter */
String UserDefinedStationaryCovarianceModel::__str__(const String & ) const
{
  return __repr__();
}

/* Method save() stores the object through the StorageManager */
void UserDefinedStationaryCovarianceModel::save(Advocate & adv) const
{
  CovarianceModelImplementation::save(adv);
  adv.saveAttribute( "covarianceCollection_", covarianceCollection_);
  adv.saveAttribute( "mesh_", mesh_);
}

/* Method load() reloads the object from the StorageManager */
void UserDefinedStationaryCovarianceModel::load(Advocate & adv)
{
  CovarianceModelImplementation::load(adv);
  adv.loadAttribute( "covarianceCollection_", covarianceCollection_);
  adv.loadAttribute( "mesh_", mesh_);
}

END_NAMESPACE_OPENTURNS
