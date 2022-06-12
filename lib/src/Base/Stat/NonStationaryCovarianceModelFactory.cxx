//                                               -*- C++ -*-
/**
 *  @brief Top-level class for all spectral model factories
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/NonStationaryCovarianceModelFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Collection.hxx"
#include "openturns/CovarianceMatrix.hxx"
#include "openturns/Point.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(NonStationaryCovarianceModelFactory)
static const Factory<NonStationaryCovarianceModelFactory> Factory_NonStationaryCovarianceModelFactory;

typedef Collection<CovarianceMatrix>  CovarianceMatrixCollection;

/* Default constructor */
NonStationaryCovarianceModelFactory::NonStationaryCovarianceModelFactory()
  : CovarianceModelFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
NonStationaryCovarianceModelFactory * NonStationaryCovarianceModelFactory::clone() const
{
  return new NonStationaryCovarianceModelFactory(*this);
}

/* String converter */
String NonStationaryCovarianceModelFactory::__repr__() const
{
  OSS oss(true);
  oss << "class=" << NonStationaryCovarianceModelFactory::GetClassName();
  return oss;
}

/* String converter */
String NonStationaryCovarianceModelFactory::__str__(const String & ) const
{
  return __repr__();
}


CovarianceModel NonStationaryCovarianceModelFactory::build(const ProcessSample & sample) const
{
  return buildAsUserDefinedCovarianceModel(sample, false).clone();
}



CovarianceModel NonStationaryCovarianceModelFactory::build(const ProcessSample & sample, const Bool isCentered) const
{
  return buildAsUserDefinedCovarianceModel(sample, isCentered).clone();
}


UserDefinedCovarianceModel NonStationaryCovarianceModelFactory::buildAsUserDefinedCovarianceModel(const ProcessSample & sample, const Bool isCentered) const
{
  return UserDefinedCovarianceModel(sample.getMesh(), buildAsCovarianceMatrix(sample, isCentered));
}

CovarianceMatrix NonStationaryCovarianceModelFactory::buildAsCovarianceMatrix(const ProcessSample & sample, const Bool isCentered) const
{
  const Mesh & mesh(sample.getMesh());
  const UnsignedInteger N = mesh.getVerticesNumber();
  const UnsignedInteger dimension = sample.getDimension();
  const UnsignedInteger fullDimension = N * dimension;
  // Create a collection of null CovarianceMatrix
  const UnsignedInteger size = sample.getSize();
  if (size == 0) return CovarianceMatrix(fullDimension);
  // Build the design matrix
  MatrixImplementation designMatrix(fullDimension, size);
  MatrixImplementation::iterator designBegin = designMatrix.begin();
  if (isCentered)
  {
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      std::copy(sample[i].data(), sample[i].data() + fullDimension, designBegin);
      designBegin += fullDimension;
    }
  } // isCentered
  else
  {
    const Point mean(sample.computeMean().getValues().getImplementation()->getData());
    Point point(fullDimension);
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      std::copy(sample[i].data(), sample[i].data() + fullDimension, &point[0]);
      point -= mean;
      std::copy(point.begin(), point.end(), designBegin);
      designBegin += fullDimension;
    }
  } // !isCentered
  MatrixImplementation gram(designMatrix.computeGram(false));
  const CovarianceMatrix result(gram / (isCentered ? size : size - 1.0));
  return result;
}

/* Method save() stores the object through the StorageManager */
void NonStationaryCovarianceModelFactory::save(Advocate & adv) const
{
  CovarianceModelFactoryImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void NonStationaryCovarianceModelFactory::load(Advocate & adv)
{
  CovarianceModelFactoryImplementation::load(adv);
}
END_NAMESPACE_OPENTURNS
