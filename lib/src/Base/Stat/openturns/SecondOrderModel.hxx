//                                               -*- C++ -*-
/**
 *  @brief This class is enables to build a second order model
 *  This is the interface class
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_SECONDORDERMODEL_HXX
#define OPENTURNS_SECONDORDERMODEL_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/SecondOrderModelImplementation.hxx"
#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/StationaryCovarianceModel.hxx"
#include "openturns/SpectralModelImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



class RegularGrid;

/**
 * @class SecondOrderModelImplementation
 */

class OT_API SecondOrderModel
  : public TypedInterfaceObject<SecondOrderModelImplementation>
{

  CLASSNAME

public:

  /** Default onstructor */
  SecondOrderModel();

  /** Copy constructors */
  SecondOrderModel(const SecondOrderModelImplementation & implementation);

  /** Constructor from implementation */
  SecondOrderModel(const Implementation & p_implementation);

  SecondOrderModel(const CovarianceModel & covarianceModel,
                   const SpectralModel & spectralModel);

#ifndef SWIG

  /** Constructor from implementation pointer */
  SecondOrderModel(SecondOrderModelImplementation * p_implementation);
#endif

  /** Dimension accessor */
  UnsignedInteger getInputDimension() const;
  UnsignedInteger getOutputDimension() const;

  /** @deprecated */
  UnsignedInteger getSpatialDimension() const;
  UnsignedInteger getDimension() const;

  /** SpectralModel and CovarianceModel accessor */
  SpectralModel getSpectralModel() const;
  CovarianceModel getCovarianceModel() const;
  void setModels(const CovarianceModel & covarianceModel,
                 const SpectralModel & spectralModel);

  /** Computation of the covariance matrix */
  virtual CovarianceMatrix computeCovariance(const Scalar s,
      const Scalar t) const;

  virtual CovarianceMatrix computeCovariance(const Scalar tau) const;


  virtual CovarianceMatrix computeCovariance(const Point & s,
      const Point & t) const;

  virtual CovarianceMatrix computeCovariance(const Point & tau) const;

  /** Computation of the spectral density function */
  virtual HermitianMatrix computeSpectralDensity(const Scalar frequency) const;

  /** Discretize the covariance function on a given TimeGrid */
  virtual CovarianceMatrix discretize(const RegularGrid & timeGrid) const;

  /** String converter */
  String __repr__() const;

  /** String converter */
  String __str__(const String & offset = "") const;


} ; /* class SecondOrderModel */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SECONDORDERMODEL_HXX */
