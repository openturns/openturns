//                                               -*- C++ -*-
/**
 *  @brief SVD-decomposition based LS solver
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

#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/SVDMethod.hxx"

BEGIN_NAMESPACE_OPENTURNS


CLASSNAMEINIT(SVDMethod)


static const Factory<SVDMethod> Factory_SVDMethod;


/* Default constructor */
SVDMethod::SVDMethod()
  : LeastSquaresMethodImplementation()
{
  // Nothing to do
}


/* Default constructor */
SVDMethod::SVDMethod(const DesignProxy & proxy,
                     const Point & weight,
                     const Indices & indices)
  : LeastSquaresMethodImplementation(proxy, weight, indices)
{
  // Nothing to do
}


/* Default constructor */
SVDMethod::SVDMethod(const DesignProxy & proxy,
                     const Indices & indices)
  : LeastSquaresMethodImplementation(proxy, indices)
{
  // Nothing to do
}


/* Default constructor */
SVDMethod::SVDMethod(const Matrix & matrix)
  : LeastSquaresMethodImplementation(matrix)
{
  // Nothing to do
}


/* Virtual constructor */
SVDMethod * SVDMethod::clone() const
{
  return new SVDMethod( *this );
}


/* String converter */
String SVDMethod::__repr__() const
{
  return OSS() << "class=" << GetClassName();
}


void SVDMethod::trashDecomposition()
{
  singularValues_.clear();
  u_ = Matrix();
  vT_ = SquareMatrix();
}


void SVDMethod::update(const Indices& addedIndices,
                       const Indices& conservedIndices,
                       const Indices& removedIndices,
                       const Bool row)
{
  // Update the SVD according to the row/column modifications of the design matrix.
  // It should be done in an incremental way to improve speed
  // If something has changed since last call
  if ((addedIndices.getSize() > 0) || (removedIndices.getSize() > 0) || (conservedIndices != currentIndices_) || (singularValues_.getSize() == 0))
  {
    // If it is a row modification, update the row filter
    if (!row)
    {
      currentIndices_ = conservedIndices;
      currentIndices_.add(addedIndices);
    } // column modification

    Matrix psiAk(computeWeightedDesign());
    // Here we compute an economic SVD
    // thus in case of least squares (nr > nc)
    // the U is not orthogonal
    singularValues_ = psiAk.computeSVD(u_, vT_, false, false); // fullSVD, keepIntact

    // check eigenvalues
    const UnsignedInteger svdSize = singularValues_.getSize();
    for (UnsignedInteger i = 0; i < svdSize; ++ i)
      if (!(singularValues_[i] > 0.0)) throw InvalidArgumentException(HERE) << "Error: null eigenvalue in svd decomposition";

  } // Something has changed
}


Point SVDMethod::solve(const Point & rhs)
{
  // This call insures that the decomposition has already been computed.
  // No cost if it is up to date.
  update(Indices(0), currentIndices_, Indices(0));
  // To take into account possible under-determined least-squares problems
  // We consider the number of singular values instead of the basis size
  // It leads to the minimal norm solution in the under-determined case
  const UnsignedInteger svdSize = singularValues_.getSize();

  // First step
  Point b(rhs);
  if (!hasUniformWeight_)
  {
    const UnsignedInteger size = rhs.getSize();
    for (UnsignedInteger i = 0; i < size; ++i) b[i] *= weightSqrt_[i];
  }
  const Point c(u_.getImplementation()->genVectProd(b, true));
  // Second step
  Point d(svdSize);
  for (UnsignedInteger i = 0; i < svdSize; ++i) d[i] = c[i] / singularValues_[i];
  // Third step
  return vT_.getImplementation()->genVectProd(d, true);
}


Point SVDMethod::solveNormal(const Point & rhs)
{
  update(Indices(0), currentIndices_, Indices(0));
  const UnsignedInteger basisSize = currentIndices_.getSize();

  Point b(rhs);
  if (!hasUniformWeight_)
  {
    const UnsignedInteger size = rhs.getSize();
    for (UnsignedInteger i = 0; i < size; ++i) b[i] *= weight_[i];
  }
  // G^-1= V*S^-2*V^T
  Point coefficients(vT_ * b);
  for (UnsignedInteger i = 0; i < basisSize; ++i)
  {
    const Scalar sv = singularValues_[i];
    coefficients[i] /= (sv * sv);
  }
  return vT_.getImplementation()->genVectProd(coefficients, true);
}


CovarianceMatrix SVDMethod::getGramInverse() const
{
  // G^{-1}=V\Sigma^{-2}V^T
  //       = V\Sigma^{-1}\Sigma^{-1} V^T
  //       = V \Sigma^{-1}\Sigma^{-T} V^T
  //       = (\Sigma^{-T} V^T)^T \Sigma^{-T} V^T
  // Note that (\Sigma^{-T} V^T)_{i,j} = (V^T)_{i,j} / sigma_[i]
  const UnsignedInteger m = vT_.getNbRows();
  const UnsignedInteger n = vT_.getNbColumns();
  const MatrixImplementation & vTimpl(*vT_.getImplementation());
  MatrixImplementation sigmaInvVt(m, n);
  UnsignedInteger index = 0;
  for (UnsignedInteger j = 0; j < n; ++j)
    for (UnsignedInteger i = 0; i < m; ++i)
    {
      sigmaInvVt[index] = (1.0 / singularValues_[i]) * vTimpl[index];
      ++ index;
    }
  return sigmaInvVt.computeGram();
}

Scalar SVDMethod::getGramInverseTrace() const
{
  Scalar traceInverse = 0.0;
  for (UnsignedInteger k = 0; k < singularValues_.getDimension(); ++ k)
  {
    traceInverse += 1.0 / singularValues_[k] / singularValues_[k];
  }
  return traceInverse;
}

SymmetricMatrix SVDMethod::getH() const
{
  // H = Psi.G^{-1}.Psi^T
  return u_.getImplementation()->genProd(*u_.getImplementation(), false, true);
}

Point SVDMethod::getHDiag() const
{
  // H is the Hat matrix corresponding to
  // H = Psi.G^{-1}.Psi^T
  // Considering :
  // G^{-1}=V\Sigma^{-2}V^T
  // H = Psi.V\Sigma^{-2}V^T.Psi^T
  // Psi = U1 \Sigma V --> U1 is the reduced factor of U
  // so that U1 * U1.transpose() != Identity
  // H = U1 * U1.transpose()
  const UnsignedInteger sampleSize = u_.getNbRows();
  const UnsignedInteger basisSize = currentIndices_.getSize();
  Point h(sampleSize);
  // matrices are stored by columns
  MatrixImplementation::const_iterator u_iterator(u_.getImplementation()->begin());
  for (UnsignedInteger j = 0; j < basisSize; ++ j)
  {
    for (MatrixImplementation::iterator h_iterator = h.begin(); h_iterator != h.end(); ++ h_iterator)
    {
      *h_iterator += (*u_iterator) * (*u_iterator);
      ++ u_iterator;
    }
  }
  return h;
}

Point SVDMethod::getGramInverseDiag() const
{
  // G^{-1}=V\Sigma^{-2}V^T
  const UnsignedInteger m = vT_.getNbRows();
  const UnsignedInteger n = vT_.getNbColumns();
  const MatrixImplementation & vTimpl(*vT_.getImplementation());
  UnsignedInteger index = 0;
  const UnsignedInteger svdSize = singularValues_.getSize();
  Point diag(n);
  for (UnsignedInteger j = 0; j < n; ++j)
  {
    Scalar diag_value = 0.0;
    for (UnsignedInteger i = 0; i < svdSize; ++i)
    {
      const Scalar val = vTimpl[index] / singularValues_[i];
      diag_value += val * val;
      ++ index;
    }
    diag[j] = diag_value;
    index += (m - svdSize);
  }

  return diag;
}


/* Method save() stores the object through the StorageManager */
void SVDMethod::save(Advocate & adv) const
{
  LeastSquaresMethodImplementation::save(adv);
}


/* Method load() reloads the object from the StorageManager */
void SVDMethod::load(Advocate & adv)
{
  LeastSquaresMethodImplementation::load(adv);
}


END_NAMESPACE_OPENTURNS
