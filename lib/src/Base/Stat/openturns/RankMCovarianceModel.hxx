//                                               -*- C++ -*-
/**
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_RANKMCOVARIANCEMODEL_HXX
#define OPENTURNS_RANKMCOVARIANCEMODEL_HXX

#include "openturns/CovarianceModelImplementation.hxx"
#include "openturns/Basis.hxx"
#include "openturns/NumericalPoint.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class RankMCovarianceModel
 */

class OT_API RankMCovarianceModel
  : public CovarianceModelImplementation
{

  CLASSNAME;

public:

  /** Default constructor */
  explicit RankMCovarianceModel(const UnsignedInteger spatialDimension = 1);

  /** Parameters constructor */
  RankMCovarianceModel(const NumericalPoint & coefficients,
		       const Basis & basis);

  /** Parameters constructor */
  RankMCovarianceModel(const NumericalPoint & coefficients,
		       const Basis & basis,
		       const CovarianceMatrix & covariance);

  /** Virtual copy constructor */
  RankMCovarianceModel * clone() const;

  /** Computation of the covariance function */
  using CovarianceModelImplementation::operator();
  CovarianceMatrix operator() (const NumericalPoint & s,
      const NumericalPoint & t) const;

  /** Gradient */
  virtual Matrix partialGradient(const NumericalPoint & s,
                                 const NumericalPoint & t) const;

  /** Basis accessor */
  const Basis & getBasis() const;
  const Basis::NumericalMathFunctionCollection & getFunctions() const;

  /** Is it a stationary covariance model ? */
  virtual Bool isStationary() const;

  /** Specific discretization method */
  using CovarianceModelImplementation::discretize;
  CovarianceMatrix discretize(const NumericalSample & vertices) const;

  /** String converter */
  String __repr__() const;

  /** String converter */
  String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:
  void setBasis(const Basis & basis);

private:
  /** The coefficients of the rank m decomposition */
  NumericalPoint coefficients_;
  
  /** The basis of the rank m decomposition */
  Basis basis_;

  /** The functions involved into the rank m decomposition */
  Basis::NumericalMathFunctionPersistentCollection functions_;

  /** The covariance matrix of the coefficients.
      If the coefficients are uncorrelated the matrix is not
      stored and its diagonal elements are aggregated into coefficients */
  CovarianceMatrix covariance_;
} ; /* class RankMCovarianceModel */

END_NAMESPACE_OPENTURNS

#endif
