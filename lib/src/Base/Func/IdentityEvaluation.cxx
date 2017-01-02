//                                               -*- C++ -*-
/**
 * @brief Class for identity evaluation.
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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

#include "openturns/IdentityEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(IdentityEvaluation);

static const Factory<IdentityEvaluation> Factory_IdentityEvaluation;

/* Parameter constructor */
IdentityEvaluation::IdentityEvaluation(const UnsignedInteger dimension)
  : NumericalMathEvaluationImplementation()
  , dimension_(dimension)
{
  /* Check if the dimension of the constant term is compatible with the linear term */
  if (dimension_ == 0) throw InvalidDimensionException(HERE) << "Error: cannot build an identity evaluation of dimension 0";
  setInputDescription(Description::BuildDefault(dimension, "x"));
  setOutputDescription(Description::BuildDefault(dimension, "y"));
}

/* Virtual constructor */
IdentityEvaluation * IdentityEvaluation::clone() const
{
  return new IdentityEvaluation(*this);
}

/* Comparison operator */
Bool IdentityEvaluation::operator ==(const IdentityEvaluation & other) const
{
  return dimension_ == other.dimension_;
}

/* String converter */
String IdentityEvaluation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << IdentityEvaluation::GetClassName()
      << " name=" << getName()
      << " dimension=" << dimension_;
  return oss;
}

String IdentityEvaluation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << IdentityEvaluation::GetClassName()
      << "(dimension=" << dimension_
      << ")";
  return oss;
}

/* Here is the interface that all derived class must implement */

/* Operator () */
NumericalPoint IdentityEvaluation::operator() (const NumericalPoint & inP) const
{
  if (inP.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Invalid input dimension";
  ++callsNumber_;
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inP);
    outputStrategy_.store(inP);
  }
  return inP;
}
/* Operator () */
NumericalSample IdentityEvaluation::operator() (const NumericalSample & inS) const
{
  if (inS.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Invalid input dimension";
  callsNumber_ += inS.getSize();
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inS);
    outputStrategy_.store(inS);
  }
  return inS;
}

/* Accessor for input point dimension */
UnsignedInteger IdentityEvaluation::getInputDimension() const
{
  return dimension_;
}

/* Accessor for output point dimension */
UnsignedInteger IdentityEvaluation::getOutputDimension() const
{
  return dimension_;
}

/* Method save() stores the object through the StorageManager */
void IdentityEvaluation::save(Advocate & adv) const
{
  NumericalMathEvaluationImplementation::save(adv);
  adv.saveAttribute( "dimension_", dimension_ );
}

/* Method load() reloads the object from the StorageManager */
void IdentityEvaluation::load(Advocate & adv)
{
  NumericalMathEvaluationImplementation::load(adv);
  adv.loadAttribute( "dimension_", dimension_ );
}

END_NAMESPACE_OPENTURNS
