//                                               -*- C++ -*-
/**
 *  @brief This class enables to build a covariance model
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

  CLASSNAME

public:

  typedef CovarianceModelImplementation::Implementation    Implementation;
  typedef CovarianceModelImplementation::ScaleParametrization ScaleParametrization;

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
  virtual UnsignedInteger getInputDimension() const;
  virtual UnsignedInteger getOutputDimension() const;

  /** Compute the covariance function */
  virtual CovarianceMatrix operator() (const Scalar s,
                                       const Scalar t) const;

  virtual CovarianceMatrix operator() (const Point & s,
                                       const Point & t) const;

  // compute standard representative computes the term \rho(s, t)
  virtual Scalar computeStandardRepresentative(const Point & s,
      const Point & t) const;

  virtual Scalar computeAsScalar (const Point & s,
                                  const Point & t) const;

  virtual CovarianceMatrix operator() (const Scalar tau) const;

  virtual CovarianceMatrix operator() (const Point & tau) const;

  /** Gradient */
  virtual Matrix partialGradient(const Point & s,
                                 const Point & t) const;

  /** Gradient wrt parameters */
  virtual Matrix parameterGradient (const Point & s,
                                    const Point & t) const;

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
                                    const HMatrixParameters & parameters) const;
  virtual HMatrix discretizeHMatrix(const Mesh & mesh,
                                    const HMatrixParameters & parameters) const;
  virtual HMatrix discretizeHMatrix(const Sample & vertices,
                                    const HMatrixParameters & parameters) const;

  /** Discretize and factorize the covariance function on a given TimeGrid/Mesh using HMatrix */
  virtual HMatrix discretizeAndFactorizeHMatrix(const RegularGrid & timeGrid,
      const HMatrixParameters & parameters) const;
  virtual HMatrix discretizeAndFactorizeHMatrix(const Mesh & mesh,
      const HMatrixParameters & parameters) const;
  virtual HMatrix discretizeAndFactorizeHMatrix(const Sample & vertices,
      const HMatrixParameters & parameters) const;


  /** Amplitude accessors */
  Point getAmplitude() const;
  void setAmplitude(const Point & amplitude);

  /** Scale accessors */
  Point getScale() const;
  void setScale(const Point & scale);

  /** Scale parametrization accessor */
  ScaleParametrization getScaleParametrization() const;
  void setScaleParametrization(const ScaleParametrization scaleParametrization);

  /** Spatial correlation accessors */
  CorrelationMatrix getOutputCorrelation() const;
  void setOutputCorrelation(const CorrelationMatrix & correlation);

  /** Nugget factor accessor */
  void setNuggetFactor(const Scalar nuggetFactor);
  Scalar getNuggetFactor() const;

  /** Parameters accessor */
  void setParameter(const Point & parameter);
  Point getParameter() const;
  Description getParameterDescription() const;

  /** Indices of the active parameters */
  void setActiveParameter(const Indices & active);
  Indices getActiveParameter() const;


  /* setter for the full parameter */
  virtual void setFullParameter(const Point & parameter);
  virtual Point getFullParameter() const;
  virtual Description getFullParameterDescription() const;

  /** Is it a stationary model ? */
  virtual Bool isStationary() const;

  /** Is it a diagonal model ? */
  virtual Bool isDiagonal() const;

  /** Is it a composite model ? */
  virtual Bool isComposite() const;

  /** Drawing method */
  virtual Graph draw(const UnsignedInteger rowIndex = 0,
                     const UnsignedInteger columnIndex = 0,
                     const Scalar tMin = ResourceMap::GetAsScalar("CovarianceModel-DefaultTMin"),
                     const Scalar tMax = ResourceMap::GetAsScalar("CovarianceModel-DefaultTMax"),
                     const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("CovarianceModel-DefaultPointNumber"),
                     const Bool asStationary = true,
                     const Bool correlationFlag = false) const;

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

  /** Marginal accessor */
  CovarianceModel getMarginal(const UnsignedInteger index) const;

  /** Marginal accessor */
  CovarianceModel getMarginal(const Indices & indices) const;

} ; /* class CovarianceModel */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COVARIANCEMODEL_HXX */
