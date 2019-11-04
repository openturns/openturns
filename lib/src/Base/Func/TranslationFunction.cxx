//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for the translation function
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/TranslationFunction.hxx"
#include "openturns/Function.hxx"
#include "openturns/TranslationEvaluation.hxx"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/ConstantGradient.hxx"
#include "openturns/NullHessian.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(TranslationFunction)

/* Default constructor */
TranslationFunction::TranslationFunction(const Point & constant)
  : Function(new TranslationEvaluation(constant),
             new ConstantGradient(IdentityMatrix(constant.getSize())),
             new NullHessian(constant.getSize(), constant.getSize()))
{
  // Nothing to do
}

/* Comparison operator */
Bool TranslationFunction::operator ==(const TranslationFunction & other) const
{
  if (this == &other) return true;
  return *getImplementation() == *other.getImplementation();
}

/* String converter */
String TranslationFunction::__repr__() const
{
  OSS oss(true);
  oss << "class=" << TranslationFunction::GetClassName()
      << " name=" << getName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

String TranslationFunction::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "class=" << TranslationFunction::GetClassName()
      << " name=" << getName()
      << " implementation=" << getImplementation()->__str__(offset);
  return oss;
}

/* Accessor for the constant term */
Point TranslationFunction::getConstant() const
{
  return dynamic_cast<TranslationEvaluation*>(getImplementation()->getEvaluation().getImplementation().get())->getConstant();
}

void TranslationFunction::setConstant(const Point & constant)
{
  copyOnWrite();
  dynamic_cast<TranslationEvaluation*>(getImplementation()->getEvaluation().getImplementation().get())->setConstant(constant);
}

END_NAMESPACE_OPENTURNS
