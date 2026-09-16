//                                               -*- C++ -*-
/**
 * @brief Penalized gradient, returning zeros on failure
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

#include "openturns/PenalizedGradient.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PenalizedGradient)

static const Factory<PenalizedGradient> Factory_PenalizedGradient;

/* Default constructor */
PenalizedGradient::PenalizedGradient()
  : GradientImplementation()
  , gradient_()
{
  // Nothing to do
}

/* Parameter constructor */
PenalizedGradient::PenalizedGradient(const Gradient & gradient)
  : GradientImplementation()
  , gradient_(gradient)
{
  // Nothing to do
}

/* Virtual constructor */
PenalizedGradient * PenalizedGradient::clone() const
{
  return new PenalizedGradient(*this);
}

/* Gradient implementation accessors */
void PenalizedGradient::setGradient(const Gradient & gradient)
{
  gradient_ = gradient;
}

Gradient PenalizedGradient::getGradient() const
{
  return gradient_;
}

/* Comparison operator */
Bool PenalizedGradient::operator ==(const PenalizedGradient & other) const
{
  return hasEqualBase(other) && (gradient_ == other.gradient_);
}

Bool PenalizedGradient::equals(const GradientImplementation & other) const
{
  return *this == dynamic_cast<const PenalizedGradient &>(other);
}

/* String converter */
String PenalizedGradient::__repr__() const
{
  return OSS(true) << "PenalizedGradient(" << gradient_.getImplementation()->__repr__() << ")";
}

String PenalizedGradient::__str__(const String & offset) const
{
  return OSS(false) << offset << "PenalizedGradient(" << gradient_.getImplementation()->__str__() << ")";
}

/* Gradient method: zeros are consistent with the penalized evaluation being locally constant on failure */
Matrix PenalizedGradient::gradient(const Point & inP) const
{
  try
  {
    const Matrix result(gradient_.gradient(inP));
    callsNumber_.increment();
    return result;
  }
  catch (const InterruptionException &)
  {
    throw;
  }
  catch (const std::exception & exc)
  {
    LOGDEBUG(OSS() << "PenalizedGradient: caught exception at point " << inP.__str__() << ": " << exc.what() << ", returning zeros");
    callsNumber_.increment();
    return Matrix(getInputDimension(), getOutputDimension());
  }
}

/* Test for actual implementation */
Bool PenalizedGradient::isActualImplementation() const
{
  return gradient_.getImplementation()->isActualImplementation();
}

/* Get the i-th marginal gradient */
Gradient PenalizedGradient::getMarginal(const UnsignedInteger i) const
{
  return getMarginal(Indices(1, i));
}

/* Get the gradient corresponding to indices components */
Gradient PenalizedGradient::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal gradient must be in the range [0, outputDimension-1] and must be different";
  return new PenalizedGradient(gradient_.getMarginal(indices));
}

/* Accessor for input point dimension */
UnsignedInteger PenalizedGradient::getInputDimension() const
{
  return gradient_.getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger PenalizedGradient::getOutputDimension() const
{
  return gradient_.getOutputDimension();
}

/* Parameters value accessor */
Point PenalizedGradient::getParameter() const
{
  return gradient_.getParameter();
}

void PenalizedGradient::setParameter(const Point & parameter)
{
  gradient_.setParameter(parameter);
}

/* Method save() stores the object through the StorageManager */
void PenalizedGradient::save(Advocate & adv) const
{
  GradientImplementation::save(adv);
  adv.saveAttribute("gradient_", gradient_);
}

/* Method load() reloads the object from the StorageManager */
void PenalizedGradient::load(Advocate & adv)
{
  GradientImplementation::load(adv);
  adv.loadAttribute("gradient_", gradient_);
}

END_NAMESPACE_OPENTURNS
