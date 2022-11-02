//                                               -*- C++ -*-
/**
 *  @brief
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/BoxCoxFactory.hxx"
#include "openturns/MethodBoundEvaluation.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Field.hxx"
#include "openturns/BoxCoxEvaluation.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Log.hxx"
#include "openturns/BoxCoxTransform.hxx"
#include "openturns/InverseBoxCoxTransform.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Cloud.hxx"
#include "openturns/OptimizationAlgorithm.hxx"
#include "openturns/Cobyla.hxx"
#include "openturns/MethodBoundEvaluation.hxx"
#include "openturns/GeneralLinearModelAlgorithm.hxx"
#include "openturns/MemoizeFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class BoxCoxFactory
 */

CLASSNAMEINIT(BoxCoxFactory)

static const Factory<BoxCoxFactory> Factory_BoxCoxFactory;

class BoxCoxSampleOptimization
{
private:

  /** only used to pass data to be used in computeLogLikeliHood */
  mutable Sample sample_;

  /** only used to pass data to be used in computeLogLikeliHood */
  mutable Scalar sumLog_;

  /** Optimization solver */
  mutable OptimizationAlgorithm solver_;

public:

  BoxCoxSampleOptimization(const Sample & sample,
                           const Scalar sumLog)
    : sample_(sample)
    , sumLog_(sumLog)
    , solver_(new Cobyla())
  {
    // Nothing to do
  }

  BoxCoxSampleOptimization(const Sample & sample,
                           const OptimizationAlgorithm & solver)
    : sample_(sample)
    , sumLog_(0.0)
    , solver_(solver)
  {
    computeSumLog();
  }

  /** Likelihood function */
  Point computeLogLikelihood(const Point & lambda) const
  {
    const UnsignedInteger size = sample_.getSize();
    // Define BoxCox transformation for sample
    BoxCoxEvaluation myBoxFunction(Point(1, lambda[0]));
    // compute the mean of the transformed sample using the Box-Cox function
    const Sample outSample(myBoxFunction(sample_));
    const Scalar ratio = 1.0 - 1.0 / size;
    const Scalar sigma2 = outSample.computeVariance()[0];
    Scalar result = -0.5 * size * log(sigma2 * ratio);

    // result is translated
    result += (lambda[0] - 1.0) * sumLog_;
    return Point(1, result);
  }

  void computeSumLog() const
  {
    // Compute the sum of the log-data
    const UnsignedInteger size = sample_.getSize();
    sumLog_ = 0.0;
    for (UnsignedInteger k = 0; k < size; ++k) sumLog_ += std::log(sample_(k, 0));
  }
  /** Likelihood function accessor */
  Function getLogLikelihoodFunction() const
  {
    return bindMethod <BoxCoxSampleOptimization, Point, Point> ( *this, &BoxCoxSampleOptimization::computeLogLikelihood, 1, 1);
  }

  Point optimizeLogLikelihood() const
  {
    // Define optimization problem
    OptimizationProblem problem(getLogLikelihoodFunction());
    problem.setMinimization(false);
    solver_.setProblem(problem);
    solver_.setStartingPoint(Point(1, 1.0));
    // run Optimization problem
    solver_.run();
    // Return optimization point
    const Point optpoint(solver_.getResult().getOptimalPoint());
    return optpoint;
  }

  Scalar getSumLog() const
  {
    return sumLog_;
  }
};


class BoxCoxGLMOptimization
{
  typedef BoxCoxFactory::BasisCollection BasisCollection;

private:

  /** only used to pass data to be used in computeLogLikeliHood */
  mutable Sample inputSample_;
  mutable Sample shiftedOutputSample_;
  mutable CovarianceModel covarianceModel_;
  mutable BasisCollection basis_;

  /** Optimization solver */
  mutable OptimizationAlgorithm solver_;

public:


  BoxCoxGLMOptimization(const Sample & inputSample,
                        const Sample & shiftedOutputSample,
                        const CovarianceModel & covarianceModel,
                        const BasisCollection & basis,
                        const OptimizationAlgorithm & solver)
    : inputSample_(inputSample)
    , shiftedOutputSample_(shiftedOutputSample)
    , covarianceModel_(covarianceModel)
    , basis_(basis)
    , solver_(solver)
  {
    // Nothing to do
  }

  /** Likelihood function */
  Point computeLogLikelihood(const Point & lambda) const
  {
    // Define BoxCox transformation for output sample
    BoxCoxEvaluation myBoxFunction(lambda);
    // compute the mean of the transformed sample using the Box-Cox function
    const Sample transformedOutputSample(myBoxFunction(shiftedOutputSample_));
    // Use of GLM to estimate the best generalized linear model
    GeneralLinearModelAlgorithm algo(inputSample_, transformedOutputSample, covarianceModel_, basis_);
    algo.run();
    // Return the optimal log-likelihood
    const Scalar result = algo.getResult().getOptimalLogLikelihood();
    return Point(1, result);
  }

  /** Likelihood function accessor */
  Function getLogLikelihoodFunction() const
  {
    return bindMethod <BoxCoxGLMOptimization, Point, Point> ( *this, &BoxCoxGLMOptimization::computeLogLikelihood, 1, 1);
  }

  Point optimizeLogLikelihood() const
  {
    // Define optimization problem
    Function objectiveFunction(getLogLikelihoodFunction());
    MemoizeFunction objectiveMemoizeFunction(objectiveFunction, Full());
    objectiveMemoizeFunction.enableCache();
    OptimizationProblem problem(objectiveMemoizeFunction);
    problem.setMinimization(false);
    solver_.setProblem(problem);
    solver_.setStartingPoint(Point(1, 1.0));
    // run Optimization problem
    solver_.run();
    // Return optimization point
    const Point optpoint(solver_.getResult().getOptimalPoint());
    return optpoint;
  }
};

class BoxCoxGLMBasisOptimization : public EvaluationImplementation
{

public:
  BoxCoxGLMBasisOptimization(const Sample &inputSample,
                             const Sample &shiftedOutputSample,
                             const CovarianceModel &covarianceModel,
                             const Basis &basis)
      : inputSample_(inputSample)
      , shiftedOutputSample_(shiftedOutputSample)
      , covarianceModel_(covarianceModel)
      , basis_(basis)
  {
    // Nothing to do
  }

  BoxCoxGLMBasisOptimization *clone() const override
  {
    return new BoxCoxGLMBasisOptimization(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    return 1;
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

  // It is a simple call to the likelihood function
  Point operator()(const Point &lambda) const override
  {
    // Define BoxCox transformation for output sample
    BoxCoxEvaluation myBoxFunction(lambda);
    // compute the mean of the transformed sample using the Box-Cox function
    const Sample transformedOutputSample(myBoxFunction(shiftedOutputSample_));
    // Use of GLM to estimate the best generalized linear model
    // TODO discuss here about freezing model parameters
    GeneralLinearModelAlgorithm algo(inputSample_, transformedOutputSample, covarianceModel_, basis_);
    algo.run();
    // Return the optimal log-likelihood
    const Scalar result = algo.getResult().getOptimalLogLikelihood();
    return Point(1, result);
  }

private:
  /** only used to pass data to be used in computeLogLikeliHood */
  Sample inputSample_;
  Sample shiftedOutputSample_;
  CovarianceModel covarianceModel_;
  Basis basis_;
};

/* Constructor with parameters */
BoxCoxFactory::BoxCoxFactory()
  : PersistentObject()
  , solver_(new Cobyla())
{
  const Scalar rhoBeg = ResourceMap::GetAsScalar("BoxCoxFactory-DefaultRhoBeg");
  dynamic_cast<Cobyla*>(solver_.getImplementation().get())->setRhoBeg(rhoBeg);
  solver_.setMaximumAbsoluteError(ResourceMap::GetAsScalar("BoxCoxFactory-DefaultRhoEnd"));
  solver_.setMaximumEvaluationNumber(ResourceMap::GetAsUnsignedInteger("BoxCoxFactory-DefaultMaximumEvaluationNumber"));
}

/* Virtual constructor */
BoxCoxFactory * BoxCoxFactory::clone() const
{
  return new BoxCoxFactory(*this);
}

OptimizationAlgorithm BoxCoxFactory::getOptimizationAlgorithm() const
{
  return solver_;
}

void BoxCoxFactory::setOptimizationAlgorithm(const OptimizationAlgorithm & solver)
{
  solver_ = solver;
}


/* Build the factory from data */
BoxCoxTransform BoxCoxFactory::build(const Field & timeSeries) const
{
  return build(timeSeries.getValues());
}

BoxCoxTransform BoxCoxFactory::build(const Field & timeSeries,
                                     const Point & shift) const
{
  return build(timeSeries.getValues(), shift);
}

BoxCoxTransform BoxCoxFactory::build(const Field & timeSeries,
                                     const Point & shift,
                                     Graph & graph) const
{
  return build(timeSeries.getValues(), shift, graph);
}

BoxCoxTransform BoxCoxFactory::build(const Sample & sample) const
{
  return build(sample, Point(sample.getDimension()));
}

BoxCoxTransform BoxCoxFactory::build(const Sample & sample,
                                     const Point & shift) const
{
  Graph tmp;
  return build(sample, shift, tmp);
}

BoxCoxTransform BoxCoxFactory::build(const Sample & sample,
                                     const Point & shift,
                                     Graph & graph) const
{
  // Check the input size
  const UnsignedInteger size = sample.getSize();
  if (size == 0)
    throw InvalidArgumentException(HERE) << "Error: cannot build a Box-Cox factory from an empty time series";

  // Check the input dimensions
  const UnsignedInteger dimension = sample.getDimension();
  if (shift.getDimension() != dimension)
    throw InvalidArgumentException(HERE) << "Error: the shift has a dimension=" << shift.getDimension() << " different from the sample dimension=" << dimension;

  // Shape parameters of the transformation
  Point lambda(dimension);
  // Sum of the log-data
  Point sumLog(dimension);

  // Keep the shifted marginal samples
  Collection< Sample > marginalSamples(dimension);
  for (UnsignedInteger d = 0; d < dimension; ++d)
  {
    // Extract the marginal sample and pply the shift
    marginalSamples[d] = sample.getMarginal(d);
    marginalSamples[d] += Point(1, shift[d]);
    BoxCoxSampleOptimization boxCoxOptimization(marginalSamples[d], solver_);
    const Point optpoint = boxCoxOptimization.optimizeLogLikelihood();
    // get the sum of the log-data
    sumLog[d] = boxCoxOptimization.getSumLog();
    // Store the result
    lambda[d] = optpoint[0];
  }
  // Graphical inspection
  graph = Graph("Box-Cox likelihood", "lambda", "log-likelihood", true, "topright");
  const Scalar lambdaMax = *std::max_element(lambda.begin(), lambda.end());
  const Scalar lambdaMin = *std::min_element(lambda.begin(), lambda.end());
  const Scalar xMin = std::min(0.0, 0.002 * round(1000.0 * lambdaMin));
  const Scalar xMax = std::max(0.0, 0.002 * round(1000.0 * lambdaMax));
  const UnsignedInteger npts = ResourceMap::GetAsUnsignedInteger("BoxCoxFactory-DefaultPointNumber");
  Sample lambdaValues(npts, 1);
  for (UnsignedInteger i = 0; i < npts; ++i) lambdaValues(i, 0) = xMin + i * (xMax - xMin) / (npts - 1.0);
  for (UnsignedInteger d = 0; d < dimension; ++d)
  {
    Sample logLikelihoodValues(npts, 1);
    BoxCoxSampleOptimization boxCoxOptimization(marginalSamples[d], sumLog[d]);
    for (UnsignedInteger i = 0; i < npts; ++i) logLikelihoodValues(i, 0) = boxCoxOptimization.computeLogLikelihood(lambdaValues[i])[0];
    Curve curve(lambdaValues, logLikelihoodValues);
    curve.setColor(Curve::ConvertFromHSV((360.0 * d) / dimension, 1.0, 1.0));
    graph.add(curve);
    Point optimum(2);
    optimum[0] = lambda[d];
    optimum[1] = boxCoxOptimization.computeLogLikelihood(optimum)[0];
    Cloud cloud(Sample(1, optimum));
    cloud.setColor(curve.getColor());
    cloud.setPointStyle("circle");
    cloud.setLegend(String(OSS() << "lambda=" << lambda[d]));
    graph.add(cloud);
  }
  // return result ==> Box Cox transform
  return BoxCoxTransform(lambda, shift);
}

void BoxCoxFactory::checkGLMData(const Sample & inputSample,
                                 const Sample & outputSample,
                                 const CovarianceModel & covarianceModel,
                                 const BasisCollection & basis)
{
  LOGWARN(OSS() << "checkGLMData method is deprecated and will be dropped");

  // Check the input size
  const UnsignedInteger size = inputSample.getSize();
  if (size == 0)
    throw InvalidArgumentException(HERE) << "Error: cannot build a Box-Cox factory from empty data";

  if (size != outputSample.getSize())
    throw InvalidArgumentException(HERE) << "Error: input and output sample have different size. Could not perform GLM & Box Cox algorithms";

  // Check the dimensions
  const UnsignedInteger dimension = outputSample.getDimension();
  const UnsignedInteger inputDimension = inputSample.getDimension();
  if (covarianceModel.getInputDimension() != inputDimension)
    throw InvalidArgumentException(HERE) << "Error: the covariance model has an input dimension=" << covarianceModel.getInputDimension() << " different from the input sample dimension=" << inputDimension;

  if (covarianceModel.getOutputDimension() != dimension)
    throw InvalidArgumentException(HERE) << "Error: the covariance model should be of dimension " << dimension << ". Here, covariance model dimension=" << covarianceModel.getOutputDimension();

  if ((basis.getSize() != 0) && (basis.getSize() != dimension))
    throw InvalidArgumentException(HERE) << "Error: the basis should be of size 0 or " << dimension << ". Here, basis size=" << basis.getSize();

}

/** Build the factory from data by estimating the best generalized linear model */
BoxCoxTransform BoxCoxFactory::build(const Sample & inputSample,
                                     const Sample & outputSample,
                                     const CovarianceModel & covarianceModel,
                                     const Basis & basis,
                                     const Point & shift,
                                     GeneralLinearModelResult & result)
{
  // Check the input size
  const UnsignedInteger size = inputSample.getSize();
  if (size == 0)
    throw InvalidArgumentException(HERE) << "Error: cannot build a Box-Cox factory from empty data";

  if (size != outputSample.getSize())
    throw InvalidArgumentException(HERE) << "Error: input and output sample have different size. Could not perform GLM & Box Cox algorithms";

  // Check the dimensions
  const UnsignedInteger dimension = outputSample.getDimension();
  const UnsignedInteger inputDimension = inputSample.getDimension();

  if (covarianceModel.getInputDimension() != inputDimension)
    throw InvalidArgumentException(HERE) << "Error: the covariance model has an input dimension=" << covarianceModel.getInputDimension() << " different from the input sample dimension=" << inputDimension;

  if (covarianceModel.getOutputDimension() != dimension)
    throw InvalidArgumentException(HERE) << "Error: the covariance model should be of dimension " << dimension << ". Here, covariance model dimension=" << covarianceModel.getOutputDimension();

  if (shift.getDimension() != dimension)
    throw InvalidArgumentException(HERE) << "Error: the shift has a dimension=" << shift.getDimension() << " different from the output sample dimension=" << dimension;

  // Keep the shifted marginal samples
  Sample shiftedSample(outputSample);
  shiftedSample += shift;

  // optimization process
  BoxCoxGLMBasisOptimization boxCoxOptimization(inputSample, shiftedSample, covarianceModel, basis);
  Function objectiveFunction(boxCoxOptimization);
  MemoizeFunction objectiveMemoizeFunction(objectiveFunction, Full());
  objectiveMemoizeFunction.enableCache();
  OptimizationProblem problem(objectiveMemoizeFunction);
  problem.setMinimization(false);
  solver_.setProblem(problem);
  solver_.setStartingPoint(Point(1, 1.0));
  // run Optimization problem
  solver_.run();
  // Return optimization point
  const Point optpoint(solver_.getResult().getOptimalPoint());

  // Define BoxCox transformation for output sample
  BoxCoxEvaluation myBoxFunction(optpoint, shift);
  // compute the transformed output sample using the Box-Cox function
  const Sample transformedOutputSample = myBoxFunction(outputSample);
  // Build the GeneralLinearModelResult
  // Use of GLM to estimate the best generalized linear model
  GeneralLinearModelAlgorithm algo(inputSample, transformedOutputSample, covarianceModel, basis);
  algo.run();
  // Get result
  result = algo.getResult();
  return BoxCoxTransform(optpoint, shift);
}

BoxCoxTransform BoxCoxFactory::build(const Sample & inputSample,
                                     const Sample & outputSample,
                                     const CovarianceModel & covarianceModel,
                                     const Point & shift,
                                     GeneralLinearModelResult & result)
{
  return build(inputSample, outputSample, covarianceModel, Basis(), shift, result);
}

BoxCoxTransform BoxCoxFactory::build(const Sample & inputSample,
                                     const Sample & outputSample,
                                     const CovarianceModel & covarianceModel,
                                     const BasisCollection & basis,
                                     const Point & shift,
                                     GeneralLinearModelResult & result)
{
  LOGWARN(OSS() << "BoxCoxFactory::build(inputSample, outpuSample, covarianceModel, basisCollection, shift) is deprecated");
  checkGLMData(inputSample, outputSample, covarianceModel, basis);

  // Check the dimensions
  const UnsignedInteger dimension = outputSample.getDimension();
  if (shift.getDimension() != dimension)
    throw InvalidArgumentException(HERE) << "Error: the shift has a dimension=" << shift.getDimension() << " different from the output sample dimension=" << dimension;

  // Keep the shifted marginal samples
  Sample shiftedSample(outputSample);
  shiftedSample += shift;

  // optimization process
  BoxCoxGLMOptimization boxCoxOptimization(inputSample, shiftedSample, covarianceModel, basis, solver_);
  const Point lambda = boxCoxOptimization.optimizeLogLikelihood();
  // Define BoxCox transformation for output sample
  BoxCoxEvaluation myBoxFunction(lambda, shift);
  // compute the transformed output sample using the Box-Cox function
  const Sample transformedOutputSample = myBoxFunction(outputSample);
  // Build the GeneralLinearModelResult
  // Use of GLM to estimate the best generalized linear model
  GeneralLinearModelAlgorithm algo(inputSample, transformedOutputSample, covarianceModel, basis);
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
String BoxCoxFactory::__str__(const String & ) const
{
  OSS oss;
  oss << "class=" << BoxCoxFactory::GetClassName();
  return oss;
}

END_NAMESPACE_OPENTURNS
