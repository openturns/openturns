//                                               -*- C++ -*-
/**
 *  @brief
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/CovarianceModel.hxx"
#include "openturns/ExponentialModel.hxx"
#include "openturns/HMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CovarianceModel
 */

CLASSNAMEINIT(CovarianceModel)

//   static const Factory<CovarianceModel> Factory_CovarianceModel;

/* Constructor without parameter */
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
UnsignedInteger CovarianceModel::getInputDimension() const
{
  return getImplementation()->getInputDimension();
}

UnsignedInteger CovarianceModel::getOutputDimension() const
{
  return getImplementation()->getOutputDimension();
}

/* Computation of the covariance function */
CovarianceMatrix CovarianceModel::operator() (const Scalar s,
    const Scalar t) const
{
  return getImplementation()->operator() (s, t);
}

CovarianceMatrix CovarianceModel::operator() (const Point & s,
    const Point & t) const
{
  return getImplementation()->operator() (s, t);
}

Scalar CovarianceModel::computeStandardRepresentative(const Point & s,
    const Point & t) const
{
  return getImplementation()->computeStandardRepresentative(s, t);
}

Scalar CovarianceModel::computeAsScalar (const Point & s,
    const Point & t) const
{
  return getImplementation()->computeAsScalar(s, t);
}

CovarianceMatrix CovarianceModel::operator() (const Scalar tau) const
{
  return getImplementation()->operator() (tau);
}

CovarianceMatrix CovarianceModel::operator() (const Point & tau) const
{
  return getImplementation()->operator() (tau);
}

/* Gradient */
Matrix CovarianceModel::partialGradient(const Point & s,
                                        const Point & t) const
{
  return getImplementation()->partialGradient(s, t);
}

/* Gradient wrt parameters */
Matrix CovarianceModel::parameterGradient(const Point & s,
    const Point & t) const
{
  return getImplementation()->parameterGradient(s, t);
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

CovarianceMatrix CovarianceModel::discretize(const Sample & vertices) const
{
  return getImplementation()->discretize(vertices);
}

Sample CovarianceModel::discretizeRow(const Sample & vertices,
                                      const UnsignedInteger p) const
{
  return getImplementation()->discretizeRow(vertices, p);
}

/** Discretize and factorize the covariance function on a given TimeGrid/Mesh */
TriangularMatrix CovarianceModel::discretizeAndFactorize(const RegularGrid & timeGrid) const
{
  return getImplementation()->discretizeAndFactorize(timeGrid);
}

TriangularMatrix CovarianceModel::discretizeAndFactorize(const Mesh & mesh) const
{
  return getImplementation()->discretizeAndFactorize(mesh);
}

TriangularMatrix CovarianceModel::discretizeAndFactorize(const Sample & vertices) const
{
  return getImplementation()->discretizeAndFactorize(vertices);
}


/** Discretize the covariance function on a given TimeGrid/Mesh using HMatrix */
HMatrix CovarianceModel::discretizeHMatrix(const RegularGrid & timeGrid,
    const HMatrixParameters & parameters) const
{
  return getImplementation()->discretizeHMatrix(timeGrid, parameters);
}

HMatrix CovarianceModel::discretizeHMatrix(const Mesh & mesh,
    const HMatrixParameters & parameters) const
{
  return getImplementation()->discretizeHMatrix(mesh, parameters);
}

HMatrix CovarianceModel::discretizeHMatrix(const Sample & vertices,
    const HMatrixParameters & parameters) const
{
  return getImplementation()->discretizeHMatrix(vertices, parameters);
}

/** Discretize and factorize the covariance function on a given TimeGrid/Mesh using HMatrix */
HMatrix CovarianceModel::discretizeAndFactorizeHMatrix(const RegularGrid & timeGrid,
    const HMatrixParameters & parameters) const
{
  return getImplementation()->discretizeAndFactorizeHMatrix(timeGrid, parameters);
}

HMatrix CovarianceModel::discretizeAndFactorizeHMatrix(const Mesh & mesh,
    const HMatrixParameters & parameters) const
{
  return getImplementation()->discretizeAndFactorizeHMatrix(mesh, parameters);
}

HMatrix CovarianceModel::discretizeAndFactorizeHMatrix(const Sample & vertices,
    const HMatrixParameters & parameters) const
{
  return getImplementation()->discretizeAndFactorizeHMatrix(vertices, parameters);
}

/* Amplitude accessor */
Point CovarianceModel::getAmplitude() const
{
  return getImplementation()->getAmplitude();
}

void CovarianceModel::setAmplitude(const Point & amplitude)
{
  copyOnWrite();
  getImplementation()->setAmplitude(amplitude);
}

/* Scale accessor */
Point CovarianceModel::getScale() const
{
  return getImplementation()->getScale();
}

void CovarianceModel::setScale(const Point & scale)
{
  copyOnWrite();
  getImplementation()->setScale(scale);
}

/* Scale accessor */
CovarianceModel::ScaleParametrization CovarianceModel::getScaleParametrization() const
{
  return getImplementation()->getScaleParametrization();
}

void CovarianceModel::setScaleParametrization(const ScaleParametrization scaleParametrization)
{
  copyOnWrite();
  getImplementation()->setScaleParametrization(scaleParametrization);
}

/* Spatial correlation accessor */
CorrelationMatrix CovarianceModel::getOutputCorrelation() const
{
  return getImplementation()->getOutputCorrelation();
}

void CovarianceModel::setOutputCorrelation(const CorrelationMatrix & outputCorrelation)
{
  copyOnWrite();
  getImplementation()->setOutputCorrelation(outputCorrelation);
}

/* Nugget factor accessor */
Scalar CovarianceModel::getNuggetFactor() const
{
  return getImplementation()->getNuggetFactor();
}

void CovarianceModel::setNuggetFactor(const Scalar nuggetFactor)
{
  copyOnWrite();
  getImplementation()->setNuggetFactor(nuggetFactor);
}

/* Parameters accessor */
void CovarianceModel::setParameter(const Point& parameter)
{
  copyOnWrite();
  getImplementation()->setParameter(parameter);
}

Point CovarianceModel::getParameter() const
{
  return getImplementation()->getParameter();
}

Description CovarianceModel::getParameterDescription() const
{
  return getImplementation()->getParameterDescription();
}

/* Indices of the active parameters */
void CovarianceModel::setActiveParameter(const Indices & active)
{
  copyOnWrite();
  getImplementation()->setActiveParameter(active);
}

Indices CovarianceModel::getActiveParameter() const
{
  return getImplementation()->getActiveParameter();
}


/* setter for the full parameter */
void CovarianceModel::setFullParameter(const Point & parameter)
{
  copyOnWrite();
  getImplementation()->setFullParameter(parameter);
}

Point CovarianceModel::getFullParameter() const
{
  return getImplementation()->getFullParameter();
}

Description CovarianceModel::getFullParameterDescription() const
{
  return getImplementation()->getFullParameterDescription();
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

/* Is it a composite covariance model ? */
Bool CovarianceModel::isComposite() const
{
  return getImplementation()->isComposite();
}

/* Drawing method */
Graph CovarianceModel::draw(const UnsignedInteger rowIndex,
                            const UnsignedInteger columnIndex,
                            const Scalar tMin,
                            const Scalar tMax,
                            const UnsignedInteger pointNumber,
                            const Bool asStationary,
                            const Bool correlationFlag) const
{
  return getImplementation()->draw(rowIndex, columnIndex, tMin, tMax, pointNumber, asStationary, correlationFlag);
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

CovarianceModel CovarianceModel::getMarginal(const Indices & indices) const
{
  return getImplementation()->getMarginal(indices);
}


END_NAMESPACE_OPENTURNS
