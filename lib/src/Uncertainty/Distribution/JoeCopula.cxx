//                                               -*- C++ -*-
/**
 *  @brief The JoeCopula distribution
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/JoeCopula.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/Brent.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(JoeCopula)

static const Factory<JoeCopula> Factory_JoeCopula;

/* Default constructor */
JoeCopula::JoeCopula()
  : ExtremeValueCopula()
  , theta_(0.5)
  , psi1_(0.5)
  , psi2_(0.5)
{
  setName("JoeCopula");
  // We set the dimension of the JoeCopula distribution
  setDimension(2);
  // update function wrt parameters
  updatePickandFunction();
  // We don't know if the function is thread-safe and it may be called in parallel through computePDF()
  setParallel(false);
}

/* Parameters constructor */
JoeCopula::JoeCopula(const Scalar theta,
                     const Scalar psi1,
                     const Scalar psi2)
  : ExtremeValueCopula()
{
  setTheta(theta);
  setPsi1(psi1);
  setPsi2(psi2);

  setName("JoeCopula");
  // We set the dimension of the JoeCopula distribution
  setDimension(2);
  // update function wrt parameters
  updatePickandFunction();
  // We don't know if the function is thread-safe and it may be called in parallel through computePDF()
  setParallel(false);
}

/* update function wrt parameters */
void JoeCopula::updatePickandFunction()
{
  String formula(OSS() << "1-((" << psi1_ << "*(1-t))^(-1.0/" << theta_ << ")+(" << psi2_
                       << "*t)^(-1.0/"  <<  theta_    << "))^(-" <<    theta_ <<  ")");

  ExtremeValueCopula::setPickandFunction(SymbolicFunction(Description(1, "t"), Description(1, formula)), false);

  computeRange();
}


/* Comparison operator */
Bool JoeCopula::operator ==(const JoeCopula & other) const
{
  if (this == &other) return true;
  return (theta_ == other.theta_) && (psi1_ == other.psi1_) && (psi2_ == other.psi2_);
}

/* String converter */
String JoeCopula::__repr__() const
{
  OSS oss(true);
  oss << "class=" << JoeCopula::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " theta=" << theta_
      << " psi1=" << psi1_
      << " psi2=" << psi2_;
  return oss;
}

String JoeCopula::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(theta = " << theta_ << ", psi1 = " << psi1_ << ", psi2 = " << psi2_ << ")";
  return oss;
}

/* Virtual constructor */
JoeCopula * JoeCopula::clone() const
{
  return new JoeCopula(*this);
}

/* Parameters value accessor */
Point JoeCopula::getParameter() const
{
  Point result;
  result.add(theta_);
  result.add(psi1_);
  result.add(psi2_);
  return result;
}

void JoeCopula::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 3) throw InvalidArgumentException(HERE) << "Error: expected 3 values, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = JoeCopula(parameter[0], parameter[1], parameter[2]);
  setWeight(w);
}

/* Parameters description accessor */
Description JoeCopula::getParameterDescription() const
{
  Description result;
  result.add("theta");
  result.add("psi1");
  result.add("psi2");
  return result;
}

/* theta parameter accessor */
Scalar JoeCopula::getTheta() const
{
  return theta_;
}

/* psi1 parameter accessor */
Scalar JoeCopula::getPsi1() const
{
  return psi1_;
}

/* theta parameter accessor */
Scalar JoeCopula::getPsi2() const
{
  return psi2_;
}

/* theta parameter accessor */
void JoeCopula::setTheta(const Scalar theta)
{
  if (!(theta > 0.0))
    throw InvalidArgumentException(HERE) << "Theta must be positive";
  theta_ = theta;
  updatePickandFunction();
}

/* theta parameter accessor */
void JoeCopula::setPsi1(const Scalar psi1)
{
  if (!(psi1 >= 0.0) || !(psi1 <= 1.0))
    throw InvalidArgumentException(HERE) << "Psi1 must be in [0, 1]";
  psi1_ = psi1;
  updatePickandFunction();
}

/* theta parameter accessor */
void JoeCopula::setPsi2(const Scalar psi2)
{
  if (!(psi2 >= 0.0) || !(psi2 <= 1.0))
    throw InvalidArgumentException(HERE) << "Psi2 must be in [0, 1]";
  psi2_ = psi2;
  updatePickandFunction();
}

/* Method save() stores the object through the StorageManager */
void JoeCopula::save(Advocate & adv) const
{
  ExtremeValueCopula::save(adv);
  adv.saveAttribute( "theta_", theta_ );
  adv.saveAttribute( "psi1_", psi1_ );
  adv.saveAttribute( "psi2_", psi2_ );
}

/* Method load() reloads the object from the StorageManager */
void JoeCopula::load(Advocate & adv)
{
  ExtremeValueCopula::load(adv);
  adv.loadAttribute( "theta_", theta_ );
  adv.loadAttribute( "psi1_", psi1_ );
  adv.loadAttribute( "psi2_", psi2_ );
  updatePickandFunction();
}


END_NAMESPACE_OPENTURNS
