//                                               -*- C++ -*-
/**
 *  @file  HMatrixFactory.cxx
 *  @brief This file supplies support for HMat
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
 */
#include "openturns/HMatrixFactory.hxx"
#include "openturns/HMatrix.hxx"
#include "openturns/HMatrixImplementation.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Log.hxx"

#ifdef OPENTURNS_HAVE_HMAT
#include <hmat/config.h>
#ifdef HMAT_HAVE_STARPU
# include <hmat/hmat_parallel.h>
#else
# include <hmat/hmat.h>
#endif
#endif  /* OPENTURNS_HAVE_HMAT */

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(HMatrixFactory)

/* Default constructor */
HMatrixFactory::HMatrixFactory()
  : PersistentObject()
{
  // Nothing to do
}

HMatrixFactory * HMatrixFactory::clone() const
{
  return new HMatrixFactory( *this );
}


/** Tell whether HMat support is available */
Bool
HMatrixFactory::IsAvailable()
{
#ifdef OPENTURNS_HAVE_HMAT
  return true;
#else
  return false;
#endif
}


HMatrix
HMatrixFactory::build(const Sample & sample, UnsignedInteger outputDimension, Bool symmetric, const HMatrixParameters & parameters)
{
#ifndef OPENTURNS_HAVE_HMAT
  throw NotYetImplementedException(HERE) << "OpenTURNS has been built without HMat support";
#else
  hmat_interface_t *hmatInterface = (hmat_interface_t*) calloc(1, sizeof(hmat_interface_t));
  hmat_settings_t settings;

#ifdef HMAT_HAVE_STARPU
  if (!ResourceMap::GetAsBool("HMatrix-ForceSequential"))
    hmat_init_starpu_interface(hmatInterface, HMAT_DOUBLE_PRECISION);
  else
#endif
    hmat_init_default_interface(hmatInterface, HMAT_DOUBLE_PRECISION);

  hmat_get_parameters(&settings);
  settings.compressionMethod = parameters.getCompressionMethodAsUnsignedInteger();
  settings.assemblyEpsilon = parameters.getAssemblyEpsilon();
  settings.recompressionEpsilon = parameters.getRecompressionEpsilon();
  settings.maxLeafSize = ResourceMap::GetAsUnsignedInteger("HMatrix-MaxLeafSize");

  settings.validationErrorThreshold = ResourceMap::GetAsScalar("HMatrix-ValidationError");
  settings.validateCompression = settings.validationErrorThreshold > 0;
  settings.validationReRun = ResourceMap::GetAsUnsignedInteger("HMatrix-ValidationRerun");
  settings.validationDump = ResourceMap::GetAsUnsignedInteger("HMatrix-ValidationDump");

  hmat_set_parameters(&settings);

  if (0 != hmatInterface->init())
  {
    throw NotYetImplementedException(HERE) << "Unable to initialize HMat library";
  }

  const UnsignedInteger size = sample.getSize();
  const UnsignedInteger inputDimension = sample.getDimension();
  double* points = new double[inputDimension * outputDimension * size];
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    for (UnsignedInteger j = 0; j < outputDimension; ++j)
      std::copy(&sample(i, 0), &sample(i, 0) + inputDimension, points + i * inputDimension * outputDimension + j * inputDimension);
  }

  hmat_clustering_algorithm_t* algo;
  const String clusteringAlgorithm = parameters.getClusteringAlgorithm();
  if (clusteringAlgorithm == "median")
    algo = hmat_create_clustering_median();
  else if (clusteringAlgorithm == "geometric")
    algo = hmat_create_clustering_geometric();
  else if (clusteringAlgorithm == "hybrid")
    algo = hmat_create_clustering_hybrid();
  else
    throw InvalidArgumentException(HERE) << "Unknown clustering method: " << clusteringAlgorithm << ", valid choices are: median, geometric or hybrid";

  hmat_cluster_tree_t* ct = hmat_create_cluster_tree(points, inputDimension, outputDimension * size, algo);
  hmat_delete_clustering(algo);
  delete[] points;

  Scalar eta = parameters.getAdmissibilityFactor();
  hmat_admissibility_t* admissibility = hmat_create_admissibility_standard(eta);
  hmat_matrix_t* ptrHMat = hmatInterface->create_empty_hmatrix_admissibility(ct, ct, symmetric, admissibility);
  hmat_delete_admissibility(admissibility);
  return HMatrix(new HMatrixImplementation(hmatInterface, ct, outputDimension * size, ptrHMat));
#endif /* OPENTURNS_HAVE_HMAT */
}

String HMatrixFactory::__repr__() const
{
  return OSS() << "class=" << getClassName();
}

/* Method save() stores the object through the StorageManager */
void HMatrixFactory::save(Advocate & adv) const
{
  PersistentObject::save(adv);
}


/* Method load() reloads the object from the StorageManager */
void HMatrixFactory::load(Advocate & adv)
{
  PersistentObject::load(adv);
}


END_NAMESPACE_OPENTURNS
