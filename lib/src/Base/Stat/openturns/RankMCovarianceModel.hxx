//                                               -*- C++ -*-
/**
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_RANKMCOVARIANCEMODEL_HXX
#define OPENTURNS_RANKMCOVARIANCEMODEL_HXX

#include "openturns/CovarianceModelImplementation.hxx"
#include "openturns/Basis.hxx"
#include "openturns/Point.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class RankMCovarianceModel
 */

class OT_API RankMCovarianceModel
  : public CovarianceModelImplementation
{

  CLASSNAME

public:

  /** Default constructor */
  explicit RankMCovarianceModel(const UnsignedInteger inputDimension = 1);

  /** Parameters constructor */
  RankMCovarianceModel(const Point & variance,
                       const Basis & basis);

  /** Parameters constructor */
  RankMCovarianceModel(const CovarianceMatrix & covariance,
                       const Basis & basis);

  /** Virtual copy constructor */
  RankMCovarianceModel * clone() const override;

  /** Computation of the covariance function */
  using CovarianceModelImplementation::operator();
  SquareMatrix operator() (const Point & s, const Point & t) const override;

  /** Gradient */
  virtual Matrix partialGradient(const Point & s,
                                 const Point & t) const override;

  /** Covariance accessor */
  CovarianceMatrix getCovariance() const;
  Point getVariance() const;

  /** Basis accessor */
  Basis getBasis() const;
  Basis::FunctionCollection getFunctions() const;

  /** Specific discretization method */
  using CovarianceModelImplementation::discretize;
  CovarianceMatrix discretize(const Sample & vertices) const override;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  void setBasis(const Basis & basis);

private:
  /** The variance of the rank m decomposition, of dimension>0 only if the covariance is diagonal */
  Point variance_;

  /** The covariance matrix of the decomposition, of dimension>0 only if not diagonal */
  CovarianceMatrix covariance_;

  /** The basis of the rank m decomposition */
  Basis basis_;

  /** The functions involved into the rank m decomposition */
  Basis::FunctionPersistentCollection functions_;

} ; /* class RankMCovarianceModel */

END_NAMESPACE_OPENTURNS

#endif
