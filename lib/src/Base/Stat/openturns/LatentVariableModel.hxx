//                                               -*- C++ -*-
/**
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_LATENTVARIABLEMODEL_HXX
#define OPENTURNS_LATENTVARIABLEMODEL_HXX

#include "openturns/CovarianceModelImplementation.hxx"
#include "openturns/SquaredExponential.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LatentVariableModel
 */

class OT_API LatentVariableModel
  : public CovarianceModelImplementation
{

  CLASSNAME

public:

  /** Parameters constructor */
  LatentVariableModel(const UnsignedInteger nLevels = 2,
						 const UnsignedInteger latentDim = 2);

  /** Virtual copy constructor */
  LatentVariableModel * clone() const override;

  /** Computation of the covariance function */
  using CovarianceModelImplementation::computeAsScalar;
  Scalar computeAsScalar(const Point & z1, const Point & z2) const override;
#ifndef SWIG
  Scalar computeAsScalar(const Collection<Scalar>::const_iterator & z1_begin,
                         const Collection<Scalar>::const_iterator & z2_begin) const override;
#endif
  Scalar computeAsScalar(const Scalar z1, const Scalar z2) const override;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Parameter accessor */
  void setFullParameter(const Point & parameter) override;
  Point getFullParameter() const override;

  /** Scale accessors */
  void setScale(const Point & scale) override;

  /** Amplitude accessors */
  void setAmplitude(const Point & amplitude) override;

  /** Nugget factor accessor */
  void setNuggetFactor(const Scalar nuggetFactor) override;

  /** Latent variables accessors */
  void setLatentVariables(const Point & latentVariablesCoordinates);
  Sample getFullLatentVariables() const;
  Point getActiveLatentVariables() const;

  /* setter for the full parameter */
  Description getFullParameterDescription() const override;

  /** Latent dimension accessor */
  UnsignedInteger getLatentDimension() const;

  /** Number of levels accessor */
  UnsignedInteger getLevelNumber() const;

  /** Drawing method */
  Graph draw(const UnsignedInteger rowIndex = 0,
             const UnsignedInteger columnIndex = 0,
             const Scalar tMin = ResourceMap::GetAsScalar("CovarianceModel-DefaultTMin"),
             const Scalar tMax = ResourceMap::GetAsScalar("CovarianceModel-DefaultTMax"),
             const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("CovarianceModel-DefaultPointNumber"),
             const Bool asStationary = true,
             const Bool correlationFlag = false) const override;

private:

  // The latent dimension
  UnsignedInteger latentDim_;

  // The number of levels
  UnsignedInteger nLevels_;

  // The latent covariance matrix
  CovarianceMatrix latentCovarianceMatrix_;

  // The latent covariance model
  SquaredExponential latentCovarianceModel_;

  // The size of the latent variable parameter
  UnsignedInteger activeLatentCoordinateDim_;

  // The active latent variable coordinates parameter
  Point activeLatentVariables_;

  // The latent variable coordinates parameter
  Sample fullLatentVariables_;

  /** Covariance matrix update */
  void updateLatentCovarianceMatrix();

} ; /* class LatentVariableModel */

END_NAMESPACE_OPENTURNS

#endif
