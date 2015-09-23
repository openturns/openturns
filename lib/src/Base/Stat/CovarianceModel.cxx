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
 */
#include "CovarianceModel.hxx"
#include "ExponentialModel.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CovarianceModel
 */

CLASSNAMEINIT(CovarianceModel);

//   static const Factory<CovarianceModel> RegisteredFactory;

/* Constructor without parameters */
CovarianceModel::CovarianceModel()
  : TypedInterfaceObject<CovarianceModelImplementation>(new ExponentialModel())
{
  // Nothing to do
}

/* Parameters constructor */
CovarianceModel::CovarianceModel(const CovarianceModelImplementation & implementation)
  : TypedInterfaceObject<CovarianceModelImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from implementation */
CovarianceModel::CovarianceModel(const Implementation & p_implementation)
  : TypedInterfaceObject<CovarianceModelImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor from implementation pointer */
CovarianceModel::CovarianceModel(CovarianceModelImplementation * p_implementation)
  : TypedInterfaceObject<CovarianceModelImplementation>(p_implementation)
{
  // Nothing to do
}

/* Dimension accessor */
UnsignedInteger CovarianceModel::getSpatialDimension() const
{
  return getImplementation()->getSpatialDimension();
}

UnsignedInteger CovarianceModel::getDimension() const
{
  return getImplementation()->getDimension();
}

/* Computation of the covariance function */
CovarianceMatrix CovarianceModel::operator() (const NumericalScalar s,
    const NumericalScalar t) const
{
  return getImplementation()->operator() (s, t);
}

CovarianceMatrix CovarianceModel::operator() (const NumericalPoint & s,
    const NumericalPoint & t) const
{
  return getImplementation()->operator() (s, t);
}

NumericalScalar CovarianceModel::computeAsScalar (const NumericalPoint & s,
    const NumericalPoint & t) const
{
  return getImplementation()->computeAsScalar(s, t);
}

CovarianceMatrix CovarianceModel::operator() (const NumericalScalar tau) const
{
  return getImplementation()->operator() (tau);
}

CovarianceMatrix CovarianceModel::operator() (const NumericalPoint & tau) const
{
  return getImplementation()->operator() (tau);
}

NumericalScalar CovarianceModel::computeAsScalar (const NumericalPoint & tau) const
{
  return getImplementation()->computeAsScalar(tau);
}

/* Gradient */
Matrix CovarianceModel::partialGradient(const NumericalPoint & s,
                                        const NumericalPoint & t) const
{
  return getImplementation()->partialGradient(s, t);
}

/* Discretize the covariance function on a given TimeGrid/Mesh */
CovarianceMatrix CovarianceModel::discretize(const RegularGrid & timeGrid) const
{
  return getImplementation()->discretize(timeGrid);
}

CovarianceMatrix CovarianceModel::discretize(const Mesh & mesh) const
{
  return getImplementation()->discretize(mesh);
}

CovarianceMatrix CovarianceModel::discretize(const NumericalSample & vertices) const
{
  return getImplementation()->discretize(vertices);
}

NumericalSample CovarianceModel::discretizeRow(const NumericalSample & vertices,
    const UnsignedInteger p) const
{
  return getImplementation()->discretizeRow(vertices, p);
}

/* Amplitude accessor */
NumericalPoint CovarianceModel::getAmplitude() const
{
  return getImplementation()->getAmplitude();
}

void CovarianceModel::setAmplitude(const NumericalPoint & amplitude)
{
  copyOnWrite();
  getImplementation()->setAmplitude(amplitude);
}

/* Scale accessor */
NumericalPoint CovarianceModel::getScale() const
{
  return getImplementation()->getScale();
}

void CovarianceModel::setScale(const NumericalPoint & scale)
{
  copyOnWrite();
  getImplementation()->setScale(scale);
}

/* Spatial correlation accessor */
CorrelationMatrix CovarianceModel::getSpatialCorrelation() const
{
  return getImplementation()->getSpatialCorrelation();
}

void CovarianceModel::setSpatialCorrelation(const CorrelationMatrix & spatialCorrelation)
{
  copyOnWrite();
  getImplementation()->setSpatialCorrelation(spatialCorrelation);
}

/* Nugget factor accessor */
NumericalScalar CovarianceModel::getNuggetFactor() const
{
  return getImplementation()->getNuggetFactor();
}

void CovarianceModel::setNuggetFactor(const NumericalScalar nuggetFactor)
{
  copyOnWrite();
  getImplementation()->setNuggetFactor(nuggetFactor);
}

/* Parameters accessor */
void CovarianceModel::setParameters(const NumericalPoint& parameters)
{
  copyOnWrite();
  getImplementation()->setParameters(parameters);
}

NumericalPointWithDescription CovarianceModel::getParameters() const
{
  return getImplementation()->getParameters();
}

/* Is it a stationary covariance model ? */
Bool CovarianceModel::isStationary() const
{
  return getImplementation()->isStationary();
}

/* Is it a diagonal covariance model ? */
Bool CovarianceModel::isDiagonal() const
{
  return getImplementation()->isDiagonal();
}

/* Drawing method */
Graph CovarianceModel::draw(const UnsignedInteger rowIndex,
                            const UnsignedInteger columnIndex) const
{
  return getImplementation()->draw(rowIndex, columnIndex);
}

/* String converter */
String CovarianceModel::__repr__() const
{
  return getImplementation()->__repr__();
}

/* String converter */
String CovarianceModel::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

/* Marginal accessor */
CovarianceModel CovarianceModel::getMarginal(const UnsignedInteger index) const
{
  return getImplementation()->getMarginal(index);
}


END_NAMESPACE_OPENTURNS
