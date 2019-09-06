//                                               -*- C++ -*-
/**
 * @brief Marginal evaluation
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

#include "openturns/MarginalEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(MarginalEvaluation)

static const Factory<MarginalEvaluation> Factory_MarginalEvaluation;

/* Default constructor */
MarginalEvaluation::MarginalEvaluation()
  : EvaluationImplementation()
  , p_evaluation_(new EvaluationImplementation)
{
  // Nothing to do
}

/* Default constructor */
MarginalEvaluation::MarginalEvaluation(const EvaluationImplementation & evaluation,
                                       const Indices & indices)
  : EvaluationImplementation()
{
  *this = MarginalEvaluation(evaluation.clone(), indices);
}

/* Default constructor */
MarginalEvaluation::MarginalEvaluation(const Pointer<EvaluationImplementation> & p_evaluation,
                                       const Indices & indices)
  : EvaluationImplementation()
  , p_evaluation_(p_evaluation)
  , indices_(indices)
{
  if (!indices.check(p_evaluation->getOutputDimension()))
    throw InvalidArgumentException(HERE) << "Error: the indices of a marginal function must be in the range [0, outputDimension-1] and must be different";
  setInputDescription(p_evaluation->getInputDescription());
  Description marginalOutputDescription(indices.getSize());
  Description outputDescription(p_evaluation->getOutputDescription());
  for (UnsignedInteger j = 0; j < getOutputDimension(); ++ j)
  {
    marginalOutputDescription[j] = outputDescription[indices[j]];
  }
  setOutputDescription(marginalOutputDescription);
}

/* Virtual constructor */
MarginalEvaluation * MarginalEvaluation::clone() const
{
  return new MarginalEvaluation(*this);
}

/* Comparison operator */
Bool MarginalEvaluation::operator ==(const MarginalEvaluation & ) const
{
  return true;
}

/* String converter */
String MarginalEvaluation::__repr__() const
{
  OSS oss;
  oss << "class=" << MarginalEvaluation::GetClassName()
      << " evaluation=" << p_evaluation_->__repr__()
      << " indices=" << indices_;
  return oss;
}

/* Indices accessor */
Indices MarginalEvaluation::getIndices() const
{
  return indices_;
}

/* Here is the interface that all derived class must implement */


/* Method save() stores the object through the StorageManager */
void MarginalEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute("evaluation_", *p_evaluation_);
  adv.saveAttribute("indices_", indices_);
}

/* Method load() reloads the object from the StorageManager */
void MarginalEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  TypedInterfaceObject<EvaluationImplementation> evaluation;
  adv.loadAttribute("evaluation_", evaluation);
  p_evaluation_ = evaluation.getImplementation();
  adv.loadAttribute("indices_", indices_);
}

/* Operator () */
Point MarginalEvaluation::operator() (const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  Point result(getOutputDimension());
  const Point value(p_evaluation_->operator()(inP));
  callsNumber_.increment();
  for (UnsignedInteger j = 0; j < getOutputDimension(); ++ j)
  {
    result[j] = value[indices_[j]];
  }
  return result;
}

/* Operator () */
Sample MarginalEvaluation::operator() (const Sample & inSample) const
{
  const UnsignedInteger size = inSample.getSize();
  const UnsignedInteger inputDimension = p_evaluation_->getInputDimension();
  const UnsignedInteger sampleDimension = inSample.getDimension();
  if (sampleDimension != inputDimension) throw InvalidArgumentException(HERE) << "Error: expected a sample of dimension=" << inputDimension << ", got dimension=" << sampleDimension;
  const Sample fullOutput(p_evaluation_->operator()(inSample));
  callsNumber_.fetchAndAdd(size);
  return fullOutput.getMarginal(indices_);
}

/* Accessor for input point dimension */
UnsignedInteger MarginalEvaluation::getInputDimension() const
{
  return p_evaluation_->getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger MarginalEvaluation::getOutputDimension() const
{
  return indices_.getSize();
}

/* Gradient according to the marginal parameters */
Matrix MarginalEvaluation::parameterGradient(const Point & inP) const
{
  const Matrix fullParameterGradient(p_evaluation_->parameterGradient(inP));
  Matrix result(getParameter().getSize(), getOutputDimension());
  for (UnsignedInteger i = 0; i < getParameter().getSize(); ++ i)
    for (UnsignedInteger j = 0; j < getOutputDimension(); ++ j)
      result(i, j) = fullParameterGradient(i, indices_[j]);
  return result;
}

/* Parameters value accessor */
Point MarginalEvaluation::getParameter() const
{
  return p_evaluation_->getParameter();
}

void MarginalEvaluation::setParameter(const Point & parameters)
{
  p_evaluation_->setParameter(parameters);
}

/* Parameters description accessor */
Description MarginalEvaluation::getParameterDescription() const
{
  return p_evaluation_->getParameterDescription();
}

void MarginalEvaluation::setParameterDescription(const Description & description)
{
  p_evaluation_->setParameterDescription(description);
}


END_NAMESPACE_OPENTURNS
