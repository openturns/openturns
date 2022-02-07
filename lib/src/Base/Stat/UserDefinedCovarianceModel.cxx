//                                               -*- C++ -*-
/**
 * @brief This class builds a spectral model using a frequency grid and a spectral function
 *  given as a collection of Covariance Matrix
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/UserDefinedCovarianceModel.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/NearestNeighbourAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(UserDefinedCovarianceModel)

static const Factory<UserDefinedCovarianceModel> Factory_UserDefinedCovarianceModel;

/* Constructor with parameters */
UserDefinedCovarianceModel::UserDefinedCovarianceModel()
  : CovarianceModelImplementation()
  , covariance_(0)
  , p_mesh_(new RegularGrid())
  , nearestNeighbour_()
{
  outputDimension_ = 0;
}

// For a non stationary model, we need N x (N+1)/2 covariance matrices with N the number of vertices in the mesh
UserDefinedCovarianceModel::UserDefinedCovarianceModel(const Mesh & mesh,
    const CovarianceMatrix & covariance)
  : CovarianceModelImplementation()
  , covariance_(0)
  , p_mesh_(0)
  , nearestNeighbour_()
{
  const UnsignedInteger N = mesh.getVerticesNumber();
  const UnsignedInteger fullDimension = covariance.getDimension();
  if (fullDimension % N != 0) throw InvalidArgumentException(HERE) << "Error: the given covariance has a dimension=" << fullDimension << " which is not a multiple of the vertices number=" << N;
  outputDimension_ = fullDimension / N;
  covariance_ = covariance;
  p_mesh_ = mesh.clone();
  nearestNeighbour_.setSample(p_mesh_->getVertices());
  inputDimension_ = mesh.getDimension();
}

/* Virtual constructor */
UserDefinedCovarianceModel * UserDefinedCovarianceModel::clone() const
{
  return new UserDefinedCovarianceModel(*this);
}

Scalar UserDefinedCovarianceModel::computeAsScalar(const Point &s,
    const Point &t) const
{
  if (s.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;
  if (outputDimension_ > 1)
    throw InvalidArgumentException(HERE) << "Error : UserDefinedCovarianceModel::computeAsScalar(s, t) should be only used if output dimension is 1. Here, output dimension = " << outputDimension_;

  const UnsignedInteger N = p_mesh_->getVerticesNumber();
  if (N == 1)
    return covariance_(0, 0);

  const UnsignedInteger i = nearestNeighbour_.query(s);
  const UnsignedInteger j = nearestNeighbour_.query(t);
  SquareMatrix result(outputDimension_);
  return covariance_(i, j);
}

Scalar UserDefinedCovarianceModel::computeAsScalar(const Collection<Scalar>::const_iterator &s_begin,
    const Collection<Scalar>::const_iterator &t_begin) const
{
  if (outputDimension_ != 1)
    throw InvalidArgumentException(HERE) << "Error : UserDefinedCovarianceModel::computeAsScalar(it, it) should be only used if output dimension is 1. Here, output dimension = " << outputDimension_;

  Collection<Scalar>::const_iterator s_it = s_begin;
  Collection<Scalar>::const_iterator t_it = t_begin;
  Point s(inputDimension_);
  Point t(inputDimension_);
  for (UnsignedInteger i = 0; i < inputDimension_; ++i, ++s_it, ++t_it)
  {
    s[i] = *s_it;
    t[i] = *t_it;
  }
  return computeAsScalar(s, t);
}

/* Computation of the covariance density function */
SquareMatrix UserDefinedCovarianceModel::operator()(const Point &s,
    const Point &t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;
  // If the grid size is one, return the covariance function
  // else find in the grid the nearest instant values
  const UnsignedInteger N = p_mesh_->getVerticesNumber();
  if (N == 1) return covariance_;

  // Use the evaluation based on indices
  return operator()(nearestNeighbour_.query(s), nearestNeighbour_.query(t));
}

SquareMatrix UserDefinedCovarianceModel::operator() (const UnsignedInteger i,
    const UnsignedInteger j) const
{
  const UnsignedInteger sShift = i * outputDimension_;
  const UnsignedInteger tShift = j * outputDimension_;
  SquareMatrix result(outputDimension_);
  for (UnsignedInteger k = 0; k < outputDimension_; ++k)
    for (UnsignedInteger l = 0; l < outputDimension_; ++l)
      result(l, k) = covariance_(sShift + l, tShift + k);
  return result;
}

CovarianceMatrix UserDefinedCovarianceModel::discretize(const Sample & vertices) const
{
  // It is better to check vertices as the simplices don't play a role in the discretization
  if (vertices == p_mesh_->getVertices())
  {
    // Here we know that the given mesh is exactly the one defining the covariance model
    return covariance_;
  }
  const UnsignedInteger size = vertices.getSize();
  CovarianceMatrix covariance(size * outputDimension_);
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
      const SquareMatrix localCovarianceMatrix(operator()(nearestIndex[rowIndex], nearestIndex[columnIndex]));
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
  SampleImplementation result(size, 1);
  if (vertices == p_mesh_->getVertices())
  {
    result.setData(*covariance_.getRow(p).getImplementation());
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
  const TriangularMatrix choleskyFactor = covariance.computeCholesky();
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
      << " covariance=" << covariance_;
  return oss;

}

/* String converter */
String UserDefinedCovarianceModel::__str__(const String & ) const
{
  return __repr__();
}

/* Method save() stores the object through the StorageManager */
void UserDefinedCovarianceModel::save(Advocate & adv) const
{
  CovarianceModelImplementation::save(adv);
  adv.saveAttribute( "mesh_", *p_mesh_);
  adv.saveAttribute("nearestNeighbour_", nearestNeighbour_);
  adv.saveAttribute( "covariance_", covariance_);
}

/* Method load() reloads the object from the StorageManager */
void UserDefinedCovarianceModel::load(Advocate & adv)
{
  TypedInterfaceObject<Mesh> mesh;
  CovarianceModelImplementation::load(adv);
  adv.loadAttribute( "mesh_", mesh);
  p_mesh_ = mesh.getImplementation();
  adv.loadAttribute( "nearestNeighbour_", nearestNeighbour_);
  adv.loadAttribute( "covariance_", covariance_);
}

END_NAMESPACE_OPENTURNS
