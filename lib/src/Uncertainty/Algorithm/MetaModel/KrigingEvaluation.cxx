//                                               -*- C++ -*-
/**
 *  @brief The class building gaussian process regression
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

#include "KrigingEvaluation.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(KrigingEvaluation);


static const Factory<KrigingEvaluation> RegisteredFactory;


/* Constructor with parameters */
KrigingEvaluation::KrigingEvaluation()
  : NumericalMathEvaluationImplementation()
{
  // Nothing to do
}


/* Constructor with parameters */
KrigingEvaluation::KrigingEvaluation (const BasisCollection & basis,
                                      const NumericalSample & inputSample,
                                      const CovarianceModel & covarianceModel,
                                      const NumericalPointCollection & beta,
                                      const NumericalSample & gamma)
  : NumericalMathEvaluationImplementation()
  , basis_(basis)
  , inputSample_(inputSample)
  , covarianceModel_(covarianceModel)
  , beta_(beta)
  , gamma_(gamma)
{
  if (basis.getSize() > 0)
  {
    if (basis.getSize() != covarianceModel.getDimension())
      throw InvalidArgumentException(HERE) << "In KrigingEvaluation::KrigingEvaluation, output sample dimension (" << covarianceModel.getDimension()  << ") does not match multi-basis dimension (" << basis_.getSize() << ")";
    // Total basis size = sum of all sizes
    UnsignedInteger basisCollectionTotalSize(0);
    for (UnsignedInteger i = 0; i < basis.getSize(); ++i) basisCollectionTotalSize += basis[i].getSize();
  }
  if (covarianceModel.getSpatialDimension() != inputSample.getDimension()) throw InvalidArgumentException(HERE) << "In KrigingEvaluation::KrigingEvaluation, error: the spatial dimension=" << covarianceModel.getSpatialDimension() << " of the covariance model should match the dimension=" << inputSample.getDimension() << " of the input sample";
  if (gamma.getSize() != inputSample.getSize()) throw InvalidArgumentException(HERE) << "In KrigingEvaluation::KrigingEvaluation, error: the number of covariance coefficients=" << gamma.getSize() << " is different from the output sample dimension=" << covarianceModel.getDimension();
  setInputDescription(Description::BuildDefault(getInputDimension(), "x"));
  setOutputDescription(Description::BuildDefault(getOutputDimension(), "y"));
  setParameters(NumericalPointWithDescription(getInputDimension()));
}


/* Virtual constructor */
KrigingEvaluation * KrigingEvaluation::clone() const
{
  return new KrigingEvaluation(*this);
}

/* Comparison operator */
Bool KrigingEvaluation::operator==(const KrigingEvaluation & other) const
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
String KrigingEvaluation::__str__(const String & offset) const
{
  return OSS(false) << offset << GetClassName();
}

/* Test for actual implementation */
Bool KrigingEvaluation::isActualImplementation() const
{
  return true;
}

// Helper for the parallel version of the point-based evaluation operator
struct KrigingEvaluationPointFunctor
{
  const NumericalPoint & input_;
  const KrigingEvaluation & evaluation_;
  NumericalPoint accumulator_;

  KrigingEvaluationPointFunctor(const NumericalPoint & input,
                                const KrigingEvaluation & evaluation)
    : input_(input)
    , evaluation_(evaluation)
    , accumulator_(evaluation.getOutputDimension())
  {}

  KrigingEvaluationPointFunctor(const KrigingEvaluationPointFunctor & other,
                                TBB::Split)
    : input_(other.input_)
    , evaluation_(other.evaluation_)
    , accumulator_(other.evaluation_.getOutputDimension())
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r )
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

/* Operator () */
NumericalPoint KrigingEvaluation::operator()(const NumericalPoint & inP) const
{
  const UnsignedInteger trainingSize(inputSample_.getSize());
  // Evaluate the kernel part in parallel
  KrigingEvaluationPointFunctor functor( inP, *this );
  TBB::ParallelReduce( 0, trainingSize, functor );
  NumericalPoint value(functor.accumulator_);
  // Evaluate the basis part sequentially
  // Number of basis is 0 or outputDimension
  for (UnsignedInteger i = 0; i < basis_.getSize(); ++i)
  {
    // Get local basis -> basis_[i]
    const Basis localBasis(basis_[i]);
    const NumericalPoint betaBasis(beta_[i]);
    const UnsignedInteger basisSize(localBasis.getSize());
    for (UnsignedInteger j = 0; j < basisSize; ++j)
      value[i] += localBasis[j](inP)[0] * betaBasis[j];
  }
  ++callsNumber_;
  return value;
}

// Helper for the parallel version of the sample-based evaluation operator
struct KrigingEvaluationSampleFunctor
{
  const NumericalSample & input_;
  NumericalSample & output_;
  const KrigingEvaluation & evaluation_;
  UnsignedInteger trainingSize_;

  KrigingEvaluationSampleFunctor(const NumericalSample & input,
                                 NumericalSample & output,
                                 const KrigingEvaluation & evaluation)
    : input_(input)
    , output_(output)
    , evaluation_(evaluation)
    , trainingSize_(evaluation.inputSample_.getSize())
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    const UnsignedInteger start(r.begin());
    const UnsignedInteger dimension(evaluation_.getOutputDimension());
    const UnsignedInteger size(r.end() - start);
    Matrix R(dimension , trainingSize_ * dimension);
    for (UnsignedInteger i = 0; i != size; ++i)
    {
      for (UnsignedInteger j = 0; j < trainingSize_; ++j)
      {
        const CovarianceMatrix localCovariance(evaluation_.covarianceModel_(input_[start + i], evaluation_.inputSample_[j]));
        for (UnsignedInteger columnIndex = 0; columnIndex < dimension; ++ columnIndex)
          for (UnsignedInteger rowIndex = 0; rowIndex < dimension; ++ rowIndex)
            R(rowIndex, columnIndex + j * dimension) = localCovariance(rowIndex, columnIndex);
      }
      output_[start + i] = R * evaluation_.gamma_.getImplementation()->getData();
    }
  } // operator()
}; // struct KrigingEvaluationSampleFunctor

NumericalSample KrigingEvaluation::operator()(const NumericalSample & inS) const
{
  // Evaluation on the sample using parallel functors
  const UnsignedInteger size(inS.getSize());
  const UnsignedInteger dimension(getOutputDimension());

  NumericalSample result(size, dimension);
  const KrigingEvaluationSampleFunctor functor( inS, result, *this );
  TBB::ParallelFor( 0, size , functor );

  // Evaluate the basis part sequentially
  // Number of basis is 0 or outputDimension
  NumericalSample trend(size, 0);
  for (UnsignedInteger i = 0; i < basis_.getSize(); ++i)
  {
    // Get local basis -> basis_[i]
    const Basis localBasis(basis_[i]);
    const NumericalPoint betaBasis(beta_[i]);
    const UnsignedInteger basisSize(localBasis.getSize());
    // For the i-th Basis (marginal), take into account the trend
    NumericalSample fi(size, 1);

    for (UnsignedInteger j = 0; j < basisSize; ++j)
    {
      NumericalSample fj(localBasis[j](inS));
      // scale ==> use of parallelism
      fj *= betaBasis[j];
      // Adding fj to fi
      fi += fj;
    }
    // add it to the trend sample
    trend.stack(fi);
  }
  // Adding trend to result, using parallelism
  // Add should be done only if there is a trend
  if (trend.getDimension() > 0) result += trend;
  // update call numbers & return result
  callsNumber_ += size;
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
  return covarianceModel_.getDimension();
}

/* Method save() stores the object through the StorageManager */
void KrigingEvaluation::save(Advocate & adv) const
{
  NumericalMathEvaluationImplementation::save(adv);
  adv.saveAttribute("basis_", basis_);
  adv.saveAttribute("inputSample_", inputSample_);
  adv.saveAttribute("covarianceModel_", covarianceModel_);
  adv.saveAttribute("beta_", beta_);
  adv.saveAttribute("gamma_", gamma_);
}

/* Method load() reloads the object from the StorageManager */
void KrigingEvaluation::load(Advocate & adv)
{
  NumericalMathEvaluationImplementation::load(adv);
  adv.loadAttribute("basis_", basis_);
  adv.loadAttribute("inputSample_", inputSample_);
  adv.loadAttribute("covarianceModel_", covarianceModel_);
  adv.loadAttribute("beta_", beta_);
  adv.loadAttribute("gamma_", gamma_);
}


END_NAMESPACE_OPENTURNS
