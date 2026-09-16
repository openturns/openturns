//                                               -*- C++ -*-
/**
 * @brief Penalized hessian, returning zeros on failure
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

#include "openturns/PenalizedHessian.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PenalizedHessian)

static const Factory<PenalizedHessian> Factory_PenalizedHessian;

/* Default constructor */
PenalizedHessian::PenalizedHessian()
  : HessianImplementation()
  , hessian_()
{
  // Nothing to do
}

/* Parameter constructor */
PenalizedHessian::PenalizedHessian(const Hessian & hessian)
  : HessianImplementation()
  , hessian_(hessian)
{
  // Nothing to do
}

/* Virtual constructor */
PenalizedHessian * PenalizedHessian::clone() const
{
  return new PenalizedHessian(*this);
}

/* Hessian implementation accessors */
void PenalizedHessian::setHessian(const Hessian & hessian)
{
  hessian_ = hessian;
}

Hessian PenalizedHessian::getHessian() const
{
  return hessian_;
}

/* Comparison operator */
Bool PenalizedHessian::operator ==(const PenalizedHessian & other) const
{
  return hasEqualBase(other) && (hessian_ == other.hessian_);
}

Bool PenalizedHessian::equals(const HessianImplementation & other) const
{
  return *this == dynamic_cast<const PenalizedHessian &>(other);
}

/* String converter */
String PenalizedHessian::__repr__() const
{
  return OSS(true) << "PenalizedHessian(" << hessian_.getImplementation()->__repr__() << ")";
}

String PenalizedHessian::__str__(const String & offset) const
{
  return OSS(false) << offset << "PenalizedHessian(" << hessian_.getImplementation()->__str__() << ")";
}

/* Hessian method: zeros are consistent with the penalized evaluation being locally constant on failure */
SymmetricTensor PenalizedHessian::hessian(const Point & inP) const
{
  try
  {
    const SymmetricTensor result(hessian_.hessian(inP));
    callsNumber_.increment();
    return result;
  }
  catch (const InterruptionException &)
  {
    throw;
  }
  catch (const std::exception & exc)
  {
    LOGDEBUG(OSS() << "PenalizedHessian: caught exception at point " << inP.__str__() << ": " << exc.what() << ", returning zeros");
    callsNumber_.increment();
    return SymmetricTensor(getInputDimension(), getOutputDimension());
  }
}

/* Test for actual implementation */
Bool PenalizedHessian::isActualImplementation() const
{
  return hessian_.getImplementation()->isActualImplementation();
}

/* Get the i-th marginal hessian */
Hessian PenalizedHessian::getMarginal(const UnsignedInteger i) const
{
  return getMarginal(Indices(1, i));
}

/* Get the hessian corresponding to indices components */
Hessian PenalizedHessian::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal hessian must be in the range [0, outputDimension-1] and must be different";
  return new PenalizedHessian(hessian_.getMarginal(indices));
}

/* Accessor for input point dimension */
UnsignedInteger PenalizedHessian::getInputDimension() const
{
  return hessian_.getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger PenalizedHessian::getOutputDimension() const
{
  return hessian_.getOutputDimension();
}

/* Parameters value accessor */
Point PenalizedHessian::getParameter() const
{
  return hessian_.getParameter();
}

void PenalizedHessian::setParameter(const Point & parameter)
{
  hessian_.setParameter(parameter);
}

/* Method save() stores the object through the StorageManager */
void PenalizedHessian::save(Advocate & adv) const
{
  HessianImplementation::save(adv);
  adv.saveAttribute("hessian_", hessian_);
}

/* Method load() reloads the object from the StorageManager */
void PenalizedHessian::load(Advocate & adv)
{
  HessianImplementation::load(adv);
  adv.loadAttribute("hessian_", hessian_);
}

END_NAMESPACE_OPENTURNS
