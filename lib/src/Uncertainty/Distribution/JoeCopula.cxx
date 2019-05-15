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
  , theta_(0.5), psi1_(0.5), psi2_(0.5)
{
  setName( "JoeCopula" );
  // We set the dimension of the ExtremeValueCopula distribution
  setDimension( 2 );
  Description inVar(1);
  inVar[0] = "t";

  String theta_str = String(OSS() << theta_);
  String psi1_str = String(OSS() << psi1_);
  String psi2_str = String(OSS() << psi2_);

  Description formula(1);
  formula[0] = String(OSS() << "1-((" << psi1_str 
	                    << "*(1-t))^(-1.0/" << theta_str << ")+(" << psi2_str 
			    << "*t)^(-1.0/"  <<  theta_str    << "))^(-" <<    theta_str <<  ")");

  setPickandFunction(SymbolicFunction(inVar, formula));
  
  computeRange();
  // We don't know if the function is thread-safe and it may be called in parallel through computePDF()
  setParallel(false);
}

/* Parameters constructor */
JoeCopula::JoeCopula(const Scalar theta, const Scalar psi1, const Scalar psi2)
  : ExtremeValueCopula()
  , theta_(theta), psi1_(psi1), psi2_(psi2)
{
  setName( "JoeCopula" );
  // We set the dimension of the JoeCopula distribution
  setDimension( 2 );
  Description inVar(1);
  inVar[0] = "t";

  String theta_str = String(OSS() << theta);
  String psi1_str = String(OSS() << psi1);
  String psi2_str = String(OSS() << psi2);

  Description formula(1);
  formula[0] = String(OSS() << "1-((" << psi1_str 
	                    << "*(1-t))^(-1.0/" << theta_str << ")+(" << psi2_str 
			    << "*t)^(-1.0/"  <<  theta_str    << "))^(-" <<    theta_str <<  ")");

  setPickandFunction(SymbolicFunction(inVar, formula));

  computeRange();
  // We don't know if the function is thread-safe and it may be called in parallel through computePDF()
  setParallel(false);
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

/* Pickand function accessor */
void JoeCopula::setPickandFunction(const Function & pickandFunction)
{
  ExtremeValueCopula::setPickandFunction(pickandFunction,false);
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
  theta_ = theta;
}

/* theta parameter accessor */
void JoeCopula::setPsi1(const Scalar psi1)
{
  theta_ = psi1;
}

/* theta parameter accessor */
void JoeCopula::setPsi2(const Scalar psi2)
{
  psi2_ = psi2;
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
  computeRange();
}


END_NAMESPACE_OPENTURNS
