//                                               -*- C++ -*-
/**
 *  @brief Rank-1 tensor evaluation
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */
#include "openturns/RankOneTensorEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(RankOneTensorEvaluation);

static const Factory<RankOneTensorEvaluation> Factory_RankOneTensorEvaluation;


/* Default constructor */
RankOneTensorEvaluation::RankOneTensorEvaluation()
  : NumericalMathEvaluationImplementation()
{
  // Nothing to do
}

/* Parameter constructor */
RankOneTensorEvaluation::RankOneTensorEvaluation(const RankOneTensor & rankOneTensor)
  : NumericalMathEvaluationImplementation()
  , rankOneTensor_(rankOneTensor)
{
//   const UnsignedInteger inputDimension(function.getInputDimension());
//   const UnsignedInteger setDimension(set.getSize());

//   setInputDescription(inputDescription);
//   setOutputDescription(function_.getOutputDescription());
}

/* Virtual constructor method */
RankOneTensorEvaluation * RankOneTensorEvaluation::clone() const
{
  return new RankOneTensorEvaluation(*this);
}

/* Evaluation operator */
NumericalPoint RankOneTensorEvaluation::operator() (const NumericalPoint & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << inputDimension << ", got dimension=" << inP.getDimension();

  NumericalScalar prodI = 1.0;
  for (UnsignedInteger i = 0; i < inputDimension; ++ i)
  {
    const NumericalPoint xi(1, inP[i]);
 
    NumericalScalar sumI = 0.0;
    const UnsignedInteger basisSize = rankOneTensor_.coefficients_[i].getSize();
    for (UnsignedInteger j = 0; j < basisSize; ++ j)
    {
      sumI += rankOneTensor_.coefficients_[i][j] * rankOneTensor_.basis_[i][j](xi)[0];
    }
    prodI *= sumI;
  }

  const NumericalPoint outP(1, prodI);
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inP);
    outputStrategy_.store(outP);
  }
  ++ callsNumber_;
  return outP;
}

/* Dimension accessor */
UnsignedInteger RankOneTensorEvaluation::getInputDimension() const
{
  return rankOneTensor_.functionFamilies_.getSize();
}

UnsignedInteger RankOneTensorEvaluation::getOutputDimension() const
{
  return 1;
}

/* String converter */
String RankOneTensorEvaluation::__repr__() const
{
  OSS oss;
  oss << "class=" << RankOneTensorEvaluation::GetClassName();
  return oss;
}

String RankOneTensorEvaluation::__str__(const String & offset) const
{
  OSS oss;
  oss << offset << RankOneTensorEvaluation::GetClassName()
      << "(" << rankOneTensor_ << ")";
  return oss;
}

/* Method save() stores the object through the StorageManager */
void RankOneTensorEvaluation::save(Advocate & adv) const
{
  NumericalMathEvaluationImplementation::save( adv );
//   adv.saveAttribute( "function_", function_ );
}

/* Method load() reloads the object from the StorageManager */
void RankOneTensorEvaluation::load(Advocate & adv)
{
  NumericalMathEvaluationImplementation::load( adv );
//   adv.loadAttribute( "function_", function_ );
}


END_NAMESPACE_OPENTURNS
