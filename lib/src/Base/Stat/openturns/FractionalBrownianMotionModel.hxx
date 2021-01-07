//                                               -*- C++ -*-
/**
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_FRACTIONALBROWNIANMOTIONMODEL_HXX
#define OPENTURNS_FRACTIONALBROWNIANMOTIONMODEL_HXX

#include "openturns/CovarianceModelImplementation.hxx"
#include "openturns/Point.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FractionalBrownianMotionModel
 */

class OT_API FractionalBrownianMotionModel
  : public CovarianceModelImplementation
{

  CLASSNAME

public:

  /** Default constructor */
  FractionalBrownianMotionModel();

  /** Parameters constructor */
  FractionalBrownianMotionModel(const Scalar scale,
                                const Scalar amplitude,
                                const Scalar exponent);

  FractionalBrownianMotionModel(const Scalar scale,
                                const Point & amplitude,
                                const Point & exponent,
                                const SquareMatrix & eta,
                                const CorrelationMatrix & rho);

  /** Virtual copy constructor */
  FractionalBrownianMotionModel * clone() const override;

  /** Computation of the covariance function */
  using CovarianceModelImplementation::operator();
  SquareMatrix operator() (const Point & s, const Point & t) const override;

  /** Gradient */
  virtual Matrix partialGradient(const Point & s,
                                 const Point & t) const override;

  /** Exponent accessor */
  void setExponentEtaRho(const Point & exponent,
                         const SquareMatrix & eta,
                         const CorrelationMatrix & rho);

  Point getExponent() const;

  /** Dissymetry accessor */
  SquareMatrix getEta() const;

  /** Correlation accessor */
  CorrelationMatrix getRho() const;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /** Parameter accessor */
  void setFullParameter(const Point & parameter) override;
  Point getFullParameter() const override;
  Description getFullParameterDescription() const override;

private:
  /** The exponent of the model */
  Point exponent_;

  /** The dissymetry between the components */
  SquareMatrix eta_;

} ; /* class FractionalBrownianMotionModel */

END_NAMESPACE_OPENTURNS

#endif
