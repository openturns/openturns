//                                               -*- C++ -*-
/**
 *  @brief The GalambosCopula distribution
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
#include <cmath>
#include "openturns/GalambosCopula.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/SymbolicFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GalambosCopula)

static const Factory<GalambosCopula> Factory_GalambosCopula;

/* Default constructor */
GalambosCopula::GalambosCopula()
  : ExtremeValueCopula()
  , theta_(0.5)
{
  setName("GalambosCopula");
  // We set the dimension of the GalambosCopula distribution
  setDimension(2);
  // update function wrt parameters
  updatePickandFunction();
  // We don't know if the function is thread-safe and it may be called in parallel through computePDF()
  setParallel(false);
}

/* Parameters constructor */
GalambosCopula::GalambosCopula(const Scalar theta)
  : ExtremeValueCopula()
  , theta_(0.0) // To force the update in the accessor
{
  setTheta(theta);

  setName("GalambosCopula");
  // We set the dimension of the GalambosCopula distribution
  setDimension(2);
  // update function wrt parameters
  updatePickandFunction();
  // We don't know if the function is thread-safe and it may be called in parallel through computePDF()
  setParallel(false);
}

/* update function wrt parameters */
void GalambosCopula::updatePickandFunction()
{
  String formula(OSS() << "1-(t^(-" << theta_ << ") + (1-t)^(-" << theta_ << "))^(-1.0/" << theta_ << ")");

  ExtremeValueCopula::setPickandFunction(SymbolicFunction(Description(1, "t"), Description(1, formula)), false);

  computeRange();
}


/* Comparison operator */
Bool GalambosCopula::operator ==(const GalambosCopula & other) const
{
  if (this == &other) return true;
  return (theta_ == other.theta_);
}

/* String converter */
String GalambosCopula::__repr__() const
{
  OSS oss(true);
  oss << "class=" << GalambosCopula::GetClassName()
      << ", name=" << getName()
      << ", dimension=" << getDimension()
      << ", theta=" << theta_;
  return oss;
}

String GalambosCopula::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(theta = " << theta_ << ")";
  return oss;
}

/* Virtual constructor */
GalambosCopula * GalambosCopula::clone() const
{
  return new GalambosCopula(*this);
}

/* Parameters value accessor */
Point GalambosCopula::getParameter() const
{
  Point result;
  result.add(theta_);
  return result;
}

void GalambosCopula::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 1) throw InvalidArgumentException(HERE) << "Error: expected at least 1 value, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = GalambosCopula(parameter[0]);
  setWeight(w);
}

/* Parameters description accessor */
Description GalambosCopula::getParameterDescription() const
{
  Description result;
  result.add("theta");
  return result;
}

/* theta parameter accessor */
Scalar GalambosCopula::getTheta() const
{
  return theta_;
}

/* theta parameter accessor */
void GalambosCopula::setTheta(const Scalar theta)
{
  if (!(theta > 0.0))
    throw InvalidArgumentException(HERE) << "Theta must be positive";
  if (theta != theta_)
  {
    theta_ = theta;
    updatePickandFunction();
  }
}

/* Tell if the distribution has independent copula */
Bool GalambosCopula::hasIndependentCopula() const
{
  return (theta_ == 0.0);
}

/* Method save() stores the object through the StorageManager */
void GalambosCopula::save(Advocate & adv) const
{
  ExtremeValueCopula::save(adv);
  adv.saveAttribute( "theta_", theta_ );
}

/* Method load() reloads the object from the StorageManager */
void GalambosCopula::load(Advocate & adv)
{
  ExtremeValueCopula::load(adv);
  adv.loadAttribute( "theta_", theta_ );
  updatePickandFunction();
}


END_NAMESPACE_OPENTURNS
