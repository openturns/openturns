//                                               -*- C++ -*-
/**
 *  @brief Monomial function implementation
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
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/MonomialFunction.hxx"
#include "openturns/OSS.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MonomialFunction)

static const Factory<MonomialFunction> Factory_MonomialFunction;


/* Default constructor */
MonomialFunction::MonomialFunction()
  : UniVariateFunctionImplementation()
  , degree_(0)
{
  // Nothing to do
}

/** Standard constructor */
MonomialFunction::MonomialFunction(const UnsignedInteger degree)
  : UniVariateFunctionImplementation()
  , degree_(degree)
{
  // Nothing to do
}

/* Virtual constructor */
MonomialFunction * MonomialFunction::clone() const
{
  return new MonomialFunction(*this);
}


/* String converter */
String MonomialFunction::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << " degree=" << degree_;
}


String MonomialFunction::__str__(const String & ) const
{
  // Pretty print
  if (degree_ == 0)
    return OSS(true) << "[x] -> 1"  ;
  else if (degree_ == 1)
  {
    return OSS(true) << "[x] --> x" ;
  }
  return OSS(true) << "[x] --> x^" << degree_ ;
}

/* MonomialFunction are evaluated as functors */
Scalar MonomialFunction::operator() (const Scalar x) const
{
  // Evaluate x ^n
  return std::pow(x, 1.0 * degree_);
}

/* MonomialFunction gradient */
Scalar MonomialFunction::gradient(const Scalar x) const
{
  // Evaluate gradien = n * x ^{n-1}
  if (degree_ == 0) return 0.0;
  return degree_ * std::pow(x, 1.0 * degree_ - 1.0);
}

/* MonomialFunction hessian */
Scalar MonomialFunction::hessian(const Scalar x) const
{
  if (degree_ < 2) return 0.0;
  return degree_ * (degree_ - 1) * std::pow(x, 1.0 * degree_ - 2.0);
}

/* Method save() stores the object through the StorageManager */
void MonomialFunction::save(Advocate & adv) const
{
  UniVariateFunctionImplementation::save(adv);
  adv.saveAttribute( "degree_", degree_ );
}

/* Method load() reloads the object from the StorageManager */
void MonomialFunction::load(Advocate & adv)
{
  UniVariateFunctionImplementation::load(adv);
  adv.loadAttribute( "degree_", degree_ );
}


END_NAMESPACE_OPENTURNS
