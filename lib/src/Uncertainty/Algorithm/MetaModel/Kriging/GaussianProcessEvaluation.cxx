//                                               -*- C++ -*-
/**
 *  @brief The class building gaussian process regression
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

#include "openturns/GaussianProcessEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/TBBImplementation.hxx"
#include "openturns/AggregatedFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GaussianProcessEvaluation)


static const Factory<GaussianProcessEvaluation> Factory_GaussianProcessEvaluation;


/* Constructor with parameters */
GaussianProcessEvaluation::GaussianProcessEvaluation()
  : EvaluationImplementation()
{
  // Nothing to do
}


/* Constructor with parameters */
GaussianProcessEvaluation::GaussianProcessEvaluation (const Basis & basis,
    const Sample & inputSample,
    const CovarianceModel & covarianceModel,
    const Point & beta,
    const Sample & gamma)
  : EvaluationImplementation()
  , basis_(basis)
  , inputSample_(inputSample)
  , covarianceModel_(covarianceModel)
  , beta_(beta)
  , gamma_(gamma)
{
  if (!basis.isFinite())
    throw InvalidArgumentException(HERE) << "In GaussianProcessEvaluation::GaussianProcessEvaluation, basis should be finite!";
  const UnsignedInteger size = basis.getSize();
  for (UnsignedInteger index = 0; index < size; ++index)
  {
    if (basis[index].getOutputDimension() != covarianceModel.getOutputDimension())
      throw InvalidArgumentException(HERE) << "In GaussianProcessEvaluation::GaussianProcessEvaluation, output sample dimension=" << covarianceModel.getOutputDimension() << " does not match basis[=" << index << "] dimension=" << basis[index].getOutputDimension();
    if (basis[index].getInputDimension() != inputSample_.getDimension())
      throw InvalidArgumentException(HERE) << "In GaussianProcessEvaluation::GaussianProcessEvaluation, input sample dimension=" << inputSample_.getDimension() << " does not match basis[=" << index << "] dimension=" << basis[index].getInputDimension();
  }

  if (covarianceModel.getInputDimension() != inputSample.getDimension()) throw InvalidArgumentException(HERE) << "In GaussianProcessEvaluation::GaussianProcessEvaluation, error: the input dimension=" << covarianceModel.getInputDimension() << " of the covariance model should match the dimension=" << inputSample.getDimension() << " of the input sample";
  if (gamma.getSize() != inputSample.getSize()) throw InvalidArgumentException(HERE) << "In GaussianProcessEvaluation::GaussianProcessEvaluation, error: the number of covariance coefficients=" << gamma.getSize() << " is different from the output sample dimension=" << covarianceModel.getOutputDimension();
  setInputDescription(Description::BuildDefault(getInputDimension(), "x"));
  setOutputDescription(Description::BuildDefault(getOutputDimension(), "y"));
  setParameter(Point(getInputDimension()));
  setParameterDescription(Description(getInputDimension()));
}


/* Virtual constructor */
GaussianProcessEvaluation * GaussianProcessEvaluation::clone() const
{
  return new GaussianProcessEvaluation(*this);
}

/* Comparison operator */
Bool GaussianProcessEvaluation::operator==(const GaussianProcessEvaluation & ) const
{
  return true;
}

/* String converter */
String GaussianProcessEvaluation::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " correlationModel=" << covarianceModel_
      << " beta=" << beta_
      << " gamma=" << gamma_;
  return oss;
}

/* String converter */
String GaussianProcessEvaluation::__str__(const String & ) const
{
  return OSS(false) << GetClassName();
}

// Helper for the parallel version of the point-based evaluation operator
struct GaussianProcessEvaluationPointFunctor
{
  const Point & input_;
  const GaussianProcessEvaluation & evaluation_;
  Point accumulator_;

  GaussianProcessEvaluationPointFunctor(const Point & input,
                                        const GaussianProcessEvaluation & evaluation)
    : input_(input)
    , evaluation_(evaluation)
    , accumulator_(evaluation.getOutputDimension())
  {}

  GaussianProcessEvaluationPointFunctor(const GaussianProcessEvaluationPointFunctor & other,
                                        TBBImplementation::Split)
    : input_(other.input_)
    , evaluation_(other.evaluation_)
    , accumulator_(other.evaluation_.getOutputDimension())
  {}

  inline void operator()( const TBBImplementation::BlockedRange<UnsignedInteger> & r )
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      accumulator_ = accumulator_ + evaluation_.covarianceModel_(input_, evaluation_.inputSample_[i]) * evaluation_.gamma_[i];
    }
  } // operator()

  inline void join(const GaussianProcessEvaluationPointFunctor & other)
  {
    accumulator_ += other.accumulator_;
  }

}; // struct GaussianProcessEvaluationPointFunctor

struct GaussianProcessEvaluationPointFunctor1D
{
  const Point & input_;
  const GaussianProcessEvaluation & evaluation_;
  Scalar accumulator_;

  GaussianProcessEvaluationPointFunctor1D(const Point & input,
                                          const GaussianProcessEvaluation & evaluation)
    : input_(input)
    , evaluation_(evaluation)
    , accumulator_(0.0)
  {}

  GaussianProcessEvaluationPointFunctor1D(const GaussianProcessEvaluationPointFunctor1D & other,
                                          TBBImplementation::Split)
    : input_(other.input_)
    , evaluation_(other.evaluation_)
    , accumulator_(0.0)
  {}

  inline void operator()( const TBBImplementation::BlockedRange<UnsignedInteger> & r )
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      accumulator_ += evaluation_.covarianceModel_.getImplementation()->computeAsScalar(input_.begin(), evaluation_.inputSample_.getImplementation()->data_begin() + i * input_.getDimension()) * evaluation_.gamma_(i, 0);
    }
  } // operator()

  inline void join(const GaussianProcessEvaluationPointFunctor1D & other)
  {
    accumulator_ += other.accumulator_;
  }

}; // struct GaussianProcessEvaluationPointFunctor1D

/* Operator () */
Point GaussianProcessEvaluation::operator()(const Point & inP) const
{
  const UnsignedInteger trainingSize = inputSample_.getSize();
  const UnsignedInteger dimension = getOutputDimension();
  Point value(dimension);
  // Evaluate the kernel part in parallel
  if (dimension == 1)
  {
    GaussianProcessEvaluationPointFunctor1D functor( inP, *this );
    TBBImplementation::ParallelReduceIf(covarianceModel_.getImplementation()->isParallel(), 0, trainingSize, functor );
    value[0] = functor.accumulator_;
  }
  else
  {
    GaussianProcessEvaluationPointFunctor functor( inP, *this );
    TBBImplementation::ParallelReduceIf(covarianceModel_.getImplementation()->isParallel(), 0, trainingSize, functor );
    value = functor.accumulator_;
  }
  // Evaluate the trend part sequentially
  UnsignedInteger index = 0;
  for (UnsignedInteger i = 0; i < basis_.getSize(); ++i)
  {
    const Point phi_i(basis_[i](inP));
    for (UnsignedInteger outputMarginal = 0; outputMarginal < getOutputDimension(); ++outputMarginal)
    {
      value[outputMarginal] += phi_i[outputMarginal] * beta_[index];
      ++index;
    }
  }
  callsNumber_.increment();
  return value;
}

// Helper for the parallel version of the sample-based evaluation operator
struct GaussianProcessEvaluationSampleFunctor
{
  const Sample & input_;
  Sample & output_;
  const GaussianProcessEvaluation & evaluation_;
  UnsignedInteger trainingSize_;

  GaussianProcessEvaluationSampleFunctor(const Sample & input,
                                         Sample & output,
                                         const GaussianProcessEvaluation & evaluation)
    : input_(input)
    , output_(output)
    , evaluation_(evaluation)
    , trainingSize_(evaluation.inputSample_.getSize())
  {}

  inline void operator()( const TBBImplementation::BlockedRange<UnsignedInteger> & r ) const
  {
    const UnsignedInteger dimension = evaluation_.getOutputDimension();
    Matrix R(dimension, trainingSize_ * dimension);
    for (UnsignedInteger i = r.begin(); i != r.end(); ++ i)
    {
      for (UnsignedInteger j = 0; j < trainingSize_; ++j)
      {
        const SquareMatrix localCovariance(evaluation_.covarianceModel_(input_[i], evaluation_.inputSample_[j]));
        for (UnsignedInteger columnIndex = 0; columnIndex < dimension; ++ columnIndex)
          for (UnsignedInteger rowIndex = 0; rowIndex < dimension; ++ rowIndex)
            R(rowIndex, columnIndex + j * dimension) = localCovariance(rowIndex, columnIndex);
      }
      const Point RGamma(R * evaluation_.gamma_.getImplementation()->getData());
      for (UnsignedInteger j = 0; j < dimension; ++j)  output_(i, j) = RGamma[j];
    }
  } // operator()
}; // struct GaussianProcessEvaluationSampleFunctor

struct GaussianProcessEvaluationSampleFunctor1D
{
  const Sample & input_;
  Sample & output_;
  const GaussianProcessEvaluation & evaluation_;
  UnsignedInteger trainingSize_;

  GaussianProcessEvaluationSampleFunctor1D(const Sample & input,
      Sample & output,
      const GaussianProcessEvaluation & evaluation)
    : input_(input)
    , output_(output)
    , evaluation_(evaluation)
    , trainingSize_(evaluation.inputSample_.getSize())
  {
    // Nothing to do
  }

  inline void operator()( const TBBImplementation::BlockedRange<UnsignedInteger> & r ) const
  {
    const UnsignedInteger inputDimension = input_.getDimension();
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      for (UnsignedInteger j = 0; j < trainingSize_; ++j)
        output_(i, 0) += evaluation_.covarianceModel_.getImplementation()->computeAsScalar(input_.getImplementation()->data_begin() + i * inputDimension, evaluation_.inputSample_.getImplementation()->data_begin() + j * inputDimension) * evaluation_.gamma_(j, 0);
    } // i
  } // operator()
}; // struct GaussianProcessEvaluationSampleFunctor1D

Sample GaussianProcessEvaluation::operator()(const Sample & inS) const
{
  // Evaluation on the sample using parallel functors
  const UnsignedInteger size = inS.getSize();
  const UnsignedInteger dimension = getOutputDimension();

  Sample result(size, dimension);
  if (dimension == 1)
  {
    const GaussianProcessEvaluationSampleFunctor1D functor( inS, result, *this);
    TBBImplementation::ParallelForIf(covarianceModel_.getImplementation()->isParallel(), 0, size, functor );
  }
  else
  {
    const GaussianProcessEvaluationSampleFunctor functor( inS, result, *this );
    TBBImplementation::ParallelForIf(covarianceModel_.getImplementation()->isParallel(), 0, size, functor );
  }

  // Evaluate the basis part sequentially
  // Number of basis is 0 or outputDimension
  Sample trend(size, getOutputDimension());
  for (UnsignedInteger i = 0; i < basis_.getSize(); ++i)
  {
    // Evaluate basis_[i] on the full sample & benefit from the parallelism
    // of the underlying functions
    const Sample phi_i(basis_[i](inS));
    for (UnsignedInteger outputMarginal = 0; outputMarginal < getOutputDimension(); ++outputMarginal)
    {
      for (UnsignedInteger k = 0; k < size; ++k)
        trend(k, outputMarginal) += phi_i(k, outputMarginal) * beta_[i * getOutputDimension() + outputMarginal];
    }
  }
  // Adding trend to result, using parallelism
  // Add should be done only if there is a trend
  if (trend.getDimension() > 0) result += trend;
  // update call numbers & return result
  callsNumber_.fetchAndAdd(size);
  return result;
}


/* Accessor for input point dimension */
UnsignedInteger GaussianProcessEvaluation::getInputDimension() const
{
  return inputSample_.getDimension();
}

/* Accessor for output point dimension */
UnsignedInteger GaussianProcessEvaluation::getOutputDimension() const
{
  return covarianceModel_.getOutputDimension();
}

/* Method save() stores the object through the StorageManager */
void GaussianProcessEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute("basis_", basis_);
  adv.saveAttribute("inputSample_", inputSample_);
  adv.saveAttribute("covarianceModel_", covarianceModel_);
  adv.saveAttribute("beta_", beta_);
  adv.saveAttribute("gamma_", gamma_);
}

/* Method load() reloads the object from the StorageManager */
void GaussianProcessEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute("inputSample_", inputSample_);
  adv.loadAttribute("covarianceModel_", covarianceModel_);
  adv.loadAttribute("gamma_", gamma_);
  adv.loadAttribute("basis_", basis_);
  adv.loadAttribute("beta_", beta_);
}

END_NAMESPACE_OPENTURNS
