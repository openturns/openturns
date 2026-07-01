//                                               -*- C++ -*-
/**
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_DIVFREEMODEL_HXX
#define OPENTURNS_DIVFREEMODEL_HXX

#include "openturns/CovarianceModelImplementation.hxx"
#include "openturns/CovarianceModel.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API DivFreeModel
  : public CovarianceModelImplementation
{

  CLASSNAME

public:

  /** Default constructor */
  explicit DivFreeModel(const UnsignedInteger inputDimension = 2);

  /** Parameters constructor */
  explicit DivFreeModel(const CovarianceModel & model);

  /** Virtual copy constructor */
  DivFreeModel * clone() const override;

  /** Computation of the covariance function */
  using CovarianceModelImplementation::operator();
  SquareMatrix operator()(const Point & s, const Point & t) const override;
  SquareMatrix operator()(const Point & tau) const override;

  /** Scalar interface (for 1D case) */
  Scalar computeAsScalar(const Point & s, const Point & t) const override;
  Scalar computeAsScalar(const Point & tau) const override;
  Scalar computeAsScalar(const Scalar s, const Scalar t) const override;
  Scalar computeAsScalar(const Scalar tau) const override;
  Scalar computeAsScalar(const Collection<Scalar>::const_iterator & s_begin,
                         const Collection<Scalar>::const_iterator & t_begin) const override;

  /** Is it a stationary covariance model ? */
  Bool isStationary() const override;

  /** Scale accessor */
  void setScale(const Point & scale) override;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Underlying model accessor */
  CovarianceModel getCovarianceModel() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** The underlying scalar covariance model */
  CovarianceModel model_;

} ; /* class DivFreeModel */

END_NAMESPACE_OPENTURNS

#endif
