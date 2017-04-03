//                                               -*- C++ -*-
/**
 *  @brief This is the evaluation part of the Soize-Ghanem basis coupling factor
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
#include "openturns/SoizeGhanemFactorEvaluation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SoizeGhanemFactorEvaluation);

static const Factory<SoizeGhanemFactorEvaluation> Factory_SoizeGhanemFactorEvaluation;

/* Default constructor */
SoizeGhanemFactorEvaluation::SoizeGhanemFactorEvaluation()
  : EvaluationImplementation()
  , measure_()
  , useCopula_()
  , copula_()
  , marginals_()
{
  // Nothing to do
}


/* Constructor */
SoizeGhanemFactorEvaluation::SoizeGhanemFactorEvaluation(const Distribution & measure,
    const Collection<Distribution> & marginals,
    const Bool useCopula)
  : EvaluationImplementation()
  , measure_(measure)
  , useCopula_(useCopula)
  , copula_()
  , marginals_(marginals)
{
  // Extract the copula if needed
  if (useCopula_) copula_ = measure.getCopula();
}


/* Virtual constructor */
SoizeGhanemFactorEvaluation * SoizeGhanemFactorEvaluation::clone() const
{
  return new SoizeGhanemFactorEvaluation(*this);
}

/* Comparison operator */
Bool SoizeGhanemFactorEvaluation::operator ==(const SoizeGhanemFactorEvaluation & other) const
{
  if (this == &other) return true;
  return (measure_ == other.measure_) && (useCopula_ == other.useCopula_);
}


/* Operator ()
 * The correction factor K(x) is given by:
 * K(x)=\sqrt{\frac{\prod_{k=1}^d p_k(x_k)}{p(x_1,\dots,x_d)}}
 * where p_k is the PDF of the kth marginal distribution and p
 * its join PDF. Introducing the copula PDF c, one gets:
 * K(x)=\frac{1}{\sqrt{c(F_1(x_1),\dots,F_d(x_d))}}
 * beause p(x_1,\dots,x_d)=c(F_1(x_1),\dots,F_d(x_d))\prod_{k=1}^d p_k(x_k)
 * where F_k is the CDF of the kth marginal distribution.
 * See: Christian Soize, R. Ghanem.
 * "Physical systems with random uncertainties: Chaos representations with
 *  arbitrary probability measure". SIAM Journal on Scientific Computing,
 *  Society for Industrial and Applied Mathematics, 2004, 26 (2), pp.395-410.
 */
NumericalPoint SoizeGhanemFactorEvaluation::operator() (const NumericalPoint & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  NumericalPoint result(1);
  if (useCopula_)
  {
    NumericalPoint u(inputDimension);
    for (UnsignedInteger i = 0; i < inputDimension; ++i)
      u[i] = marginals_[i].computeCDF(inP[i]);
    result[0] = 1.0 / std::sqrt(std::max(SpecFunc::MinNumericalScalar, copula_.computePDF(u)));
  }
  else
  {
    NumericalScalar logFactor = 0.0;
    for (UnsignedInteger i = 0; i < inputDimension; ++i)
      logFactor += marginals_[i].computeLogPDF(inP[i]);
    // \sqrt{\frac{\prod_{k=1}^d p_k(x_k)}{p(x_1,\dots,x_d)}}
    result[0] = std::exp(0.5 * (logFactor - measure_.computeLogPDF(inP)));
  }
  ++callsNumber_;
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inP);
    outputStrategy_.store(result);
  }
  return result;
}

NumericalSample SoizeGhanemFactorEvaluation::operator() (const NumericalSample & inS) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inS.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inS.getDimension();
  const UnsignedInteger size = inS.getSize();
  if (size == 0) return NumericalSample(0, 1);
  NumericalSample result(size, 1);
  if (useCopula_)
  {
    NumericalSample u(size, 0);
    for (UnsignedInteger i = 0; i < inputDimension; ++i)
      u.stack(marginals_[i].computeCDF(inS.getMarginal(i)));
    const NumericalSample pdf(copula_.computePDF(u));
    for (UnsignedInteger i = 0; i < size; ++i)
      result[i][0] = 1.0 / std::sqrt(std::max(SpecFunc::MinNumericalScalar, pdf[i][0]));
  }
  else
  {
    NumericalSample logFactor(size, 1);
    for (UnsignedInteger i = 0; i < inputDimension; ++i)
      logFactor += marginals_[i].computeLogPDF(inS.getMarginal(i));
    // \sqrt{\frac{\prod_{k=1}^d p_k(x_k)}{p(x_1,\dots,x_d)}}
    const NumericalSample logResult(logFactor - measure_.computeLogPDF(inS));
    for (UnsignedInteger i = 0; i < size; ++i)
      result[i][0] = std::exp(0.5 * logResult[i][0]);
  }
  callsNumber_ += size;
  if (isHistoryEnabled_)
  {
    inputStrategy_.store(inS);
    outputStrategy_.store(result);
  }
  return result;
}


/* Accessor for input point dimension */
UnsignedInteger SoizeGhanemFactorEvaluation::getInputDimension() const
{
  return measure_.getDimension();
}

/* Accessor for output point dimension */
UnsignedInteger SoizeGhanemFactorEvaluation::getOutputDimension() const
{
  return 1;
}

/* Accessor for input point description */
Description SoizeGhanemFactorEvaluation::getInputDescription() const
{
  return measure_.getDescription();
}

/* Accessor for output point description */
Description SoizeGhanemFactorEvaluation::getOutputDescription() const
{
  return Description(1, "k");
}

/* String converter */
String SoizeGhanemFactorEvaluation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << GetClassName()
      << " measure=" << measure_
      << " marginals=" << marginals_
      << " useCopula=" << useCopula_;
  if (useCopula_) oss << " copula=" << copula_;
  return oss;
}

String SoizeGhanemFactorEvaluation::__str__(const String & offset) const
{
  OSS oss(true);
  const Description input(getInputDescription());
  oss << input << "->1/sqrt(" << (useCopula_ ? copula_.__str__() : measure_.__str__()) << ".computePDF(";
  String separator(", ");
  for (UnsignedInteger i = 0; i < input.getSize(); ++i)
  {
    if (i == input.getSize() - 1) separator = ")";
    if (useCopula_)
      oss << marginals_[i].__str__() << ".computeCDF(" << input[i] << ")" << separator;
    else
      oss << input[i] << separator;
  }
  return oss;
}

/* Method save() stores the object through the StorageManager */
void SoizeGhanemFactorEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute( "measure_", measure_ );
  adv.saveAttribute( "marginals_", marginals_ );
  adv.saveAttribute( "useCopula_", useCopula_ );
  adv.saveAttribute( "copula_", copula_ );
}


/* Method load() reloads the object from the StorageManager */
void SoizeGhanemFactorEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute( "measure_", measure_ );
  adv.loadAttribute( "marginals_", marginals_ );
  adv.loadAttribute( "useCopula_", useCopula_ );
  adv.loadAttribute( "copula_", copula_ );
}

END_NAMESPACE_OPENTURNS
