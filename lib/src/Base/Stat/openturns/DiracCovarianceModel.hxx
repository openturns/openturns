//                                               -*- C++ -*-
/**
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
#ifndef OPENTURNS_DIRACCOVARIANCEMODEL_HXX
#define OPENTURNS_DIRACCOVARIANCEMODEL_HXX

#include "openturns/StationaryCovarianceModel.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class DiracCovarianceModel
 */

class OT_API DiracCovarianceModel
  : public StationaryCovarianceModel
{

  CLASSNAME;

public:


  /** Default constructor */
  explicit DiracCovarianceModel(const UnsignedInteger spatialDimension = 1);

  /** Parameters constructor */
  DiracCovarianceModel(const UnsignedInteger spatialDimension,
                       const NumericalPoint & sigma);

  /** Parameters constructor */
  DiracCovarianceModel(const UnsignedInteger spatialDimension,
                       const NumericalPoint & sigma,
                       const CorrelationMatrix & correlation);

  /** Parameters constructor */
  DiracCovarianceModel(const UnsignedInteger spatialDimension,
                       const CovarianceMatrix & covariance);

  /** Virtual copy constructor */
  DiracCovarianceModel * clone() const;

  /** Computation of the covariance function */
  using StationaryCovarianceModel::operator();
  CovarianceMatrix operator() (const NumericalPoint & tau) const;

  /** Discretize the covariance function */
  using StationaryCovarianceModel::discretize;
  CovarianceMatrix discretize(const NumericalSample & vertices) const;
  NumericalSample discretizeRow(const NumericalSample & vertices,
                                const UnsignedInteger p) const;

  using StationaryCovarianceModel::discretizeAndFactorize;
  TriangularMatrix discretizeAndFactorize(const NumericalSample & vertices) const;
  // discretize with use of HMatrix
  using StationaryCovarianceModel::discretizeHMatrix;
  HMatrix discretizeHMatrix(const NumericalSample & vertices,
                            const NumericalScalar nuggetFactor,
                            const HMatrixParameters & parameters) const;

  /** Gradient */
  Matrix partialGradient(const NumericalPoint & s,
                         const NumericalPoint & t) const;

  /** Parameters accessor */
  void setParameter(const NumericalPoint & parameters);
  NumericalPoint getParameter() const;
  Description getParameterDescription() const;

  /** Scale/amplitude set accessors */
  void setScale(const NumericalPoint & scale);
  void setAmplitude(const NumericalPoint & amplitude);
  void setSpatialCorrelation(const CorrelationMatrix & correlation);

  /** Is it a stationary covariance model ? */
  Bool isStationary() const;

  /** String converter */
  String __repr__() const;

  /** String converter */
  String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:
  friend struct DiracCovarianceModelDiscretizePolicy;
  friend struct DiracCovarianceModelDiscretizeAndFactorizePolicy;
  mutable CovarianceMatrix covariance_;
  mutable TriangularMatrix covarianceFactor_;
  void computeCovariance();

} ; /* class DiracCovarianceModel */

END_NAMESPACE_OPENTURNS

#endif
