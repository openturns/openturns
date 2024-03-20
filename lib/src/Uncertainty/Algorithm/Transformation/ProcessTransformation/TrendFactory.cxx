//                                               -*- C++ -*-
/**
 *  @brief
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
#include "openturns/TrendFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/LeastSquaresMetaModelSelection.hxx"
#include "openturns/Point.hxx"
#include "openturns/DualLinearCombinationFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class TrendFactory
 */

CLASSNAMEINIT(TrendFactory)

// static const Factory<TrendFactory> Factory_TrendFactory;

/* Constructor with parameters */
TrendFactory::TrendFactory(const BasisSequenceFactory & basisSequenceFactory,
                           const FittingAlgorithm & fittingAlgorithm)
  : PersistentObject(),
    basisSequenceFactory_(basisSequenceFactory),
    fittingAlgorithm_(fittingAlgorithm)
{
  // Nothing to do
}

/* Virtual constructor */
TrendFactory * TrendFactory::clone() const
{
  return new TrendFactory(*this);
}

/* BasisSequenceFactory get accessor */
BasisSequenceFactory TrendFactory::getBasisSequenceFactory() const
{
  return basisSequenceFactory_;
}

/* BasisSequenceFactory set accessor */
void TrendFactory::setBasisSequenceFactory(const BasisSequenceFactory & basisSequenceFactory)
{
  basisSequenceFactory_ = basisSequenceFactory;
}

/* FittingAlgorithm get accessor */
FittingAlgorithm TrendFactory::getFittingAlgorithm() const
{
  return fittingAlgorithm_;
}

/* FittingAlgorithm set accessor */
void TrendFactory::setFittingAlgorithm(const FittingAlgorithm & fittingAlgorithm)
{
  fittingAlgorithm_ = fittingAlgorithm;
}

/* Build the factory from data */
TrendTransform TrendFactory::build(const Field & field,
                                   const Basis & basis) const
{
  // size of the collection
  const UnsignedInteger N = basis.getSize();

  // Dimension and size of the time series
  const UnsignedInteger dimension = field.getOutputDimension();
  Sample inputSample(field.getMesh().getVertices());

  // We need a sample to get coefficients result
  Sample coefficients(N, dimension);

  Collection<Function> coll(N);
  Indices indices(N);
  indices.fill();
  for (UnsignedInteger k = 0; k < N ; ++k) coll[k] = basis[k];

  for (UnsignedInteger d = 0; d < dimension; ++d)
  {
    // We look for best coefficients by marginal
    const Sample outputSample(field.getValues().getMarginal(d));
    LeastSquaresMetaModelSelection selectionAlgo(inputSample, outputSample, coll, indices, basisSequenceFactory_, fittingAlgorithm_);
    selectionAlgo.run();
    Point localCoefficients(selectionAlgo.getCoefficients());
    for (UnsignedInteger k = 0; k < N ; ++k) coefficients(k, d) = localCoefficients[k];
  }
  const DualLinearCombinationFunction trendFunction(coll, coefficients);

  return TrendTransform(trendFunction, field.getMesh());
}

/* String converter */
String TrendFactory::__repr__() const
{
  OSS oss(true);
  oss << "class=" << TrendFactory::GetClassName()
      << " fittingAlgorithm = " << fittingAlgorithm_
      << " basisSequenceFactory = " << basisSequenceFactory_;
  return oss;
}

/* String converter */
String TrendFactory::__str__(const String & ) const
{
  return __repr__();
}


/* Method save() stores the object through the StorageManager */
void TrendFactory::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("fittingAlgorithm_", fittingAlgorithm_);
  adv.saveAttribute("basisSequenceFactory_", basisSequenceFactory_);
}

/* Method load() reloads the object from the StorageManager */
void TrendFactory::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("fittingAlgorithm_", fittingAlgorithm_);
  adv.loadAttribute("basisSequenceFactory_", basisSequenceFactory_);
}

END_NAMESPACE_OPENTURNS
