//                                               -*- C++ -*-
/**
 *  @brief Result of GEV covariates likelihood
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
#include "openturns/CovariatesResult.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ChiSquare.hxx"
#include "openturns/Brent.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Text.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(CovariatesResult)

static const Factory<CovariatesResult> Factory_CovariatesResult;


CovariatesResult::CovariatesResult()
  : PersistentObject()
{
  // Nothing to do
}

CovariatesResult::CovariatesResult(const DistributionFactory & factory,
                                     const Function & parameterFunction,
                                     const Sample & covariates,
                                     const Distribution & parameterDistribution,
                                     const LinearFunction & normalizationFunction,
                                     const Scalar logLikelihood)
  : PersistentObject()
  , factory_(factory)
  , parameterFunction_(parameterFunction)
  , covariates_(covariates)
  , parameterDistribution_(parameterDistribution)
  , normalizationFunction_(normalizationFunction)
  , logLikelihood_(logLikelihood)
{
  if (covariates.getDimension() != parameterFunction.getInputDimension())
    throw InvalidArgumentException(HERE) << "the covariates dimension must match the parameter function input dimension";
  if (covariates.getDimension() != normalizationFunction.getInputDimension())
    throw InvalidArgumentException(HERE) << "the covariates dimension must match the normalization function input dimension";
  if (normalizationFunction.getInputDimension() != normalizationFunction.getOutputDimension())
    throw InvalidArgumentException(HERE) << "the normalization function must have the same input and output dimensions";
  if (parameterDistribution.getDimension() != parameterFunction.getParameter().getDimension())
    throw InvalidArgumentException(HERE) << "the parameter distribution dimension must match the parameter function parameter dimension";
}

CovariatesResult * CovariatesResult::clone() const
{
  return new CovariatesResult(*this);
}

Point CovariatesResult::getOptimalParameter() const
{
  return parameterDistribution_.getMean();
}

void CovariatesResult::setParameterDistribution(const Distribution & parameterDistribution)
{
  parameterDistribution_ = parameterDistribution;
}

Distribution CovariatesResult::getParameterDistribution() const
{
  return parameterDistribution_;
}

void CovariatesResult::setLogLikelihood(const Scalar logLikelihood)
{
  logLikelihood_ = logLikelihood;
}

Scalar CovariatesResult::getLogLikelihood() const
{
  return logLikelihood_;
}

/* Draw parameter for all time values */
Graph CovariatesResult::drawParameterFunction(const UnsignedInteger parameterIndex) const
{
  if (covariates_.getDimension() > 2) throw NotDefinedException(HERE) << "Error: cannot draw a parameter function when there is more than 2 covariates";
  const Point xMin(covariates_.getMin());
  const Point xMax(covariates_.getMax());
  Graph result(parameterFunction_.getMarginal(parameterIndex).draw(xMin, xMax));
  result.setTitle("Parameter function");
  return result;
}

class CovariatesResultQuantileEvaluation : public EvaluationImplementation
{
public:
  CovariatesResultQuantileEvaluation(const CovariatesResult & result, const Scalar p)
    : result_(result)
    , p_(p)
  {
    setInputDescription({"t"});
    setOutputDescription({"quantile(t)"});
  }

  CovariatesResultQuantileEvaluation * clone() const override
  {
    return new CovariatesResultQuantileEvaluation(*this);
  }

  Point operator()(const Point & inP) const override
  {
    const Scalar t = inP[0];
    return result_.getDistribution(t).computeQuantile(p_);
  }

  UnsignedInteger getInputDimension() const override
  {
    return 1;
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

private:
  CovariatesResult result_;
  Scalar p_ = 0.0;
};

/* Draw quantile for all time values */
Graph CovariatesResult::drawQuantileFunction(const Scalar p) const
{
  const Sample grid(covariates_.getVertices());
  const Scalar xMin = grid.getMin()[0];
  const Scalar xMax = grid.getMax()[0];

  Function quantileFunction(CovariatesResultQuantileEvaluation(*this, p));
  Graph result(quantileFunction.draw(xMin, xMax));
  result.setTitle("Quantile function");
  return result;
}

String CovariatesResult::__repr__() const
{
  return OSS() << PersistentObject::__repr__();
}

Function CovariatesResult::getParameterFunction() const
{
  return parameterFunction_;
}

LinearFunction CovariatesResult::getNormalizationFunction() const
{
  return normalizationFunction_;
}

/* Accessor to the distribution at a given time */
Distribution CovariatesResult::getDistribution(const Scalar t) const
{
  const Point parameters(parameterFunction_(Point(1, t)));
  return factory_.build(parameters);
}

/* Method save() stores the object through the StorageManager */
void CovariatesResult::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("parameterFunction_", parameterFunction_);
  adv.saveAttribute("covariates_", covariates_);
  adv.saveAttribute("parameterDistribution_", parameterDistribution_);
  adv.saveAttribute("normalizationFunction_", normalizationFunction_);
  adv.saveAttribute("logLikelihood_", logLikelihood_);
}

/* Method load() reloads the object from the StorageManager */
void CovariatesResult::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("parameterFunction_", parameterFunction_);
  adv.loadAttribute("covariates_", covariates_);
  adv.loadAttribute("parameterDistribution_", parameterDistribution_);
  adv.loadAttribute("normalizationFunction_", normalizationFunction_);
  adv.loadAttribute("logLikelihood_", logLikelihood_);
}

END_NAMESPACE_OPENTURNS
