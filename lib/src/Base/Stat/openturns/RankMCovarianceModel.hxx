//                                               -*- C++ -*-
/**
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
  RankMCovarianceModel(const NumericalPoint & variance,
		       const Basis & basis);

  /** Parameters constructor */
  RankMCovarianceModel(const CovarianceMatrix & covariance,
		       const Basis & basis);

  /** Virtual copy constructor */
  RankMCovarianceModel * clone() const;

  /** Computation of the covariance function */
  using CovarianceModelImplementation::operator();
  CovarianceMatrix operator() (const NumericalPoint & s,
      const NumericalPoint & t) const;

  /** Gradient */
  virtual Matrix partialGradient(const NumericalPoint & s,
                                 const NumericalPoint & t) const;

  /** Covariance accessor */
  CovarianceMatrix getCovariance() const;
  NumericalPoint getVariance() const;

  /** Basis accessor */
  Basis getBasis() const;
  Basis::FunctionCollection getFunctions() const;

  /** Is it a stationary covariance model ? */
  virtual Bool isStationary() const;

  /** Specific discretization method */
  using CovarianceModelImplementation::discretize;
  CovarianceMatrix discretize(const Sample & vertices) const;

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
  /** The variance of the rank m decomposition, of dimension>0 only if the covariance is diagonal */
  NumericalPoint variance_;
  
  /** The covariance matrix of the decomposition, of dimension>0 only if not diagonal */
  CovarianceMatrix covariance_;

  /** The basis of the rank m decomposition */
  Basis basis_;

  /** The functions involved into the rank m decomposition */
  Basis::FunctionPersistentCollection functions_;

} ; /* class RankMCovarianceModel */

END_NAMESPACE_OPENTURNS

#endif
