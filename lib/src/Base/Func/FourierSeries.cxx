//                                               -*- C++ -*-
/**
 *  @brief Fourier series function implementation
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/FourierSeries.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FourierSeries)

static const Factory<FourierSeries> Factory_FourierSeries;


/* Parameter constructor */
FourierSeries::FourierSeries(const Bool isCosine,
                             const UnsignedInteger k)
  : UniVariateFunctionImplementation()
  , isCosine_(isCosine)
  , k_(k)
{
  // Nothing to do
}


/* Virtual constructor */
FourierSeries * FourierSeries::clone() const
{
  return new FourierSeries(*this);
}


/* String converter */
String FourierSeries::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " isCosine=" << isCosine_
         << " k=" << k_;
}


String FourierSeries::__str__(const String & ) const
{
  OSS oss(false);
  if (k_ == 0)
    oss << "f:X -> 1";
  else if (k_ == 1)
    oss << "f:X -> sqrt(2) * " << (isCosine_ ? "cos(X)" : "sin(X)");
  else
    oss << "f:X -> sqrt(2) * " << (isCosine_ ? "cos(" : "sin(") << k_ << " * X)";
  return oss;
}


/* FourierSeries are evaluated as functors */
Scalar FourierSeries::operator() (const Scalar x) const
{
  const Scalar coef = k_ == 0 ? 1.0 : M_SQRT2;
  return coef * (isCosine_ ? cos(k_ * x) : sin(k_ * x));
}

/* FourierSeries gradient */
Scalar FourierSeries::gradient(const Scalar x) const
{
  if (k_ == 0) return 0.0;
  return M_SQRT2 * k_ * (isCosine_ ? -sin(k_ * x) : cos(k_ * x));
}

/* FourierSeries hessian */
Scalar FourierSeries::hessian(const Scalar x) const
{
  if (k_ == 0) return 0.0;
  return M_SQRT2 * k_ * k_ * (isCosine_ ? -cos(k_ * x) : -sin(k_ * x));
}

/* Method save() stores the object through the StorageManager */
void FourierSeries::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("isCosine_", isCosine_);
  adv.saveAttribute("k_", k_);
}

/* Method load() reloads the object from the StorageManager */
void FourierSeries::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("isCosine_", isCosine_);
  adv.loadAttribute("k_", k_);
}


END_NAMESPACE_OPENTURNS
