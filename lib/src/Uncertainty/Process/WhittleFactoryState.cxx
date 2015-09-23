//                                               -*- C++ -*-
/**
 *  @brief This class stores the current state of the fitting step in the WhittleFactory class.
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
#include "PersistentObjectFactory.hxx"
#include "WhittleFactoryState.hxx"
#include "Normal.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(WhittleFactoryState);

static const Factory<WhittleFactoryState> RegisteredFactory;


/* Default constructor */
WhittleFactoryState::WhittleFactoryState()
  : PersistentObject()
  , p_(0)
  , theta_(0)
  , sigma2_(0.0)
  , informationCriteria_(0)
  , timeGrid_()
{
  // Nothing to do
}

/* Standard constructor */
WhittleFactoryState::WhittleFactoryState(const UnsignedInteger p,
    const NumericalPoint & theta,
    const NumericalScalar sigma2,
    const NumericalPoint & informationCriteria,
    const RegularGrid & timeGrid)
  : PersistentObject()
  , p_(p)
  , theta_(theta)
  , sigma2_(sigma2)
  , informationCriteria_(informationCriteria)
  , timeGrid_(timeGrid)
{
  // Check the variance
  if (sigma2_ <= 0.0) throw InvalidArgumentException(HERE) << "Error: the given variance must be positive, here sigma2=" << sigma2;
  // Check the AR order with respect to the number of parameters
  if (p > theta.getSize()) throw InvalidArgumentException(HERE) << "Error: the AR order p=" << p << " is greater than the number of parameters n=" << theta.getSize();
}

/* Virtual constructor */
WhittleFactoryState * WhittleFactoryState::clone() const
{
  return new WhittleFactoryState(*this);
}

String WhittleFactoryState::__repr__() const
{
  OSS oss;
  oss << "class= " << WhittleFactoryState::GetClassName()
      << " p=" << p_
      << " theta=" << theta_
      << " sigma2=" << sigma2_
      << " informationCriteria=" << informationCriteria_;
  return oss;
}

/* String converter */
String WhittleFactoryState::__str__(const String & offset) const
{
  OSS oss;
  oss << getARMA().__str__(offset) << "\n";
  oss << offset << "Information criteria:\n";
  oss << offset << "AICc = " << informationCriteria_[0] << "\n";
  oss << offset << "AIC  = " << informationCriteria_[1] << "\n";
  oss << offset << "BIC  = " << informationCriteria_[2];
  return oss;
}

/* P accessor */
UnsignedInteger WhittleFactoryState::getP() const
{
  return p_;
}

/* Q accessor */
UnsignedInteger WhittleFactoryState::getQ() const
{
  return theta_.getSize() - p_;
}

/* Theta accessor */
NumericalPoint WhittleFactoryState::getTheta() const
{
  return theta_;
}

/* Sigma2 accessor */
NumericalScalar WhittleFactoryState::getSigma2() const
{
  return sigma2_;
}

/* AR coefficients accessor */
ARMACoefficients WhittleFactoryState::getARCoefficients() const
{
  NumericalPoint arCoefficients(p_);
  for (UnsignedInteger k = 0; k < p_; ++k) arCoefficients[k] = theta_[k];
  return ARMACoefficients(arCoefficients);
}

/* MA coefficients accessor */
ARMACoefficients WhittleFactoryState::getMACoefficients() const
{
  const UnsignedInteger q(getQ());
  NumericalPoint maCoefficients(q);
  for (UnsignedInteger k = 0; k < q; ++k) maCoefficients[k] = theta_[k + p_];
  return ARMACoefficients(maCoefficients);
}

/* White noise accessor */
WhiteNoise WhittleFactoryState::getWhiteNoise() const
{
  return WhiteNoise(Normal(0.0, sqrt(sigma2_)));
}

/* ARMA process accessor */
ARMA WhittleFactoryState::getARMA() const
{
  ARMA process(getARCoefficients(), getMACoefficients(), getWhiteNoise());
  process.setTimeGrid(timeGrid_);
  return process;
}

/* Information criteria accessor */
NumericalPoint WhittleFactoryState::getInformationCriteria() const
{
  return informationCriteria_;
}

/* Time grid accessor */
RegularGrid WhittleFactoryState::getTimeGrid() const
{
  return timeGrid_;
}

/* Method save() stores the object through the StorageManager */
void WhittleFactoryState::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "p_", p_);
  adv.saveAttribute( "theta_", theta_);
  adv.saveAttribute( "sigma2_", sigma2_);
  adv.saveAttribute( "informationCriteria_", informationCriteria_);
  adv.saveAttribute( "timeGrid_", timeGrid_);
}

/* Method load() reloads the object from the StorageManager */
void WhittleFactoryState::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "p_", p_);
  adv.loadAttribute( "theta_", theta_);
  adv.loadAttribute( "sigma2_", sigma2_);
  adv.loadAttribute( "informationCriteria_", informationCriteria_);
  adv.loadAttribute( "timeGrid_", timeGrid_);
}

END_NAMESPACE_OPENTURNS
