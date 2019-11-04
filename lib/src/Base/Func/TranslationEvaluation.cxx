//                                               -*- C++ -*-
/**
 * @brief Class for a numerical math function implementation
 *        of the form y = constant + x
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

#include "openturns/TranslationEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(TranslationEvaluation)

static const Factory<TranslationEvaluation> Factory_TranslationEvaluation;

/* Default constructor */
TranslationEvaluation::TranslationEvaluation()
  : EvaluationImplementation()
{
  // Nothing to do
}

/* Parameter constructor */
TranslationEvaluation::TranslationEvaluation(const Point & constant)
  : EvaluationImplementation()
  , constant_(constant)
{
  setInputDescription(Description::BuildDefault(getInputDimension(), "x"));
  setOutputDescription(Description::BuildDefault(getOutputDimension(), "y"));
}

/* Virtual constructor */
TranslationEvaluation * TranslationEvaluation::clone() const
{
  return new TranslationEvaluation(*this);
}

/* Comparison operator */
Bool TranslationEvaluation::operator ==(const TranslationEvaluation & other) const
{
  return constant_ == other.constant_;
}

/* String converter */
String TranslationEvaluation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << TranslationEvaluation::GetClassName()
      << " name=" << getName()
      << " constant=" << constant_;
  return oss;
}

String TranslationEvaluation::__str__(const String & ) const
{
  OSS oss(false);
  oss << "class=" << TranslationEvaluation::GetClassName()
      << " name=" << getName()
      << " constant=" << constant_;
  return oss;
}

/* Accessor for the constant term */
Point TranslationEvaluation::getConstant() const
{
  return constant_;
}

void TranslationEvaluation::setConstant(const Point & constant)
{
  if (constant.getDimension() != constant_.getDimension()) throw InvalidArgumentException(HERE) << "Invalid input dimension";
  constant_ = constant;
}

/* Here is the interface that all derived class must implement */

/* Operator () */
Point TranslationEvaluation::operator() (const Point & inP) const
{
  if (inP.getDimension() != constant_.getDimension()) throw InvalidArgumentException(HERE) << "Invalid input dimension";
  const Point result(inP + constant_);
  callsNumber_.increment();
  return result;
}
/* Operator () */
Sample TranslationEvaluation::operator() (const Sample & inS) const
{
  if (inS.getDimension() != constant_.getDimension()) throw InvalidArgumentException(HERE) << "Invalid input dimension";
  const Sample result((*inS.getImplementation()) + constant_);
  callsNumber_.fetchAndAdd(inS.getSize());
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger TranslationEvaluation::getInputDimension() const
{
  return constant_.getDimension();
}

/* Accessor for output point dimension */
UnsignedInteger TranslationEvaluation::getOutputDimension() const
{
  return constant_.getDimension();
}

/* Linearity accessors */
Bool TranslationEvaluation::isLinear() const
{
  return true;
}

Bool TranslationEvaluation::isLinearlyDependent(const UnsignedInteger index) const
{
  if (index >= getInputDimension())
    throw InvalidDimensionException(HERE) << "index (" << index << ") exceeds function input dimension (" << getInputDimension() << ")";

  return true;
}

/* Method save() stores the object through the StorageManager */
void TranslationEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute( "constant_", constant_ );
}

/* Method load() reloads the object from the StorageManager */
void TranslationEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute( "constant_", constant_ );
  setInputDescription(Description::BuildDefault(getInputDimension(), "x"));
  setOutputDescription(Description::BuildDefault(getOutputDimension(), "y"));
}

END_NAMESPACE_OPENTURNS
