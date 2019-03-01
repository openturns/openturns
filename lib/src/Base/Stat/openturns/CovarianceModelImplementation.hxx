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
#ifndef OPENTURNS_COVARIANCEMODELIMPLEMENTATION_HXX
#define OPENTURNS_COVARIANCEMODELIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/CovarianceMatrix.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/RegularGrid.hxx"
#include "openturns/Mesh.hxx"
#include "openturns/HMatrixParameters.hxx"

BEGIN_NAMESPACE_OPENTURNS

class HMatrix;
class CovarianceModel;

/**
 * @class CovarianceModelImplementation
 */

class OT_API CovarianceModelImplementation
  : public PersistentObject
{

  CLASSNAME

public:

  typedef Pointer<CovarianceModelImplementation> Implementation;
  typedef enum {STANDARD, INVERSE, LOGINVERSE} ScaleParametrization;

  /** Dimension-based constructor */
  explicit CovarianceModelImplementation(const UnsignedInteger outputDimension = 1);


  /** Standard constructor with scale and amplitude parameter parameter */
  CovarianceModelImplementation(const Point & scale,
                                const Point & amplitude);

  /** Standard constructor with scale, amplitude and output correlation parameter parameter */
  CovarianceModelImplementation(const Point & scale,
                                const Point & amplitude,
                                const CorrelationMatrix & outputCorrelation);

  /** Standard constructor with scale and output covariance parameter parameter */
  CovarianceModelImplementation(const Point & scale,
                                const CovarianceMatrix & outputCovariance);

  /** Virtual copy constructor */
  virtual CovarianceModelImplementation * clone() const;

  /** Dimensions accessors */
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

#ifndef SWIG
  virtual Scalar computeStandardRepresentative(const Collection<Scalar>::const_iterator & s_begin,
      const Collection<Scalar>::const_iterator & t_begin) const;
#endif

  // Special case for 1D model
  virtual Scalar computeAsScalar (const Point & s,
                                  const Point & t) const;

#ifndef SWIG
  // Special case for 1D model
  virtual Scalar computeAsScalar(const Collection<Scalar>::const_iterator & s_begin,
                                 const Collection<Scalar>::const_iterator & t_begin) const;
#endif

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

  /** Is it a stationary covariance model ? */
  virtual Bool isStationary() const;

  /** Is it a diagonal covariance model ? */
  virtual Bool isDiagonal() const;

  /** Is it a composite covariance model ? */
  virtual Bool isComposite() const;

  /** Amplitude accessors */
  virtual Point getAmplitude() const;
  virtual void setAmplitude(const Point & amplitude);

  /** Scale accessors */
  virtual Point getScale() const;
  virtual void setScale(const Point & scale);

  // Scale parametrization accessor
  virtual ScaleParametrization getScaleParametrization() const;
  virtual void setScaleParametrization(const ScaleParametrization scaleParametrization);

  /** Output correlation accessors */
  virtual CorrelationMatrix getOutputCorrelation() const;
  virtual void setOutputCorrelation(const CorrelationMatrix & correlation);

  /** Nugget factor accessor */
  virtual void setNuggetFactor(const Scalar nuggetFactor);
  virtual Scalar getNuggetFactor() const;

  /** Parameters accessor */
  virtual void setParameter(const Point & parameter);
  virtual Point getParameter() const;
  virtual Description getParameterDescription() const;

  /** Indices of the active parameters */
  virtual void setActiveParameter(const Indices & active);
  virtual Indices getActiveParameter() const;

  /* setter for the full parameter */
  virtual void setFullParameter(const Point & parameter);
  virtual Point getFullParameter() const;
  virtual Description getFullParameterDescription() const;

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

  /** Marginal accessor */
  virtual CovarianceModel getMarginal(const UnsignedInteger index) const;

  /** Marginal accessor */
  virtual CovarianceModel getMarginal(const Indices & indices) const;

  /** Drawing method */
  virtual Graph draw(const UnsignedInteger rowIndex = 0,
                     const UnsignedInteger columnIndex = 0,
                     const Scalar tMin = ResourceMap::GetAsScalar("CovarianceModel-DefaultTMin"),
                     const Scalar tMax = ResourceMap::GetAsScalar("CovarianceModel-DefaultTMax"),
                     const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("CovarianceModel-DefaultPointNumber"),
                     const Bool asStationary = true,
                     const Bool correlationFlag = false) const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:

  // set the covariance structure
  void updateOutputCovariance();

  /** Container for scale values  */
  Point scale_;

  // scale parametrization
  ScaleParametrization scaleParametrization_;

  /** Input dimension */
  UnsignedInteger inputDimension_;

  /** Amplitude values  */
  Point amplitude_;

  /** Output dimension */
  UnsignedInteger outputDimension_;

  /** Correlation matrix of the output dependence structure */
  CorrelationMatrix outputCorrelation_;

  /** Covariance matrix of the output dependence structure */
  mutable CovarianceMatrix outputCovariance_;

  /** Cholesky factor of covariance matrix of the output dependence structure */
  mutable TriangularMatrix outputCovarianceCholeskyFactor_;

  /** Flag to tell if computeStandardRepresentative() method is defined */
  Bool definesComputeStandardRepresentative_;

  /** Flag to tell if the model is diagonal */
  Bool isDiagonal_;

  /** Nugget factor */
  Scalar nuggetFactor_;

  /** Active parameters */
  Indices activeParameter_;

} ; /* class CovarianceModelImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COVARIANCEMODELIMPLEMENTATION_HXX */
