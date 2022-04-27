//                                               -*- C++ -*-
/**
 *  @brief This class stores the current state of the fitting step in the WhittleFactory class.
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_WHITTLEFACTORYSTATE_HXX
#define OPENTURNS_WHITTLEFACTORYSTATE_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/ARMA.hxx"
#include "openturns/ARMACoefficients.hxx"
#include "openturns/Point.hxx"
#include "openturns/WhiteNoise.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class WhittleFactoryState
 *
 * The class implements the state of the WhittleFactory class that results form a particular fitting
 */
class OT_API WhittleFactoryState
  : public PersistentObject
{
  CLASSNAME

public:

  /** Default constructor */
  WhittleFactoryState();

  /** Default constructor */
  WhittleFactoryState(const UnsignedInteger p,
                      const Point & theta,
                      const Scalar sigma2,
                      const Point & informationCriteria,
                      const RegularGrid & timeGrid);

  /** Virtual constructor */
  WhittleFactoryState * clone() const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** P accessor */
  UnsignedInteger getP() const;

  /** Q accessor */
  UnsignedInteger getQ() const;

  /** Theta accessor */
  Point getTheta() const;

  /** Sigma2 accessor */
  Scalar getSigma2() const;

  /** AR coefficients accessor */
  ARMACoefficients getARCoefficients() const;

  /** MA coefficients accessor */
  ARMACoefficients getMACoefficients() const;

  /** White noise accessor */
  WhiteNoise getWhiteNoise() const;

  /** ARMA process accessor */
  ARMA getARMA() const;

  /** Information criteria accessor */
  Point getInformationCriteria() const;

  /** Time grid accessor */
  RegularGrid getTimeGrid() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private :

  /* The AR order of the ARMA process */
  UnsignedInteger p_;

  /* The flat vector of coefficients of the ARMA process */
  Point theta_;

  /* The variance of the white noise */
  Scalar sigma2_;

  /* The information criteria of the fitting */
  Point informationCriteria_;

  /* The time grid associated with the estimation */
  RegularGrid timeGrid_;

}; /* class WhittleFactoryState */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_WHITTLEFACTORYSTATE_HXX */
