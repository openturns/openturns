//                                               -*- C++ -*-
/**
 *  @brief This class implements the computation of the Karhunen-Loeve
 *         basis and eigenvalues of a given covariance model based on
 *         P1 Lagrange approximation.
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/KarhunenLoeveP1Factory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/SquareComplexMatrix.hxx"
#include "openturns/NumericalSample.hxx"
#include "openturns/P1LagrangeEvaluationImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KarhunenLoeveP1Factory
 */

CLASSNAMEINIT(KarhunenLoeveP1Factory);

static const Factory<KarhunenLoeveP1Factory> Factory_KarhunenLoeveP1Factory;

/* Constructor without parameters */
KarhunenLoeveP1Factory::KarhunenLoeveP1Factory()
  : PersistentObject()
  , mesh_()
  , threshold_(0.0)
{
  // Nothing to do
}

/* Constructor with parameters */
KarhunenLoeveP1Factory::KarhunenLoeveP1Factory(const Mesh & mesh,
                                               const NumericalScalar threshold)
  : PersistentObject()
  , mesh_(mesh)
  , threshold_(threshold)
{
  // Compute the gram of the mesh
  gram_ = mesh.computeP1Gram();
  const NumericalScalar epsilon = ResourceMap::GetAsNumericalScalar("KarhunenLoeveP1Factory-RegularizationFactor");
  if (epsilon > 0.0)
    for (UnsignedInteger i = 0; i < gram_.getDimension(); ++i) gram_(i, i) += epsilon;
}

/* Virtual constructor */
KarhunenLoeveP1Factory * KarhunenLoeveP1Factory::clone() const
{
  return new KarhunenLoeveP1Factory( *this );
}

/* Solve the Fredholm eigenvalues problem:
 * find (\phi_k, \lambda_k) such that
 * \int_{D} C(s,t)\phi_k(s)ds=\lambda_k\phi_k(t)
 * where C is a given covariance model, using P1 approximation
 */
Basis KarhunenLoeveP1Factory::build(const CovarianceModel & covarianceModel,
                                    NumericalPoint & selectedEV) const
{
  const ProcessSample resultAsProcessSample(buildAsProcessSample(covarianceModel, selectedEV));
  Basis result(0);
  for (UnsignedInteger i = 0; i < resultAsProcessSample.getSize(); ++i)
    result.add(P1LagrangeEvaluationImplementation(resultAsProcessSample.getField(i)));
  return result;
}

/* Here we discretize the following Fredholm problem:
   \int_{\Omega}C(s,t)\phi_n(s)ds=\lambda_n\phi_n(t)
   using a P1 approximation of C and \phi_n:
   C(s,t)=\sum_{i,j}C(s_i,s_j)\theta_i(s)\theta_j(t)
   \phi_n(t)=\sum_k\alpha_k^n\theta_k(t)
   where s_i,s_j are vertices of the mesh, C(s_i,s_j)\in\cS^+_d(\R), \alpha_k^n\in\R^d
   leading to:
   \forall t\in\Omega, \sum_{i,j}C(s_i,s_j)\theta_j(t)\int_{\Omega}\theta_i(s)\sum_k\alpha_k^n\theta_k(s)=\lambda_n\sum_l\alpha_k^l\theta_l(t)
   For each value of n we get d equations in \alpha^n. We write these equations for t=s_1,...,s_N the N vertices of the mesh:
   \sum_{i,j}C(s_i,s_j)\theta_j(s_m)\int_{\Omega}\theta_i(s)\sum_k\alpha_k^n\theta_k(s)=\lambda_n\sum_l\alpha_l^n\theta_l(s_m)
   ie:
   \sum_i C(s_i,s_m)\int_{\Omega}\theta_i(s)\sum_k\alpha_k^n\theta_k(s)=\lambda_n\alpha_m^n\theta_m(s_m)
   In a block-matrix form we get:
   [C(s_1,s_1) ... C(s_1,s_N)][K_11 ... K_1N][\alpha_1]             [\alpha_1]
   [    ...            ...   ][ ...      ...][   ...  ] = \lambda_n [   ...  ]
   [C(s_N,s_1) ... C(s_N,s_N)][K_N1 ... K_NN][\alpha_N]             [\alpha_N]
   Where:
   K_ij = \int_{\Omega}\theta_i(s)\theta_j(s)ds I
   with I the dxd identity matrix
*/
ProcessSample KarhunenLoeveP1Factory::buildAsProcessSample(const CovarianceModel & covarianceModel,
                                                           NumericalPoint & selectedEV) const
{
  const UnsignedInteger numVertices(mesh_.getVerticesNumber());
  // Extend the Gram matrix of the mesh
  const UnsignedInteger dimension(covarianceModel.getDimension());
  const UnsignedInteger augmentedDimension(dimension * numVertices);
  CovarianceMatrix G(augmentedDimension);
  for (UnsignedInteger i = 0; i < numVertices; ++i)
    {
      for (UnsignedInteger j = 0; j <= i; ++j)
        {
          const NumericalScalar gij(gram_(i, j));
          for (UnsignedInteger k = 0; k < dimension; ++k)
              G(i * dimension + k, j * dimension + k) = gij;
        } // Loop over j
    } // Loop over i
  // Discretize the covariance model
  CovarianceMatrix C(covarianceModel.discretize(mesh_));
  SquareMatrix M((C * G).getImplementation());
  SquareComplexMatrix eigenVectorsComplex;
  SquareMatrix::NumericalComplexCollection eigenValuesComplex(M.computeEV(eigenVectorsComplex, false));
  NumericalSample eigenPairs(augmentedDimension, augmentedDimension + 1);
  for (UnsignedInteger i = 0; i < augmentedDimension; ++i)
    {
      for (UnsignedInteger j = 0; j < augmentedDimension; ++j) eigenPairs[i][j] = eigenVectorsComplex(j, i).real();
      eigenPairs[i][augmentedDimension] = -eigenValuesComplex[i].real();
    }
  eigenPairs = eigenPairs.sortAccordingToAComponent(augmentedDimension);
  SquareMatrix eigenVectors(augmentedDimension);
  NumericalPoint eigenValues(augmentedDimension);
  for (UnsignedInteger i = 0; i < augmentedDimension; ++i)
    {
      for (UnsignedInteger j = 0; j < augmentedDimension; ++j) eigenVectors(i, j) = eigenPairs[j][i];
      eigenValues[i] = -eigenPairs[i][augmentedDimension];
    }
  LOGINFO(OSS(false) << "eigenVectors=\n" << eigenVectors << ", eigenValues=" << eigenValues);
  selectedEV = NumericalPoint(0);
  ProcessSample result(mesh_, 0, dimension);
  UnsignedInteger j(0);
  while ((j < augmentedDimension) && (eigenValues[j] > threshold_ * std::abs(eigenValues[0])))
    {
      selectedEV.add(eigenValues[j]);
      NumericalSample values(numVertices, dimension);
      const Matrix a(eigenVectors.getColumn(j));
      const NumericalScalar norm(std::sqrt((a.transpose() * (G * a))(0, 0)));
      const NumericalScalar factor(eigenVectors(0, j) < 0.0 ? -1.0 / norm : 1.0 / norm);
      for (UnsignedInteger i = 0; i < numVertices; ++i)
	for (UnsignedInteger k = 0; k < dimension; ++k)
	  values[i][k] = eigenVectors(i * dimension + k, j) * factor;
      result.add(values);
      ++j;
    }
  return result;
}

/* String converter */
String KarhunenLoeveP1Factory::__repr__() const
{
  OSS oss(true);
  oss << "class=" << KarhunenLoeveP1Factory::GetClassName()
      << ", mesh=" << mesh_
      << ", threshold=" << threshold_;
  return oss;
}

/* String converter */
String KarhunenLoeveP1Factory::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "class=" << KarhunenLoeveP1Factory::GetClassName()
      << ", mesh=" << mesh_
      << ", threshold=" << threshold_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void KarhunenLoeveP1Factory::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "mesh_", mesh_ );
  adv.saveAttribute( "threshold_", threshold_ );
}

/* Method load() reloads the object from the StorageManager */
void KarhunenLoeveP1Factory::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "mesh_", mesh_ );
  adv.loadAttribute( "threshold_", threshold_ );
}

END_NAMESPACE_OPENTURNS
