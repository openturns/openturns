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
#include "openturns/UserDefinedCovarianceModel.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

TEMPLATE_CLASSNAMEINIT(PersistentCollection< CovarianceMatrix >);

static const Factory<PersistentCollection<CovarianceMatrix> > RegisteredFactory1;

CLASSNAMEINIT(UserDefinedCovarianceModel);

static const Factory<UserDefinedCovarianceModel> RegisteredFactory;

/* Constructor with parameters */
UserDefinedCovarianceModel::UserDefinedCovarianceModel()
  : CovarianceModelImplementation()
  , covarianceCollection_(0)
  , p_mesh_(RegularGrid().clone())
{
  dimension_ = 0;
}

// For a non stationary model, we need N x N covariance functions with N the number of vertices in the mesh
UserDefinedCovarianceModel::UserDefinedCovarianceModel(const Mesh & mesh,
    const CovarianceMatrixCollection & covarianceFunction)
  : CovarianceModelImplementation()
  , covarianceCollection_(0)
  , p_mesh_(0)
{
  const UnsignedInteger N(mesh.getVerticesNumber());
  const UnsignedInteger size((N * (N + 1)) / 2);
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: the mesh is empty.";
  if (size != covarianceFunction.getSize())
    throw InvalidArgumentException(HERE) << "Error: for a non stationary covariance model, sizes are incoherent:"
                                         << " mesh size=" << N << " and covariance function size=" << covarianceFunction.getSize() << " instead of " << size;
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
      throw InvalidArgumentException(HERE) << " Error with dimension; all the covariance matrices must have the same dimension";
    covarianceCollection_[k] = covarianceFunction[k];
  }
}

/* Virtual constructor */
UserDefinedCovarianceModel * UserDefinedCovarianceModel::clone() const
{
  return new UserDefinedCovarianceModel(*this);
}


/* Computation of the covariance density function */
CovarianceMatrix UserDefinedCovarianceModel::operator() (const NumericalPoint & s,
    const NumericalPoint & t) const
{
  if (s.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << spatialDimension_;
  if (t.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << spatialDimension_;
  // If the grid size is one, return the covariance function
  // else find in the grid the nearest instant values
  const UnsignedInteger N(p_mesh_->getVerticesNumber());
  if (N == 1) return covarianceCollection_[0];

  // Use the evaluation based on indices
  return operator()(p_mesh_->getNearestVertexIndex(s), p_mesh_->getNearestVertexIndex(t));
}

CovarianceMatrix UserDefinedCovarianceModel::operator() (const UnsignedInteger i,
    const UnsignedInteger j) const
{
  UnsignedInteger sIndex(i);
  UnsignedInteger tIndex(j);
  // The covariance matrices correspond to sIndex >= tIndex.
  // As C(s, t) = C(t, s), we swap sIndex and tIndex if sIndex < tIndex
  if (sIndex < tIndex) std::swap(sIndex, tIndex);
  // The covariances are stored the following way:
  // sIndex=0, tIndex=0 -> index=0
  // sIndex=1, tIndex=0 -> index=1
  // sIndex=1, tIndex=1 -> index=2
  // sIndex=2, tIndex=0 -> index=3
  // sIndex=2, tIndex=1 -> index=4
  // sIndex=2, tIndex=2 -> index=5
  // ie index = tIndex + sIndex * (sIndex + 1) / 2
  const SignedInteger index(tIndex + (sIndex * (sIndex + 1)) / 2);
  return covarianceCollection_[index];
}

CovarianceMatrix UserDefinedCovarianceModel::discretize(const Mesh & mesh) const
{
  const UnsignedInteger verticesNumber(mesh.getVerticesNumber());
  CovarianceMatrix covariance(verticesNumber * dimension_);
  // It is better to check vertices as the simplices don't play a role in the discretization
  if (p_mesh_->getVertices() == mesh.getVertices())
  {
    // Here we know that the given mesh is exactly the one defining the covariance model
    for (UnsignedInteger i = 0; i < covarianceCollection_.getSize(); ++i)
    {
      const UnsignedInteger jBase(static_cast< UnsignedInteger >(sqrt(2 * i + 0.25) - 0.5));
      const UnsignedInteger kBase(i - (jBase * (jBase + 1)) / 2);
      for (UnsignedInteger k = 0; k < dimension_; ++k)
        for (UnsignedInteger j = 0; j < dimension_; ++j)
          covariance(jBase + j, kBase + k) = covarianceCollection_[i](j, k);
    }
    return covariance;
  }
  // Here we have to project the given mesh on the underlying mesh
  // We try to call the getNearestVertexIndex() method a minimum number
  // of time as it is the most costly part of the discretization
  Indices nearestIndex(verticesNumber);
  for (UnsignedInteger i = 0; i < verticesNumber; ++i)
  {
    nearestIndex[i] = p_mesh_->getNearestVertexIndex(mesh.getVertex(i));
    LOGINFO(OSS() << "The vertex " << i << " over " << verticesNumber - 1 << " in the given mesh corresponds to the vertex " << nearestIndex[i] << " in the underlying mesh (" << mesh.getVertex(i).__str__() << "->" << p_mesh_->getVertex(nearestIndex[i]).__str__() << ")");
  }
  // Now, we use a set of loops similar to the default algorithm
  // Fill-in the matrix by blocks
  for (UnsignedInteger rowIndex = 0; rowIndex < verticesNumber; ++rowIndex)
  {
    const UnsignedInteger rowBase(rowIndex * dimension_);
    // Only the lower part has to be filled-in
    for (UnsignedInteger columnIndex = 0; columnIndex <= rowIndex; ++columnIndex)
    {
      const UnsignedInteger columnBase(columnIndex * dimension_);
      const CovarianceMatrix localCovarianceMatrix(operator()(nearestIndex[rowIndex], nearestIndex[columnIndex]));
      // We fill the covariance matrix using the previous local one
      // The full local covariance matrix has to be copied as it is
      // not copied on a symmetric position
      for (UnsignedInteger rowIndexLocal = 0; rowIndexLocal < dimension_; ++rowIndexLocal)
      {
        for (UnsignedInteger columnIndexLocal = 0; columnIndexLocal < dimension_; ++columnIndexLocal)
        {
          covariance(rowBase + rowIndexLocal, columnBase + columnIndexLocal) = localCovarianceMatrix(rowIndexLocal, columnIndexLocal) ;
        } // column index within the block
      } // row index within the block
    } // column index of the block
  } // row index of the block
  return covariance;
}

NumericalSample UserDefinedCovarianceModel::discretizeRow(const NumericalSample & vertices,
    const UnsignedInteger p) const
{
  if (dimension_ != 1) throw InternalException(HERE) << "Error: the discretizeRow() method is not defined if the output dimension is not 1. Here, dimension=" << dimension_;
  const UnsignedInteger size(vertices.getSize());
  NumericalSample result(size, 1);
  if (vertices == p_mesh_->getVertices())
  {
    UnsignedInteger index((p * (p + 1)) / 2);
    for (UnsignedInteger i = 0; i < p; ++i)
    {
      result[i][0] = covarianceCollection_[index](0, 0);
      ++index;
    }
    UnsignedInteger shift(p);
    for (UnsignedInteger i = p; i < size; ++i)
    {
      result[i][0] = covarianceCollection_[index](0, 0);
      ++shift;
      index += shift;
    }
    return result;
  }
  Indices nearestIndex(size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    nearestIndex[i] = p_mesh_->getNearestVertexIndex(vertices[i]);
    LOGINFO(OSS() << "The vertex " << i << " over " << size - 1 << " in the given sample corresponds to the vertex " << nearestIndex[i] << " in the underlying mesh (" << NumericalPoint(vertices[i]).__str__() << "->" << p_mesh_->getVertex(nearestIndex[i]).__str__() << ")");
  }
  for (UnsignedInteger i = 0; i < size; ++i) result[i][0] = operator()(nearestIndex[p], nearestIndex[i])(0, 0);
  return result;
}

/* Mesh accessor */
Mesh UserDefinedCovarianceModel::getMesh() const
{
  return *p_mesh_;
}

/* Time grid accessor */
RegularGrid UserDefinedCovarianceModel::getTimeGrid() const
{
  return RegularGrid(*p_mesh_);
}

/* String converter */
String UserDefinedCovarianceModel::__repr__() const
{
  OSS oss(true);
  oss << "class=" << UserDefinedCovarianceModel::GetClassName()
      << " mesh=" << p_mesh_->__repr__()
      << " covarianceCollection=" << covarianceCollection_;
  return oss;

}

/* String converter */
String UserDefinedCovarianceModel::__str__(const String & offset) const
{
  return __repr__();
}

/* Method save() stores the object through the StorageManager */
void UserDefinedCovarianceModel::save(Advocate & adv) const
{
  CovarianceModelImplementation::save(adv);
  adv.saveAttribute( "mesh_", *p_mesh_);
  adv.saveAttribute( "covarianceCollection_", covarianceCollection_);
}

/* Method load() reloads the object from the StorageManager */
void UserDefinedCovarianceModel::load(Advocate & adv)
{
  TypedInterfaceObject<Mesh> mesh;
  CovarianceModelImplementation::load(adv);
  adv.loadAttribute( "mesh_", mesh);
  p_mesh_ = mesh.getImplementation();
  adv.loadAttribute( "covarianceCollection_", covarianceCollection_);
}

END_NAMESPACE_OPENTURNS
