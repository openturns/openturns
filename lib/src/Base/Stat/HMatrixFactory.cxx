//                                               -*- C++ -*-
/**
 *  @file  HMatrixFactory.cxx
 *  @brief This file supplies support for HMat
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
 */
#include "HMatrixFactory.hxx"
#include "HMatrix.hxx"
#include "HMatrixImplementation.hxx"
#include "NumericalSample.hxx"
#include "Log.hxx"

#ifdef OPENTURNS_HAVE_HMAT
#include <hmat/config.h>
#ifdef HMAT_HAVE_STARPU
# include <hmat/hmat_parallel.h>
#else
# include <hmat/hmat.h>
#endif
#endif  /* OPENTURNS_HAVE_HMAT */

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(HMatrixFactory);

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
HMatrixFactory::build(const NumericalSample & sample, UnsignedInteger outputDimension, Bool symmetric)
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
  settings.compressionMethod = ResourceMap::GetAsUnsignedInteger("HMatrix-CompressionMethod");
  settings.assemblyEpsilon = ResourceMap::GetAsNumericalScalar("HMatrix-AssemblyEpsilon");
  settings.recompressionEpsilon = ResourceMap::GetAsNumericalScalar("HMatrix-RecompressionEpsilon");
  settings.maxLeafSize = ResourceMap::GetAsUnsignedInteger("HMatrix-MaxLeafSize");
  settings.maxParallelLeaves = ResourceMap::GetAsUnsignedInteger("HMatrix-MaxParallelLeaves");

  settings.validationErrorThreshold = ResourceMap::GetAsNumericalScalar("HMatrix-ValidationError");
  settings.validateCompression = settings.validationErrorThreshold > 0;
  settings.validationReRun = ResourceMap::GetAsUnsignedInteger("HMatrix-ValidationRerun");
  settings.validationDump = ResourceMap::GetAsUnsignedInteger("HMatrix-ValidationDump");

  hmat_set_parameters(&settings);

  if (0 != hmatInterface->init())
  {
    throw NotYetImplementedException(HERE) << "Unable to initialize HMat library";
  }

  const UnsignedInteger size(sample.getSize());
  const UnsignedInteger spatialDimension(sample.getDimension());
  double* points = new double[spatialDimension * outputDimension * size];
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    for (UnsignedInteger j = 0; j < outputDimension; ++j)
      memcpy(points + i * spatialDimension * outputDimension + j * spatialDimension, &sample[i][0], spatialDimension * sizeof(double));
  }
  hmat_clustering_algorithm_t* algo = hmat_create_clustering_median();
  hmat_cluster_tree_t* ct = hmat_create_cluster_tree(points, spatialDimension, outputDimension * size, algo);
  hmat_delete_clustering(algo);
  delete[] points;
  hmat_matrix_t* ptrHMat = hmatInterface->create_empty_hmatrix(ct, ct, symmetric);
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
