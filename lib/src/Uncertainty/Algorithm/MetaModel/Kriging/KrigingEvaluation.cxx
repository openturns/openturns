//                                               -*- C++ -*-
/**
 *  @brief The class building gaussian process regression
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

#include "openturns/KrigingEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/TBBImplementation.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/AggregatedFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(KrigingEvaluation)


static const Factory<KrigingEvaluation> Factory_KrigingEvaluation;

// needed for backward compatibility
static const Factory<PersistentCollection<Basis> > Factory_PersistentCollection_Basis;

/* Constructor with parameters */
KrigingEvaluation::KrigingEvaluation()
  : EvaluationImplementation()
{
  // Nothing to do
}


/* Constructor with parameters */
KrigingEvaluation::KrigingEvaluation (const Basis & basis,
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
    throw InvalidArgumentException(HERE) << "In KrigingEvaluation::KrigingEvaluation, basis should be finite!";
  const UnsignedInteger size = basis.getSize();
  for (UnsignedInteger index = 0; index < size; ++index)
  {
    if (basis[index].getOutputDimension() != covarianceModel.getOutputDimension())
      throw InvalidArgumentException(HERE) << "In KrigingEvaluation::KrigingEvaluation, output sample dimension=" << covarianceModel.getOutputDimension() << " does not match basis[=" << index << "] dimension=" << basis[index].getOutputDimension();
    if (basis[index].getInputDimension() != inputSample_.getDimension())
      throw InvalidArgumentException(HERE) << "In KrigingEvaluation::KrigingEvaluation, input sample dimension=" << inputSample_.getDimension() << " does not match basis[=" << index << "] dimension=" << basis[index].getInputDimension();
  }

  if (covarianceModel.getInputDimension() != inputSample.getDimension()) throw InvalidArgumentException(HERE) << "In KrigingEvaluation::KrigingEvaluation, error: the input dimension=" << covarianceModel.getInputDimension() << " of the covariance model should match the dimension=" << inputSample.getDimension() << " of the input sample";
  if (gamma.getSize() != inputSample.getSize()) throw InvalidArgumentException(HERE) << "In KrigingEvaluation::KrigingEvaluation, error: the number of covariance coefficients=" << gamma.getSize() << " is different from the output sample dimension=" << covarianceModel.getOutputDimension();
  setInputDescription(Description::BuildDefault(getInputDimension(), "x"));
  setOutputDescription(Description::BuildDefault(getOutputDimension(), "y"));
  setParameter(Point(getInputDimension()));
  setParameterDescription(Description(getInputDimension()));
}


/* Virtual constructor */
KrigingEvaluation * KrigingEvaluation::clone() const
{
  return new KrigingEvaluation(*this);
}

/* Comparison operator */
Bool KrigingEvaluation::operator==(const KrigingEvaluation & ) const
{
  return true;
}

/* String converter */
String KrigingEvaluation::__repr__() const
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
String KrigingEvaluation::__str__(const String & ) const
{
  return OSS(false) << GetClassName();
}

// Helper for the parallel version of the point-based evaluation operator
struct KrigingEvaluationPointFunctor
{
  const Point & input_;
  const KrigingEvaluation & evaluation_;
  Point accumulator_;

  KrigingEvaluationPointFunctor(const Point & input,
                                const KrigingEvaluation & evaluation)
    : input_(input)
    , evaluation_(evaluation)
    , accumulator_(evaluation.getOutputDimension())
  {}

  KrigingEvaluationPointFunctor(const KrigingEvaluationPointFunctor & other,
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

  inline void join(const KrigingEvaluationPointFunctor & other)
  {
    accumulator_ += other.accumulator_;
  }

}; // struct KrigingEvaluationPointFunctor

struct KrigingEvaluationPointFunctor1D
{
  const Point & input_;
  const KrigingEvaluation & evaluation_;
  Scalar accumulator_;

  KrigingEvaluationPointFunctor1D(const Point & input,
                                  const KrigingEvaluation & evaluation)
    : input_(input)
    , evaluation_(evaluation)
    , accumulator_(0.0)
  {}

  KrigingEvaluationPointFunctor1D(const KrigingEvaluationPointFunctor1D & other,
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

  inline void join(const KrigingEvaluationPointFunctor1D & other)
  {
    accumulator_ += other.accumulator_;
  }

}; // struct KrigingEvaluationPointFunctor1D

/* Operator () */
Point KrigingEvaluation::operator()(const Point & inP) const
{
  const UnsignedInteger trainingSize = inputSample_.getSize();
  const UnsignedInteger dimension = getOutputDimension();
  Point value(dimension);
  // Evaluate the kernel part in parallel
  if (dimension == 1)
  {
    KrigingEvaluationPointFunctor1D functor( inP, *this );
    TBBImplementation::ParallelReduceIf(covarianceModel_.getImplementation()->isParallel(), 0, trainingSize, functor );
    value[0] = functor.accumulator_;
  }
  else
  {
    KrigingEvaluationPointFunctor functor( inP, *this );
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
struct KrigingEvaluationSampleFunctor
{
  const Sample & input_;
  Sample & output_;
  const KrigingEvaluation & evaluation_;
  UnsignedInteger trainingSize_;

  KrigingEvaluationSampleFunctor(const Sample & input,
                                 Sample & output,
                                 const KrigingEvaluation & evaluation)
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
}; // struct KrigingEvaluationSampleFunctor

struct KrigingEvaluationSampleFunctor1D
{
  const Sample & input_;
  Sample & output_;
  const KrigingEvaluation & evaluation_;
  UnsignedInteger trainingSize_;

  KrigingEvaluationSampleFunctor1D(const Sample & input,
                                   Sample & output,
                                   const KrigingEvaluation & evaluation)
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
}; // struct KrigingEvaluationSampleFunctor1D

Sample KrigingEvaluation::operator()(const Sample & inS) const
{
  // Evaluation on the sample using parallel functors
  const UnsignedInteger size = inS.getSize();
  const UnsignedInteger dimension = getOutputDimension();

  Sample result(size, dimension);
  if (dimension == 1)
  {
    const KrigingEvaluationSampleFunctor1D functor( inS, result, *this);
    TBBImplementation::ParallelForIf(covarianceModel_.getImplementation()->isParallel(), 0, size, functor );
  }
  else
  {
    const KrigingEvaluationSampleFunctor functor( inS, result, *this );
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
UnsignedInteger KrigingEvaluation::getInputDimension() const
{
  return inputSample_.getDimension();
}

/* Accessor for output point dimension */
UnsignedInteger KrigingEvaluation::getOutputDimension() const
{
  return covarianceModel_.getOutputDimension();
}

/* Method save() stores the object through the StorageManager */
void KrigingEvaluation::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute("basis_", basis_);
  adv.saveAttribute("inputSample_", inputSample_);
  adv.saveAttribute("covarianceModel_", covarianceModel_);
  adv.saveAttribute("beta_", beta_);
  adv.saveAttribute("gamma_", gamma_);
}

/* Method load() reloads the object from the StorageManager */
void KrigingEvaluation::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute("inputSample_", inputSample_);
  adv.loadAttribute("covarianceModel_", covarianceModel_);
  adv.loadAttribute("gamma_", gamma_);

  if (adv.getStudyVersion() >= 102100)
  {
    adv.loadAttribute("basis_", basis_);
    adv.loadAttribute("beta_", beta_);
  }
  else
  {
    // Backward load method
    // Here the implementation suggests that we rely on a collection of Basis and Point
    // Partially compatible with new implementation : we should have here the basis of
    // same size
    PersistentCollection<Basis> basis;
    PersistentCollection<Point> beta;
    adv.loadAttribute("basis_", basis);
    adv.loadAttribute("beta_", beta);
    if (basis.getSize() != covarianceModel_.getOutputDimension())
      throw InvalidArgumentException(HERE) << "Collection size differ from covariance model output dimension. Collection size="
                                           << basis.getSize() << " whereas covariance model output dimension = " << covarianceModel_.getOutputDimension();
    const UnsignedInteger basisSize = basis[0].getSize();
    for (UnsignedInteger outputMarginalIndex = 1; outputMarginalIndex < basis.getSize(); ++outputMarginalIndex)
    {
      if (basis[outputMarginalIndex].getSize() != basisSize)
        throw InvalidArgumentException(HERE) << "With new implementation, we should have all basis of same size. Here, basis[0].size = " << basisSize
                                             << " whereas basis[" << outputMarginalIndex << "].size = " << basis[outputMarginalIndex].getSize();
    }
    // Now we convert the persistent collection of Basis/Point into a standard multivariate Basis
    beta_ = Point(basisSize * basis.getSize());
    Collection<Function> marginalCollection(covarianceModel_.getOutputDimension());
    Collection<Function> phi(basisSize);
    UnsignedInteger index = 0;

    for (UnsignedInteger j = 0; j < basisSize; ++j)
    {
      for (UnsignedInteger outputMarginalIndex = 0; outputMarginalIndex < basis.getSize(); ++outputMarginalIndex)
      {
        marginalCollection[outputMarginalIndex] = basis[outputMarginalIndex].build(j);
        beta_[index] = beta[outputMarginalIndex][j];
      }
      phi[j] = AggregatedFunction(marginalCollection);
    }
    basis_ = Basis(phi);
  } //else
}

END_NAMESPACE_OPENTURNS
