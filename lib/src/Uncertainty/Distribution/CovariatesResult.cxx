//                                               -*- C++ -*-
/**
 *  @brief Result of GEV covariates likelihood
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/ParametricFunction.hxx"

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


class CovariatesResultQuantileEvaluation : public EvaluationImplementation
{
public:
  CovariatesResultQuantileEvaluation(const DistributionFactory & factory,
                                     const Function & parameterFunction,
                                     const Scalar p)
    : factory_(factory)
    , parameterFunction_(parameterFunction)
    , p_(p)
  {
    setInputDescription(parameterFunction.getInputDescription());
    setOutputDescription({"quantile(y)"});
  }

  CovariatesResultQuantileEvaluation * clone() const override
  {
    return new CovariatesResultQuantileEvaluation(*this);
  }

  Point operator()(const Point & covariate) const override
  {
    const Point theta(parameterFunction_(covariate));
    return factory_.build(theta).computeQuantile(p_);
  }

  UnsignedInteger getInputDimension() const override
  {
    return parameterFunction_.getInputDimension();
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

private:
  DistributionFactory factory_;
  Function parameterFunction_;
  Scalar p_ = 0.0;
};

/* Draw parameter according to 1 or 2 covariates
   the reference point sets the values of the frozen covariates */
GridLayout CovariatesResult::drawParameterFunction1D(const UnsignedInteger parameterIndex,
                                                     const Point & referencePoint0) const
{
  if (parameterIndex > 2)
    throw InvalidArgumentException(HERE) << "CovariatesResult: parameter index (" << parameterIndex << ") should be < 3";
  const UnsignedInteger covariatesDimension = covariates_.getDimension();
  Point referencePoint(referencePoint0);
  if (!referencePoint.getDimension())
    referencePoint = covariates_.computeMean();
  if (referencePoint.getDimension() != covariatesDimension)
    throw InvalidArgumentException(HERE) << "CovariatesResult: reference point dimension (" << referencePoint.getDimension()
                                         << ") should match covariates dimension (" << covariatesDimension << ")";
  GridLayout grid(1, covariatesDimension);
  const Point xMin(covariates_.getMin());
  const Point xMax(covariates_.getMax());
  for (UnsignedInteger i = 0; i < covariatesDimension; ++ i)
  {
    Point referencePoint2(referencePoint);
    referencePoint2.erase(i);
    const ParametricFunction parametric(parameterFunction_.getMarginal(parameterIndex),
                                        Indices({i}), referencePoint2, false);
    Graph graph(parametric.draw(xMin[i], xMax[i]));
    grid.setGraph(0, i, graph);
  }
  return grid;
}

GridLayout CovariatesResult::drawParameterFunction2D(const UnsignedInteger parameterIndex,
                                                     const Point & referencePoint0) const
{
  const UnsignedInteger covariatesDimension = covariates_.getDimension();
  if (covariatesDimension < 2)
    throw NotDefinedException(HERE) << "CovariatesResult: cannot draw a parameter function when there are less than 2 covariates";
  if (parameterIndex > 2)
    throw InvalidArgumentException(HERE) << "CovariatesResult: parameter index (" << parameterIndex << ") should be < 3";
  Point referencePoint(referencePoint0);
  if (!referencePoint.getDimension())
    referencePoint = covariates_.computeMean();
  if (referencePoint.getDimension() != covariatesDimension)
    throw InvalidArgumentException(HERE) << "CovariatesResult: reference point dimension (" << referencePoint.getDimension()
                                         << ") should match covariates dimension (" << covariatesDimension << ")";
  GridLayout grid(covariatesDimension - 1, covariatesDimension - 1);
  const Point xMin(covariates_.getMin());
  const Point xMax(covariates_.getMax());
  for (UnsignedInteger i = 1; i < covariatesDimension; ++ i)
  {
    for (UnsignedInteger j = 0; j < i; ++ j)
    {
      Point referencePoint2(referencePoint);
      referencePoint2.erase(i);// erase greatest first
      referencePoint2.erase(j);
      const ParametricFunction parametric(parameterFunction_.getMarginal(parameterIndex),
                                          Indices({i, j}), referencePoint2, false);
      Graph graph(parametric.draw(Point({xMin[i], xMin[j]}), Point({xMax[i], xMax[j]})));
      grid.setGraph(i - 1, j, graph);
    }
  }
  return grid;
}


/* Draw quantile according to 1 or 2 covariates
    the reference point sets the values of the frozen covariates */
GridLayout CovariatesResult::drawQuantileFunction1D(const Scalar p,
                                                    const Point & referencePoint0) const
{
  const UnsignedInteger covariatesDimension = covariates_.getDimension();
  if (covariatesDimension < 2)
    throw NotDefinedException(HERE) << "CovariatesResult: cannot draw a quantile function when there are less than 2 covariates";
  Point referencePoint(referencePoint0);
  if (!referencePoint.getDimension())
    referencePoint = covariates_.computeMean();
  if (referencePoint.getDimension() != covariatesDimension)
    throw InvalidArgumentException(HERE) << "CovariatesResult: reference point dimension (" << referencePoint.getDimension()
                                         << ") should match covariates dimension (" << covariatesDimension << ")";
  GridLayout grid(1, covariatesDimension);
  const Point xMin(covariates_.getMin());
  const Point xMax(covariates_.getMax());
  for (UnsignedInteger i = 0; i < covariatesDimension; ++ i)
  {
    Point referencePoint2(referencePoint);
    referencePoint2.erase(i);
    const ParametricFunction parametric(parameterFunction_, Indices({i}), referencePoint2, false);
    const Function quantileFunction(CovariatesResultQuantileEvaluation(factory_, parametric, p));
    Graph graph(quantileFunction.draw(xMin[i], xMax[i]));
    grid.setGraph(0, i, graph);
  }
  return grid;
}

GridLayout CovariatesResult::drawQuantileFunction2D(const Scalar p,
                                                    const Point & referencePoint0) const
{
  const UnsignedInteger covariatesDimension = covariates_.getDimension();
  Point referencePoint(referencePoint0);
  if (!referencePoint.getDimension())
    referencePoint = covariates_.computeMean();
  if (referencePoint.getDimension() != covariatesDimension)
    throw InvalidArgumentException(HERE) << "CovariatesResult: reference point dimension (" << referencePoint.getDimension()
                                         << ") should match covariates dimension (" << covariatesDimension << ")";
  GridLayout grid(covariatesDimension - 1, covariatesDimension - 1);
  const Point xMin(covariates_.getMin());
  const Point xMax(covariates_.getMax());
  for (UnsignedInteger i = 0; i < covariatesDimension; ++ i)
  {
    for (UnsignedInteger j = 0; j < i; ++ j)
    {
      Point referencePoint2(referencePoint);
      referencePoint2.erase(i);// erase greatest first
      referencePoint2.erase(j);
      const ParametricFunction parametric(parameterFunction_, Indices({i, j}), referencePoint2, false);
      const Function quantileFunction(CovariatesResultQuantileEvaluation(factory_, parametric, p));
      Graph graph(quantileFunction.draw(Point({xMin[i], xMin[j]}), Point({xMax[i], xMax[j]})));
      grid.setGraph(i - 1, j, graph);
    }
  }
  return grid;
}

String CovariatesResult::__repr__() const
{
  return OSS() << PersistentObject::__repr__();
}

Function CovariatesResult::getParameterFunction() const
{
  return parameterFunction_;
}

Sample CovariatesResult::getCovariates() const
{
  return covariates_;
}

LinearFunction CovariatesResult::getNormalizationFunction() const
{
  return normalizationFunction_;
}

/* Accessor to the distribution at a given time */
Distribution CovariatesResult::getDistribution(const Point & covariates) const
{
  const Point parameters(parameterFunction_(covariates));
  return factory_.build(parameters);
}

/* Method save() stores the object through the StorageManager */
void CovariatesResult::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("factory_", factory_);
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
  adv.loadAttribute("factory_", factory_);
  adv.loadAttribute("parameterFunction_", parameterFunction_);
  adv.loadAttribute("covariates_", covariates_);
  adv.loadAttribute("parameterDistribution_", parameterDistribution_);
  adv.loadAttribute("normalizationFunction_", normalizationFunction_);
  adv.loadAttribute("logLikelihood_", logLikelihood_);
}

END_NAMESPACE_OPENTURNS
