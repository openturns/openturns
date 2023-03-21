
//                                               -*- C++ -*-
/**
 * @brief HSICEstimatorImplementation implements the HSIC sensivity indices.
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
#include "openturns/HSICEstimatorImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Log.hxx"
#include "openturns/HSICStat.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Cloud.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Pie.hxx"
#include "openturns/Text.hxx"
#include "openturns/TBBImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS
CLASSNAMEINIT(HSICEstimatorImplementation)

static const Factory<HSICEstimatorImplementation> Factory_HSICEstimatorImplementation;

/* Default constructor */
HSICEstimatorImplementation::HSICEstimatorImplementation()
  : PersistentObject()
  , covarianceModelCollection_()
  , inputSample_()
  , outputSample_()
  , estimatorType_()
  , n_(0)
  , inputDimension_(0)
  , HSIC_XY_()
  , HSIC_XX_()
  , HSIC_YY_()
  , R2HSICIndices_()
  , PValuesPermutation_()
  , inputCovarianceMatrixCollection_()
  , outputCovarianceMatrix_()
  , permutationSize_(ResourceMap::GetAsUnsignedInteger("HSICEstimator-PermutationSize"))
{
// Nothing
}


/* Constructor */
HSICEstimatorImplementation::HSICEstimatorImplementation(
  const CovarianceModelCollection & covarianceModelCollection
  , const Sample & X
  , const Sample & Y
  , const HSICStat & estimatorType)
  : PersistentObject()
  , covarianceModelCollection_(covarianceModelCollection)
  , inputSample_(X)
  , outputSample_(Y)
  , estimatorType_(estimatorType)
  , n_(X.getSize())
  , inputDimension_(X.getDimension())
  , HSIC_XY_ ()
  , HSIC_XX_ ()
  , HSIC_YY_ ()
  , R2HSICIndices_ ()
  , inputCovarianceMatrixCollection_(n_)
  , outputCovarianceMatrix_ ()
  , permutationSize_(ResourceMap::GetAsUnsignedInteger("HSICEstimator-PermutationSize"))
{
  if (covarianceModelCollection_.getSize() != (inputSample_.getDimension() + outputSample_.getDimension())) throw InvalidDimensionException(HERE) << "The number of covariance momdels is the dimension of the input +1";
  if (outputSample_.getDimension() != 1) throw InvalidDimensionException(HERE) << "The dimension of the output is 1.";
  if (inputSample_.getSize() != outputSample_.getSize()) throw InvalidDimensionException(HERE) << "Input and output samples must have the same size";
}

/* Virtual constructor */
HSICEstimatorImplementation * HSICEstimatorImplementation::clone() const
{
  return new HSICEstimatorImplementation(*this);
}

/* Compute the covariance matrices associated to the inputs and outputs */
void HSICEstimatorImplementation::computeCovarianceMatrices()
{
  for(UnsignedInteger dim = 0; dim < inputDimension_; ++dim)
  {
	inputCovarianceMatrixCollection_[dim] = covarianceModelCollection_[dim].discretize(inputSample_.getMarginal(dim));
  }
  outputCovarianceMatrix_ = covarianceModelCollection_[inputDimension_].discretize(outputSample_);
}


/* Compute the weight matrix from the weight function */
SquareMatrix HSICEstimatorImplementation::computeWeightMatrix(const Sample&) const
{
  throw NotYetImplementedException(HERE) << "Use the class HSICEstimatorConditionalSensitivity in order to perform conditional sensitivity analysis";
}

/* Compute a HSIC index (one marginal) by using the underlying estimator (biased or not) */
Scalar HSICEstimatorImplementation::computeHSICIndex(const CovarianceMatrix & covMat1,
    const CovarianceMatrix & covMat2,
    const SquareMatrix & weightMatrix) const
{
  return estimatorType_.computeHSICIndex(covMat1, covMat2, weightMatrix);
}

/* Compute HSIC and R2-HSIC indices */
void HSICEstimatorImplementation::computeIndices() const
{
  /* Compute weights */
  const SquareMatrix W(computeWeightMatrix(outputSample_));

  /* Init */
  HSIC_XX_ = Point(inputDimension_);
  HSIC_XY_ = Point(inputDimension_);
  HSIC_YY_ = Point(1);

  /* Loop over marginals: HSIC indices */
  for (UnsignedInteger dim = 0; dim < inputDimension_; ++dim)
  {
    HSIC_XY_[dim] = computeHSICIndex(inputCovarianceMatrixCollection_[dim], outputCovarianceMatrix_, W);
    HSIC_XX_[dim] = computeHSICIndex(inputCovarianceMatrixCollection_[dim], inputCovarianceMatrixCollection_[dim], W);
  }
  HSIC_YY_[0] = computeHSICIndex(outputCovarianceMatrix_, outputCovarianceMatrix_, W);

  /* Compute R2-HSIC */
  R2HSICIndices_ = Point(inputDimension_);
  for (UnsignedInteger dim = 0; dim < inputDimension_; ++dim)
  {
    R2HSICIndices_[dim] = HSIC_XY_[dim] / std::sqrt(HSIC_XX_[dim] * HSIC_YY_[0]);
  }

  isAlreadyComputedIndices_ = true ;
}

/* Set permutation size */
void HSICEstimatorImplementation::setPermutationSize(const UnsignedInteger B)
{
  permutationSize_ = B;
  PValuesPermutation_ = Point();
  isAlreadyComputedPValuesPermutation_ = false;
}

/* Get permutation size */
UnsignedInteger HSICEstimatorImplementation::getPermutationSize() const
{
  return permutationSize_;
}

/* Compute p-value with permutation */
void HSICEstimatorImplementation::computePValuesPermutationSequential() const
{
  const SquareMatrix Wobs(computeWeightMatrix(outputSample_));
  Point HSICobs(inputDimension_);
  for (UnsignedInteger dim = 0; dim < inputDimension_; ++dim)
  {
    HSICobs[dim] = computeHSICIndex(inputCovarianceMatrixCollection_[dim], outputCovarianceMatrix_, Wobs);
  }
  
  PValuesPermutation_ = Point(inputDimension_);
  Sample shuffledSample(outputSample_);
  CovarianceMatrix shuffledCovariance(outputCovarianceMatrix_);
  SquareMatrix shuffledWeight(Wobs);
  for (UnsignedInteger b = 0; b < permutationSize_; ++b)
  {
    const Indices indices(shuffleIndices(outputSample_.getSize()));
    for (UnsignedInteger j = 0; j < outputCovarianceMatrix_.getDimension(); ++j)
      {
        const UnsignedInteger newJ = indices[j];
        shuffledSample(j, 0) = outputSample_(newJ, 0);
        for (UnsignedInteger i = j; i < outputCovarianceMatrix_.getDimension(); ++i)
          {
            const UnsignedInteger newI = indices[i];
            shuffledCovariance(i, j) = outputCovarianceMatrix_(newI, newJ);
          } // i
      } // j
    shuffledWeight = computeWeightMatrix(shuffledSample);
    for (UnsignedInteger dim = 0; dim < inputDimension_; ++dim)
      {
        const Scalar HSICloc = computeHSICIndex(inputCovarianceMatrixCollection_[dim], shuffledCovariance, shuffledWeight);
        if (HSICloc > HSICobs[dim]) ++PValuesPermutation_[dim];
      }
  } // b
  PValuesPermutation_ /= (permutationSize_ + 1.0);

  isAlreadyComputedPValuesPermutation_ = true;
}

// Helper for the parallel version of the pvalue-permutation computation
struct HSICPValuesPermutationFunctor
{
  const Collection<Indices> indicesCollection_;
  const Point HSICobs_;
  const Collection<CovarianceMatrix> inputCovarianceMatrixCollection_;
  const Sample outputSample_;
  const CovarianceMatrix outputCovarianceMatrix_;
  const Pointer<HSICEstimatorImplementation> p_hsic_;
  Point accumulator_;

  HSICPValuesPermutationFunctor(const Collection<Indices> & indicesCollection,
                                const Point & HSICobs,
                                const HSICEstimatorImplementation & hsic)
    : indicesCollection_(indicesCollection)
    , HSICobs_(HSICobs)
    , p_hsic_(hsic.clone())
    , accumulator_(HSICobs.getDimension())
  {}

  HSICPValuesPermutationFunctor(const HSICPValuesPermutationFunctor & other,
                                TBBImplementation::Split)
    : indicesCollection_(other.indicesCollection_)
    , HSICobs_(other.HSICobs_)
    , p_hsic_(other.p_hsic_)
    , accumulator_(other.accumulator_.getDimension())
  {}

  inline void operator()( const TBBImplementation::BlockedRange<UnsignedInteger> & r )
  {
    Sample shuffledSample(p_hsic_->outputSample_.getSize(), p_hsic_->outputSample_.getDimension());
    CovarianceMatrix shuffledCovariance(p_hsic_->outputCovarianceMatrix_.getDimension());
    SquareMatrix shuffledWeight;
    for (UnsignedInteger b = r.begin(); b != r.end(); ++b)
      {
        const Indices indices(indicesCollection_[b]);
        for (UnsignedInteger j = 0; j < p_hsic_->outputCovarianceMatrix_.getDimension(); ++j)
        {
            const UnsignedInteger newJ = indices[j];
            shuffledSample(j, 0) = p_hsic_->outputSample_(newJ, 0);
            for (UnsignedInteger i = j; i < p_hsic_->outputCovarianceMatrix_.getDimension(); ++i)
              {
                const UnsignedInteger newI = indices[i];
                shuffledCovariance(i, j) = p_hsic_->outputCovarianceMatrix_(newI, newJ);
              } // i
          } // j
        shuffledWeight = p_hsic_->computeWeightMatrix(shuffledSample);
        for (UnsignedInteger dim = 0; dim < accumulator_.getDimension(); ++dim)
          {
            const Scalar HSICloc = p_hsic_->computeHSICIndex(p_hsic_->inputCovarianceMatrixCollection_[dim], shuffledCovariance, shuffledWeight);
            if (HSICloc > HSICobs_[dim]) ++accumulator_[dim];
          }
      } // b
  } // operator()

  inline void join(const HSICPValuesPermutationFunctor & other)
  {
    accumulator_ += other.accumulator_;
  }

}; // struct HSICPValuesPermutationFunctor

/* Compute p-value with permutation */
void HSICEstimatorImplementation::computePValuesPermutationParallel() const
{
  const SquareMatrix Wobs(computeWeightMatrix(outputSample_));
  Point HSICobs(inputDimension_);
  for (UnsignedInteger dim = 0; dim < inputDimension_; ++dim)
  {
    HSICobs[dim] = computeHSICIndex(inputCovarianceMatrixCollection_[dim], outputCovarianceMatrix_, Wobs);
  }
  Collection<Indices> indicesCollection(permutationSize_);
  for (UnsignedInteger b = 0; b < permutationSize_; ++b)
  {
    const Indices indices(shuffleIndices(outputSample_.getSize()));
    indicesCollection[b] = indices;
  }
  HSICPValuesPermutationFunctor functor(indicesCollection,
                                        HSICobs,
                                        *this);
  TBBImplementation::ParallelReduceIf(weightFunction_.getImplementation()->isParallel(), 0, permutationSize_, functor );
  PValuesPermutation_ = functor.accumulator_ / (permutationSize_ + 1.0);
  isAlreadyComputedPValuesPermutation_ = true;
}

/* Compute the asymptotic p-values */
void HSICEstimatorImplementation::computePValuesAsymptotic() const
{
  // The interest is to compute the asymptotic p-value, which is the sum of
  // elements of the matrix B that writes :
  // B[i,j] = Bx[i,j]^2 * By[i,j]^2 for i !=j, 0 if i = j
  // where :
  // Bx = H * Kx * H
  // By = H * Ky * h
  // H being the matrix:
  // H = Id - U/n where U[i,j] = 1
  // To perform easily this, one could notice that the left side (Bx) is easy to evaluate.
  // (H * Kx * H) = (I - U / n) * Kx * (I -  U/n)
  //                = (Kx - Kx * U / n - U * Kx / n + U * Ky * U / n / n)
  // Having a deeper eye,
  // (Kx * U)[i,j] = \sum_k Kx[i,k]
  // Thus summing the two provides :
  // 1/n * (Kx * U + U * Kx)[i,j] = 1/n \sum_k (Kx[i,k] + Kx[k,j])
  //                              = 1/n \sum_k (Ky[i,k] + Ky[j,k])
  // We get the vector easily using Kx * 1 vector for example
  // For the last part of the second block, we need to compute (U * Kx * U)
  // It is easy to notice that
  // (U Kx U)[i,j] = sum_{k, l} Kx[k,l] for all i, j
  // sum_{k, l} Kx[k,l] is the sum of elements!
  // Finally we get the full structure of the Bx matrix:
  // (H * Kx * H)[i,j] = Kx[i,j] - \sum_k (Kx[i,k] + Ky[j,k]) / n + \sum_{k,l} Kx[k,l] /n /n
  // For By, we replace Kx per Ky
  // As the interest is to get sum_{k,l} Bx@Bx@By@By[k,l] (except the diagonal), we can easily implement it!
  // We compute easily the squared elements, product and the sum of scalars without building the structures
  // Thanks to the symmetry, the quantity of interest is twice the sum of the lower part

  PValuesAsymptotic_ = Point(inputDimension_);

  const Scalar traceKy = outputCovarianceMatrix_.computeTrace();
  const Scalar sumKy = outputCovarianceMatrix_.computeSumElements();
  const Scalar oneOverSqauaredN = 1.0 / n_ / n_;
  // Compute sum rows/columns
  const Point ones(n_, 1.0 / n_);
  const Point sumKyRows(outputCovarianceMatrix_ * ones);

  const Scalar Ey = (sumKy - traceKy) / n_ / (n_ - 1 );
  //const Matrix By(H * outputCovarianceMatrix_ * H);
  const Point HSICobsPt(getHSICIndices());
  // Scaling factor for varHSIC
  const Scalar factor = 2.0 * (n_ - 4) * (n_ - 5) / n_ / (n_ - 1) / (n_ - 2) / (n_ - 3) / n_ / (n_ - 1);

  for(UnsignedInteger dim = 0; dim < inputDimension_; ++dim)
  {
    const CovarianceMatrix Kx(inputCovarianceMatrixCollection_[dim]);
    const Scalar traceKx = inputCovarianceMatrixCollection_[dim].computeTrace();
    const Scalar sumKx = inputCovarianceMatrixCollection_[dim].computeSumElements();
    const Point sumKxRows(inputCovarianceMatrixCollection_[dim] * ones);
    const Scalar Ex = (sumKx - traceKx) / n_ / (n_ - 1);

    const Scalar mHSIC = (1 + Ex * Ey - Ex - Ey) / n_;
    Scalar varHSIC = 0.0;
    for (UnsignedInteger j = 0; j < n_; ++j)
    {
      for (UnsignedInteger i = j + 1; i < n_; ++i)
      {
        const Scalar left = Kx(i, j) - sumKxRows[i] - sumKxRows[j] + sumKx * oneOverSqauaredN;
        const Scalar right = outputCovarianceMatrix_(i, j) - sumKyRows[i] - sumKyRows[j] + sumKy * oneOverSqauaredN;
        varHSIC += 2.0 * left * left * right * right;
      }
    }

    // scaling with the right factor
    varHSIC *= factor;

    const Scalar alpha = mHSIC * mHSIC / varHSIC;
    const Scalar beta = n_ * varHSIC / mHSIC;

    const Gamma distribution(alpha, 1.0 / beta);
    const Scalar p = estimatorType_.computePValue(distribution, n_, HSICobsPt[dim], mHSIC);
    PValuesAsymptotic_[dim] = p;
  }
  isAlreadyComputedPValuesAsymptotic_ = true ;
}

/* Get the HSIC indices */
Point HSICEstimatorImplementation::getHSICIndices() const
{
  if(!(isAlreadyComputedIndices_))
  {
    computeIndices();
    isAlreadyComputedIndices_ = true ;
  }
  return HSIC_XY_;
}

/* Get the R2-HSIC indices */
Point HSICEstimatorImplementation::getR2HSICIndices() const
{
  if(!(isAlreadyComputedIndices_))
  {
    computeIndices();
    isAlreadyComputedIndices_ = true ;
  }
  return R2HSICIndices_;
}

/* Get the p-values by permutation */
Point HSICEstimatorImplementation::getPValuesPermutation() const
{
  if( !(isAlreadyComputedPValuesPermutation_))
  {
    if (ResourceMap::GetAsBool("HSICEstimator-ParallelPValues"))
      computePValuesPermutationParallel();
    else
      computePValuesPermutationSequential();
      
    isAlreadyComputedPValuesPermutation_ = true ;
  }
  return PValuesPermutation_;
}

/* Draw the HSIC indices */
Graph HSICEstimatorImplementation::drawValues(const Point &values, const String &title) const
{
  if (values.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "Values size does not match input dimension";
  Graph graph(title, "Input marginal number", "", true, "");

  /* Define cloud */
  Sample data(values.getDimension(), 2);
  for (UnsignedInteger k = 0; k < values.getDimension(); ++k)
  {
    data(k, 0) = k + 1.0;
    data(k, 1) = values[k];
  }

  Cloud cloud(data, "red", "circle", "");
  graph.add(cloud);

  /* Min & max indices values */
  const Scalar minInd = data.getMin()[1];
  const Scalar maxInd = data.getMax()[1];


  /* Add text description */
  for (UnsignedInteger k = 0; k < values.getDimension(); ++k)
  {
    data(k, 0) = (k + 1.0) + 0.08;
    data(k, 1) = values[k];
  }

  const Description names(inputSample_.getDescription());

  Text text(data, names, "right");
  text.setColor("black");
  graph.add(text);

  // Set bounding box
  const Scalar step = maxInd - minInd;

  Point lowerBound(2);
  lowerBound[0] = 0.9;
  lowerBound[1] = minInd - 0.05 * step;

  Point upperBound(2);
  upperBound[0] = values.getDimension() + 0.2 ;
  upperBound[1] = maxInd + 0.05 * step;

  graph.setBoundingBox(Interval(lowerBound, upperBound));

  return graph;
}


/* Draw the HSIC indices */
Graph HSICEstimatorImplementation::drawHSICIndices() const
{
  return drawValues(getHSICIndices(), "HSIC indices");
}

/* Draw the R2-HSIC indices */
Graph HSICEstimatorImplementation::drawR2HSICIndices() const
{
  return drawValues(getR2HSICIndices(), "R2-HSIC indices");
}

/* Draw the p-values */
Graph HSICEstimatorImplementation::drawPValuesPermutation() const
{
  return drawValues(getPValuesPermutation(), "p-values by permutation");
}

/* Get the covariance list */
HSICEstimatorImplementation::CovarianceModelCollection HSICEstimatorImplementation::getCovarianceModelCollection() const
{
  return covarianceModelCollection_;
}

/* Set the covariance list: dimension is input dimension + 1 */
void HSICEstimatorImplementation::setCovarianceModelCollection(const CovarianceModelCollection & coll)
{
  covarianceModelCollection_ = coll ;
  resetIndices();
  computeCovarianceMatrices();
}

/* Get the input sample */
Sample HSICEstimatorImplementation::getInputSample() const
{
  return inputSample_;
}

/* Set the input sample */
void HSICEstimatorImplementation::setInputSample(const Sample & inputSample)
{
  inputSample_ = inputSample;
  resetIndices();
  computeCovarianceMatrices();
}

/* Get the output sample */
Sample HSICEstimatorImplementation::getOutputSample() const
{
  return outputSample_;
}

/* Set the output sample: must be of dimension one */
void HSICEstimatorImplementation::setOutputSample(const Sample & outputSample)
{
  if(outputSample.getDimension() != 1)
  {
    throw InvalidArgumentException(HERE) << "Dimension of output sample should be 1.";
  }

  outputSample_ = outputSample;
  resetIndices();
  computeCovarianceMatrices();
}


/* Reset all indices to void */
void HSICEstimatorImplementation::resetIndices()
{
  HSIC_XY_ = Point();
  HSIC_XX_ = Point();
  HSIC_YY_ = Point();
  R2HSICIndices_ = Point();
  PValuesPermutation_ = Point();
  isAlreadyComputedIndices_ = false;
  isAlreadyComputedPValuesPermutation_ = false;
  isAlreadyComputedPValuesAsymptotic_ = false;
}

/* Get the dimension of the indices: the number of marginals */
UnsignedInteger HSICEstimatorImplementation::getDimension() const
{
  return inputDimension_;
}

/* Get the size of the study sample */
UnsignedInteger HSICEstimatorImplementation::getSize() const
{
  return n_;
}

/* Get the underlying estimator: biased or unbiased*/
HSICStat HSICEstimatorImplementation::getEstimator() const
{
  return estimatorType_;
}

/* Return a shuffled copy of a sample */
Indices HSICEstimatorImplementation::shuffleIndices(const UnsignedInteger size) const
{
  /* Shuffle an array a of n elements (indices 0..n-1)
    see https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle#The_modern_algorithm
  */
  Indices output(size);
  output.fill();
  for(UnsignedInteger i = size - 1; i > 0; --i)
  {
    const UnsignedInteger j = RandomGenerator::IntegerGenerate(i + 1);
    std::swap(output[j], output[i]);
  }
  return output;
}

/* Run all computations at once */
void HSICEstimatorImplementation::run() const
{
  /* Compute the HSIC indices */
  (void) getHSICIndices();

  /* Compute the R2-HSIC indices */
  (void) getR2HSICIndices();

  /* Compute the p-values by permutation */
  (void) getPValuesPermutation();
}

/* Method save() stores the object through the StorageManager */
void HSICEstimatorImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "covarianceModelCollection_", covarianceModelCollection_ );
  adv.saveAttribute( "inputSample_", inputSample_ );
  adv.saveAttribute( "outputSample_", outputSample_ );
  adv.saveAttribute( "estimatorType_", estimatorType_ );
  adv.saveAttribute( "weightFunction_", weightFunction_ );
  adv.saveAttribute( "n_", n_ );
  adv.saveAttribute( "inputDimension_", inputDimension_ );
  adv.saveAttribute( "HSIC_XY_", HSIC_XY_ );
  adv.saveAttribute( "HSIC_XX_", HSIC_XX_ );
  adv.saveAttribute( "HSIC_YY_", HSIC_YY_ );
  adv.saveAttribute( "R2HSICIndices_", R2HSICIndices_ );
  adv.saveAttribute( "PValuesPermutation_", PValuesPermutation_ );
  adv.saveAttribute( "permutationSize_", permutationSize_ );
  adv.saveAttribute( "isAlreadyComputedIndices_", isAlreadyComputedIndices_ );
  adv.saveAttribute( "isAlreadyComputedPValuesPermutation_", isAlreadyComputedPValuesPermutation_ );
  adv.saveAttribute( "inputCovarianceMatrixCollection_", inputCovarianceMatrixCollection_ );
  adv.saveAttribute( "outputCovarianceMatrix_", outputCovarianceMatrix_ );
}

/* Method load() reloads the object from the StorageManager */
void HSICEstimatorImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "covarianceModelCollection_", covarianceModelCollection_ );
  adv.loadAttribute( "inputSample_", inputSample_ );
  adv.loadAttribute( "outputSample_", outputSample_ );
  adv.loadAttribute( "estimatorType_", estimatorType_ );
  adv.loadAttribute( "weightFunction_", weightFunction_ );
  adv.loadAttribute( "n_", n_ );
  adv.loadAttribute( "inputDimension_", inputDimension_ );
  adv.loadAttribute( "HSIC_XY_", HSIC_XY_ );
  adv.loadAttribute( "HSIC_XX_", HSIC_XX_ );
  adv.loadAttribute( "HSIC_YY_", HSIC_YY_ );
  adv.loadAttribute( "R2HSICIndices_", R2HSICIndices_ );
  adv.loadAttribute( "PValuesPermutation_", PValuesPermutation_ );
  adv.loadAttribute( "permutationSize_", permutationSize_ );
  adv.loadAttribute( "isAlreadyComputedIndices_", isAlreadyComputedIndices_ );
  adv.loadAttribute( "isAlreadyComputedPValuesPermutation_", isAlreadyComputedPValuesPermutation_ );
  adv.loadAttribute( "inputCovarianceMatrixCollection_", inputCovarianceMatrixCollection_ );
  adv.loadAttribute( "outputCovarianceMatrix_", outputCovarianceMatrix_ );
}

END_NAMESPACE_OPENTURNS

