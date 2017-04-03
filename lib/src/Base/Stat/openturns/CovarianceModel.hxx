//                                               -*- C++ -*-
/**
 *  @brief This class enables to build a covariance model
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_COVARIANCEMODEL_HXX
#define OPENTURNS_COVARIANCEMODEL_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/CovarianceModelImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CovarianceModel
 */

class OT_API CovarianceModel
  : public TypedInterfaceObject<CovarianceModelImplementation>
{

  CLASSNAME;

public:

  typedef CovarianceModelImplementation::Implementation    Implementation;

  /** Default constructor without parameter */
  CovarianceModel();

  /** Copy constructors */
  CovarianceModel(const CovarianceModelImplementation & implementation);

  /** Constructor from implementation */
  CovarianceModel(const Implementation & p_implementation);

#ifndef SWIG
  /** Constructor from implementation pointer */
  CovarianceModel(CovarianceModelImplementation * p_implementation);
#endif

  /** Dimension accessor */
  virtual UnsignedInteger getSpatialDimension() const;
  virtual UnsignedInteger getDimension() const;

  /** Compute the covariance function */
  virtual CovarianceMatrix operator() (const NumericalScalar s,
                                       const NumericalScalar t) const;

  virtual CovarianceMatrix operator() (const NumericalPoint & s,
                                       const NumericalPoint & t) const;

  // compute standard representative computes the term \rho(s, t)
  virtual NumericalScalar computeStandardRepresentative(const NumericalPoint & s,
      const NumericalPoint & t) const;

  virtual NumericalScalar computeStandardRepresentative(const NumericalScalar & s,
      const NumericalScalar & t) const;

  virtual NumericalScalar computeStandardRepresentative(const NumericalPoint & tau) const;

  virtual NumericalScalar computeStandardRepresentative(const NumericalScalar & tau) const;

  virtual NumericalScalar computeAsScalar (const NumericalPoint & s,
      const NumericalPoint & t) const;

  virtual CovarianceMatrix operator() (const NumericalScalar tau) const;

  virtual CovarianceMatrix operator() (const NumericalPoint & tau) const;

  virtual NumericalScalar computeAsScalar (const NumericalPoint & tau) const;

  /** Gradient */
  virtual Matrix partialGradient(const NumericalPoint & s,
                                 const NumericalPoint & t) const;

  /** Gradient wrt parameters */
  virtual Matrix parameterGradient (const NumericalPoint & s,
                                    const NumericalPoint & t) const;

  /** Discretize the covariance function on a given TimeGrid/Mesh */
  virtual CovarianceMatrix discretize(const RegularGrid & timeGrid) const;
  virtual CovarianceMatrix discretize(const Mesh & mesh) const;
  virtual CovarianceMatrix discretize(const Sample & vertices) const;
  virtual Sample discretizeRow(const Sample & vertices,
                                        const UnsignedInteger p) const;

  /** Discretize and factorize the covariance function on a given TimeGrid/Mesh */
  virtual TriangularMatrix discretizeAndFactorize(const RegularGrid & timeGrid) const;
  virtual TriangularMatrix discretizeAndFactorize(const Mesh & mesh) const;
  virtual TriangularMatrix discretizeAndFactorize(const Sample & vertices) const;

  /** Discretize the covariance function on a given TimeGrid/Mesh using HMatrix */
  virtual HMatrix discretizeHMatrix(const RegularGrid & timeGrid,
                                    const NumericalScalar nuggetFactor,
                                    const HMatrixParameters & parameters) const;
  virtual HMatrix discretizeHMatrix(const Mesh & mesh,
                                    const NumericalScalar nuggetFactor,
                                    const HMatrixParameters & parameters) const;
  virtual HMatrix discretizeHMatrix(const Sample & vertices,
                                    const NumericalScalar nuggetFactor,
                                    const HMatrixParameters & parameters) const;

  /** Discretize and factorize the covariance function on a given TimeGrid/Mesh using HMatrix */
  virtual HMatrix discretizeAndFactorizeHMatrix(const RegularGrid & timeGrid,
      const NumericalScalar nuggetFactor,
      const HMatrixParameters & parameters) const;
  virtual HMatrix discretizeAndFactorizeHMatrix(const Mesh & mesh,
      const NumericalScalar nuggetFactor,
      const HMatrixParameters & parameters) const;
  virtual HMatrix discretizeAndFactorizeHMatrix(const Sample & vertices,
      const NumericalScalar nuggetFactor,
      const HMatrixParameters & parameters) const;


  /** Amplitude accessors */
  NumericalPoint getAmplitude() const;
  void setAmplitude(const NumericalPoint & amplitude);

  /** Scale accessors */
  NumericalPoint getScale() const;
  void setScale(const NumericalPoint & scale);

  /** Spatial correlation accessors */
  CorrelationMatrix getSpatialCorrelation() const;
  void setSpatialCorrelation(const CorrelationMatrix & correlation);

  /** Nugget factor accessor */
  void setNuggetFactor(const NumericalScalar nuggetFactor);
  NumericalScalar getNuggetFactor() const;

  /** Parameters accessor */
  void setParameter(const NumericalPoint & parameter);
  NumericalPoint getParameter() const;
  Description getParameterDescription() const;

  /** Indices of the active parameters */
  void setActiveParameter(const Indices & active);
  Indices getActiveParameter() const;

  /** Is it a stationary model ? */
  virtual Bool isStationary() const;

  /** Is it a diagonal model ? */
  virtual Bool isDiagonal() const;

  /** Drawing method */
  virtual Graph draw(const UnsignedInteger rowIndex = 0,
		     const UnsignedInteger columnIndex = 0,
		     const NumericalScalar tMin = ResourceMap::GetAsNumericalScalar("CovarianceModel-DefaultTMin"),
		     const NumericalScalar tMax = ResourceMap::GetAsNumericalScalar("CovarianceModel-DefaultTMax"),
		     const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("CovarianceModel-DefaultPointNumber"),
		     const Bool asStationary = true,
		     const Bool correlationFlag = false) const;
  
  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

  /** Marginal accessor */
  CovarianceModel getMarginal(const UnsignedInteger index) const;

} ; /* class CovarianceModel */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COVARIANCEMODEL_HXX */
