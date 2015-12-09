//                                               -*- C++ -*-
/**
 *  @brief
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include "BoxCoxFactory.hxx"
#include "MethodBoundNumericalMathEvaluationImplementation.hxx"
#include "Exception.hxx"
#include "PersistentObjectFactory.hxx"
#include "Field.hxx"
#include "BoxCoxEvaluationImplementation.hxx"
#include "SpecFunc.hxx"
#include "algocobyla.h"
#include "Log.hxx"
#include "BoxCoxTransform.hxx"
#include "InverseBoxCoxTransform.hxx"
#include "Curve.hxx"
#include "Cloud.hxx"
#include "OptimizationSolver.hxx"
#include "Cobyla.hxx"
#include "MethodBoundNumericalMathEvaluationImplementation.hxx"
#include "GeneralizedLinearModelAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class BoxCoxFactory
 */

CLASSNAMEINIT(BoxCoxFactory);

static const Factory<BoxCoxFactory> RegisteredFactory;

class BoxCoxSampleOptimization
{
private:

  /** only used to pass data to be used in computeLogLikeliHood */
  mutable NumericalSample sample_;

  /** only used to pass data to be used in computeLogLikeliHood */
  mutable NumericalScalar sumLog_;

  /** Optimization solver */
  mutable OptimizationSolver solver_;

public:

  BoxCoxSampleOptimization(const NumericalSample & sample,
                           const NumericalScalar sumLog)
    : sample_(sample)
    , sumLog_(sumLog)
    , solver_(new Cobyla())
  {
    // Nothing to do
  }

  BoxCoxSampleOptimization(const NumericalSample & sample,
                           const OptimizationSolver & solver)
    : sample_(sample)
    , sumLog_(0.0)
    , solver_(solver)
  {
    computeSumLog();
  }

  /** Likelihood function */
  NumericalScalar computeLogLikelihood(const NumericalPoint & lambda) const
  {
    const UnsignedInteger size(sample_.getSize());
    // Define BoxCox trannsformation for sample
    BoxCoxEvaluationImplementation myBoxFunction(NumericalPoint(1, lambda[0]));
    // compute the mean of the transformed sample using the Box-Cox function
    const NumericalSample outSample(myBoxFunction(sample_));
    const NumericalScalar ratio(1.0 - 1.0 / size);
    const NumericalScalar sigma2(outSample.computeVariance()[0]);
    NumericalScalar result(-0.5 * size * log(sigma2 * ratio));

    // result is translated
    result += (lambda[0] - 1.0) * sumLog_;
    return result;
  }

  void computeSumLog() const
  {
    // Compute the sum of the log-data
    const UnsignedInteger size(sample_.getSize());
    sumLog_ = 0.0;
    for (UnsignedInteger k = 0; k < size; ++k) sumLog_ += std::log(sample_[k][0]);
  }
  /** Likelihood function accessor */
  NumericalMathFunction getLogLikelihoodFunction() const
  {
    return bindMethod <BoxCoxSampleOptimization, NumericalScalar, NumericalPoint> ( *this, &BoxCoxSampleOptimization::computeLogLikelihood, 1, 1);
  }

  NumericalPoint optimizeLogLikelihood() const
  {
    // Define optimization problem
    OptimizationProblem problem;
    problem.setObjective(getLogLikelihoodFunction());
    problem.setMinimization(false);
    solver_.setProblem(problem);
    solver_.setStartingPoint(NumericalPoint(1, 1.0));
    // run Optimization problem
    solver_.run();
    // Return optimization point
    const NumericalPoint optpoint(solver_.getResult().getOptimalPoint());
    return optpoint;
  }

  NumericalScalar getSumLog() const
  {
    return sumLog_;
  }
};


class BoxCoxGLMOptimization
{
  typedef BoxCoxFactory::BasisCollection BasisCollection;

private:

  /** only used to pass data to be used in computeLogLikeliHood */
  mutable NumericalSample inputSample_;
  mutable NumericalSample shiftedOutputSample_;
  mutable CovarianceModel covarianceModel_;
  mutable BasisCollection basis_;

  /** Optimization solver */
  mutable OptimizationSolver solver_;

public:


  BoxCoxGLMOptimization(const NumericalSample & inputSample,
                        const NumericalSample & shiftedOutputSample,
                        const CovarianceModel & covarianceModel,
                        const BasisCollection & basis,
                        const OptimizationSolver & solver)
    : inputSample_(inputSample)
    , shiftedOutputSample_(shiftedOutputSample)
    , covarianceModel_(covarianceModel)
    , basis_(basis)
    , solver_(solver)
  {
    // Nothing to do
  }

  /** Likelihood function */
  NumericalScalar computeLogLikelihood(const NumericalPoint & lambda) const
  {
    // Define BoxCox trannsformation for output sample
    BoxCoxEvaluationImplementation myBoxFunction(lambda);
    // compute the mean of the transformed sample using the Box-Cox function
    const NumericalSample transformedOutputSample(myBoxFunction(shiftedOutputSample_));
    // Use of GLM to estimate the best generalized linear model
    GeneralizedLinearModelAlgorithm algo(inputSample_, transformedOutputSample, covarianceModel_, basis_);
    algo.run();
    // Get result
    GeneralizedLinearModelResult result(algo.getResult());
    // Conditional covariance model
    CovarianceModel conditionalCovarianceModel(result.getCovarianceModel());
    // Parameters for the evaluation of likelihood function
    const NumericalPoint covarianceParameters(conditionalCovarianceModel.getParameter());
    // Finally evaluate the log-likelihood function
    const NumericalScalar loglikelihood(algo.getObjectiveFunction()(covarianceParameters)[0]);
    return loglikelihood;
  }

  /** Likelihood function accessor */
  NumericalMathFunction getLogLikelihoodFunction() const
  {
    return bindMethod <BoxCoxGLMOptimization, NumericalScalar, NumericalPoint> ( *this, &BoxCoxGLMOptimization::computeLogLikelihood, 1, 1);
  }

  NumericalPoint optimizeLogLikelihood() const
  {
    // Define optimization problem
    OptimizationProblem problem;
    NumericalMathFunction objectiveFunction(getLogLikelihoodFunction());
    objectiveFunction.enableCache();
    objectiveFunction.enableHistory();
    problem.setObjective(objectiveFunction);
    problem.setMinimization(false);
    solver_.setProblem(problem);
    solver_.setStartingPoint(NumericalPoint(1, 1.0));
    // run Optimization problem
    solver_.run();
    // Return optimization point
    const NumericalPoint optpoint(solver_.getResult().getOptimalPoint());
    return optpoint;
  }
};


/* Constructor with parameters */
BoxCoxFactory::BoxCoxFactory()
  : PersistentObject()
  , solver_(new Cobyla())
{
  CobylaSpecificParameters parameters(ResourceMap::GetAsNumericalScalar("BoxCoxFactory-DefaultRhoBeg"));
  static_cast<Cobyla*>(solver_.getImplementation().get())->setSpecificParameters(parameters);
  solver_.setMaximumAbsoluteError(ResourceMap::GetAsNumericalScalar("BoxCoxFactory-DefaultRhoEnd"));
  solver_.setMaximumIterationNumber(ResourceMap::GetAsUnsignedInteger("BoxCoxFactory-DefaultMaxFun"));
}

/* Virtual constructor */
BoxCoxFactory * BoxCoxFactory::clone() const
{
  return new BoxCoxFactory(*this);
}

OptimizationSolver BoxCoxFactory::getOptimizationSolver() const
{
  return solver_;
}

void BoxCoxFactory::setOptimizationSolver(const OptimizationSolver & solver)
{
  solver_ = solver;
}


/* Build the factory from data */
BoxCoxTransform BoxCoxFactory::build(const Field & timeSeries) const
{
  return build(timeSeries.getSample());
}

BoxCoxTransform BoxCoxFactory::build(const Field & timeSeries,
                                     const NumericalPoint & shift) const
{
  return build(timeSeries.getSample(), shift);
}

BoxCoxTransform BoxCoxFactory::build(const Field & timeSeries,
                                     const NumericalPoint & shift,
                                     Graph & graph) const
{
  return build(timeSeries.getSample(), shift, graph);
}

BoxCoxTransform BoxCoxFactory::build(const NumericalSample & sample) const
{
  return build(sample, NumericalPoint(sample.getDimension()));
}

BoxCoxTransform BoxCoxFactory::build(const NumericalSample & sample,
                                     const NumericalPoint & shift) const
{
  Graph tmp;
  return build(sample, shift, tmp);
}

BoxCoxTransform BoxCoxFactory::build(const NumericalSample & sample,
                                     const NumericalPoint & shift,
                                     Graph & graph) const
{
  // Check the input size
  const UnsignedInteger size(sample.getSize());
  if (size == 0)
    throw InvalidArgumentException(HERE) << "Error: cannot build a Box-Cox factory from an empty time series";

  // Check the input dimensions
  const UnsignedInteger dimension(sample.getDimension());
  if (shift.getDimension() != dimension)
    throw InvalidArgumentException(HERE) << "Error: the shift has a dimension=" << shift.getDimension() << " different from the sample dimension=" << dimension;

  // Shape parameters of the transformation
  NumericalPoint lambda(dimension);
  // Sum of the log-data
  NumericalPoint sumLog(dimension);

  // Keep the shifted marginal samples
  Collection< NumericalSample > marginalSamples(dimension);
  for (UnsignedInteger d = 0; d < dimension; ++d)
  {
    // Extract the marginal sample and pply the shift
    marginalSamples[d] = sample.getMarginal(d);
    marginalSamples[d] += NumericalPoint(1, shift[d]);
    BoxCoxSampleOptimization boxCoxOptimization(marginalSamples[d], solver_);
    const NumericalPoint optpoint = boxCoxOptimization.optimizeLogLikelihood();
    // get the sum of the log-data
    sumLog[d] = boxCoxOptimization.getSumLog();
    // Store the result
    lambda[d] = optpoint[0];
  }
  // Graphical inspection
  graph = Graph("Box-Cox likelihood", "lambda", "log-likelihood", true, "topright");
  const NumericalScalar lambdaMax(*std::max_element(lambda.begin(), lambda.end()));
  const NumericalScalar lambdaMin(*std::min_element(lambda.begin(), lambda.end()));
  const NumericalScalar xMin(std::min(0.0, 0.002 * round(1000.0 * lambdaMin)));
  const NumericalScalar xMax(std::max(0.0, 0.002 * round(1000.0 * lambdaMax)));
  const UnsignedInteger npts(ResourceMap::GetAsUnsignedInteger("BoxCoxFactory-DefaultPointNumber"));
  NumericalSample lambdaValues(npts, 1);
  for (UnsignedInteger i = 0; i < npts; ++i) lambdaValues[i][0] = xMin + i * (xMax - xMin) / (npts - 1.0);
  for (UnsignedInteger d = 0; d < dimension; ++d)
  {
    NumericalSample logLikelihoodValues(npts, 1);
    BoxCoxSampleOptimization boxCoxOptimization(marginalSamples[d], sumLog[d]);
    for (UnsignedInteger i = 0; i < npts; ++i) logLikelihoodValues[i][0] = boxCoxOptimization.computeLogLikelihood(lambdaValues[i]);
    Curve curve(lambdaValues, logLikelihoodValues);
    curve.setColor(Curve::ConvertFromHSV((360.0 * d) / dimension, 1.0, 1.0));
    graph.add(curve);
    NumericalPoint optimum(2);
    optimum[0] = lambda[d];
    optimum[1] = boxCoxOptimization.computeLogLikelihood(optimum);
    Cloud cloud(NumericalSample(1, optimum));
    cloud.setColor(curve.getColor());
    cloud.setPointStyle("circle");
    cloud.setLegend(String(OSS() << "lambda=" << lambda[d]));
    graph.add(cloud);
    NumericalPoint bb(graph.getBoundingBox());
    bb[3] += 0.1 * (bb[3] - bb[2]);
    graph.setBoundingBox(bb);
  }
  // return result ==> Box Cox transform
  return BoxCoxTransform(lambda, shift);
}

void BoxCoxFactory::checkGLMData(const NumericalSample & inputSample,
                                 const NumericalSample & outputSample,
                                 const CovarianceModel & covarianceModel,
                                 const BasisCollection & basis)
{
  // Check the input size
  const UnsignedInteger size = inputSample.getSize();
  if (size == 0)
    throw InvalidArgumentException(HERE) << "Error: cannot build a Box-Cox factory from empty data";

  if (size != outputSample.getSize())
    throw InvalidArgumentException(HERE) << "Error: input and output sample have different size. Could not perform GLM & Box Cox algorithms";

  // Check the dimensions
  const UnsignedInteger dimension = outputSample.getDimension();
  const UnsignedInteger spatialDimension = inputSample.getDimension();
  if (covarianceModel.getSpatialDimension() != spatialDimension)
    throw InvalidArgumentException(HERE) << "Error: the covariance model has a spatial dimension=" << covarianceModel.getSpatialDimension() << " different from the input sample dimension=" << spatialDimension;

  if (covarianceModel.getDimension() != dimension)
    throw InvalidArgumentException(HERE) << "Error: the covariance model should be of dimension " << dimension << ". Here, covariance model dimension=" << covarianceModel.getDimension();

  if ((basis.getSize() != 0) && (basis.getSize() != dimension))
    throw InvalidArgumentException(HERE) << "Error: the basis should be of size 0 or " << dimension << ". Here, basis size=" << basis.getSize();

}


/** Build the factory from data by estimating the best generalized linear model */
BoxCoxTransform BoxCoxFactory::build(const NumericalSample & inputSample,
                                     const NumericalSample & outputSample,
                                     const CovarianceModel & covarianceModel,
                                     const Basis & basis,
                                     const NumericalPoint & shift,
                                     GeneralizedLinearModelResult & result)
{
  BasisCollection basisColl(outputSample.getDimension(), basis);
  return build(inputSample, outputSample, covarianceModel, basisColl, shift, result);
}

BoxCoxTransform BoxCoxFactory::build(const NumericalSample & inputSample,
                                     const NumericalSample & outputSample,
                                     const CovarianceModel & covarianceModel,
                                     const NumericalPoint & shift,
                                     GeneralizedLinearModelResult & result)
{
  BasisCollection basis;
  return build(inputSample, outputSample, covarianceModel, basis, shift, result);
}

BoxCoxTransform BoxCoxFactory::build(const NumericalSample & inputSample,
                                     const NumericalSample & outputSample,
                                     const CovarianceModel & covarianceModel,
                                     const BasisCollection & basis,
                                     const NumericalPoint & shift,
                                     GeneralizedLinearModelResult & result)
{
  checkGLMData(inputSample, outputSample, covarianceModel, basis);

  // Check the dimensions
  const UnsignedInteger dimension = outputSample.getDimension();
  if (shift.getDimension() != dimension)
    throw InvalidArgumentException(HERE) << "Error: the shift has a dimension=" << shift.getDimension() << " different from the output sample dimension=" << dimension;

  // Keep the shifted marginal samples
  NumericalSample shiftedSample(outputSample);
  shiftedSample += shift;

  // optimization process
  BoxCoxGLMOptimization boxCoxOptimization(inputSample, shiftedSample, covarianceModel, basis, solver_);
  const NumericalPoint lambda = boxCoxOptimization.optimizeLogLikelihood();
  // Define BoxCox trannsformation for output sample
  BoxCoxEvaluationImplementation myBoxFunction(lambda, shift);
  // compute the transformed output sample using the Box-Cox function
  const NumericalSample transformedOutputSample = myBoxFunction(outputSample);
  // Build the GeneralizedLinearModelResult
  // Use of GLM to estimate the best generalized linear model
  GeneralizedLinearModelAlgorithm algo(inputSample, transformedOutputSample, covarianceModel, basis);
  algo.run();
  // Get result
  result = algo.getResult();
  return BoxCoxTransform(lambda, shift);
}

/* String converter */
String BoxCoxFactory::__repr__() const
{
  OSS oss;
  oss << "class=" << BoxCoxFactory::GetClassName();
  return oss;
}

/* String converter */
String BoxCoxFactory::__str__(const String & offset) const
{
  OSS oss;
  oss << "class=" << BoxCoxFactory::GetClassName();
  return oss;
}

END_NAMESPACE_OPENTURNS
