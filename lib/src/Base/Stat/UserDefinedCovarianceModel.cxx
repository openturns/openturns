//                                               -*- C++ -*-
/**
 * @brief This class builds a spectral model using a frequency grid and a spectral function
 *  given as a collection of Covariance Matrix
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/UserDefinedCovarianceModel.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/NearestNeighbourAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS

TEMPLATE_CLASSNAMEINIT(PersistentCollection< CovarianceMatrix >)

static const Factory<PersistentCollection<CovarianceMatrix> > Factory_PersistentCollection_CovarianceMatrix;

CLASSNAMEINIT(UserDefinedCovarianceModel)

static const Factory<UserDefinedCovarianceModel> Factory_UserDefinedCovarianceModel;

/* Constructor with parameters */
UserDefinedCovarianceModel::UserDefinedCovarianceModel()
  : CovarianceModelImplementation()
  , covarianceCollection_(0)
  , p_mesh_(RegularGrid().clone())
  , nearestNeighbour_()
{
  outputDimension_ = 0;
}

// For a non stationary model, we need N x N covariance functions with N the number of vertices in the mesh
UserDefinedCovarianceModel::UserDefinedCovarianceModel(const Mesh & mesh,
    const CovarianceMatrixCollection & covarianceFunction)
  : CovarianceModelImplementation()
  , covarianceCollection_(0)
  , p_mesh_(0)
  , nearestNeighbour_()
{
  const UnsignedInteger N = mesh.getVerticesNumber();
  const UnsignedInteger size = (N * (N + 1)) / 2;
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: the mesh is empty.";
  if (size != covarianceFunction.getSize())
    throw InvalidArgumentException(HERE) << "Error: for a non stationary covariance model, sizes are incoherent:"
                                         << " mesh size=" << N << " and covariance function size=" << covarianceFunction.getSize() << " instead of " << size;
  p_mesh_ = mesh.clone();
  nearestNeighbour_.setSample(p_mesh_->getVertices());
  inputDimension_ = mesh.getDimension();
  covarianceCollection_ = CovarianceMatrixCollection(size);
  // put the first element
  covarianceCollection_[0] = covarianceFunction[0];
  outputDimension_ = covarianceCollection_[0].getDimension();
  // put the next elements if dimension is ok
  for (UnsignedInteger k = 1; k < size; ++k)
  {
    if (covarianceFunction[k].getDimension() != outputDimension_)
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
CovarianceMatrix UserDefinedCovarianceModel::operator() (const Point & s,
    const Point & t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;
  // If the grid size is one, return the covariance function
  // else find in the grid the nearest instant values
  const UnsignedInteger N = p_mesh_->getVerticesNumber();
  if (N == 1) return covarianceCollection_[0];

  // Use the evaluation based on indices
  return operator()(nearestNeighbour_.query(s), nearestNeighbour_.query(t));
}

CovarianceMatrix UserDefinedCovarianceModel::operator() (const UnsignedInteger i,
    const UnsignedInteger j) const
{
  UnsignedInteger sIndex = i;
  UnsignedInteger tIndex = j;
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
  const SignedInteger index = tIndex + (sIndex * (sIndex + 1)) / 2;
  return covarianceCollection_[index];
}

CovarianceMatrix UserDefinedCovarianceModel::discretize(const Sample & vertices) const
{
  const UnsignedInteger size = vertices.getSize();
  CovarianceMatrix covariance(size * outputDimension_);
  // It is better to check vertices as the simplices don't play a role in the discretization
  if (vertices == p_mesh_->getVertices())
  {
    // Here we know that the given mesh is exactly the one defining the covariance model
    for (UnsignedInteger i = 0; i < covarianceCollection_.getSize(); ++i)
    {
      const UnsignedInteger jBase = static_cast< UnsignedInteger >(sqrt(2 * i + 0.25) - 0.5);
      const UnsignedInteger kBase = i - (jBase * (jBase + 1)) / 2;
      for (UnsignedInteger k = 0; k < outputDimension_; ++k)
        for (UnsignedInteger j = 0; j < outputDimension_; ++j)
          covariance(jBase + j, kBase + k) = covarianceCollection_[i](j, k);
    }
    return covariance;
  }
  // Here we have to project the given vertices on the underlying mesh
  // We try to call the query() method a minimum number of time as it
  // is the most costly part of the discretization
  const Indices nearestIndex(nearestNeighbour_.query(vertices));

  // Now, we use a set of loops similar to the default algorithm
  // Fill-in the matrix by blocks
  for (UnsignedInteger rowIndex = 0; rowIndex < size; ++rowIndex)
  {
    const UnsignedInteger rowBase = rowIndex * outputDimension_;
    // Only the lower part has to be filled-in
    for (UnsignedInteger columnIndex = 0; columnIndex <= rowIndex; ++columnIndex)
    {
      const UnsignedInteger columnBase = columnIndex * outputDimension_;
      const CovarianceMatrix localCovarianceMatrix(operator()(nearestIndex[rowIndex], nearestIndex[columnIndex]));
      // We fill the covariance matrix using the previous local one
      // The full local covariance matrix has to be copied as it is
      // not copied on a symmetric position
      for (UnsignedInteger rowIndexLocal = 0; rowIndexLocal < outputDimension_; ++rowIndexLocal)
      {
        for (UnsignedInteger columnIndexLocal = 0; columnIndexLocal < outputDimension_; ++columnIndexLocal)
        {
          covariance(rowBase + rowIndexLocal, columnBase + columnIndexLocal) = localCovarianceMatrix(rowIndexLocal, columnIndexLocal) ;
        } // column index within the block
      } // row index within the block
    } // column index of the block
  } // row index of the block
  return covariance;
}

Sample UserDefinedCovarianceModel::discretizeRow(const Sample & vertices,
    const UnsignedInteger p) const
{
  if (outputDimension_ != 1) throw InternalException(HERE) << "Error: the discretizeRow() method is not defined if the output dimension is not 1. Here, dimension=" << outputDimension_;
  const UnsignedInteger size = vertices.getSize();
  Sample result(size, 1);
  if (vertices == p_mesh_->getVertices())
  {
    UnsignedInteger index = (p * (p + 1)) / 2;
    for (UnsignedInteger i = 0; i < p; ++i)
    {
      result(i, 0) = covarianceCollection_[index](0, 0);
      ++index;
    }
    UnsignedInteger shift = p;
    for (UnsignedInteger i = p; i < size; ++i)
    {
      result(i, 0) = covarianceCollection_[index](0, 0);
      ++shift;
      index += shift;
    }
    return result;
  }
  const Indices nearestIndex(nearestNeighbour_.query(vertices));
  for (UnsignedInteger i = 0; i < size; ++i) result(i, 0) = operator()(nearestIndex[p], nearestIndex[i])(0, 0);
  return result;
}

TriangularMatrix UserDefinedCovarianceModel::discretizeAndFactorize(const Sample & vertices) const
{
  // We suppose that covariance matrix is symmetric positive definite
  CovarianceMatrix covariance = discretize(vertices);
  TriangularMatrix choleskyFactor = covariance.computeCholesky();
  return choleskyFactor;
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
  adv.saveAttribute("nearestNeighbour_", nearestNeighbour_);
  adv.saveAttribute( "covarianceCollection_", covarianceCollection_);
}

/* Method load() reloads the object from the StorageManager */
void UserDefinedCovarianceModel::load(Advocate & adv)
{
  TypedInterfaceObject<Mesh> mesh;
  CovarianceModelImplementation::load(adv);
  adv.loadAttribute( "mesh_", mesh);
  p_mesh_ = mesh.getImplementation();
  adv.loadAttribute( "nearestNeighbour_", nearestNeighbour_);
  adv.loadAttribute( "covarianceCollection_", covarianceCollection_);
}

END_NAMESPACE_OPENTURNS
