//                                               -*- C++ -*-
/**
 * @brief Class for constant evaluation.
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/ConstantEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Function.hxx" // for Evaluation

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ConstantEvaluation)

static const Factory<ConstantEvaluation> Factory_ConstantEvaluation;

/* Parameter constructor */
ConstantEvaluation::ConstantEvaluation(const UnsignedInteger inputDimension,
                                       const Point & constant)
  : EvaluationImplementation()
  , inputDimension_(inputDimension)
  , constant_(constant)
{
  /* Check if the dimension of the constant term is compatible with the linear term */
  if (!constant.getDimension()) throw InvalidDimensionException(HERE) << "Error: cannot build a constant evaluation of dimension 0";
  setInputDescription(Description::BuildDefault(inputDimension, "x"));
  setOutputDescription(Description::BuildDefault(constant.getDimension(), "y"));
}

/* Virtual constructor */
ConstantEvaluation * ConstantEvaluation::clone() const
{
  return new ConstantEvaluation(*this);
}

/* Comparison operator */
Bool ConstantEvaluation::operator ==(const ConstantEvaluation & other) const
{
  return inputDimension_ == other.inputDimension_ && constant_ == other.constant_;
}

/* String converter */
String ConstantEvaluation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << ConstantEvaluation::GetClassName()
      << " name=" << getName()
      << " constant=" << constant_;
  return oss;
}

String ConstantEvaluation::__str__(const String & ) const
{
  OSS oss(false);
  oss << getInputDescription().__str__() << "->" << constant_.__str__();
  return oss;
}

/* Here is the interface that all derived class must implement */

/* Operator () */
Point ConstantEvaluation::operator() (const Point & inP) const
{
  if (inP.getDimension() != getInputDimension()) throw InvalidArgumentException(HERE) << "Invalid input dimension";
  callsNumber_.increment();
  return constant_;
}
/* Operator () */
Sample ConstantEvaluation::operator() (const Sample & inS) const
{
  if (inS.getDimension() != getInputDimension()) throw InvalidArgumentException(HERE) << "Invalid input dimension";
  callsNumber_.fetchAndAdd(inS.getSize());
  return Sample(inS.getSize(), constant_);
}

/* Accessor for input point dimension */
UnsignedInteger ConstantEvaluation::getInputDimension() const
{
  return inputDimension_;
}

/* Accessor for output point dimension */
UnsignedInteger ConstantEvaluation::getOutputDimension() const
{
  return constant_.getDimension();
}

Evaluation ConstantEvaluation::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal evaluation must be in the range [0, outputDimension-1] and must be different";
  return new ConstantEvaluation(inputDimension_, constant_.select(indices));
}


/* Method save() stores the object through the StorageManager */
void ConstantEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute( "inputDimension_", inputDimension_ );
  adv.saveAttribute( "constant_", constant_ );
}

/* Method load() reloads the object from the StorageManager */
void ConstantEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute( "inputDimension_", inputDimension_ );
  adv.loadAttribute( "constant_", constant_ );
}

/* Constant accessor */
Point ConstantEvaluation::getConstant() const
{
  return constant_;
}

END_NAMESPACE_OPENTURNS
