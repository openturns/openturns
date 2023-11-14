//                                               -*- C++ -*-
/**
 *  @brief
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
#include "openturns/LinearModelAlgorithm.hxx"
#include "openturns/DesignProxy.hxx"
#include "openturns/LeastSquaresMethod.hxx"
#include "openturns/LinearBasisFactory.hxx"
#include "openturns/LinearCombinationFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class BoxCoxFactory
 */

CLASSNAMEINIT(BoxCoxFactory)

static const Factory<BoxCoxFactory> Factory_BoxCoxFactory;

class BoxCoxSampleOptimization : public EvaluationImplementation
{

public:

  BoxCoxSampleOptimization(const Sample & sample,
                           const Scalar sumLog)
    : sample_(sample)
    , sumLog_(sumLog)
  {
    // Nothing to do
  }

  explicit BoxCoxSampleOptimization(const Sample & sample)
    : sample_(sample)
    , sumLog_(0.0)
  {
    computeSumLog();
  }

  BoxCoxSampleOptimization *clone() const override
  {
    return new BoxCoxSampleOptimization(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    return 1;
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

  /** Likelihood function */
  Point operator()(const Point & lambda) const override
  {
    const UnsignedInteger size = sample_.getSize();
    // Define BoxCox transformation for sample
    // lambda has the right dimension as BoxCoxSampleOptimization is
    // an EvaluationImplementation
    BoxCoxEvaluation myBoxFunction(lambda);
    // compute the mean of the transformed sample using the Box-Cox function
    const Sample outSample(myBoxFunction(sample_));
    const Scalar ratio = 1.0 - 1.0 / size;
    const Scalar sigma2 = outSample.computeVariance()[0];
    Scalar result = -0.5 * size * log(sigma2 * ratio);

    // result is translated
    result += (lambda[0] - 1.0) * sumLog_;
    return Point(1, result);
  }

  void computeSumLog()
  {
    // Compute the sum of the log-data
    const UnsignedInteger size = sample_.getSize();
    sumLog_ = 0.0;
    for (UnsignedInteger k = 0; k < size; ++k) sumLog_ += std::log(sample_(k, 0));
  }

  Scalar getSumLog() const
  {
    return sumLog_;
  }

private:
  /** only used to pass data to be used in computeLogLikeliHood */
  Sample sample_;

  /** only used to pass data to be used in computeLogLikeliHood */
  Scalar sumLog_;
};

class BoxCoxGLMOptimization : public EvaluationImplementation
{

public:
  BoxCoxGLMOptimization(const Sample &inputSample,
                        const Sample &shiftedOutputSample,
                        const CovarianceModel &covarianceModel,
                        const Basis &basis)
    : inputSample_(inputSample)
    , shiftedOutputSample_(shiftedOutputSample)
    , covarianceModel_(covarianceModel)
    , basis_(basis)
    , sumLog_(0.0)
  {
    computeSumLog();
  }

  BoxCoxGLMOptimization(const Sample &inputSample,
                        const Sample &shiftedOutputSample,
                        const CovarianceModel &covarianceModel,
                        const Basis &basis,
                        const Scalar sumLog)
    : inputSample_(inputSample)
    , shiftedOutputSample_(shiftedOutputSample)
    , covarianceModel_(covarianceModel)
    , basis_(basis)
    , sumLog_(sumLog)
  {
    // Nothing to do
  }

  BoxCoxGLMOptimization *clone() const override
  {
    return new BoxCoxGLMOptimization(*this);
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
    GeneralLinearModelAlgorithm algo(inputSample_, transformedOutputSample, covarianceModel_, basis_);
    algo.run();
    // Return the optimal log-likelihood
    // Global likelihood do include (lambda - 1) * sumLog_
    const Scalar result = algo.getResult().getOptimalLogLikelihood() + (lambda[0] - 1) * getSumLog();
    return Point(1, result);
  }

  void computeSumLog()
  {
    // Compute the sum of the log-data
    const UnsignedInteger size = shiftedOutputSample_.getSize();
    const UnsignedInteger dimension = shiftedOutputSample_.getDimension();
    sumLog_ = 0.0;
    for (UnsignedInteger k = 0; k < size; ++k)
      for (UnsignedInteger d = 0; d < dimension; ++d)
        sumLog_ += std::log(shiftedOutputSample_(k, d));
  }

  Scalar getSumLog() const
  {
    return sumLog_;
  }

private:
  /** only used to pass data to be used in computeLogLikeliHood */
  Sample inputSample_;
  Sample shiftedOutputSample_;
  CovarianceModel covarianceModel_;
  Basis basis_;
  Scalar sumLog_;
};

class BoxCoxLMOptimizationEvaluation : public EvaluationImplementation
{

public:
  BoxCoxLMOptimizationEvaluation(const Sample & inputSample,
                                 const Sample & shiftedOutputSample,
                                 const Basis & basis)
    : inputSample_(inputSample)
    , shiftedOutputSample_(shiftedOutputSample)
    , basis_(basis)
    , sumLog_(0.0)
    , algo_()
  {
    initialize();
    computeSumLog();
  }

  void initialize()
  {
    const UnsignedInteger size = inputSample_.getSize();
    if (shiftedOutputSample_.getDimension() != 1)
      throw InvalidArgumentException(HERE) << "We can only handle and 1-d output sample.";

    const UnsignedInteger basisSize = basis_.getSize();
    // basisSize should be < size (not <=)
    // In case of =, residual sample is always 0 and not possible to perform an optimization
    // to set the optimal lambda value
    if (!(basisSize < size))
      throw InvalidArgumentException(HERE) << "Number of basis elements is greater than sample size. Data size = " << shiftedOutputSample_.getSize()
                                           << ", basis size = " << basisSize;

    // No particular strategy : using the full basis
    Indices indices(basis_.getSize());
    indices.fill();

    // Define the design proxy
    const DesignProxy proxy(inputSample_, basis_);

    // Compute using a least squares method
    algo_ = LeastSquaresMethod::Build(ResourceMap::GetAsString("LinearModelAlgorithm-DecompositionMethod"), proxy, indices);
  }
  BoxCoxLMOptimizationEvaluation *clone() const override
  {
    return new BoxCoxLMOptimizationEvaluation(*this);
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
    const BoxCoxEvaluation myBoxFunction(lambda);
    // compute the mean of the transformed sample using the Box-Cox function
    const Sample transformedOutputSample(myBoxFunction(shiftedOutputSample_));

    // Solve linear system
    const Point coefficients(algo_.solve(transformedOutputSample.asPoint()));

    // Define the linear model
    const LinearCombinationFunction metaModel(basis_, coefficients);

    // Residual sample
    const Sample residualSample(transformedOutputSample - metaModel(inputSample_));

    // noise variance
    const UnsignedInteger size = shiftedOutputSample_.getSize();
    const Scalar sigma2 = residualSample.computeRawMoment(2)[0];
    const Scalar result = -0.5 * size * std::log(sigma2) + (lambda[0] - 1.0) * sumLog_;
    return Point(1, result);
  }

  void computeSumLog()
  {
    // Compute the sum of the log-data
    const UnsignedInteger size = shiftedOutputSample_.getSize();
    const UnsignedInteger dimension = shiftedOutputSample_.getDimension();
    sumLog_ = 0.0;
    for (UnsignedInteger k = 0; k < size; ++k)
      for (UnsignedInteger d = 0; d < dimension; ++d)
        sumLog_ += std::log(shiftedOutputSample_(k, d));
  }

  Scalar getSumLog() const
  {
    return sumLog_;
  }

private:
  /** only used to pass data to be used in computeLogLikeliHood */
  Sample inputSample_;
  Sample shiftedOutputSample_;
  Basis basis_;
  Scalar sumLog_;
  mutable LeastSquaresMethod algo_;
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

BoxCoxTransform BoxCoxFactory::buildWithGraph(const Field & timeSeries,
    const Point & shift,
    Graph & graph) const
{
  return buildWithGraph(timeSeries.getValues(), shift, graph);
}

BoxCoxTransform BoxCoxFactory::build(const Sample & sample) const
{
  return build(sample, Point(sample.getDimension()));
}

BoxCoxTransform BoxCoxFactory::build(const Sample & sample,
                                     const Point & shift) const
{
  Graph tmp;
  return buildWithGraph(sample, shift, tmp);
}

BoxCoxTransform BoxCoxFactory::buildWithGraph(const Sample & sample,
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
    // Extract the marginal sample and apply the shift
    marginalSamples[d] = sample.getMarginal(d);
    marginalSamples[d] += Point(1, shift[d]);

    BoxCoxSampleOptimization boxCoxOptimization(marginalSamples[d]);
    Function objectiveFunction(boxCoxOptimization);
    // Define optimization problem
    OptimizationProblem problem((objectiveFunction));
    problem.setMinimization(false);
    OptimizationAlgorithm solver(solver_);
    solver.setProblem(problem);
    solver.setStartingPoint(Point(1, 1.0));
    // run Optimization problem
    solver.run();
    // Return optimization point
    const Point optpoint(solver.getResult().getOptimalPoint());
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
  Point lambdaMarginal(1, 0.0);
  Sample logLikelihoodValues(npts, 1);
  for (UnsignedInteger d = 0; d < dimension; ++d)
  {
    BoxCoxSampleOptimization boxCoxOptimization(marginalSamples[d], sumLog[d]);
    for (UnsignedInteger i = 0; i < npts; ++i)
    {
      lambdaMarginal[0] = lambdaValues(i, 0);
      logLikelihoodValues(i, 0) = boxCoxOptimization(lambdaMarginal)[0];
    }
    Curve curve(lambdaValues, logLikelihoodValues);
    curve.setColor(Curve::ConvertFromHSV((360.0 * d) / dimension, 1.0, 1.0));
    graph.add(curve);
    Point optimum(2);
    optimum[0] = lambda[d];
    optimum[1] = boxCoxOptimization(Point(1, optimum[0]))[0];
    Cloud cloud(Sample(1, optimum));
    cloud.setColor(curve.getColor());
    cloud.setPointStyle("circle");
    cloud.setLegend(String(OSS() << "lambda=" << lambda[d]));
    graph.add(cloud);
  }
  // return result ==> Box Cox transform
  return BoxCoxTransform(lambda, shift);
}

/** Build the factory from data by estimating the best generalized linear model */
BoxCoxTransform BoxCoxFactory::buildWithGLM(const Sample & inputSample,
    const Sample & outputSample,
    const CovarianceModel & covarianceModel,
    const Basis & basis,
    const Point & shift,
    GeneralLinearModelResult & generalLinearModelResult)
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
  BoxCoxGLMOptimization boxCoxOptimization(inputSample, shiftedSample, covarianceModel, basis);
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
  generalLinearModelResult = algo.getResult();
  return BoxCoxTransform(optpoint, shift);
}

BoxCoxTransform BoxCoxFactory::buildWithGLM(const Sample &inputSample,
    const Sample &outputSample,
    const CovarianceModel &covarianceModel,
    const Point &shift,
    GeneralLinearModelResult &generalLinearModelResult)
{
  return buildWithGLM(inputSample, outputSample, covarianceModel, Basis(), shift, generalLinearModelResult);
}

/** Build the factory from data by estimating the best generalized linear model */
BoxCoxTransform BoxCoxFactory::buildWithLM(const Sample &inputSample,
    const Sample &outputSample,
    const Basis &basis,
    const Point &shift,
    LinearModelResult &linearModelResult)
{
  // Check the input size
  const UnsignedInteger size = inputSample.getSize();
  if (size == 0)
    throw InvalidArgumentException(HERE) << "Error: cannot build a Box-Cox factory from empty data";

  if (size != outputSample.getSize())
    throw InvalidArgumentException(HERE) << "Error: input and output sample have different size. Could not perform linear model & Box-Cox algorithms";

  // Check the dimensions
  const UnsignedInteger dimension = outputSample.getDimension();

  if (shift.getDimension() != dimension)
    throw InvalidArgumentException(HERE) << "Error: the shift has a dimension=" << shift.getDimension() << " different from the output sample dimension=" << dimension;

  // Keep the shifted marginal samples
  Sample shiftedSample(outputSample);
  shiftedSample += shift;

  // optimization process
  const BoxCoxLMOptimizationEvaluation boxCoxOptimization(inputSample, shiftedSample, basis);
  const Function objectiveFunction(boxCoxOptimization);
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
  const BoxCoxEvaluation myBoxFunction(optpoint, shift);
  // compute the transformed output sample using the Box-Cox function
  const Sample transformedOutputSample(myBoxFunction(outputSample));
  // Build the LinearModelResult
  LinearModelAlgorithm algo(inputSample, transformedOutputSample, basis);
  algo.run();
  // Get result
  linearModelResult = algo.getResult();
  return BoxCoxTransform(optpoint, shift);
}

BoxCoxTransform BoxCoxFactory::buildWithLM(const Sample &inputSample,
    const Sample &outputSample,
    const Point &shift,
    LinearModelResult &linearModelResult)
{
  const Basis basis(LinearBasisFactory(inputSample.getDimension()).build());
  return buildWithLM(inputSample, outputSample, basis, shift, linearModelResult);
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
