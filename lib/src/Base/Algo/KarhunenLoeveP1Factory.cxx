//                                               -*- C++ -*-
/**
 *  @brief This class implements the computation of the Karhunen-Loeve
 *         basis and eigenvalues of a given covariance model based on
 *         P1 Lagrange approximation.
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
 */
#include "KarhunenLoeveP1Factory.hxx"
#include "Exception.hxx"
#include "SquareMatrix.hxx"
#include "SquareComplexMatrix.hxx"
#include "NumericalSample.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KarhunenLoeveP1Factory
 */

CLASSNAMEINIT(KarhunenLoeveP1Factory);

static const Factory<KarhunenLoeveP1Factory> RegisteredFactory;

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
  // Compute the stiffness of the mesh
  stiffness_ = mesh.computeP1Stiffness();
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
    //    result.add(P1LagrangeEvaluationImplementation(resultAsProcessSample.getField(i)));
  return result;
}

ProcessSample KarhunenLoeveP1Factory::buildAsProcessSample(const CovarianceModel & covarianceModel,
                                                   NumericalPoint & selectedEV) const
{
  const UnsignedInteger numVertices(mesh_.getVerticesNumber());
  CovarianceMatrix C(covarianceModel.discretize(mesh_));
  SquareMatrix M((C * stiffness_).getImplementation());
  LOGINFO(OSS() << "M=\n" << M.__str__());
  SquareComplexMatrix eigenVectorsComplex;
  SquareMatrix::NumericalComplexCollection eigenValuesComplex(M.computeEV(eigenVectorsComplex, false));
  const UnsignedInteger nbColumns(eigenValuesComplex.getSize());
  NumericalSample eigenPairs(nbColumns, nbColumns + 1);
  for (UnsignedInteger i = 0; i < nbColumns; ++i)
    {
      for (UnsignedInteger j = 0; j < nbColumns; ++j) eigenPairs[i][j] = eigenVectorsComplex(j, i).real();
      eigenPairs[i][nbColumns] = -eigenValuesComplex[i].real();
    }
  eigenPairs = eigenPairs.sortAccordingToAComponent(nbColumns);
  SquareMatrix eigenVectors(nbColumns);
  NumericalPoint eigenValues(nbColumns);
  for (UnsignedInteger i = 0; i < nbColumns; ++i)
    {
      for (UnsignedInteger j = 0; j < nbColumns; ++j) eigenVectors(i, j) = eigenPairs[j][i];
      eigenValues[i] = -eigenPairs[i][nbColumns];
    }
  selectedEV = NumericalPoint(0);
  ProcessSample result(mesh_, 0, covarianceModel.getDimension());
  UnsignedInteger j(0);
  while ((j < eigenValues.getSize()) && (eigenValues[j] > threshold_ * std::abs(eigenValues[0])))
    {
      selectedEV.add(eigenValues[j]);
      NumericalSample values(numVertices, 1);
      const Matrix a(eigenVectors.getColumn(j));
      const NumericalScalar norm(std::sqrt((a.transpose() * (stiffness_ * a))(0, 0)));
      const NumericalScalar factor(eigenVectors(0, j) < 0.0 ? -1.0 / norm : 1.0 / norm);
      for (UnsignedInteger i = 0; i < numVertices; ++i)
        values[i][0] = eigenVectors(i, j) * factor;
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
